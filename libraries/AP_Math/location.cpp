/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
 * location.cpp
 * Copyright (C) Andrew Tridgell 2011
 *
 * This file is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 *  this module deals with calculations involving struct Location
 */
#include <stdlib.h>
#include "AP_Math.h"

// radius of earth in meters
#define RADIUS_OF_EARTH 6378100

float longitude_scale(const struct Location *loc)
{
    static int32_t last_lat;
    static float scale = 1.0;
    if (labs(last_lat - loc->lat) < 100000) {
        // we are within 0.01 degrees (about 1km) of the
        // same latitude. We can avoid the cos() and return
        // the same scale factor.
        return scale;
    }
    scale = cosf((fabsf((float)loc->lat)/1.0e7f) * 0.0174532925f);
    last_lat = loc->lat;
    return scale;
}



// return distance in meters to between two locations
float get_distance(const struct Location *loc1, const struct Location *loc2)
{
    float dlat              = (float)(loc2->lat - loc1->lat);
    float dlong             = ((float)(loc2->lng - loc1->lng)) * longitude_scale(loc2);
    return pythagorous2(dlat, dlong) * 0.01113195f;
}

// return distance in centimeters to between two locations
uint32_t get_distance_cm(const struct Location *loc1, const struct Location *loc2)
{
    return get_distance(loc1, loc2) * 100;
}

// return bearing in centi-degrees between two locations
int32_t get_bearing_cd(const struct Location *loc1, const struct Location *loc2)
{
    int32_t off_x = loc2->lng - loc1->lng;
    int32_t off_y = (loc2->lat - loc1->lat) / longitude_scale(loc2);
    int32_t bearing = 9000 + atan2f(-off_y, off_x) * 5729.57795f;
    if (bearing < 0) bearing += 36000;
    return bearing;
}

// see if location is past a line perpendicular to
// the line between point1 and point2. If point1 is
// our previous waypoint and point2 is our target waypoint
// then this function returns true if we have flown past
// the target waypoint
bool location_passed_point(const struct Location &location,
                           const struct Location &point1,
                           const struct Location &point2)
{
    // the 3 points form a triangle. If the angle between lines
    // point1->point2 and location->point2 is greater than 90
    // degrees then we have passed the waypoint
    Vector2f loc1(location.lat, location.lng);
    Vector2f pt1(point1.lat, point1.lng);
    Vector2f pt2(point2.lat, point2.lng);
    float angle = (loc1 - pt2).angle(pt1 - pt2);
    if (isinf(angle)) {
        // two of the points are co-located.
        // If location is equal to point2 then say we have passed the
        // waypoint, otherwise say we haven't
        if (get_distance(&location, &point2) == 0) {
            return true;
        }
        return false;
    } else if (angle == 0) {
        // if we are exactly on the line between point1 and
        // point2 then we are past the waypoint if the
        // distance from location to point1 is greater then
        // the distance from point2 to point1
        return get_distance(&location, &point1) >
               get_distance(&point2, &point1);

    }
    if (degrees(angle) > 90) {
        return true;
    }
    return false;
}

/*
 *  extrapolate latitude/longitude given bearing and distance
 *  thanks to http://www.movable-type.co.uk/scripts/latlong.html
 *
 *  This function is precise, but costs about 1.7 milliseconds on an AVR2560
 */
void location_update(struct Location *loc, float bearing, float distance)
{
    float lat1 = radians(loc->lat*1.0e-7f);
    float lon1 = radians(loc->lng*1.0e-7f);
    float brng = radians(bearing);
    float dr = distance/RADIUS_OF_EARTH;

    float lat2 = asinf(sinf(lat1)*cosf(dr) +
                       cosf(lat1)*sinf(dr)*cosf(brng));
    float lon2 = lon1 + atan2f(sinf(brng)*sinf(dr)*cosf(lat1),
                               cosf(dr)-sinf(lat1)*sinf(lat2));
    loc->lat = degrees(lat2)*1.0e7f;
    loc->lng = degrees(lon2)*1.0e7f;
}

/*
 *  extrapolate latitude/longitude given distances north and east
 *  This function costs about 80 usec on an AVR2560
 */
void location_offset(struct Location *loc, float ofs_north, float ofs_east)
{
    if (ofs_north != 0 || ofs_east != 0) {
        float dlat = ofs_north * 89.831520982f;
        float dlng = (ofs_east * 89.831520982f) / longitude_scale(loc);
        loc->lat += dlat;
        loc->lng += dlng;
    }
}

/*
  wrap an angle in centi-degrees to 0..36000
 */
int32_t wrap_360_cd(int32_t error)
{
    while (error > 36000) error -= 36000;
    while (error < 0) error += 36000;
    return error;
}

/*
  wrap an angle in centi-degrees to -18000..18000
 */
int32_t wrap_180_cd(int32_t error)
{
    while (error > 18000) error -= 36000;
    while (error < -18000) error += 36000;
    return error;
}

