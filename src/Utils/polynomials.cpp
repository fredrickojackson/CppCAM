/* -*- coding: utf-8 -*-

Copyright 2008-2014 Lode Leroy

This file is part of CppCAM.

CppCAM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

CppCAM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with CppCAM.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "polynomials.h"

#include <cmath>
#include <algorithm>

#if TESTPOLY

#include <iostream>

std::ostream& operator<<(std::ostream& os, const std::vector<double>& v) {
    for(size_t i=0; i<v.size(); i++) {
        if (i>0) os << ", ";
        os << v[i];
    }
    return os;
}

#endif

// see BRL-CAD/src/libbn/poly.c

const double EPSILON = 1e-4;
const double SMALL = 1e-4;
const double INV_2 = 0.5;
const double INV_3 = 1.0 / 3.0;
const double INV_4 = 0.25;
const double INV_27 = 1.0 / 27.0;
const double SQRT3 = sqrt(3.0);
const double PI_DIV_3 = M_PI / 3.0;

bool near_zero(double x, double epsilon=EPSILON)
{
    if (x < epsilon && x > -epsilon)
        return true;
    else
        return false;
}
    
double cuberoot(double x) 
{
    if (x == 0) 
        return 0;
    if (x == 1)
        return 1;
    if (x == -1)
        return -1;
    if (x >= 0)
        return pow(x, INV_3);
    else
        return -pow(-x, INV_3);
}

std::vector<double> 
poly1_roots(double a, double b)
{
    if (near_zero(a))
    {
        return std::vector<double>();
    }
    std::vector<double> roots;
    roots.push_back(-b / a);
    return roots;
}

std::vector<double> 
poly2_roots(double a, double b, double c)
{
    std::vector<double> roots;
   
    double d = b * b - 4 * a * c;
    if (d < 0) {
        return roots;
    }
    if (near_zero(a)) {
        return poly1_roots(b, c);
    }
    if (d == 0) {
        roots.push_back(-b / (2 * a));
        return roots;
    }
    double q = sqrt(d);
    if (a < 0) {
        roots.push_back((-b + q) / (2 * a));
        roots.push_back((-b - q) / (2 * a));
    }
    else {
        roots.push_back((-b - q) / (2 * a));
        roots.push_back((-b + q) / (2 * a));
    }
    return roots;
}

std::vector<double> 
poly3_roots(double f0, double f1, double f2, double f3)
{
    if (near_zero(f0)) {
        return poly2_roots(f1, f2, f3);
    }
    std::vector<double> roots;

    double c1 = f1 / f0;
    double c2 = f2 / f0;
    double c3 = f3 / f0;
    
    double c1_3 = c1 * INV_3;
    double a = c2 - c1 * c1_3;
    double b = (2 * c1 * c1 * c1 - 9 * c1 * c2 + 27 * c3) * INV_27;
    double delta = a * a;
    delta = b * b * INV_4 + delta * a * INV_27;
    if (delta > EPSILON) 
    {
        double r_delta = sqrt(delta);
        double A = -INV_2 * b + r_delta;
        double B = -INV_2 * b - r_delta;
        A = cuberoot(A);
        B = cuberoot(B);
        roots.push_back(A + B - c1_3);
        return roots;
    }
    else if (near_zero(delta))
    {
        double b_2 = -b * INV_2;
        double s = cuberoot(b_2);
        roots.push_back(2 * s - c1_3);
        roots.push_back(-s - c1_3);
        roots.push_back(-s - c1_3);
        return roots;
    }
    
    double fact;
    double phi;
    double cs_phi;
    double sn_phi_s3;
    if (a >= 0) {
        fact = 0;
        phi = 0;
        cs_phi = 1.0;
        sn_phi_s3 = 0.0;
    } else {
        a *= -INV_3;
        fact = sqrt(a);
        double f = -b * INV_2 / (a * fact);
        if (f >= 1.0) {
            phi = 0;
            cs_phi = 1.0;
            sn_phi_s3 = 0.0;
        } else if (f <= -1.0) {
            phi = PI_DIV_3;
            cs_phi = cos(phi);
            sn_phi_s3 = sin(phi) * SQRT3;
        } else {
            phi = acos(f) * INV_3;
            cs_phi = cos(phi);
            sn_phi_s3 = sin(phi) * SQRT3;
        }
    }
    double r1 = 2 * fact * cs_phi;
    double r2 = fact * ( sn_phi_s3 - cs_phi);
    double r3 = fact * (-sn_phi_s3 - cs_phi);
    roots.push_back(r1 - c1_3);
    roots.push_back(r2 - c1_3);
    roots.push_back(r3 - c1_3);
    return roots;
}

std::vector<double> 
poly4_roots(double f0, double f1, double f2, double f3, double f4)
{
    if (near_zero(f0)) {
        return poly3_roots(f1, f2, f3, f4);
    }
    double c1 = f1 / f0;
    double c2 = f2 / f0;
    double c3 = f3 / f0;
    double c4 = f4 / f0;
    std::vector<double> roots3 = poly3_roots(1.0, -c2, c3 * c1 - 4 * c4, -c3 * c3 - c4 * c1 * c1 + 4 * c4 * c2);
    if (roots3.size() == 0) {
        return std::vector<double>();
    }
    double U;
    double p;
    double q;
    if (roots3.size() == 1) {
        U = roots3[0];
    } else {
        U = std::max(roots3[0], std::max(roots3[1], roots3[2]));
    }
    p = c1 * c1 * INV_4 + U - c2;
    U *= INV_2;
    q = U * U - c4;

    if (p < 0) {
        if (p < -SMALL) {
            return std::vector<double>();
        }
        p = 0;
    } else {
        p = sqrt(p);
    }
    if (q < 0) {
        if (q < -SMALL) {
            return std::vector<double>();
        }
        q = 0;
    } else {
        q = sqrt(q);
    }

    std::vector<double> roots;
    std::vector<double> quad1; quad1.push_back(1.0); quad1.push_back(c1 * INV_2 - p); quad1.push_back(0);
    std::vector<double> quad2; quad2.push_back(1.0); quad2.push_back(c1 * INV_2 + p); quad2.push_back(0);

    double q1 = U - q;
    double q2 = U + q;
    p = quad1[1] * q2 + quad2[1] * q1 - c3;
    if (near_zero(p)) {
        quad1[2] = q1;
        quad2[2] = q2;
    } else {
        double q = quad1[1] * q1 + quad2[1] * q2 - c3;
        if (near_zero(q)) {
            quad1[2] = q2;
            quad2[2] = q1;
        } else {
            return std::vector<double>();
        }
    }
    //print "f1(x)=%g*x**2%+g*x%+g" % (quad1[0], quad1[1], quad1[2])
    //print "f2(x)=%g*x**2%+g*x%+g" % (quad2[0], quad2[1], quad2[2])
    std::vector<double> roots1 = poly2_roots(quad1[0], quad1[1], quad1[2]);
    std::vector<double> roots2 = poly2_roots(quad2[0], quad2[1], quad2[2]);
    if (roots1.size()>0 && roots2.size()>0) {
        std::vector<double> roots;
        roots.resize(roots1.size()+roots2.size());
        std::merge(roots1.begin(), roots1.end(), roots2.begin(), roots2.end(), roots.begin());
        return roots;
    } else if (roots1.size()>0) {
        return roots1;
    } else if (roots2.size()>0) {
        return roots2;
    } else {
        return std::vector<double>();
    }
}

#if TESTPOLY

void test_poly1(double a, double b)
{
    std::vector<double> roots = poly1_roots(a, b);
    std::cout << a << "*x" << (b>=0?"+":"") <<  b<< "=0 : " << roots <<std::endl;
    if (roots.size()>0) {
        for(size_t i=0; i<roots.size(); i++) {
            double r = roots[i];
            double f = a * r + b;
            if (!near_zero(f)) {
                std::cout << "ERROR:"
                          << "    f(" << r << ")=" << f 
                          << std::endl;
            }
        }
    }
}

void test_poly2(double a, double b, double c) 
{
    std::vector<double> roots = poly2_roots(a, b, c);
    std::cout << a << "*x^2" << (b>=0?"+":"") << b << "*x" << (c>=0?"+":"") << c << "=0 : " << roots <<std::endl;
    if (roots.size()>0) {
        for(size_t i=0; i<roots.size(); i++) {
            double r = roots[i];
            double f = a * r * r + b * r + c;
            if (!near_zero(f)) {
                std::cout << "ERROR:"
                          << "    f(" << r << ")=" << f 
                          << std::endl;
            }
        }
    }
}

void test_poly3(double a, double b, double c, double d)
{
    std::vector<double> roots = poly3_roots(a, b, c, d);
    std::cout << a << "*x^3" << (b>=0?"+":"") << b << "*x^2" << (c>=0?"+":"") << c << "*x" << (d>=0?"+":"") << d << "=0 : " << roots <<std::endl;
    if (roots.size()>0) {
        for(size_t i=0; i<roots.size(); i++) {
            double r = roots[i];
            double f = a * r * r * r + b * r * r + c * r + d;
            if (!near_zero(f)) {
                std::cout << "ERROR:"
                          << "    f(" << r << ")=" << f 
                          << std::endl;
            }
        }
    }
}

void test_poly4(double a, double b, double c, double d, double e)
{
    std::vector<double> roots = poly4_roots(a, b, c, d, e);
    std::cout << a << "*x^4" << (b>=0?"+":"") << b << "*x^3" << (c>=0?"+":"") << c << "*x^2" << (d>=0?"+":"") << d << "*x" << (e>=0?"+":"") <<  e << "=0 : " << roots <<std::endl;
    if (roots.size()>0) {
        for(size_t i=0; i<roots.size(); i++) {
            double r = roots[i];
            double f = a * r * r * r * r + b * r * r * r + c * r * r + d * r + e;
            if (!near_zero(f, SMALL)) {
                std::cout << "ERROR:"
                          << "    f(" << r << ")=" << f 
                          << std::endl;
            }
        }
    }
}

int main(int argc, char* argv[])
{
#if 1
    test_poly1(1, 2);
#endif
#if 1
    test_poly2(1, 2, 0);
    test_poly2(1, 2, 1);
    test_poly2(1, 2, 2);
#endif
#if 1
    test_poly3(1, 0, 0, 0);
    test_poly3(1, 0, 0, -1);
    test_poly3(1, -1, 0, 0);
    test_poly3(1, 0, -2, 0);
    test_poly3(1, 0, -2, 1);
#endif
#if 1
    test_poly4(1, 0, 0, 0, 0);
    test_poly4(1, 0, 0, 0, -1);
    test_poly4(1, 0, -2, 0, 1);
    test_poly4(1, -10, 35, -50, +24);
    test_poly4(1, 0, 6, -60, 36);
    test_poly4(1, -25, 235.895, -995.565, 1585.25);
#endif
    return 0;
}

#endif

