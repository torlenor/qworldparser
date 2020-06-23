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

#include "qworldparser.h"

#include "ui_qworldparser.h"

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <QElapsedTimer>
#include <QFileDialog>
#include <QTextStream>
#include <QTimer>
#include <QtCharts>
#include <QtDataVisualization>
#include <QtMath>

#include "delaunay.hpp"
#include "point.hpp"
#include "heightmapscatterplot.hpp"

using namespace QtDataVisualization;
using namespace QtCharts;

namespace {
    double OFFSET_LAT = 0.0;
    double OFFSET_LON = 0.0;
    double DISTANCE_LAT = 0.02;
    double DISTANCE_LON = 0.02;
    double RESOLUTION_LAT = 0.0001;
    double RESOLUTION_LON = 0.0001;
    double OUT_SCALE = 500; // 1 UU = 1cm

    double HEIGHTMAP_RESOLUTION_LAT_M = 1; // m
    double HEIGHTMAP_RESOLUTION_LON_M = 1; // m
    double HEIGHTMAP_DISTANCE_LAT_M = 500; // m
    double HEIGHTMAP_DISTANCE_LON_M = 500; // m
    double HEIGHTMAP_OUT_SCALE_M_CM_UE = 100; // m->cm ; 1 UU = 1cm

    int HEIGHTMAP_SEGMENTS_LAT = 10;
    int HEIGHTMAP_SEGMENTS_LON = 10;
}

QWorldParser::QWorldParser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QWorldParser)
{
    ui->setupUi(this);

    QSettings settings(SETTINGS_COMPANY, SETTINGS_PRODUCT);
    QString path = settings.value("hgtpath", "").toString();
    ui->labelHeghtMapFolder->setText(path);
    ui->labelHeghtMapFolder_2->setText(path);
}

void QWorldParser::critError(const QString& errorString) const
{
    QMessageBox messageBox;
    messageBox.critical(0, "Error", errorString);
}

void QWorldParser::doHeightMapParsing()
{
    bool ok = false;
    double latStart = ui->latStart->text().toDouble(&ok);
    if (not ok) {
        critError(QString("Invalid Lattitude Start value entered: ") + ui->latStart->text());
        return;
    }
    double latEnd = ui->latEnd->text().toDouble(&ok);
    if (not ok) {
        critError(QString("Invalid Lattitude End value entered: ") + ui->latEnd->text());
        return;
    }
    double latRes = ui->latRes->text().toDouble(&ok);
    if (not ok) {
        critError(QString("Invalid Lattitude Resolution value entered: ") + ui->latRes->text());
        return;
    }
    double lonStart = ui->lonStart->text().toDouble(&ok);
    if (not ok) {
        critError(QString("Invalid Longitude Start value entered: ") + ui->lonStart->text());
        return;
    }
    double lonEnd = ui->lonEnd->text().toDouble(&ok);
    if (not ok) {
        critError(QString("Invalid Longitude End value entered: ") + ui->lonEnd->text());
        return;
    }
    double lonRes = ui->lonRes->text().toDouble(&ok);
    if (not ok) {
        critError(QString("Invalid Longitude Resolution value entered: ") + ui->lonRes->text());
        return;
    }

    QSettings settings(SETTINGS_COMPANY, SETTINGS_PRODUCT);
    QString path = settings.value("hgtpath", "").toString();

    QFileInfo fileInfo(path);
    if (not fileInfo.exists()) {
        critError("Error folder does not exist");
        return;
    }

    QString NorS = ((latStart >= 0) ? QString("N") : QString("S"));
    QString EorW = (lonStart >= 0) ? QString("E") : QString("W");
    QString fileName = path + QString("/")
                     + NorS + QString::number(abs(floor(latStart))).rightJustified(2, '0')
                     + EorW + QString::number(abs(floor(lonStart))).rightJustified(3, '0')
                     + QString(".hgt");

    m_srtmParser = new SRTMParser(fileName.toStdString());

    if(m_srtmParser->parseData()) {
        QScatterDataArray *dataArray = new QScatterDataArray;

        for (float lat = latStart; lat <= latEnd; lat += latRes) {
            for (float lon = lonStart; lon <= lonEnd; lon += lonRes) {
                *dataArray << QVector3D(lon,
                                         m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::LINEAR_INTERPOLATION),
                                        lat);
            }
        }

        HeightmapScatterPlot *heightmapScatterPlot = new HeightmapScatterPlot(dataArray);
    } else {
        critError(QString("Couldn't parse Heightmap data. Check if ") + fileName + QString(" exists."));
    }
}

