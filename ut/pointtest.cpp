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

#include <point.hpp>

TEST_CASE( "Point Class tests", "[point]" ) {
    SECTION("Create a Point") {
        Point<float> A;
        REQUIRE( A.getX() == 0.0f );
        REQUIRE( A.getY() == 0.0f );

        Point<float> B(-3.4f, 624.0f);
        REQUIRE( B.getX() == -3.4f );
        REQUIRE( B.getY() == 624.0f );

        A.setX(-4.0f);
        A.setY(52.0f);
        REQUIRE( A.getX() == -4.0f );
        REQUIRE( A.getY() == 52.0f );
    }

    SECTION("Point A + Point B") {
        Point<float> A(-10.2f, 4.0f);
        Point<float> B(8.0f, -2.9f);
        Point<float> C = A + B;

        REQUIRE( C.getX() == ( -10.2f + 8.0f ) );
        REQUIRE( C.getY() == ( 4.0f + -2.9f ) );
    }
    SECTION("Point A - Point B") {
        Point<float> A(-10.2f, 4.0f);
        Point<float> B(8.0f, -2.9f);
        Point<float> C = A - B;

        REQUIRE( C.getX() == ( -10.2f - 8.0f ) );
        REQUIRE( C.getY() == ( 4.0f - -2.9f ) );
    }
    SECTION("Dot Product of two Points") {
        Point<float> A(-10.2f, 4.0f);
        Point<float> B(8.0f, -2.9f);

        REQUIRE( A.dot(B) == ( -81.6f + -11.6f ) );
    }
    SECTION("Distance between two Points") {
        Point<float> A(-10.2f, 4.0f);
        Point<float> B(8.0f, -2.9f);

        REQUIRE( A.distance(B) == Approx(sqrt( (8.0f - -10.2f)*(8.0f - -10.2f) + (4.0f - -2.9f)*(4.0f - -2.9f) ) ));
    }

    SECTION("Comparison of two Points") {
        Point<float> A(-10.2f, 4.0f);
        Point<float> B(-10.2f, 4.0f);
        Point<float> C(8.0f, -2.9f);

        REQUIRE( A == B );
        REQUIRE( not(A == C) );
    }

    SECTION("Usage of PointPtr") {
        PointPtr<float> A = std::make_shared<Point<float>>(-10.2f, 4.0f);
        PointPtr<float> B = std::make_shared<Point<float>>(-10.2f, 4.0f);
        PointPtr<float> C = std::make_shared<Point<float>>(8.0f, -2.9f);
        PointPtr<float> alsoA = A;

        REQUIRE( *A == *B );
        REQUIRE( not(*A == *C) );

        REQUIRE( not(A == B) );
        REQUIRE( not(A == C) );
        REQUIRE( A == alsoA );
    }

    SECTION("Get and set an ID") {
        PointPtr<float> A = std::make_shared<Point<float>>(-10.2f, 4.0f);
        PointPtr<float> B = std::make_shared<Point<float>>(-10.2f, 4.0f);
        PointPtr<float> C = std::make_shared<Point<float>>(8.0f, -2.9f, 3);
        PointPtr<float> alsoA = A;

        REQUIRE( A->getId() == 0 );
        REQUIRE( B->getId() == 0 );
        REQUIRE( C->getId() == 3 );

        A->setId(1);
        B->setId(2);

        REQUIRE( A->getId() == 1 );
        REQUIRE( B->getId() == 2 );
        REQUIRE( C->getId() == 3 );
        REQUIRE( A->getId() == alsoA->getId() );
    }
}
