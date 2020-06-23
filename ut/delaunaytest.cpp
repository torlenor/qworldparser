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

#include <catch.hpp>

#include <delaunay.hpp>

TEST_CASE( "Delaunay Class tests", "[delaunay]" ) {
    SECTION("Delaunay triangulation - one triangle") {
        std::vector<Point<float>> points { {0.0f, 0.0f, 0},
                                           {0.0f, 1.0f, 1},
                                           {1.0f, 1.0f, 2}
                                         };

        Delaunay<float> delaunay(points);

        delaunay.triangulate();

        auto triangles = delaunay.getTriangles();

        REQUIRE(triangles.size() == 1);

        REQUIRE( triangles[0].getB() == Point<float>(0.0f, 0.0f) );
        REQUIRE( triangles[0].getA() == Point<float>(0.0f, 1.0f) );
        REQUIRE( triangles[0].getC() == Point<float>(1.0f, 1.0f) );

        REQUIRE( triangles[0].getB().getId() == 0 );
        REQUIRE( triangles[0].getA().getId() == 1 );
        REQUIRE( triangles[0].getC().getId() == 2 );
    }

    SECTION("Delaunay triangulation - two triangles") {
        std::vector<Point<float>> points { {0,0},
                                           {1,0},
                                           {0,1},
                                           {1,1}
                                         };

        Delaunay<float> delaunay(points);

        delaunay.triangulate();

        auto triangles = delaunay.getTriangles();

        REQUIRE(triangles.size() == 2);

        REQUIRE( triangles[0].getA() == Point<float>(0.0f, 0.0f) );
        REQUIRE( triangles[0].getB() == Point<float>(0.0f, 1.0f) );
        REQUIRE( triangles[0].getC() == Point<float>(1.0f, 1.0f) );

        REQUIRE( triangles[1].getB() == Point<float>(0.0f, 0.0f) );
        REQUIRE( triangles[1].getA() == Point<float>(1.0f, 0.0f) );
        REQUIRE( triangles[1].getC() == Point<float>(1.0f, 1.0f) );
    }
}
