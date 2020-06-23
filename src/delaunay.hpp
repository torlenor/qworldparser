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

#include <algorithm>
#include <iostream>
#include <vector>

#include "point.hpp"
#include "triangle.hpp"

template <class F>
class Delaunay
{
public:
    Delaunay()
        :Delaunay(std::vector<Point<F> >{})
    {}

    Delaunay(const std::vector<Point<F> > &points);

    void setPoints(const std::vector<Point<F> > &points) {
        m_points = points;
        m_triangles.clear();
        std::cout << "Delaunnay new initialized with " << m_points.size() << " points" << std::endl;
    }

    void triangulate();

    std::vector<Triangle<Point<F>, F> > getTriangles() const;

private:
    void bowyerWatson();

    std::vector<Point<F>> m_points;
    std::vector<Triangle<Point<F>, F> > m_triangles;
    Triangle<Point<F>, F> constructSuperTriangle();
};

template <class F>
Delaunay<F>::Delaunay(const std::vector<Point<F> > &points) :
    m_points(points)
{
    std::cout << "Delaunnay initialized with " << m_points.size() << " points" << std::endl;
}

template <class F>
void Delaunay<F>::triangulate()
{
    bowyerWatson();
}

template <class F>
std::vector<Triangle<Point<F>, F> > Delaunay<F>::getTriangles() const
{
    return m_triangles;
}

template <class F>
void Delaunay<F>::bowyerWatson()
{
// https://en.wikipedia.org/wiki/Bowyer%E2%80%93Watson_algorithm
//    function BowyerWatson (pointList)
//       // pointList is a set of coordinates defining the points to be triangulated
//       triangulation := empty triangle mesh data structure
//       add super-triangle to triangulation // must be large enough to completely contain all the points in pointList
//       for each point in pointList do // add all the points one at a time to the triangulation
//          badTriangles := empty set
//          for each triangle in triangulation do // first find all the triangles that are no longer valid due to the insertion
//             if point is inside circumcircle of triangle
//                add triangle to badTriangles
//          polygon := empty set
//          for each triangle in badTriangles do // find the boundary of the polygonal hole
//             for each edge in triangle do
//                if edge is not shared by any other triangles in badTriangles
//                   add edge to polygon
//          for each triangle in badTriangles do // remove them from the data structure
//             remove triangle from triangulation
//          for each edge in polygon do // re-triangulate the polygonal hole
//             newTri := form a triangle from edge to point
//             add newTri to triangulation
//       for each triangle in triangulation // done inserting points, now clean up
//          if triangle contains a vertex from original super-triangle
//             remove triangle from triangulation
//       return triangulation

    m_triangles.clear();

    Triangle<Point<F>, F> superTriangle = constructSuperTriangle();
    std::cout << "Super Triangle coordinates:" << std::endl;
    std::cout << superTriangle << std::endl;
    m_triangles.push_back(superTriangle);

    for (auto& p : m_points) {
        std::vector<Triangle<Point<F>, F>> badTriangles;
        std::vector<Edge<Point<F>>> edges;
        // find bad triangles
        for (auto t_it = m_triangles.begin(); t_it < m_triangles.end(); ++t_it) {
            if (t_it->isInCircle(p)) {
               badTriangles.push_back(*t_it);
               edges.push_back((t_it->getEdge1()));
               edges.push_back((t_it->getEdge2()));
               edges.push_back((t_it->getEdge3()));
               t_it->setIsBad(true);
            }
        }

        m_triangles.erase(std::remove_if(begin(m_triangles), end(m_triangles),
                                         [](Triangle<Point<F>, F>& t) {
                                            return t.getIsBad();
                                          }
                                        ), end(m_triangles));

        // find the boundary of the polygonal hole
        for (auto edges_it1 = edges.begin(); edges_it1 < edges.end(); ++edges_it1) {
            for (auto edges_it2 = edges.begin(); edges_it2 < edges.end(); ++edges_it2) {
            // if edges are not shared by any other triangles in badTriangles add edge to polygon -> remove all edges which are shared
                if (edges_it1 == edges_it2) {
                    continue;
                }

                if (*edges_it1 == *edges_it2) {
                    edges_it2->setBadEdge(true);
                }
            }
        }

        // for each edge in polygon do // re-triangulate the polygonal hole
       //             newTri := form a triangle from edge to point
       //             add newTri to triangulation
        for (auto edges_it = edges.begin(); edges_it < edges.end(); ++edges_it) {
            if (!(edges_it->getBadEdge())) {
                m_triangles.push_back(Triangle<Point<F>, F>(*edges_it, p));
            }
        }
    }

    //          if triangle contains a vertex from original super-triangle
    //             remove triangle from triangulation
    m_triangles.erase(std::remove_if(begin(m_triangles), end(m_triangles),
                                     [superTriangle](Triangle<Point<F>, F>& t) {
                                          return  t.getA() == superTriangle.getA() ||
                                                  t.getB() == superTriangle.getA() ||
                                                  t.getC() == superTriangle.getA() ||
                                                  t.getA() == superTriangle.getB() ||
                                                  t.getB() == superTriangle.getB() ||
                                                  t.getC() == superTriangle.getB() ||
                                                  t.getA() == superTriangle.getC() ||
                                                  t.getB() == superTriangle.getC() ||
                                                  t.getC() == superTriangle.getC();
                                      }
                                    ), end(m_triangles));
}

template <class F>
Triangle<Point<F>, F> Delaunay<F>::constructSuperTriangle()
{
    // Determinate the super triangle
    float minX = m_points.front().getX();
    float minY = m_points.front().getY();
    float maxX = minX;
    float maxY = minY;

    for (auto& t : m_points) {
        if (t.getX() < minX) minX = t.getX();
        if (t.getY() < minY) minY = t.getY();
        if (t.getX() > maxX) maxX = t.getX();
        if (t.getY() > maxY) maxY = t.getY();
    }

    F dx = maxX - minX;
    F dy = maxY - minY;
    F deltaMax = std::max(dx, dy);
    F midx = (minX + maxX)/2.0f;
    F midy = (minY + maxY)/2.0f;

    Point<F> p1(midx - 20 * deltaMax, midy - deltaMax);
    Point<F> p2(midx, midy + 20 * deltaMax);
    Point<F> p3(midx + 20 * deltaMax, midy - deltaMax);

    return Triangle<Point<F>, F>(p1, p2, p3);
}
