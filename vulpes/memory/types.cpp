/*
 * This file is part of Vulpes, an extension of Halo Custom Edition's capabilities.
 * Copyright (C) 2019-2020 gbMichelle (Michelle van der Graaf)
 *
 * Vulpes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * Vulpes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * long with Vulpes.  If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
 */

#include <cmath>

#include "types.hpp"

float Vec2d::get_magnitude() {
    return sqrt(x*x + y*y);
}

float Vec3d::get_magnitude() {
    return sqrt(x*x + y*y + z*z);
}

void Euler2d::normalize() {
    i += fmod(i + 1.0, 2.0) - 1.0;
    j += fmod(j + 1.0, 2.0) - 1.0;
}

void Euler3d::normalize() {
    i += fmod(i + 1.0, 2.0) - 1.0;
    j += fmod(j + 1.0, 2.0) - 1.0;
    k += fmod(k + 1.0, 2.0) - 1.0;
}

void Quaternion::normalize() {
    float magnitude = sqrt(i*i + j*j + k*k + w*w);
    i = i*magnitude;
    j = j*magnitude;
    k = k*magnitude;
    w = w*magnitude;
}
