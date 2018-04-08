#ifndef __INTERSECTION_H
#define __INTERSECTION_H

/* -*- coding: utf-8 -*-

Copyright 2008-2014 Lode Leroy

This file is part of PyCAM.

PyCAM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

PyCAM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with PyCAM.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Geometry/Geometry.h"
#include "Utils/polynomials.h"

struct Line
{
    Line(const Point& p1, const Point& p2) :
        m_p1(p1),m_p2(p2)
    {
        Point d = m_p2.sub(m_p1);
        m_dir = d.normalized();
        m_len = d.norm();
    }
    Line(const Edge& e) :
        m_p1(e.p1()),m_p2(e.p2())
    {
        Point d = m_p2.sub(m_p1);
        m_dir = d.normalized();
        m_len = d.norm();
    }
    const Point& p1() const { return m_p1; }
    const Point& p2() const { return m_p2; }
    const Point dir() const { return m_dir; }
    double len() const { return m_len; }
    Point m_p1;
    Point m_p2;
    Point m_dir;
    double m_len;
};

struct Plane
{
    Plane(const Point& p, const Point& n) :
        m_p(p),m_n(n.normalized()) {}
    Plane(const Triangle& t) :
        m_p(t.p1()),m_n(t.n().normalized()) {}
    const Point& p() const { return m_p; }
    const Point& n() const { return m_n; }
    Point m_p;
    Point m_n;
};

class Intersection
{
public:
    static bool
    intersect_lines(double xl, double zl, double nxl, double nzl, double xm, double zm, double nxm, double nzm, double& X, double &Z)
    {
        X = 0;
        Z = 0;
        //try
        {
            if (isZero(nzl) && isZero(nzm)) {
                return true;
            }
            if (isZero(nzl) || isZero(nxl)) {
                X = xl;
                Z = zm + (xm - xl) * nxm / nzm;
                return true;
            }
            if (isZero(nzm) || isZero(nxm)) {
                X = xm;
                Z = zl - (xm - xl) * nxl / nzl;
                return true;
            }

            X = (zl - zm +(xm * nxm / nzm - xl * nxl / nzl))
                / (nxm / nzm - nxl / nzl);
            if (xl < X && X < xm) {
                Z = zl + (X -xl) * nxl / nzl;
                return true;
            }
            //} catch (...) {
        }
        return false;
    }

    static bool
    edge_closest_point(const Line& edge, const Point& point,
                       Point& cp_out)
    {
        Point v = edge.dir();
        if (isZero(v.normsq()))
        {
            cp_out = edge.p1();
            return true;
        }
        double l = edge.p1().dot(v) - point.dot(v);
        cp_out = edge.p1().sub(v.mul(l));
        return true;
    }

    static bool
    point_inside_triangle(const Point& point, const Triangle& triangle)
    {
        // http://www.blackpawn.com/texts/pointinpoly/default.html
        // Compute vectors
        Point v0 = triangle.p3().sub(triangle.p1());
        Point v1 = triangle.p2().sub(triangle.p1());
        Point v2 = point.sub(triangle.p1());
        // Compute dot products
        double dot00 = v0.dot(v0);
        double dot01 = v0.dot(v1);
        double dot02 = v0.dot(v2);
        double dot11 = v1.dot(v1);
        double dot12 = v1.dot(v2);
        // Compute barycentric coordinates
        double denom = dot00 * dot11 - dot01 * dot01;
        if (isZero(denom)) {
            return false;
        }
        double invDenom = 1.0 / denom;
        double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
        //  Check if point is in triangle
        return (u >= 0) && (v >= 0) && (u + v <= 1);
    }

    static bool
    intersect_plane_point(const Plane& plane, const Point& _direction, const Point& point,
                          Point& cp_out, double& l_out)
    {
        Point direction(_direction);
        // calculations will go wrong, if the direction is not a unit vector
        if (isZero(direction.normsq())){
            l_out = INFINITE;
            return false;
        }
        direction.normalize();
        double denom = plane.n().dot(direction);
        if (isZero(denom)) {
            l_out = INFINITE;
            return false;
        }
        double l = -(plane.n().dot(point) - plane.n().dot(plane.p())) / denom;
        cp_out = point.add(direction.mul(l));
        l_out = l;
        return true;
    }

    static bool
    intersect_cylinder_point(const Point& center, const Point& axis, double radius, double radiussq, const Point& direction, const Point& point,
                             Point& ccp_out, Point& cp_out, double& l_out)
    {
        // take a plane along direction and axis
        Point n = direction.cross(axis).normalized();
        // distance of the point to this plane
        double d = n.dot(point) - n.dot(center);
        if (fabs(d) > radius - epsilon)
        {
            l_out = INFINITE;
            return false;
        }
        // ccl is on cylinder
        double d2 = sqrt(radiussq-d*d);
        Point ccl = center.add(n.mul(d)).add(direction.mul(d2));
        // take plane through ccl and axis
        Plane plane = Plane(ccl, direction);
        // intersect point with plane

        Point ccp;
        double l;
        if (intersect_plane_point(plane, direction, point, ccp, l)) {
            ccp_out = ccp;
            cp_out = point;
            l_out = -l;
            return true;
        }
        l_out = INFINITE;
        return false;
    }

    static bool
    intersect_cylinder_line(const Point& center, const Point& axis, double radius, double /*radiussq*/, const Point& direction, const Line& edge,
                            Point& ccp_out, Point& cp_out, double& l_out)
    {
        Point d = edge.dir();
        // take a plane throught the line and along the cylinder axis (1)
        Point n = d.cross(axis);
        if (isZero(n.normsq())) {
            // no contact point, but should check here if cylinder *always*
            // intersects line...
            l_out = INFINITE;
            return false;
        }

        n.normalize();
        // the contact line between the cylinder and this plane (1)
        // is where the surface normal is perpendicular to the plane
        // so line := ccl + \lambda * axis
        Point ccl;
        if (n.dot(direction) < 0) {
            ccl = center.sub(n.mul(radius));
        } else {
            ccl = center.add(n.mul(radius));
        }
        // now extrude the contact line along the direction, this is a plane (2)
        Point n2 = direction.cross(axis);
        if (isZero(n2.normsq())) {
            // no contact point, but should check here if cylinder *always*
            // intersects line...
            l_out = INFINITE;
            return false;
        }
        n2.normalize();
        Plane plane1 = Plane(ccl, n2);
        // intersect this plane with the line, this gives us the contact point
        Point cp;
        double l;
        if (!intersect_plane_point(plane1, d, edge.p1(), cp, l)) {
            l_out = INFINITE;
            return false;
        }
        // now take a plane through the contact line and perpendicular to the
        // direction (3)
        Plane plane2 = Plane(ccl, direction);
        // the intersection of this plane (3) with the line through the contact point
        // gives us the cutter contact point
        Point ccp;
        intersect_plane_point(plane2, direction, cp, ccp, l);
        ccp_out = ccp;
        cp_out = ccp_out.add(direction.mul(-l));
        l_out = -l;
        return true;
    }

    static bool
    intersect_circle_plane(const Point& center, double radius, const Point& direction, const Plane& plane,
                           Point& ccp_out, Point& cp_out, double& l_out)
    {
        // let n be the normal to the plane
        Point n = plane.n();
        if (isZero(n.dot(direction))) {
            l_out = INFINITE;
            return false;
        }
        // project onto z=0
        Point n2 = Point(n.x(), n.y(), 0);
        if (isZero(n2.normsq())) {
            Point cp;
            double d;
            if (intersect_plane_point(plane, direction, center, cp, d)) {
                cp_out = cp;
                ccp_out = cp.sub(direction.mul(d));
                l_out = d;
                return true;
            }
            l_out = INFINITE;
            return false;
        }
        n2.normalize();
        // the cutter contact point is on the circle, where the surface normal is n
        Point ccp = center.add(n2.mul(-radius));
        // intersect the plane with a line through the contact point
        Point cp;
        double d;
        if (intersect_plane_point(plane, direction, ccp, cp, d)) {
            ccp_out = ccp;
            cp_out = cp;
            l_out = d;
            return true;
        }
        l_out = INFINITE;
        return false;
    }

    static bool
    intersect_circle_point(const Point& center, const Point& axis, double /*radius*/, double radiussq, const Point& direction, const Point& point,
                           Point& ccp_out, Point& cp_out, double& l_out)
    {
        // take a plane through the base
        Plane plane = Plane(center, axis);
        // intersect with line gives ccp
        Point ccp;
        double l;
        if (intersect_plane_point(plane, direction, point, ccp, l)) {
            // check if inside circle
            if (center.sub(ccp).normsq() < radiussq - epsilon) {
                ccp_out = ccp;
                cp_out = point;
                l_out = -l;
                return true;
            }
        }
        l_out = INFINITE;
        return false;
    }

    static bool
    intersect_circle_line(const Point& center, const Point& axis, double /*radius*/, double radiussq, const Point& direction, const Line& edge,
                          Point& ccp_out, Point& cp_out, double& l_out)
    {
        // make a plane by sliding the line along the direction (1)
        Point d = edge.dir();
        if (isZero(d.dot(axis))) {
            if (isZero(direction.dot(axis))) {
                l_out = INFINITE;
                return false;
            }
            Plane plane = Plane(center, axis);
            Point p1;
            double l;
            intersect_plane_point(plane, direction, edge.p1(), p1, l);
            Point p2;
            intersect_plane_point(plane, direction, edge.p2(), p2, l);
            Point pc;
            edge_closest_point(Line(p1, p2), center, pc);
            double d_sq = pc.sub(center).normsq();
            if (d_sq > radiussq) {
                l_out = INFINITE;
                return false;
            }
            double a = sqrt(radiussq - d_sq);
            double d1 = p1.sub(pc).dot(d);
            double d2 = p2.sub(pc).dot(d);
            if (fabs(d1) < a - epsilon) {
                ccp_out = p1;
                cp_out = p1.sub(direction.mul(l));
                l_out = -l;
                return true;
            }
            else if (fabs(d2) < a - epsilon) {
                ccp_out = p2;
                cp_out = p2.sub(direction.mul(l));
                l_out = -l;
                return true;
            } else if (((d1 < -a + epsilon) && (d2 > a - epsilon))
                       || ((d2 < -a + epsilon) && (d1 > a - epsilon))) {
                ccp_out = pc;
                cp_out = pc.sub(direction.mul(l));
                l_out = -l;
                return true;
            }
            l_out = INFINITE;
            return false;
        }
        Point n = d.cross(direction);
        if (isZero(n.normsq())) {
            // no contact point, but should check here if circle *always* intersects
            // line...
            l_out = INFINITE;
            return false;
        }
        n.normalize();
        // take a plane through the base
        Plane plane = Plane(center, axis);
        // intersect base with line
        Point lp;
        double l;
        if (!intersect_plane_point(plane, d, edge.p1(), lp, l)) {
            l_out = INFINITE;
            return false;
        }
        // intersection of 2 planes: lp + \lambda v
        Point v = axis.cross(n);
        if (isZero(n.normsq())) {
            l_out = INFINITE;
            return false;
        }
        v.normalize();
        // take plane through intersection line and parallel to axis
        Point n2 = v.cross(axis);
        if (isZero(n2.normsq())) {
            l_out = INFINITE;
            return false;
        }
        n2.normalize();
        // distance from center to this plane
        double dist = n2.dot(center) - n2.dot(lp);
        double distsq = dist * dist;
        if (distsq > radiussq - epsilon) {
            l_out = INFINITE;
            return false;
        }
        // must be on circle
        double dist2 = sqrt(radiussq - distsq);
        if (d.dot(axis) < 0) {
            dist2 = -dist2;
        }
        Point ccp = center.sub(n2.mul(dist)).sub(v.mul(dist2));
        plane = Plane(edge.p1(), d.cross(direction).cross(d));
        Point cp;
        if (intersect_plane_point(plane, direction, ccp, cp, l)) {
            ccp_out = ccp;
            cp_out = cp;
            l_out = l;
            return true;
        }
        l_out = INFINITE;
        return false;
    }

    static bool
    intersect_sphere_plane(const Point& center, double radius, const Point& direction, const Plane& plane,
                           Point& ccp_out, Point& cp_out, double& l_out)
    {
        // let n be the normal to the plane
        Point n = plane.n();
        if (isZero(n.dot(direction))) {
            l_out = INFINITE;
            return false;
        }
        Point ccp;
        // the cutter contact point is on the sphere, where the surface normal is n
        if (n.dot(direction) < 0) {
            ccp = center.sub(n.mul(radius));
        } else {
            ccp = center.add(n.mul(radius));
        }
        Point cp;
        double d;
        // intersect the plane with a line through the contact point
        if (intersect_plane_point(plane, direction, ccp, cp, d)) {
            ccp_out = ccp;
            cp_out = cp;
            l_out = d;
            return true;
        }
        l_out = INFINITE;
        return false;
    }

    static bool
    intersect_sphere_point(const Point& center, double /*radius*/, double radiussq, const Point& direction, const Point& point,
                           Point& ccp_out, Point& cp_out, double& l_out)
    {
        // line equation
        // (1) x = p_0 + \lambda * d
        // sphere equation
        // (2) (x-x_0)^2 = R^2
        // (1) in (2) gives a quadratic in \lambda
        Point p0_x0 = center.sub(point);
        double a = direction.normsq();
        double b = 2 * p0_x0.dot(direction);
        double c = p0_x0.normsq() - radiussq;
        double d = b * b - 4 * a * c;
        if (d < 0) {
            l_out = INFINITE;
            return false;
        }
        double l;
        if (a < 0) {
            l = (-b + sqrt(d)) / (2 * a);
        } else {
            l = (-b - sqrt(d)) / (2 * a);
        }
        // cutter contact point
        ccp_out = point.add(direction.mul(-l));
        cp_out = point;
        l_out = l;
        return true;
    }

    static bool
    intersect_sphere_line(const Point& center, double radius, double radiussq, const Point& direction, const Line& edge,
                          Point& ccp_out, Point& cp_out, double& l_out)
    {
        // make a plane by sliding the line along the direction (1)
        Point d = edge.dir();
        Point n = d.cross(direction);
        if (isZero(n.normsq())) {
            // no contact point, but should check here if sphere *always* intersects
            // line...
            l_out = INFINITE;
            return false;
        }
        n.normalize();

        // calculate the distance from the sphere center to the plane
        double dist = - center.dot(n) + edge.p1().dot(n);
        if (fabs(dist) > radius - epsilon) {
            l_out = INFINITE;
            return false;
        }
        // this gives us the intersection circle on the sphere

        // now take a plane through the edge and perpendicular to the direction (2)
        // find the center on the circle closest to this plane

        // which means the other component is perpendicular to this plane (2)
        Point n2 = n.cross(d).normalized();

        // the contact point is on a big circle through the sphere...
        double dist2 = sqrt(radiussq - dist * dist);

        // ... and it's on the plane (1)
        Point ccp = center.add(n.mul(dist)).add(n2.mul(dist2));

        // now intersect a line through this point with the plane (2)
        Plane plane = Plane(edge.p1(), n2);
        Point cp;
        double l;
        if (intersect_plane_point(plane, direction, ccp, cp, l)) {
            ccp_out = ccp;
            cp_out = cp;
            l_out = l;
            return true;
        }
        l_out = INFINITE;
        return false;
    }

    static bool
    intersect_torus_plane(const Point& center, const Point& axis, double majorradius, double minorradius,
                          const Point& direction, const Plane& plane,
                          Point& ccp_out, Point& cp_out, double& l_out)
    {
        // take normal to the plane
        Point n = plane.n();
        if (isZero(n.dot(direction))) {
            l_out = INFINITE;
            return false;
        }
        if (n.dot(axis) == 1) {
            l_out = INFINITE;
            return false;
        }
        // find place on torus where surface normal is n
        Point b = n.mul(-1);
        Point z = axis;
        Point a = b.sub(z.mul(z.dot(b)));
        double a_sq = a.normsq();
        if (a_sq <= 0) {
            l_out = INFINITE;
            return false;
        }
        a = a.div(sqrt(a_sq));
        Point ccp = center.add(a.mul(majorradius)).add(b.mul(minorradius));
        // find intersection with plane
        Point cp;
        double l;
        if (intersect_plane_point(plane, direction, ccp, cp, l)) {
            ccp_out = ccp;
            cp_out = cp;
            l_out = l;
            return true;
        }
        l_out = INFINITE;
        return false;
    }

    static bool
    intersect_torus_point(const Point& center, const Point& axis, double majorradius, double minorradius, double majorradiussq,
                          double minorradiussq, const Point& direction, const Point& point,
                          Point& ccp_out, Point& cp_out, double& l_out)
    {
        if (isZero(direction.x()) && (isZero(direction.y()))) {
            // drop
            double minlsq = sqr(majorradius - minorradius);
            double maxlsq = sqr(majorradius + minorradius);
            double l_sq = sqr(point.x()-center.x()) + sqr(point.y() - center.y());
            if ((l_sq < minlsq + epsilon) || (l_sq > maxlsq - epsilon)) {
                l_out = INFINITE;
                return false;
            }
            double l = sqrt(l_sq);
            double z_sq = minorradiussq - sqr(majorradius - l);
            if (z_sq < 0) {
                l_out = INFINITE;
                return false;
            }
            double z = sqrt(z_sq);
            Point ccp = Point(point.x(), point.y(), center.z() - z);
            double dist = ccp.z() - point.z();
            ccp_out = ccp;
            cp_out = point;
            l_out = dist;
            return true;
        } else if (isZero(direction.z())) {
            // push
            double z = point.z() - center.z();
            if (fabs(z) > minorradius - epsilon) {
                l_out = INFINITE;
                return false;
            }
            double l = majorradius + sqrt(minorradiussq - sqr(z));
            Point n = axis.cross(direction);
            double d = n.dot(point) - n.dot(center);
            if (fabs(d) > l - epsilon) {
                l_out = INFINITE;
                return false;
            }
            double a = sqrt(sqr(l) - sqr(d));
            Point ccp = center.add(n.mul(d).add(direction.mul(a)));
            ccp.m_z = point.z();
            double dist = point.sub(ccp).dot(direction);
            ccp_out = ccp;
            cp_out = point;
            l_out = dist;
            return true;
        } else {
            // general case
            Point x = point.sub(center);
            Point v = direction.mul(-1);
            double x_x = x.dot(x);
            double x_v = x.dot(v);
            Point x1 = Point(x.x(), x.y(), 0);
            Point v1 = Point(v.x(), v.y(), 0);
            double x1_x1 = x1.dot(x1);
            double x1_v1 = x1.dot(v1);
            double v1_v1 = v1.dot(v1);
            double R2 = majorradiussq;
            double r2 = minorradiussq;
            double a = 1.0;
            double b = 4 * x_v;
            double c = 2 * (x_x + 2 * sqr(x_v) + (R2 - r2) - 2 * R2 * v1_v1);
            double d = 4 * (x_x * x_v + x_v * (R2 - r2) - 2 * R2 * x1_v1);
            double e = sqr(x_x) + 2 * x_x * (R2 - r2) + sqr(R2 - r2) - 4 * R2 * x1_x1;
            std::vector<double> r = poly4_roots(a, b, c, d, e);

            if (r.size() == 0) {
                l_out = INFINITE;
                return false;
            }
            else {
                double l = r[0];
                for(size_t i=1; i<r.size(); i++) {
                    l = std::min(l, r[i]);
                }
                ccp_out = point.add(direction.mul(-l));
                cp_out = point;
                l_out = l;
                return true;
            }
        }
        l_out = INFINITE;
        return false;
    }
};

#endif