void QWorldParser::testHeight()
{
    qDebug() << "QWorldParser::testHeight()";
    float lat = 46.0f;
    float lon = 15.0f;
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::LINEAR_INTERPOLATION);
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::NO_INTERPOLATION);

    lat = 47.0f; lon = 16.0f;
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::LINEAR_INTERPOLATION);
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::NO_INTERPOLATION);

    lat = 47.0f + 1.0f/3600.0f; lon = 16.0f + 1.0f/3600.0f;
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::LINEAR_INTERPOLATION);
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::NO_INTERPOLATION);

    lat = 46.0f - 1.0f/3600.0f; lon = 15.0f - 1.0f/3600.0f;
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::LINEAR_INTERPOLATION);
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::NO_INTERPOLATION);

    lat = 46.4124f; lon = 15.896445f;
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::LINEAR_INTERPOLATION);
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::NO_INTERPOLATION);

    lat = 46.4844f; lon = 15.4819f;
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::LINEAR_INTERPOLATION);
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::NO_INTERPOLATION);

    lat = 46.3851f; lon = 15.7100f;
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::LINEAR_INTERPOLATION);
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::NO_INTERPOLATION);

    lat = 46.5f;
    lon = 15.5f;
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::LINEAR_INTERPOLATION);
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::NO_INTERPOLATION);

    lat = 46.5f+0.2f/3600;
    lon = 15.5f;
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::LINEAR_INTERPOLATION);
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::NO_INTERPOLATION);

    lat = 46.5f;
    lon = 15.5f+0.2f/3600;
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::LINEAR_INTERPOLATION);
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::NO_INTERPOLATION);

    lat = 46.5f+0.2f/3600 ;
    lon = 15.5f+0.2f/3600;
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::LINEAR_INTERPOLATION);
    qDebug() << "Latitude = " << lat << " Longitude = " << lon << " Height = " << m_srtmParser->getHeight(lat, lon, SRTMParser::InterpolationType::NO_INTERPOLATION);

    qDebug() << "END QWorldParser::testHeight()";
}

void QWorldParser::createTriangulatedHeightMap()
{
    QSettings settings(SETTINGS_COMPANY, SETTINGS_PRODUCT);
    QString path = settings.value("hgtpath", "").toString();
    QString fileName;
    if (path.size() > 0) {
        fileName = QFileDialog::getOpenFileName(this, tr("Open HGT File"), path);
    } else {
        fileName = QFileDialog::getOpenFileName(this, tr("Open HGT File"));
    }
    QFileInfo fileInfo(fileName);
    path = fileInfo.path();
    settings.setValue("hgtpath", path);

    // Parse the heightmap
    m_srtmParser = new SRTMParser(fileName.toStdString());

    if(!m_srtmParser->parseData()) {
        std::cerr << "Error parsing heightdata" << std::endl;
        return;
     }

    // Create a grid
    m_points.clear();
    unsigned point_id = 0;
    for (double lat = m_srtmParser->getLatOrigin() + ::OFFSET_LAT;
                lat <= m_srtmParser->getLatOrigin() + ::OFFSET_LAT + ::DISTANCE_LAT + ::RESOLUTION_LAT/2.0 ;
                lat += ::RESOLUTION_LAT) {
        for (double lon = m_srtmParser->getLonOrigin() + ::OFFSET_LON;
                    lon <= m_srtmParser->getLonOrigin() + ::OFFSET_LON + ::DISTANCE_LON  + ::RESOLUTION_LON/2.0;
                    lon += RESOLUTION_LON) {
            ++point_id;
            m_points.push_back({ lat , lon, point_id });
        }
    }

    // Do the triangulation
    qsrand(QDateTime::currentMSecsSinceEpoch() / 1000);
    QElapsedTimer timer;
    timer.start();

    m_delaunay.setPoints(m_points);
    m_delaunay.triangulate();
    std::cout << "Delaunay::triangulate(): Triangulation took " << timer.elapsed()/1000.0 << " seconds" << std::endl;

    m_triangles = m_delaunay.getTriangles();

    writeTriangles();

    writeTrianglesPlot();

    writePoints();

    writeObj();
}

