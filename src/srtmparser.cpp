/****************************************************************************
**
** Copyright (C) 2018 Hans-Peter Schadler <hps@abyle.org>
**
** This program is free software: you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the Free
** Software Foundation, either version 3 of the License, or (at your option)
** any later version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
** FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
** more details.
**
** You should have received a copy of the GNU General Public License along with
** this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "srtmparser.h"

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>

SRTMParser::SRTMParser(const std::string hgtFileName)
{
    m_hgtFileNameString = hgtFileName;

    if (parseCoordsFromFileName()) {
        std::cout << "SRTMParser::SRTMParser(const std::string hgtFileName): Lat = " << m_lat << " Lon = " << m_lon << std::endl;
    } else {
        std::cout << "SRTMParser::SRTMParser(const std::string hgtFileName): Can't parse filename " << hgtFileName << std::endl;
    }
}

int SRTMParser::endianSwap(unsigned char* c)
{
    return 256*(unsigned char)c[0] + (unsigned char)c[1];
}

int SRTMParser::getFileSize(std::ifstream& file)
{
    if (not file.is_open()) {
        std::cerr << "File is not open" << std::endl;
        return -1;
    }

    file.seekg(0, std::ios_base::end);
    int fileSize = file.tellg();
    file.seekg(0, std::ios_base::beg);
    return fileSize;
}

bool SRTMParser::parseData()
{
    auto start = std::chrono::system_clock::now();
    
    m_heightData.clear();

    std::ifstream file(m_hgtFileNameString, std::ios::binary);
    auto fileSize = getFileSize(file);
    if (fileSize == 2*3601*3601) {
        std::cout << "SRTMParser::parseData(): HGT 1\" file detected" << std::endl;
        m_hgtType = HGT_1;
    } else if (fileSize == 2*1201*1201) {
        std::cout << "SRTMParser::parseData(): HGT 3\" file detected" << std::endl;
        m_hgtType = HGT_3;
    } else {
        std::cout << "SRTMParser::parseData(): Not a valid HGT file" << std::endl;
        return false;
    }

    if (not file.is_open()) {
        std::cerr << "File is not open" << std::endl;
        return false;
    }

    bool ok = false;
    switch (m_hgtType) {
        case HGT_1: ok = parseHgt1(file);
                    break;
        case HGT_3: ok = parseHgt3(file);
                    break;
        default: break;
    }

    file.close();
    
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "SRTMParser::parseData(): The parsing operation took " << elapsed.count() << " milliseconds" << std::endl;

    if (ok) {
        std::cout << "SRTMParser::parseData(): First elevation Value = " << m_heightData.at(1).at(1) << std::endl;
        std::cout << "SRTMParser::parseData(): Last elevation Value = " << m_heightData.back().back() << std::endl;
        std::cout << "SRTMParser::parseData(): Rows = " << m_heightData.size() << std::endl;
        return true;
    } else {
        std::cout << "SRTMParser::parseData(): Error parsing the hgt file" << std::endl;
        return false;
    }
}

bool SRTMParser::parseHgt1(std::ifstream &file)
{
    std::vector<int> vecRow;

    unsigned char c[2];

    int col = 0;
    int numRead = 0;

    while (file.read((char*)c, 2)) {
        col++;

        vecRow.push_back(endianSwap(c));
        if (col == 3601) {
            m_heightData.push_back(vecRow);
            numRead += col;
            col = 0;
            vecRow.clear();
        }
    }

    if (numRead == 3601*3601) {
        return true;
    } else {
        return false;
    }
}

bool SRTMParser::parseHgt3(std::ifstream &file)
{
    std::vector<int> vecRow;

    unsigned char c[2];

    int col = 0;
    int numRead = 0;

    while (file.read((char*)c, 2)) {
        col++;

        vecRow.push_back(endianSwap(c));
        if (col == 1201) {
            m_heightData.push_back(vecRow);
            numRead += col;
            col = 0;
            vecRow.clear();
        }
    }

    if (numRead == 1201*1201) {
        return true;
    } else {
        return false;
    }
}

std::vector<std::vector<int>> SRTMParser::getHeightData()
{
    return m_heightData;
}

inline double
SRTMParser::BilinearInterpolation(double q11, double q12, double q21, double q22, double x1, double x2, double y1, double y2, double x, double y)
{
    double x2x1, y2y1, x2x, y2y, yy1, xx1;
    x2x1 = x2 - x1;
    y2y1 = y2 - y1;
    x2x = x2 - x;
    y2y = y2 - y;
    yy1 = y - y1;
    xx1 = x - x1;
    return 1.0 / (x2x1 * y2y1) * (
        q11 * x2x * y2y +
        q21 * xx1 * y2y +
        q12 * x2x * yy1 +
        q22 * xx1 * yy1
    );
}

double SRTMParser::getHgt1HeightBilinearInterpolation(const double latitude, const double longitude)
{
    // First get the values in the columns
    double latSecShifted = (latitude - m_lat)*3600;
    double lonSecShifted = (longitude - m_lon)*3600;

    if (latSecShifted < 0 || lonSecShifted < 0) {
        return -10000.0f; // invalid request
    }
    int latInt = latSecShifted;
    int longInt = lonSecShifted;
    int row = (3601 - 1) - latInt;
    int col = longInt;

    int row_q11 = row;
    int col_q11 = col;

    int row_q12 = row;
    int col_q12 = col + 1;

    int row_q21 = row - 1; // must go in negative direction because in original heightmap data set the lat coordinate goes in negative direction for +delta_lat
    int col_q21 = col;

    int row_q22 = row - 1; // must go in negative direction because in original heightmap data set the lat coordinate goes in negative direction for +delta_lat
    int col_q22 = col + 1;

    if (row_q11 < 0 || row_q11 > 3600 || col_q11 < 0 || col_q11 > 3600) {
        return -10000.0f; // invalid request
    }

    if (row_q12 < 0 || row_q12 > 3600 || col_q12 < 0 || col_q12 > 3600) {
        return m_heightData[row][col]; // no extrapolation
    }

    if (row_q21 < 0 || row_q21 > 3600 || col_q21 < 0 || col_q21 > 3600) {
        return m_heightData[row][col]; // no extrapolation
    }

    if (row_q22 < 0 || row_q22 > 3600 || col_q22 < 0 || col_q22 > 3600) {
        return m_heightData[row][col]; // no extrapolation
    }

    double q11 = m_heightData[row_q11][col_q11];
    double q12 = m_heightData[row_q12][col_q12];
    double q21 = m_heightData[row_q21][col_q21];
    double q22 = m_heightData[row_q22][col_q22];

    double x1 = ((3601 - 1) - row_q11)/3600.0 + m_lat;
    double y1 = col_q11/3600.0 + m_lon;

    double x2 = ((3601 - 1) - row_q21)/3600.0 + m_lat;
    double y2 = col_q12/3600.0 + m_lon;

    return BilinearInterpolation(q11, q12, q21, q22, x1, x2, y1, y2, latitude, longitude);
}

double SRTMParser::getHgt1HeightNoInterpol(const double latitude, const double longitude)
{
    // find entry in vector
    double latSecShifted = (latitude - m_lat)*3600;
    double lonSecShifted = (longitude - m_lon)*3600;

    if (latSecShifted < 0 || lonSecShifted < 0) {
        return -10000.0f; // invalid request
    }
    int latInt = latSecShifted;
    int longInt = lonSecShifted;
    int row = (3601 - 1) - latInt;
    int col = longInt;
    if (row < 0 || row > 3600 || col < 0 || col > 3600) {
        return -10000.0f; // invalid request
    }

    return m_heightData[row][col];
}

double SRTMParser::getHgt3HeightNoInterpol(const double latitude, const double longitude)
{
    // find entry in vector
    double latSecShifted = (latitude - m_lat)*1200;
    double lonSecShifted = (longitude - m_lon)*1200;

    if (latSecShifted < 0 || lonSecShifted < 0) {
        return -10000.0f; // invalid request
    }
    int latInt = latSecShifted;
    int longInt = lonSecShifted;
    int row = (1201 - 1) - latInt;
    int col = longInt;
    if (row < 0 || row > 1200 || col < 0 || col > 1200) {
        return -10000.0f; // invalid request
    }

    return m_heightData[row][col];
}

double SRTMParser::getHeight(const double latitude, const double longitude, const SRTMParser::InterpolationType interpolationType)
{
    // TODO find height data based on lat/lon
    // m_lat,m_lon = m_heightData[lastrow][firstcol]
    // the algo depends on HGT_TYPE
    // HGT_1: +1 entry is + 1 minute
    // HGT_3: +1 entry is + 3 minutes
    double height = 0;

    switch (interpolationType) {
        case InterpolationType::NO_INTERPOLATION:
            if (m_hgtType == HgtType::HGT_1) {
                height = getHgt1HeightNoInterpol(latitude, longitude);
            } else if (m_hgtType == HgtType::HGT_3) {
                height = getHgt3HeightNoInterpol(latitude, longitude);
            }
            break;

        case InterpolationType::LINEAR_INTERPOLATION:
            if (m_hgtType == HgtType::HGT_1) {
                height = getHgt1HeightBilinearInterpolation(latitude, longitude);
            } else if (m_hgtType == HgtType::HGT_3) {
                // height = getHgt3HeightBilinearInterpolation(latitude, longitude);
            }
            break;

        default: break;
    }

    return height;
}

std::string SRTMParser::getFileBaseName(std::string const & path)
{
  return path.substr(path.find_last_of("/\\") + 1);
}

bool SRTMParser::parseCoordsFromFileName()
{
    std::string fileName = getFileBaseName(m_hgtFileNameString);
    std::string latString = fileName.substr(1,2);
    std::string lonString = fileName.substr(4,3);

    try {
        m_lat = std::stoi(latString);
        m_lon = std::stoi(lonString);
    }
    catch(std::invalid_argument& e){
        std::cerr << e.what() << std::endl;
        return false;
    }
    catch(std::out_of_range& e){
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

int SRTMParser::getLatOrigin() const
{
    return m_lat;
}

int SRTMParser::getLonOrigin() const
{
    return m_lon;
}
