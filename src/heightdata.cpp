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

#include "heightdata.h"

HeightData::HeightData(QVector<QVector<qint16>> rawHeightData)
{
    m_rawHeightData = rawHeightData;
}

float HeightData::getHeight(const double lat, const double lon) const
{
 return 0.0f;
}