void QWorldParser::writeObj()
{
    QSettings settings(SETTINGS_COMPANY, SETTINGS_PRODUCT);

    QString path = settings.value("path_obj", "").toString();
    QString fileName;
    if (path.size() > 0) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save Heightmap Wavefront Obj"), path);
    } else {
        fileName = QFileDialog::getSaveFileName(this, tr("Save Heightmap Wavefront Obj"));
    }
    QFileInfo objFileInfo(fileName);
    path = objFileInfo.path();
    settings.setValue("path_obj", path);

    // wavefront format
    QFile objFile(fileName);

    objFile.open(QIODevice::WriteOnly);
    QTextStream objStream(&objFile);

    // name
    objStream << "o " << "Heightmap" << endl;

    double distanceX = 111.2;
    double distanceY = 75.83;

    for (auto& point : m_points) {
        objStream << "v " << ::OUT_SCALE*distanceX*(point.getX() - m_srtmParser->getLatOrigin())
                  << " "  << ::OUT_SCALE*distanceY*(point.getY() - m_srtmParser->getLonOrigin())
                  << " "  << ::OUT_SCALE*m_srtmParser->getHeight(point.getX(), point.getY())/1000.0f << endl;
    }

    // triangles
    for (auto& triangle : m_triangles) {
        objStream << "f " << triangle.getA().getId() << " " << triangle.getB().getId() << " " << triangle.getC().getId() << endl;
    }

    objFile.close();
}

void QWorldParser::writePoints()
{
    QSettings settings(SETTINGS_COMPANY, SETTINGS_PRODUCT);

    QString path = settings.value("path_points", "").toString();
    QString fileName;
    if (path.size() > 0) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save Heightmap Points"), path);
    } else {
        fileName = QFileDialog::getSaveFileName(this, tr("Save Heightmap Points"));
    }
    QFileInfo pointsFileInfo(fileName);
    path = pointsFileInfo.path();
    settings.setValue("path_points", path);

    QFile pointsFile(fileName);

    pointsFile.open(QIODevice::WriteOnly);
    QTextStream pointsStream(&pointsFile);

    pointsStream.setRealNumberPrecision(5);

    for (auto& point : m_points) {
        pointsStream << ::OUT_SCALE*(point.getX() - m_srtmParser->getLatOrigin())/DISTANCE_LAT
                     << " "  << ::OUT_SCALE*(point.getY() - m_srtmParser->getLonOrigin())/DISTANCE_LON
                     << " "  << ::OUT_SCALE*m_srtmParser->getHeight(point.getX(), point.getY())/1000.0f << endl;
    }


    pointsFile.close();
}

