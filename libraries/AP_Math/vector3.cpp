/*
 * vector3.cpp
 * Copyright (C) Andrew Tridgell 2012
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

#pragma GCC optimize("O2")

#include "AP_Math.h"
#include <AP_InternalError/AP_InternalError.h>

// rotate a vector by a standard rotation, attempting
// to use the minimum number of floating point operations
template <typename T>
void Vector3<T>::rotate(enum Rotation rotation)
{
    T tmp;
    switch (rotation) {
    case ROTATION_NONE:
        return;
    case ROTATION_YAW_45: {
        tmp = HALF_SQRT_2*(float)(x - y);
        y   = HALF_SQRT_2*(float)(x + y);
        x = tmp;
        return;
    }
    case ROTATION_YAW_90: {
        tmp = x; x = -y; y = tmp;
        return;
    }
    case ROTATION_YAW_135: {
        tmp = -HALF_SQRT_2*(float)(x + y);
        y   =  HALF_SQRT_2*(float)(x - y);
        x = tmp;
        return;
    }
    case ROTATION_YAW_180:
        x = -x; y = -y;
        return;
    case ROTATION_YAW_225: {
        tmp = HALF_SQRT_2*(float)(y - x);
        y   = -HALF_SQRT_2*(float)(x + y);
        x = tmp;
        return;
    }
    case ROTATION_YAW_270: {
        tmp = x; x = y; y = -tmp;
        return;
    }
    case ROTATION_YAW_315: {
        tmp = HALF_SQRT_2*(float)(x + y);
        y   = HALF_SQRT_2*(float)(y - x);
        x = tmp;
        return;
    }
    case ROTATION_ROLL_180: {
        y = -y; z = -z;
        return;
    }
    case ROTATION_ROLL_180_YAW_45: {
        tmp = HALF_SQRT_2*(float)(x + y);
        y   = HALF_SQRT_2*(float)(x - y);
        x = tmp; z = -z;
        return;
    }
    case ROTATION_ROLL_180_YAW_90: {
        tmp = x; x = y; y = tmp; z = -z;
        return;
    }
    case ROTATION_ROLL_180_YAW_135: {
        tmp = HALF_SQRT_2*(float)(y - x);
        y   = HALF_SQRT_2*(float)(y + x);
        x = tmp; z = -z;
        return;
    }
    case ROTATION_PITCH_180: {
        x = -x; z = -z;
        return;
    }
    case ROTATION_ROLL_180_YAW_225: {
        tmp = -HALF_SQRT_2*(float)(x + y);
        y   =  HALF_SQRT_2*(float)(y - x);
        x = tmp; z = -z;
        return;
    }
    case ROTATION_ROLL_180_YAW_270: {
        tmp = x; x = -y; y = -tmp; z = -z;
        return;
    }
    case ROTATION_ROLL_180_YAW_315: {
        tmp =  HALF_SQRT_2*(float)(x - y);
        y   = -HALF_SQRT_2*(float)(x + y);
        x = tmp; z = -z;
        return;
    }
    case ROTATION_ROLL_90: {
        tmp = z; z = y; y = -tmp;
        return;
    }
    case ROTATION_ROLL_90_YAW_45: {
        tmp = z; z = y; y = -tmp;
        tmp = HALF_SQRT_2*(float)(x - y);
        y   = HALF_SQRT_2*(float)(x + y);
        x = tmp;
        return;
    }
    case ROTATION_ROLL_90_YAW_90: {
        tmp = z; z = y; y = -tmp;
        tmp = x; x = -y; y = tmp;
        return;
    }
    case ROTATION_ROLL_90_YAW_135: {
        tmp = z; z = y; y = -tmp;
        tmp = -HALF_SQRT_2*(float)(x + y);
        y   =  HALF_SQRT_2*(float)(x - y);
        x = tmp;
        return;
    }
    case ROTATION_ROLL_270: {
        tmp = z; z = -y; y = tmp;
        return;
    }
    case ROTATION_ROLL_270_YAW_45: {
        tmp = z; z = -y; y = tmp;
        tmp = HALF_SQRT_2*(float)(x - y);
        y   = HALF_SQRT_2*(float)(x + y);
        x = tmp;
        return;
    }
    case ROTATION_ROLL_270_YAW_90: {
        tmp = z; z = -y; y = tmp;
        tmp = x; x = -y; y = tmp;
        return;
    }
    case ROTATION_ROLL_270_YAW_135: {
        tmp = z; z = -y; y = tmp;
        tmp = -HALF_SQRT_2*(float)(x + y);
        y   =  HALF_SQRT_2*(float)(x - y);
        x = tmp;
        return;
    }
    case ROTATION_PITCH_90: {
        tmp = z; z = -x; x = tmp;
        return;
    }
    case ROTATION_PITCH_270: {
        tmp = z; z = x; x = -tmp;
        return;
    }
    case ROTATION_PITCH_180_YAW_90: {
        z = -z;
        tmp = -x; x = -y; y = tmp;
        return;
    }
    case ROTATION_PITCH_180_YAW_270: {
        x = -x; z = -z;
        tmp = x; x = y; y = -tmp;
        return;
    }
    case ROTATION_ROLL_90_PITCH_90: {
        tmp = z; z = y; y = -tmp;
        tmp = z; z = -x; x = tmp;
        return;
    }
    case ROTATION_ROLL_180_PITCH_90: {
        y = -y; z = -z;
        tmp = z; z = -x; x = tmp;
        return;
    }
    case ROTATION_ROLL_270_PITCH_90: {
        tmp = z; z = -y; y = tmp;
        tmp = z; z = -x; x = tmp;
        return;
    }
    case ROTATION_ROLL_90_PITCH_180: {
        tmp = z; z = y; y = -tmp;
        x = -x; z = -z;
        return;
    }
    case ROTATION_ROLL_270_PITCH_180: {
        tmp = z; z = -y; y = tmp;
        x = -x; z = -z;
        return;
    }
    case ROTATION_ROLL_90_PITCH_270: {
        tmp = z; z = y; y = -tmp;
        tmp = z; z = x; x = -tmp;
        return;
    }
    case ROTATION_ROLL_180_PITCH_270: {
        y = -y; z = -z;
        tmp = z; z = x; x = -tmp;
        return;
    }
    case ROTATION_ROLL_270_PITCH_270: {
        tmp = z; z = -y; y = tmp;
        tmp = z; z = x; x = -tmp;
        return;
    }
    case ROTATION_ROLL_90_PITCH_180_YAW_90: {
        tmp = z; z = y; y = -tmp;
        x = -x; z = -z;
        tmp = x; x = -y; y = tmp;
        return;
    }
    case ROTATION_ROLL_90_YAW_270: {
        tmp = z; z = y; y = -tmp;
        tmp = x; x = y; y = -tmp;
        return;
    }
    case ROTATION_ROLL_90_PITCH_68_YAW_293: {
        float tmpx = x;
        float tmpy = y;
        float tmpz = z;
        x =  0.143039f * tmpx +  0.368776f * tmpy + -0.918446f * tmpz;
        y = -0.332133f * tmpx + -0.856289f * tmpy + -0.395546f * tmpz;
        z = -0.932324f * tmpx +  0.361625f * tmpy +  0.000000f * tmpz;
        return;
    }
    case ROTATION_PITCH_315: {
        tmp = HALF_SQRT_2*(float)(x - z);
        z   = HALF_SQRT_2*(float)(x + z);
        x = tmp;
        return;
    }
    case ROTATION_ROLL_90_PITCH_315: {
        tmp = z; z = y; y = -tmp;
        tmp = HALF_SQRT_2*(float)(x - z);
        z   = HALF_SQRT_2*(float)(x + z);
        x = tmp;
        return;
    }
    case ROTATION_PITCH_7: {
        const float sin_pitch = 0.12186934340514748f; // sinf(pitch);
        const float cos_pitch = 0.992546151641322f; // cosf(pitch);
        float tmpx = x;
        float tmpz = z;
        x =  cos_pitch * tmpx + sin_pitch * tmpz;
        z = -sin_pitch * tmpx + cos_pitch * tmpz;
        return;
    }
    case ROTATION_CUSTOM: 
        // Error: caller must perform custom rotations via matrix multiplication
        INTERNAL_ERROR(AP_InternalError::error_t::flow_of_control);
        return;
    case ROTATION_MAX:
        break;
    }
    // rotation invalid
    INTERNAL_ERROR(AP_InternalError::error_t::bad_rotation);
}

template <typename T>
void Vector3<T>::rotate_inverse(enum Rotation rotation)
{
    Vector3<T> x_vec(1.0f,0.0f,0.0f);
    Vector3<T> y_vec(0.0f,1.0f,0.0f);
    Vector3<T> z_vec(0.0f,0.0f,1.0f);

    x_vec.rotate(rotation);
    y_vec.rotate(rotation);
    z_vec.rotate(rotation);

    Matrix3<T> M(
        x_vec.x, y_vec.x, z_vec.x,
        x_vec.y, y_vec.y, z_vec.y,
        x_vec.z, y_vec.z, z_vec.z
    );

    (*this) = M.mul_transpose(*this);
}

// vector cross product
template <typename T>
Vector3<T> Vector3<T>::operator %(const Vector3<T> &v) const
{
    Vector3<T> temp(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    return temp;
}

// dot product
template <typename T>
T Vector3<T>::operator *(const Vector3<T> &v) const
{
    return x*v.x + y*v.y + z*v.z;
}

template <typename T>
float Vector3<T>::length(void) const
{
    return norm(x, y, z);
}

template <typename T>
Vector3<T> &Vector3<T>::operator *=(const T num)
{
    x*=num; y*=num; z*=num;
    return *this;
}

template <typename T>
Vector3<T> &Vector3<T>::operator /=(const T num)
{
    x /= num; y /= num; z /= num;
    return *this;
}

template <typename T>
Vector3<T> &Vector3<T>::operator -=(const Vector3<T> &v)
{
    x -= v.x; y -= v.y; z -= v.z;
    return *this;
}

template <typename T>
bool Vector3<T>::is_nan(void) const
{
    return isnan(x) || isnan(y) || isnan(z);
}

template <typename T>
bool Vector3<T>::is_inf(void) const
{
    return isinf(x) || isinf(y) || isinf(z);
}

template <typename T>
Vector3<T> &Vector3<T>::operator +=(const Vector3<T> &v)
{
    x+=v.x; y+=v.y; z+=v.z;
    return *this;
}

template <typename T>
Vector3<T> Vector3<T>::operator /(const T num) const
{
    return Vector3<T>(x/num, y/num, z/num);
}

template <typename T>
Vector3<T> Vector3<T>::operator *(const T num) const
{
    return Vector3<T>(x*num, y*num, z*num);
}

template <typename T>
Vector3<T> Vector3<T>::operator -(const Vector3<T> &v) const
{
    return Vector3<T>(x-v.x, y-v.y, z-v.z);
}

template <typename T>
Vector3<T> Vector3<T>::operator +(const Vector3<T> &v) const
{
    return Vector3<T>(x+v.x, y+v.y, z+v.z);
}

template <typename T>
Vector3<T> Vector3<T>::operator -(void) const
{
    return Vector3<T>(-x,-y,-z);
}

template <typename T>
bool Vector3<T>::operator ==(const Vector3<T> &v) const
{
    return (is_equal(x,v.x) && is_equal(y,v.y) && is_equal(z,v.z));
}

template <typename T>
bool Vector3<T>::operator !=(const Vector3<T> &v) const
{
    return (!is_equal(x,v.x) || !is_equal(y,v.y) || !is_equal(z,v.z));
}

template <typename T>
float Vector3<T>::angle(const Vector3<T> &v2) const
{
    const float len = this->length() * v2.length();
    if (len <= 0) {
        return 0.0f;
    }
    const float cosv = ((*this)*v2) / len;
    if (fabsf(cosv) >= 1) {
        return 0.0f;
    }
    return acosf(cosv);
}

// multiplication of transpose by a vector
template <typename T>
Vector3<T> Vector3<T>::operator *(const Matrix3<T> &m) const
{
    return Vector3<T>(*this * m.colx(),
                      *this * m.coly(),
                      *this * m.colz());
}

// multiply a column vector by a row vector, returning a 3x3 matrix
template <typename T>
Matrix3<T> Vector3<T>::mul_rowcol(const Vector3<T> &v2) const
{
    const Vector3<T> v1 = *this;
    return Matrix3<T>(v1.x * v2.x, v1.x * v2.y, v1.x * v2.z,
                      v1.y * v2.x, v1.y * v2.y, v1.y * v2.z,
                      v1.z * v2.x, v1.z * v2.y, v1.z * v2.z);
}

// extrapolate position given bearing and pitch (in degrees) and distance
template <typename T>
void Vector3<T>::offset_bearing(float bearing, float pitch, float distance)
{
    y += cosf(radians(pitch)) * sinf(radians(bearing)) * distance;
    x += cosf(radians(pitch)) * cosf(radians(bearing)) * distance;
    z += sinf(radians(pitch)) * distance;
}

// distance from the tip of this vector to a line segment specified by two vectors
template <typename T>
float Vector3<T>::distance_to_segment(const Vector3<T> &seg_start, const Vector3<T> &seg_end) const
{
    // triangle side lengths
    const float a = (*this-seg_start).length();
    const float b = (seg_start-seg_end).length();
    const float c = (seg_end-*this).length();

    // protect against divide by zero later
    if (::is_zero(b)) {
        return 0.0f;
    }

    // semiperimeter of triangle
    const float s = (a+b+c) * 0.5f;

    float area_squared = s*(s-a)*(s-b)*(s-c);
    // area must be constrained above 0 because a triangle could have 3 points could be on a line and float rounding could push this under 0
    if (area_squared < 0.0f) {
        area_squared = 0.0f;
    }
    const float area = safe_sqrt(area_squared);
    return 2.0f*area/b;
}

// Shortest distance between point(p) to a point contained in the line segment defined by w1,w2
// this is based on the explanation given here: www.fundza.com/vectors/point2line/index.html
template <typename T>
float Vector3<T>::closest_distance_between_line_and_point(const Vector3<T> &w1, const Vector3<T> &w2, const Vector3<T> &p)
{   
    const Vector3<T> line_vec = w2-w1;
    const Vector3<T> p_vec = p - w1;
    
    const float line_vec_len = line_vec.length();
    // protection against divide by zero
    if(::is_zero(line_vec_len)) {
        return 0.0f;
    }

    const float scale = 1/line_vec_len;
    const Vector3<T> unit_vec = line_vec * scale;
    const Vector3<T> scaled_p_vec = p_vec * scale;

    float dot_product = unit_vec * scaled_p_vec;
    dot_product = constrain_float(dot_product,0.0f,1.0f); 
 
    const Vector3<T> nearest = line_vec * dot_product;
    const float dist = (nearest - p_vec).length();
    return dist;
}

// define for float and double
template class Vector3<float>;
template class Vector3<double>;

// define needed ops for Vector3l
template Vector3<int32_t> &Vector3<int32_t>::operator +=(const Vector3<int32_t> &v);
