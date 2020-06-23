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

#include <point.hpp>

template <class T>
class Edge
{
public:
    Edge();
    Edge(const T p1, const T p2);

    T getP1() const { return m_p1; }
    T getP2() const { return m_p2; }

    void setP1(const T& p1) {
        m_p1 = p1;
    }
    void setP2(const T& p2) {
        m_p2 = p2;
    }

    std::ostream& operator<<(std::ostream& out) const
    {
       return out << "(" << this->getP1() << "," << this->getP2() << ")";
    }

    bool operator==(const Edge<T> &rhs) const
    {
        return ( (this->getP1() == rhs.getP1() && this->getP2() == rhs.getP2()) ||
                 (this->getP1() == rhs.getP2() && this->getP2() == rhs.getP1())) ;
    }

    void setBadEdge(const bool badEdge) {
        m_badEdge = badEdge;
    }

    bool getBadEdge() const { return m_badEdge; }

private:
    T m_p1;
    T m_p2;

    bool m_badEdge;
};

template <class T>
Edge<T>::Edge():
    Edge<T>( T{0,0}, T{1,1} )
{ }

template <class T>
Edge<T>::Edge(const T p1, const T p2) :
    m_p1(p1),
    m_p2(p2),
    m_badEdge{false}
{ }