void QWorldParser::writeTrianglesPlot()
{
    QSettings settings(SETTINGS_COMPANY, SETTINGS_PRODUCT);

    QString path = settings.value("path_triangles", "").toString();
    QString fileName;
    if (path.size() > 0) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save Heightmap Triangles For Plot"), path);
    } else {
        fileName = QFileDialog::getSaveFileName(this, tr("Save Heightmap Triangles For Plot"));
    }
    QFileInfo triangleFileInfo(fileName);
    path = triangleFileInfo.path();
    settings.setValue("path_triangles", path);

    QFile triangleFile(fileName);

    triangleFile.open(QIODevice::WriteOnly);
    QTextStream triangleStream(&triangleFile);

    // vertices
    std::cout << "Number of final triangles = " << m_triangles.size() << std::endl;
    triangleStream << "# Number of final triangles = " << m_triangles.size() << endl;
    int t = 0;
    for (auto& triangle : m_triangles) {
        t++;

        triangleStream << triangle.getEdge1().getP1().getX() << " " << triangle.getEdge1().getP1().getY() << " " << m_srtmParser->getHeight(triangle.getEdge1().getP1().getX(), triangle.getEdge1().getP1().getY()) << " " << t << endl;
        triangleStream << triangle.getEdge1().getP2().getX() << " " << triangle.getEdge1().getP2().getY() << " " << m_srtmParser->getHeight(triangle.getEdge1().getP2().getX(), triangle.getEdge1().getP2().getY()) << " " << t << endl;
        triangleStream << endl;
        triangleStream << triangle.getEdge2().getP1().getX() << " " << triangle.getEdge2().getP1().getY() << " " << m_srtmParser->getHeight(triangle.getEdge2().getP1().getX(), triangle.getEdge2().getP1().getY()) << " " << t << endl;
        triangleStream << triangle.getEdge2().getP2().getX() << " " << triangle.getEdge2().getP2().getY() << " " << m_srtmParser->getHeight(triangle.getEdge2().getP2().getX(), triangle.getEdge2().getP2().getY()) << " " << t << endl;
        triangleStream << endl;
        triangleStream << triangle.getEdge3().getP1().getX() << " " << triangle.getEdge3().getP1().getY() << " " << m_srtmParser->getHeight(triangle.getEdge3().getP1().getX(), triangle.getEdge3().getP1().getY()) << " " << t << endl;
        triangleStream << triangle.getEdge3().getP2().getX() << " " << triangle.getEdge3().getP2().getY() << " " << m_srtmParser->getHeight(triangle.getEdge3().getP2().getX(), triangle.getEdge3().getP2().getY()) << " " << t << endl;
        triangleStream << endl;
        triangleStream << endl;
    }

    triangleFile.close();
}

void QWorldParser::writeTriangles()
{
    QSettings settings(SETTINGS_COMPANY, SETTINGS_PRODUCT);

    QString path = settings.value("path_triangles", "").toString();
    QString fileName;
    if (path.size() > 0) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save Heightmap Triangles"), path);
    } else {
        fileName = QFileDialog::getSaveFileName(this, tr("Save Heightmap Triangles"));
    }
    QFileInfo triangleFileInfo(fileName);
    path = triangleFileInfo.path();
    settings.setValue("path_triangles", path);

    QFile triangleFile(fileName);

    triangleFile.open(QIODevice::WriteOnly);
    QTextStream triangleStream(&triangleFile);

    // vertices
    std::cout << "Number of final triangles = " << m_triangles.size() << std::endl;

    // triangles
    for (auto& triangle : m_triangles) {
        triangleStream << triangle.getA().getId()-1 << " " << triangle.getB().getId()-1 << " " << triangle.getC().getId()-1 << endl;
    }

    triangleFile.close();
}

QWorldParser::~QWorldParser()
{
    delete ui;
}

void QWorldParser::on_pushButtonHeightMapParsing_clicked()
{
    doHeightMapParsing();
}

void QWorldParser::on_pushButtonCreateHeightMapTriangulation_clicked()
{
    createTriangulatedHeightMap();
}

