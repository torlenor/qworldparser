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
#include <iostream>

#include "edge.hpp"

template <class T, class F>
class Triangle
{
public:
    Triangle();
    Triangle(const T& A, const T& B, const T& C);
    Triangle(const Edge<T>& E, const T& p);

    T getCircumCenter();
    F getCircumRadius();

    bool isInCircle(const T& p);

    T getA() const { return m_A; }
    T getB() const { return m_B; }
    T getC() const { return m_C; }

    void setA(const T& A) { m_A = A; }
    void setB(const T& B) { m_B = B; }
    void setC(const T& C) { m_C = C; }

    Edge<T> getEdge1() const { return Edge<T>(m_A, m_C); }
    Edge<T> getEdge2() const { return Edge<T>(m_C, m_B); }
    Edge<T> getEdge3() const { return Edge<T>(m_B, m_A); }

    friend std::ostream& operator<< (std::ostream& out, const Triangle<T, F>& triangle)
    {
       return out << "A = " << triangle.getA() << ", "
                  << "B = " << triangle.getB() << ", "
                  << "C = " << triangle.getC();
    }

    bool getIsBad() const { return m_isBad; }
    void setIsBad(const bool isBad) {
        m_isBad = isBad;
    }

private:
    T m_A;
    T m_B;
    T m_C;

    bool m_isCircCalc;
    bool m_isBad;

    T m_circumCenter;
    F m_circumRadius;

    void calcCircum();

};

template <class T, class F>
Triangle<T, F>::Triangle() :
    Triangle<T, F>(T(F(0),F(0)), T(F(1), F(0)), T(F(0), F(1)))
{ }

template <class T, class F>
Triangle<T, F>::Triangle(const Edge<T> &E, const T& p) :
    Triangle<T, F>(E.getP1(), E.getP2(), p)
{}

template <class T, class F>
Triangle<T, F>::Triangle(const T& A, const T& B, const T& C) :
    m_A(A),
    m_B(B),
    m_C(C),
    m_isCircCalc(false),
    m_isBad(false),
    m_circumRadius(F(0))
{ }

template <class T, class F>
void Triangle<T, F>::calcCircum()
{
    // We have to calculate the circumcenter and circumradius of the triangle tri
    // 1) Translate points such that A -> A' = (0,0)
    // 2) Calculate coordinates in ' coordinate system
    // 3) Calculate radius
    // 4) Transform U = U' + A

    // T Aprime = T{0,0};
    T Bprime = m_B - m_A;
    T Cprime = m_C - m_A;

    F d = 2.0*(Bprime.getX()*Cprime.getY() - Bprime.getY()*Cprime.getX());

    F Ux = ( Cprime.getY()*(Bprime.getX()*Bprime.getX() + Bprime.getY()*Bprime.getY()) -
             Bprime.getY()*(Cprime.getX()*Cprime.getX() + Cprime.getY()*Cprime.getY()))/d;

    F Uy = ( Bprime.getX()*(Cprime.getX()*Cprime.getX() + Cprime.getY()*Cprime.getY()) -
             Cprime.getX()*(Bprime.getX()*Bprime.getX() + Bprime.getY()*Bprime.getY()))/d;

    m_circumCenter = T(Ux, Uy) + m_A;
    m_circumRadius = sqrt(Ux*Ux + Uy*Uy);

    m_isCircCalc = true;
}

template <class T, class F>
T Triangle<T, F>::getCircumCenter()
{
    if (m_isCircCalc) {
        return m_circumCenter;
    } else {
        calcCircum();
        return m_circumCenter;
    }
}

template <class T, class F>
F Triangle<T, F>::getCircumRadius()
{
    if (m_isCircCalc) {
        return m_circumRadius;
    } else {
        calcCircum();
        return m_circumRadius;
    }
}

template <class T, class F>
bool Triangle<T, F>::isInCircle(const T& p)
{
    auto center = getCircumCenter();

    F dist = sqrt(   (center.getX() - p.getX())*(center.getX() - p.getX())
                   + (center.getY() - p.getY())*(center.getY() - p.getY()) );

    return dist <= getCircumRadius();
}
