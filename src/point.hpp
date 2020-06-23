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

#include <iostream>
#include <memory>

template <class T>
class Point;

template <class T>
using PointPtr = std::shared_ptr<Point<T>>;

template <class T>
class Point
{
public:
    Point();
    Point(const T x, const T y);
    Point(const T x, const T y, const unsigned id);

     Point<T>& operator+=(const Point<T>& rhs)
     {
        this->m_x += rhs.m_x;
        this->m_y += rhs.m_y;
        return *this; // return the result by reference
     }

     Point<T>& operator-=(const Point<T>& rhs)
     {
        this->m_x -= rhs.m_x;
        this->m_y -= rhs.m_y;
        return *this; // return the result by reference
     }

     std::ostream& operator<<(std::ostream& out) const
     {
        return out << "(" << this->m_x << "," << this->m_y << ")";
     }

     friend std::ostream& operator<< (std::ostream& out, const Point<T>& point)
     {
        return out << "(" << point.getX() << "," << point.getY() << ")";
     }

     bool operator==(const Point &rhs) const
     {
         return ( this->getX() == rhs.getX() && this->getY() == rhs.getY() );
     }

     T dot(const Point<T>& rhs)
     {
         return this->m_x*rhs.m_x + this->m_y*rhs.m_y;
     }

     T distance(const Point<T>& rhs)
     {
         return sqrt( (this->getX() - rhs.getX())*(this->getX() - rhs.getX()) + (this->getY() - rhs.getY())*(this->getY() - rhs.getY()) );
     }

     void setX(const T& x) {
         m_x = x;
     }
     T getX() const {
         return m_x;
     }

     void setY(const T& y) {
         m_y = y;
     }
     T getY() const {
         return m_y;
     }

     void setId(const unsigned &id) {
         m_id = id;
     }
    unsigned getId() const { return m_id; }

private:
     float m_x;
     float m_y;

     unsigned m_id;
};

template <class T>
inline Point<T> operator+(Point<T> lhs, const Point<T>& rhs)
{
  lhs += rhs;
  return lhs;
}

template <class T>
inline Point<T> operator-(Point<T> lhs, const Point<T>& rhs)
{
  lhs -= rhs;
  return lhs;
}

template <class T>
Point<T>::Point():
    Point<T>(T(0),T(0))
{ }

template <class T>
Point<T>::Point(const T x, const T y) :
    Point<T>(x, y, 0)
{ }

template <class T>
Point<T>::Point(const T x, const T y, const unsigned id) :
    m_x(x),
    m_y(y),
    m_id(id)
{ }