void QWorldParser::exportHeightMapCarthesian()
{
    QSettings settings(SETTINGS_COMPANY, SETTINGS_PRODUCT);
    QString path = settings.value("hgtpath", "").toString();
    QString fileName;
    if (path.size() > 0) {
        fileName = QFileDialog::getOpenFileName(this, tr("Open HGT File"), path);
    } else {
        fileName = QFileDialog::getOpenFileName(this, tr("Open HGT File"));
    }
    QFileInfo fileInfo(fileName);
    path = fileInfo.path();
    settings.setValue("hgtpath", path);

    // Parse the heightmap
    m_srtmParser = new SRTMParser(fileName.toStdString());

    if(!m_srtmParser->parseData()) {
        std::cerr << "Error parsing heightdata" << std::endl;
        return;
     }

    path = settings.value("folder_points_heightmap", "").toString();
    QString outputFolder;
    if (path.size() > 0) {
        outputFolder = QFileDialog::getExistingDirectory(0, ("Select Output Folder"), path);
    } else {
        outputFolder = QFileDialog::getExistingDirectory(0, ("Select Output Folder"));
    }
    if (outputFolder.size() == 0) {
        std::cout << "Error selecting folder for heightmap output" << std::endl;
        return;
    }
    settings.setValue("folder_points_heightmap", outputFolder);




    bool ok = false;
    double latStart = ui->latStart_2->text().toDouble(&ok);
    if (not ok) {
        critError(QString("Invalid Lattitude Start value entered: ") + ui->latStart_2->text());
        return;
    }
    double latRes = ui->lineEditResolutionLat->text().toDouble(&ok);
    if (not ok) {
        critError(QString("Invalid Lattitude Resolution value entered: ") + ui->lineEditResolutionLat->text());
        return;
    }
    double lonStart = ui->lonStart_2->text().toDouble(&ok);
    if (not ok) {
        critError(QString("Invalid Longitude Start value entered: ") + ui->lonStart_2->text());
        return;
    }
    double lonRes = ui->lineEditResolutionLon->text().toDouble(&ok);
    if (not ok) {
        critError(QString("Invalid Longitude Resolution value entered: ") + ui->lineEditResolutionLon->text());
        return;
    }

    ::HEIGHTMAP_RESOLUTION_LAT_M = ui->lineEditResolutionLat->text().toDouble(); // m
    ::HEIGHTMAP_RESOLUTION_LON_M = ui->lineEditResolutionLon->text().toDouble(); // m
    ::HEIGHTMAP_DISTANCE_LAT_M = ui->lineEditSegmentSizeLat->text().toDouble(); // m
    ::HEIGHTMAP_DISTANCE_LON_M = ui->lineEditSegmentSizeLon->text().toDouble(); // m
    ::HEIGHTMAP_OUT_SCALE_M_CM_UE = 100; // m->cm ; 1 UU = 1cm

    ::HEIGHTMAP_SEGMENTS_LAT = ui->spinBoxLatSegments->text().toDouble();
    ::HEIGHTMAP_SEGMENTS_LON = ui->spinBoxLonSegments->text().toDouble();

    double latZero = ui->latZero->text().toDouble();
    double lonZero = ui->lonZero->text().toDouble();

    // Create a grid and write the data to files
    for (int i=0; i<::HEIGHTMAP_SEGMENTS_LAT; i++) {
        for (int j=0; j<HEIGHTMAP_SEGMENTS_LON; j++) {
            std::vector<Point<double>> points; // m

            unsigned point_id = 0;
            for (double lat_m = i*::HEIGHTMAP_DISTANCE_LAT_M;
                        lat_m <= (i+1)*::HEIGHTMAP_DISTANCE_LAT_M ;
                        lat_m += ::HEIGHTMAP_RESOLUTION_LAT_M) {
                for (double lon_m = j*::HEIGHTMAP_DISTANCE_LON_M;
                            lon_m <= (j+1)*::HEIGHTMAP_DISTANCE_LON_M;
                            lon_m += ::HEIGHTMAP_RESOLUTION_LON_M) {
                    ++point_id;
                    points.push_back({ lat_m , lon_m, point_id });
                }
            }

            std::cout << "Generated segment " << i << "x" << j << " (Npoints = " << points.size() << ")" << std::endl;

            writePointsHeightMapCarthesian(points, i, j, outputFolder, latZero, lonZero);
        }
    }
}

