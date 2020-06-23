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

#include <vector>

#include <QMainWindow>

#include "delaunay.hpp"
#include "point.hpp"
#include "srtmparser.h"

namespace Ui {
class QWorldParser;
}

class QWorldParser : public QMainWindow
{
    Q_OBJECT

public:
    explicit QWorldParser(QWidget *parent = 0);
    ~QWorldParser();

private slots:
    void doHeightMapParsing();

    void on_pushButtonHeightMapParsing_clicked();
    void on_pushButtonCreateHeightMapTriangulation_clicked();
    void on_pushButtonExportHeightMap_clicked();
    
    void on_pushButtonWriteBinaryFileTest_clicked();

    void createTriangulatedHeightMap();

    void exportHeightMapCarthesian();

    void on_pushButtonSetHeightMapFolder_clicked();

    void on_pushButtonSetHeightMapFolder_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::QWorldParser *ui;

    SRTMParser* m_srtmParser;
    Delaunay<double> m_delaunay;

    std::vector<Point<double>> m_points;

    std::vector<Triangle<Point<double>, double> > m_triangles;

    void testHeight();
    void writePoints();
    void writeTriangles();
    void writeTrianglesPlot();
    void writeObj();
    void writePointsHeightMapCarthesian(const std::vector<Point<double> >& points, const int x, const int y, const QString &outputFolder, const double latZero, const double lonZero);
    void critError(const QString &errorString) const;
    void setHeightMapFolder();
    void exportHeightMap();
    void writePointsHeightMap(const std::vector<Point<double> > &points, const int x, const int y, const QString &outputFolder);
};
