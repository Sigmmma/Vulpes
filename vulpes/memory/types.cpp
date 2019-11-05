/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <cmath>

#include "types.hpp"

float Vec2d::get_magnitude(){
    return sqrt(x*x + y*y);
}

float Vec3d::get_magnitude(){
    return sqrt(x*x + y*y + z*z);
}

void Euler2d::normalize(){
    i += fmod(i + 1.0, 2.0) - 1.0;
    j += fmod(j + 1.0, 2.0) - 1.0;
}

void Euler3d::normalize(){
    i += fmod(i + 1.0, 2.0) - 1.0;
    j += fmod(j + 1.0, 2.0) - 1.0;
    k += fmod(k + 1.0, 2.0) - 1.0;
}

void Quaternion::normalize(){
    float magnitude = sqrt(i*i + j*j + k*k + w*w);
    i = i*magnitude;
    j = j*magnitude;
    k = k*magnitude;
    w = w*magnitude;
}