void QWorldParser::writePointsHeightMapCarthesian(const std::vector<Point<double>>& points, const int x, const int y, const QString& outputFolder, const double latZero, const double lonZero)
{
    QString fileName = outputFolder + QString("/heightmap_") + QString::number(x) + QString("_") + QString::number(y) + QString(".dat");

    double distanceLat1Lat2Lon1 = SRTMParser::calcDistance(latZero, latZero+1, lonZero, lonZero); // m
    double distanceLat1Lat2Lon2 = SRTMParser::calcDistance(latZero, latZero+1, lonZero+1, lonZero+1); // m
    double distanceLon1Lon2Lat1 = SRTMParser::calcDistance(latZero, latZero, lonZero, lonZero+1); // m
    double distanceLon1Lon2Lat2 = SRTMParser::calcDistance(latZero+1, latZero+1, lonZero, lonZero+1); // m

    std::cout << "distanceLat1Lat2Lon1 = " << distanceLat1Lat2Lon1 << std::endl;
    std::cout << "distanceLat1Lat2Lon2 = " << distanceLat1Lat2Lon2 << std::endl;
    std::cout << "distanceLon1Lon2Lat1 = " << distanceLon1Lon2Lat1 << std::endl;
    std::cout << "distanceLon1Lon2Lat2 = " << distanceLon1Lon2Lat2 << std::endl;

//    QFile pointsFile(fileName);

//    pointsFile.open(QIODevice::WriteOnly);
//    if (!pointsFile.isOpen()) {
//        std::cout << "Error opening file: " << fileName.toStdString() << std::endl;
//    }

//    std::cout << "Writing points data to: " << fileName.toStdString() << std::endl;

//    QTextStream pointsStream(&pointsFile);
//    pointsStream.setRealNumberPrecision(10);

//    for (auto& point : points) {
//        double lat_coord = point.getX()/SRTMParser::calcDistance(latZero, point.getX(), lonZero, lonZero) + latZero;
//        double lon_coord = point.getY()/( 0.5*(SRTMParser::calcDistance(latZero, latZero, lonZero, point.getY()) + SRTMParser::calcDistance(point.getX(), point.getX(), lonZero, point.getY()))) + m_srtmParser->getLonOrigin();
//        pointsStream
//                << ::HEIGHTMAP_OUT_SCALE_M_CM_UE*point.getX()
//        << " "  << ::HEIGHTMAP_OUT_SCALE_M_CM_UE*point.getY()
//        << " "  << ::HEIGHTMAP_OUT_SCALE_M_CM_UE*m_srtmParser->getHeight(lat_coord, lon_coord, SRTMParser::InterpolationType::LINEAR_INTERPOLATION) << endl;
//    }

//    pointsFile.close();

    QString gnuplotFileName = outputFolder + QString("/heightmap_plot_") + QString::number(x) + QString("_") + QString::number(y) + QString(".dat");

    QFile gnuplotPointsFile(gnuplotFileName);

    gnuplotPointsFile.open(QIODevice::WriteOnly);
    QTextStream gnuplotPointsStream(&gnuplotPointsFile);

    gnuplotPointsStream.setRealNumberPrecision(10);

    double old_x_coord = points.front().getX();
    for (auto& point : points) {
        double lat_coord = point.getX()/distanceLat1Lat2Lon1 + m_srtmParser->getLatOrigin();
        double lon_coord = point.getY()/( 0.5*(distanceLon1Lon2Lat1 + distanceLon1Lon2Lat2)) + m_srtmParser->getLonOrigin();

//        double lat_coord = point.getX()/SRTMParser::calcDistance(latZero, point.getX(), lonZero, lonZero) + latZero;
//        double lon_coord = point.getY()/( 0.5*(SRTMParser::calcDistance(latZero, latZero, lonZero, point.getY()) + SRTMParser::calcDistance(point.getX(), point.getX(), lonZero, point.getY()))) + m_srtmParser->getLonOrigin();

        gnuplotPointsStream
                << ::HEIGHTMAP_OUT_SCALE_M_CM_UE*point.getX()
        << " "  << ::HEIGHTMAP_OUT_SCALE_M_CM_UE*point.getY()
        << " "  << ::HEIGHTMAP_OUT_SCALE_M_CM_UE*m_srtmParser->getHeight(lat_coord, lon_coord, SRTMParser::InterpolationType::LINEAR_INTERPOLATION) << endl;

        if (old_x_coord != point.getX()) {
            gnuplotPointsStream << endl;
            old_x_coord = point.getX();
        }
    }

    gnuplotPointsFile.close();

    std::cout << "Files written" << std::endl;
}

