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

#include "heightmapscatterplot.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>

using namespace QtDataVisualization;

HeightmapScatterPlot::HeightmapScatterPlot(QtDataVisualization::QScatterDataArray* scatterDataArray, QWidget *parent) :
    QMainWindow(parent)
{
    graph = new Q3DScatter();
    QWidget *container = QWidget::createWindowContainer(graph);

    QSize screenSize = graph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.5));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

    widget->setWindowTitle(QStringLiteral("Heightmap Data Visualization"));

    QScatterDataProxy *proxy = new QScatterDataProxy;
    QScatter3DSeries *series = new QScatter3DSeries(proxy);
    series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    series->setMeshSmooth(true);
    graph->addSeries(series);

    QLinearGradient linearGrad(QPointF(100, 100), QPointF(200, 200));
    linearGrad.setColorAt(0, Qt::blue);
    linearGrad.setColorAt(1, Qt::red);

    graph->seriesList().at(0)->setBaseGradient(linearGrad);
    graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);

    graph->axisX()->setTitle("LAT");
    graph->axisY()->setTitle("HEIGHT");
    graph->axisZ()->setTitle("LON");

    graph->seriesList().at(0)->dataProxy()->resetArray(scatterDataArray);

    widget->show();
}

HeightmapScatterPlot::~HeightmapScatterPlot()
{
    graph->deleteLater();
}
