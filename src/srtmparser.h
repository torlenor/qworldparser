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

#pragma once

#include <cmath>
#include <string>
#include <vector>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

class SRTMParser
{
public:

    enum HgtType {
        HGT_1,
        HGT_3
    };

    enum InterpolationType {
        NO_INTERPOLATION,
        LINEAR_INTERPOLATION
    };

    SRTMParser(const std::string hgtFileName);

    bool parseData();
    std::vector<std::vector<int> > getHeightData();

    int getLatOrigin() const;
    int getLonOrigin() const;

    double getHeight(const double latitude, const double longitude, const InterpolationType interpolationType = InterpolationType::NO_INTERPOLATION);

    static double calcDistance(const double lat1, const double lat2, const double lon1, const double lon2)
    {
        double R = 6371e3; // metres
        double phi1 = lat1/180.0*M_PI;
        double phi2 = lat2/180.0*M_PI;
        double deltaPhi = (lat2-lat1)/180.0*M_PI;
        double deltaTheta = (lon2-lon1)/180.0*M_PI;

        double a = sin(deltaPhi/2.0) * sin(deltaPhi/2.0) +
                   cos(phi1) * cos(phi2) *
                   sin(deltaTheta/2.0) * sin(deltaTheta/2.0);
        double c = 2 * atan2(sqrt(a), sqrt(1.0-a));

        return R*c;
    }

private:
    bool parseCoordsFromFileName();
    int endianSwap(unsigned char *c);
    bool parseHgt1(std::ifstream &file);
    bool parseHgt3(std::ifstream &file);

    std::string m_hgtFileNameString;
    HgtType m_hgtType;
    int m_lat;
    int m_lon;

    std::vector<std::vector<int>> m_heightData;
    double getHgt1HeightNoInterpol(const double latitude, const double longitude);
    double getHgt3HeightNoInterpol(const double latitude, const double longitude);

    double BilinearInterpolation(double q11, double q12, double q21, double q22, double x1, double x2, double y1, double y2, double x, double y);

    double getHgt1HeightBilinearInterpolation(const double latitude, const double longitude);
    
    int getFileSize(std::ifstream &file);
    std::string getFileBaseName(const std::string &path);
};