void QWorldParser::on_pushButtonExportHeightMap_clicked()
{
    exportHeightMapCarthesian();
}

void QWorldParser::on_pushButtonWriteBinaryFileTest_clicked()
{
    std::cout << "writing binary file..." << std::endl;
    // int16_t x=4242;
    float f=42.42;

    std::ofstream outfile ("H:/git/QWorldParser/output/binary/test.bin",std::ofstream::binary);

    outfile.write ((char*)&f, sizeof (f));
    outfile.close();

    std::cout << "done" << std::endl;
}

void QWorldParser::setHeightMapFolder()
{
    QSettings settings(SETTINGS_COMPANY, SETTINGS_PRODUCT);
    QString path = settings.value("hgtpath", "").toString();

    QString inputFolder;
    if (path.size() > 0) {
        inputFolder = QFileDialog::getExistingDirectory(0, ("Select SRTM HGT Heightmap Input Folder"), path);
    } else {
        inputFolder = QFileDialog::getExistingDirectory(0, ("Select SRTM HGT Heightmap Input Folder"));
    }
    if (inputFolder.size() == 0) {
        critError("Error selecting folder for heightmap input");
        return;
    }

    QFileInfo fileInfo(inputFolder);
    path = fileInfo.absoluteFilePath();
    if (not fileInfo.exists()) {
        critError("Error folder does not exist");
        return;
    }
    settings.setValue("hgtpath", path);
    ui->labelHeghtMapFolder->setText(path);
    ui->labelHeghtMapFolder_2->setText(path);
}

void QWorldParser::on_pushButtonSetHeightMapFolder_clicked()
{
    setHeightMapFolder();
}

void QWorldParser::on_pushButtonSetHeightMapFolder_2_clicked()
{
    setHeightMapFolder();
}








void QWorldParser::exportHeightMap()
{
    QSettings settings(SETTINGS_COMPANY, SETTINGS_PRODUCT);
    QString path = settings.value("hgtpath", "").toString();
    QString fileName;
    if (path.size() > 0) {
        fileName = QFileDialog::getOpenFileName(this, tr("Open HGT File"), path);
    } else {
        fileName = QFileDialog::getOpenFileName(this, tr("Open HGT File"));
    }
    QFileInfo fileInfo(fileName);
    path = fileInfo.path();
    settings.setValue("hgtpath", path);

    // Parse the heightmap
    m_srtmParser = new SRTMParser(fileName.toStdString());

    if(!m_srtmParser->parseData()) {
        std::cerr << "Error parsing heightdata" << std::endl;
        return;
     }

    path = settings.value("folder_points_heightmap", "").toString();
    QString outputFolder;
    if (path.size() > 0) {
        outputFolder = QFileDialog::getExistingDirectory(0, ("Select Output Folder"), path);
    } else {
        outputFolder = QFileDialog::getExistingDirectory(0, ("Select Output Folder"));
    }
    if (outputFolder.size() == 0) {
        std::cout << "Error selecting folder for heightmap output" << std::endl;
        return;
    }
    settings.setValue("folder_points_heightmap", outputFolder);

    double LAT_S_START = 47;
    double LON_S_START = 15;
    double HEIGHTMAP_RESOLUTION_LAT_S = 0.0002; // °
    double HEIGHTMAP_RESOLUTION_LON_S = 0.0002; // °
    double HEIGHTMAP_DISTANCE_LAT_S = 0.1; // °
    double HEIGHTMAP_DISTANCE_LON_S = 0.1; // °

    std::vector<Point<double>> points; // °

    unsigned point_id = 0;
    for (double lat_s = LAT_S_START;
                lat_s <= LAT_S_START + HEIGHTMAP_DISTANCE_LAT_S;
                lat_s += HEIGHTMAP_RESOLUTION_LAT_S) {
        for (double lon_s = LON_S_START;
                    lon_s <= LON_S_START + HEIGHTMAP_DISTANCE_LON_S;
                    lon_s += HEIGHTMAP_RESOLUTION_LON_S) {
            ++point_id;
            points.push_back({ lat_s , lon_s, point_id });
        }
    }

    std::cout << "Generated segment (Npoints = " << points.size() << ")" << std::endl;

    writePointsHeightMap(points, 0, 0, outputFolder);
}

