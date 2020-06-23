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

#include <memory>

#include <triangle.hpp>

TEST_CASE( "Triangle Class tests", "[triangle]" ) {
    SECTION("Create a Triangle") {
        Triangle<Point<float>, float> tri1;
        REQUIRE( tri1.getA() == Point<float>(0.0f, 0.0f) );
        REQUIRE( tri1.getB() == Point<float>(1.0f, 0.0f) );
        REQUIRE( tri1.getC() == Point<float>(0.0f, 1.0f) );
    }

    SECTION("Get Triangle Circumcenter") {
        Triangle<Point<float>, float> tri1 { {0,0}, {1,0}, {0,1} };

        REQUIRE( tri1.getCircumCenter().getX() == Approx(0.5f) );
        REQUIRE( tri1.getCircumCenter().getY() == Approx(0.5f) );
    }

    SECTION("Get Triangle Circumradius") {
        Triangle<Point<float>, float> tri1 { {0,0}, {1,0}, {0,1} };

        REQUIRE( tri1.getCircumRadius() == Approx(0.707f).epsilon(0.01));
    }

    SECTION("Check Point in Circumcircle") {
        Triangle<Point<float>, float> tri1 { {0,0}, {1,0}, {0,1} };
        Point<float> p1 {0.5, 0.5};
        Point<float> p2 {2.0, 0.0};
        Point<float> p3 {0.0, 2.0};
        Point<float> p4 {0.01, 0.01}; // slightly inside
        Point<float> p5 {-0.01, -0.01}; // slightly outside

        REQUIRE( tri1.isInCircle(p1) );
        REQUIRE( not(tri1.isInCircle(p2)) );
        REQUIRE( not(tri1.isInCircle(p3)) );
        REQUIRE( tri1.isInCircle(p4) );
        REQUIRE( not(tri1.isInCircle(p5)) );
    }

    SECTION("Create a Triangle from a PointPtr") {
        PointPtr<float> A = std::make_shared<Point<float>>(0.5, 0.5);
        PointPtr<float> B = std::make_shared<Point<float>>(2.0, 0.0);
        PointPtr<float> C = std::make_shared<Point<float>>(0.0, 2.0);

        Triangle<PointPtr<float>, float> tri1(A, B, C);

        REQUIRE( tri1.getA() == A );
        REQUIRE( tri1.getB() == B );
        REQUIRE( tri1.getC() == C );

        REQUIRE( *tri1.getA() == Point<float>(0.5, 0.5) );
        REQUIRE( *tri1.getB() == Point<float>(2.0, 0.0) );
        REQUIRE( *tri1.getC() == Point<float>(0.0, 2.0) );
    }
}
