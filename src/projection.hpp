/*
 * projetion.hpp
 *
 *  Created on:  2019-09-04
 *      Author: Michael Reichert <michael.reichert@geofabrik.de>
 */
/*

This file contains code of the Osmium project (https://osmcode.org/libosmium).

Copyright 2013-2019 Jochen Topf <jochen@topf.org> and others (see README).

*/

#ifndef SRC_PROJECTION_HPP_
#define SRC_PROJECTION_HPP_

#include <cmath>
#include <cstdint>

namespace projection {

    constexpr double PI = 3.14159265358979323846;

    /// Convert angle from degrees to radians.
    inline constexpr double deg_to_rad(const double degree) noexcept {
        return degree * (PI / 180.0);
    }

    /// Convert angle from radians to degrees.
    inline constexpr double rad_to_deg(double radians) noexcept {
        return radians * (180.0 / PI);
    }

    constexpr double earth_radius_for_epsg3857 = 6378137.0;
    constexpr double earth_circumfence = earth_radius_for_epsg3857 * 2 * PI;
    constexpr double mercator_max_value = earth_circumfence / 2;

    constexpr inline double in_bounds(const double value, const double min, const double max) {
        return value > max ? max : (value < min ? min : value);
    }

    constexpr inline double lon_to_x(double lon) noexcept {
        return in_bounds(earth_radius_for_epsg3857 * deg_to_rad(lon), -mercator_max_value, mercator_max_value);
    }

    inline double lat_to_y_with_tan(double lat) { // not constexpr because math functions aren't
        return in_bounds(earth_radius_for_epsg3857 * std::log(std::tan(PI / 4 + deg_to_rad(lat) / 2)), -mercator_max_value, mercator_max_value);
    }

    inline double lat_to_y(double lat) { // not constexpr because math functions aren't
        if (lat < -78.0 || lat > 78.0) {
            return lat_to_y_with_tan(lat);
        }

        return earth_radius_for_epsg3857 *
            ((((((((((-3.1112583378460085319e-23  * lat +
                       2.0465852743943268009e-19) * lat +
                       6.4905282018672673884e-18) * lat +
                      -1.9685447939983315591e-14) * lat +
                      -2.2022588158115104182e-13) * lat +
                       5.1617537365509453239e-10) * lat +
                       2.5380136069803016519e-9)  * lat +
                      -5.1448323697228488745e-6)  * lat +
                      -9.4888671473357768301e-6)  * lat +
                       1.7453292518154191887e-2)  * lat)
            /
            ((((((((((-1.9741136066814230637e-22  * lat +
                      -1.258514031244679556e-20)  * lat +
                       4.8141483273572351796e-17) * lat +
                       8.6876090870176172185e-16) * lat +
                      -2.3298743439377541768e-12) * lat +
                      -1.9300094785736130185e-11) * lat +
                       4.3251609106864178231e-8)  * lat +
                       1.7301944508516974048e-7)  * lat +
                      -3.4554675198786337842e-4)  * lat +
                      -5.4367203601085991108e-4)  * lat + 1.0);
    }

    inline double y_to_lat(double y) { // not constexpr because math functions aren't
        return rad_to_deg(2 * std::atan(std::exp(y / earth_radius_for_epsg3857)) - PI / 2);
    }

    inline double mercator_scale(const double latitude) {
        return 1 / cos(projection::deg_to_rad(latitude));
    }

    constexpr uint32_t get_tile_count(const uint32_t zoom) {
        return 1u << zoom;
    }

    constexpr double get_tile_width_in_merc(const uint32_t zoom) {
        return earth_circumfence / get_tile_count(zoom);
    }

    inline double merc_x_to_tile(const double x, const uint32_t zoom) {
        return in_bounds((x / earth_circumfence + 0.5) * get_tile_count(zoom), 0, get_tile_count(zoom) - 1);
    }

    inline double merc_y_to_tile(const double y, const uint32_t zoom) {
        return in_bounds((1 - (y / earth_circumfence + 0.5)) * get_tile_count(zoom), 0, get_tile_count(zoom) - 1);
    }

    inline double tile_x_to_merc(const double x, const uint32_t zoom) {
        return (x / get_tile_count(zoom) - 0.5) * earth_circumfence;
    }

    inline double tile_y_to_merc(const double y, const uint32_t zoom) {
        return ((0.5 * get_tile_count(zoom) - y) / get_tile_count(zoom)) * earth_circumfence;
    }

} // namespace projection

#endif /* SRC_PROJECTION_HPP_ */