void QWorldParser::writePointsHeightMap(const std::vector<Point<double>>& points, const int x, const int y, const QString& outputFolder)
{
    QString fileName = outputFolder + QString("/heightmap_") + QString::number(x) + QString("_") + QString::number(y) + QString(".dat");

    double EARTH_RADIUS = 6371000; // m

    QFile pointsFile(fileName);

    pointsFile.open(QIODevice::WriteOnly);
    if (!pointsFile.isOpen()) {
        std::cout << "Error opening file: " << fileName.toStdString() << std::endl;
    }

    std::cout << "Writing points data to: " << fileName.toStdString() << std::endl;

    QTextStream pointsStream(&pointsFile);
    pointsStream.setRealNumberPrecision(10);

    for (auto& point : points) {
        double carthesianX = EARTH_RADIUS*cos(point.getX()*M_PI/180.0)*cos(point.getY()*M_PI/180.0);
        double carthesianY = EARTH_RADIUS*cos(point.getX()*M_PI/180.0)*sin(point.getY()*M_PI/180.0);
        double carthesianZ = EARTH_RADIUS*sin(point.getX()*M_PI/180.0);
        pointsStream
                << ::HEIGHTMAP_OUT_SCALE_M_CM_UE*carthesianX - 419695887.4
        << " "  << ::HEIGHTMAP_OUT_SCALE_M_CM_UE*carthesianY - 112457174.1
        << " "  << ::HEIGHTMAP_OUT_SCALE_M_CM_UE*(carthesianZ + m_srtmParser->getHeight(point.getX(), point.getY(), SRTMParser::InterpolationType::LINEAR_INTERPOLATION)) - 466036243.3 << endl;
    }

    pointsFile.close();

    QString gnuplotFileName = outputFolder + QString("/heightmap_plot_") + QString::number(x) + QString("_") + QString::number(y) + QString(".dat");

    QFile gnuplotPointsFile(gnuplotFileName);

    gnuplotPointsFile.open(QIODevice::WriteOnly);
    QTextStream gnuplotPointsStream(&gnuplotPointsFile);

    gnuplotPointsStream.setRealNumberPrecision(10);

    double old_x_coord = points.front().getX();
    for (auto& point : points) {
        double carthesianX = EARTH_RADIUS*cos(point.getX()*M_PI/180.0)*cos(point.getY()*M_PI/180.0);
        double carthesianY = EARTH_RADIUS*cos(point.getX()*M_PI/180.0)*sin(point.getY()*M_PI/180.0);
        double carthesianZ = EARTH_RADIUS*sin(point.getX()*M_PI/180.0);

        gnuplotPointsStream
                << ::HEIGHTMAP_OUT_SCALE_M_CM_UE*carthesianX - 450068737.3
        << " "  << ::HEIGHTMAP_OUT_SCALE_M_CM_UE*carthesianY - 419695887.4
        << " "  << ::HEIGHTMAP_OUT_SCALE_M_CM_UE*(carthesianZ + m_srtmParser->getHeight(point.getX(), point.getY(), SRTMParser::InterpolationType::LINEAR_INTERPOLATION)) - 615482143.9 << endl;

        if (old_x_coord != point.getX()) {
            gnuplotPointsStream << endl;
            old_x_coord = point.getX();
        }
    }

    gnuplotPointsFile.close();

    std::cout << "Files written" << std::endl;
}

void QWorldParser::on_pushButton_clicked()
{
    exportHeightMap();
}
