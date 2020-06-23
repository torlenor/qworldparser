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

#include <edge.hpp>

TEST_CASE( "Edge Class tests", "[edge]" ) {
    SECTION("Create an Edge") {
        Edge<Point<float>> e1;
        REQUIRE( e1.getP1().getX() == 0.0f );
        REQUIRE( e1.getP1().getY() == 0.0f );
        REQUIRE( e1.getP2().getX() == 1.0f );
        REQUIRE( e1.getP2().getY() == 1.0f );

        Edge<Point<float>> e2(Point<float>(2.0, 3.5), Point<float>(3.0, 2.5));
        REQUIRE( e2.getP1().getX() == 2.0f );
        REQUIRE( e2.getP1().getY() == 3.5f );
        REQUIRE( e2.getP2().getX() == 3.0f );
        REQUIRE( e2.getP2().getY() == 2.5f );

        e2.setP1(Point<float>(4.0, 3.0));
        e2.setP2(Point<float>(-1.0, -2.0));

        REQUIRE( e2.getP1().getX() == 4.0f );
        REQUIRE( e2.getP1().getY() == 3.0f );
        REQUIRE( e2.getP2().getX() == -1.0f );
        REQUIRE( e2.getP2().getY() == -2.0f );
    }

    SECTION("Compare two Edges") {
        Edge<Point<float>> e1(Point<float>(2.0, 3.5), Point<float>(3.0, 2.5));
        Edge<Point<float>> e2(Point<float>(2.0, 3.5), Point<float>(3.0, 2.5));
        Edge<Point<float>> e3(Point<float>(4.0, 3.5), Point<float>(3.0, 2.5));

        Edge<Point<float>> e4(Point<float>(3.0, 2.5), Point<float>(2.0, 3.5));

        REQUIRE(e1 == e2);
        REQUIRE( not(e1 == e3) );
        REQUIRE(e1 == e4);
    }

        SECTION("Create an Edge from PointPTr") {
            PointPtr<float> A = std::make_shared<Point<float> >(2.0, 3.5);
            PointPtr<float> B = std::make_shared<Point<float> >(3.0, 2.5);

            Edge<PointPtr<float>> e1(A, B);

            REQUIRE( e1.getP1()->getX() == 2.0f );
            REQUIRE( e1.getP1()->getY() == 3.5f );
            REQUIRE( e1.getP2()->getX() == 3.0f );
            REQUIRE( e1.getP2()->getY() == 2.5f );
        }

        SECTION("Compare two Edges made from PointPtr") {
            PointPtr<float> A = std::make_shared<Point<float> >(2.0, 3.5);
            PointPtr<float> B = std::make_shared<Point<float> >(3.0, 2.5);
            PointPtr<float> C = std::make_shared<Point<float> >(4.0, 3.5);

            Edge<PointPtr<float>> e1(A, B);
            Edge<PointPtr<float>> e2(A, B);
            Edge<PointPtr<float>> e3(C, B);

            Edge<PointPtr<float>> e4(B, A);

            REQUIRE(e1 == e2);
            REQUIRE( not(e1 == e3) );
            REQUIRE(e1 == e4);
        }
}
