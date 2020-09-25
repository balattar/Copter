// scaleddecode.h was generated by ProtoGen version 3.2.a

/*
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
 *
 * Author: Oliver Walters
 */

#ifndef _SCALEDDECODE_H
#define _SCALEDDECODE_H

// Language target is C, C++ compilers: don't mangle us
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \file
 */

/*!
 * \file
 * scaleddecode routines extract scaled numbers from a byte stream.
 *
 * scaleddecode routines extract scaled numbers from a byte stream. The routines
 * in this module are the reverse operation of the routines in scaledencode.
 *
 * Code generation for this module was affected by these global flags:
 * 64-bit integers are not supported.
 * Normal bitfields are supported, long bitfields are not.
 * Double precision floating points are not supported.
 */

#define __STDC_CONSTANT_MACROS
#include <stdint.h>

//! Compute a float using inverse floating point scaling from the base integer type used for bitfields.
float float32ScaledFromBitfield(unsigned int value, float min, float invscaler);

//! Decode a float from a byte stream by inverse floating point scaling from 4 unsigned bytes in big endian order.
float float32ScaledFrom4UnsignedBeBytes(const uint8_t* bytes, int* index, float min, float invscaler);

//! Decode a float from a byte stream by inverse floating point scaling from 4 unsigned bytes in little endian order.
float float32ScaledFrom4UnsignedLeBytes(const uint8_t* bytes, int* index, float min, float invscaler);

//! Decode a float from a byte stream by inverse floating point scaling from 4 signed bytes in big endian order.
float float32ScaledFrom4SignedBeBytes(const uint8_t* bytes, int* index, float invscaler);

//! Decode a float from a byte stream by inverse floating point scaling from 4 signed bytes in little endian order.
float float32ScaledFrom4SignedLeBytes(const uint8_t* bytes, int* index, float invscaler);

//! Decode a float from a byte stream by inverse floating point scaling from 3 unsigned bytes in big endian order.
float float32ScaledFrom3UnsignedBeBytes(const uint8_t* bytes, int* index, float min, float invscaler);

//! Decode a float from a byte stream by inverse floating point scaling from 3 unsigned bytes in little endian order.
float float32ScaledFrom3UnsignedLeBytes(const uint8_t* bytes, int* index, float min, float invscaler);

//! Decode a float from a byte stream by inverse floating point scaling from 3 signed bytes in big endian order.
float float32ScaledFrom3SignedBeBytes(const uint8_t* bytes, int* index, float invscaler);

//! Decode a float from a byte stream by inverse floating point scaling from 3 signed bytes in little endian order.
float float32ScaledFrom3SignedLeBytes(const uint8_t* bytes, int* index, float invscaler);

//! Decode a float from a byte stream by inverse floating point scaling from 2 unsigned bytes in big endian order.
float float32ScaledFrom2UnsignedBeBytes(const uint8_t* bytes, int* index, float min, float invscaler);

//! Decode a float from a byte stream by inverse floating point scaling from 2 unsigned bytes in little endian order.
float float32ScaledFrom2UnsignedLeBytes(const uint8_t* bytes, int* index, float min, float invscaler);

//! Decode a float from a byte stream by inverse floating point scaling from 2 signed bytes in big endian order.
float float32ScaledFrom2SignedBeBytes(const uint8_t* bytes, int* index, float invscaler);

//! Decode a float from a byte stream by inverse floating point scaling from 2 signed bytes in little endian order.
float float32ScaledFrom2SignedLeBytes(const uint8_t* bytes, int* index, float invscaler);

//! Decode a float from a byte stream by inverse floating point scaling from 1 unsigned byte.
float float32ScaledFrom1UnsignedBytes(const uint8_t* bytes, int* index, float min, float invscaler);

//! Decode a float from a byte stream by inverse floating point scaling from 1 signed byte.
float float32ScaledFrom1SignedBytes(const uint8_t* bytes, int* index, float invscaler);

//! Compute a uint32_t using inverse integer scaling from the base integer type used for bitfields.
uint32_t uint32ScaledFromBitfield(unsigned int value, int32_t min, uint32_t divisor);

//! Decode a uint32_t from a byte stream by inverse integer scaling from 4 unsigned bytes in big endian order.
uint32_t uint32ScaledFrom4UnsignedBeBytes(const uint8_t* bytes, int* index, int32_t min, uint32_t divisor);

//! Decode a uint32_t from a byte stream by inverse integer scaling from 4 unsigned bytes in little endian order.
uint32_t uint32ScaledFrom4UnsignedLeBytes(const uint8_t* bytes, int* index, int32_t min, uint32_t divisor);

//! Decode a uint32_t from a byte stream by inverse integer scaling from 4 signed bytes in big endian order.
uint32_t uint32ScaledFrom4SignedBeBytes(const uint8_t* bytes, int* index, uint32_t divisor);

//! Decode a uint32_t from a byte stream by inverse integer scaling from 4 signed bytes in little endian order.
uint32_t uint32ScaledFrom4SignedLeBytes(const uint8_t* bytes, int* index, uint32_t divisor);

//! Decode a uint32_t from a byte stream by inverse integer scaling from 3 unsigned bytes in big endian order.
uint32_t uint32ScaledFrom3UnsignedBeBytes(const uint8_t* bytes, int* index, int32_t min, uint32_t divisor);

//! Decode a uint32_t from a byte stream by inverse integer scaling from 3 unsigned bytes in little endian order.
uint32_t uint32ScaledFrom3UnsignedLeBytes(const uint8_t* bytes, int* index, int32_t min, uint32_t divisor);

//! Decode a uint32_t from a byte stream by inverse integer scaling from 3 signed bytes in big endian order.
uint32_t uint32ScaledFrom3SignedBeBytes(const uint8_t* bytes, int* index, uint32_t divisor);

//! Decode a uint32_t from a byte stream by inverse integer scaling from 3 signed bytes in little endian order.
uint32_t uint32ScaledFrom3SignedLeBytes(const uint8_t* bytes, int* index, uint32_t divisor);

//! Decode a uint32_t from a byte stream by inverse integer scaling from 2 unsigned bytes in big endian order.
uint32_t uint32ScaledFrom2UnsignedBeBytes(const uint8_t* bytes, int* index, int32_t min, uint32_t divisor);

//! Decode a uint32_t from a byte stream by inverse integer scaling from 2 unsigned bytes in little endian order.
uint32_t uint32ScaledFrom2UnsignedLeBytes(const uint8_t* bytes, int* index, int32_t min, uint32_t divisor);

//! Decode a uint32_t from a byte stream by inverse integer scaling from 2 signed bytes in big endian order.
uint32_t uint32ScaledFrom2SignedBeBytes(const uint8_t* bytes, int* index, uint32_t divisor);

//! Decode a uint32_t from a byte stream by inverse integer scaling from 2 signed bytes in little endian order.
uint32_t uint32ScaledFrom2SignedLeBytes(const uint8_t* bytes, int* index, uint32_t divisor);

//! Decode a uint32_t from a byte stream by inverse integer scaling from 1 unsigned byte.
uint32_t uint32ScaledFrom1UnsignedBytes(const uint8_t* bytes, int* index, int32_t min, uint32_t divisor);

//! Decode a uint32_t from a byte stream by inverse integer scaling from 1 signed byte.
uint32_t uint32ScaledFrom1SignedBytes(const uint8_t* bytes, int* index, uint32_t divisor);

//! Compute a int32_t using inverse integer scaling from the base integer type used for bitfields.
int32_t int32ScaledFromBitfield(unsigned int value, int32_t min, uint32_t divisor);

//! Decode a int32_t from a byte stream by inverse integer scaling from 4 unsigned bytes in big endian order.
int32_t int32ScaledFrom4UnsignedBeBytes(const uint8_t* bytes, int* index, int32_t min, uint32_t divisor);

//! Decode a int32_t from a byte stream by inverse integer scaling from 4 unsigned bytes in little endian order.
int32_t int32ScaledFrom4UnsignedLeBytes(const uint8_t* bytes, int* index, int32_t min, uint32_t divisor);

//! Decode a int32_t from a byte stream by inverse integer scaling from 4 signed bytes in big endian order.
int32_t int32ScaledFrom4SignedBeBytes(const uint8_t* bytes, int* index, uint32_t divisor);

//! Decode a int32_t from a byte stream by inverse integer scaling from 4 signed bytes in little endian order.
int32_t int32ScaledFrom4SignedLeBytes(const uint8_t* bytes, int* index, uint32_t divisor);

//! Decode a int32_t from a byte stream by inverse integer scaling from 3 unsigned bytes in big endian order.
int32_t int32ScaledFrom3UnsignedBeBytes(const uint8_t* bytes, int* index, int32_t min, uint32_t divisor);

//! Decode a int32_t from a byte stream by inverse integer scaling from 3 unsigned bytes in little endian order.
int32_t int32ScaledFrom3UnsignedLeBytes(const uint8_t* bytes, int* index, int32_t min, uint32_t divisor);

//! Decode a int32_t from a byte stream by inverse integer scaling from 3 signed bytes in big endian order.
int32_t int32ScaledFrom3SignedBeBytes(const uint8_t* bytes, int* index, uint32_t divisor);

//! Decode a int32_t from a byte stream by inverse integer scaling from 3 signed bytes in little endian order.
int32_t int32ScaledFrom3SignedLeBytes(const uint8_t* bytes, int* index, uint32_t divisor);

//! Decode a int32_t from a byte stream by inverse integer scaling from 2 unsigned bytes in big endian order.
int32_t int32ScaledFrom2UnsignedBeBytes(const uint8_t* bytes, int* index, int32_t min, uint32_t divisor);

//! Decode a int32_t from a byte stream by inverse integer scaling from 2 unsigned bytes in little endian order.
int32_t int32ScaledFrom2UnsignedLeBytes(const uint8_t* bytes, int* index, int32_t min, uint32_t divisor);

//! Decode a int32_t from a byte stream by inverse integer scaling from 2 signed bytes in big endian order.
int32_t int32ScaledFrom2SignedBeBytes(const uint8_t* bytes, int* index, uint32_t divisor);

//! Decode a int32_t from a byte stream by inverse integer scaling from 2 signed bytes in little endian order.
int32_t int32ScaledFrom2SignedLeBytes(const uint8_t* bytes, int* index, uint32_t divisor);

//! Decode a int32_t from a byte stream by inverse integer scaling from 1 unsigned byte.
int32_t int32ScaledFrom1UnsignedBytes(const uint8_t* bytes, int* index, int32_t min, uint32_t divisor);

//! Decode a int32_t from a byte stream by inverse integer scaling from 1 signed byte.
int32_t int32ScaledFrom1SignedBytes(const uint8_t* bytes, int* index, uint32_t divisor);

//! Compute a uint16_t using inverse integer scaling from the base integer type used for bitfields.
uint16_t uint16ScaledFromBitfield(unsigned int value, int16_t min, uint16_t divisor);

//! Decode a uint16_t from a byte stream by inverse integer scaling from 2 unsigned bytes in big endian order.
uint16_t uint16ScaledFrom2UnsignedBeBytes(const uint8_t* bytes, int* index, int16_t min, uint16_t divisor);

//! Decode a uint16_t from a byte stream by inverse integer scaling from 2 unsigned bytes in little endian order.
uint16_t uint16ScaledFrom2UnsignedLeBytes(const uint8_t* bytes, int* index, int16_t min, uint16_t divisor);

//! Decode a uint16_t from a byte stream by inverse integer scaling from 2 signed bytes in big endian order.
uint16_t uint16ScaledFrom2SignedBeBytes(const uint8_t* bytes, int* index, uint16_t divisor);

//! Decode a uint16_t from a byte stream by inverse integer scaling from 2 signed bytes in little endian order.
uint16_t uint16ScaledFrom2SignedLeBytes(const uint8_t* bytes, int* index, uint16_t divisor);

//! Decode a uint16_t from a byte stream by inverse integer scaling from 1 unsigned byte.
uint16_t uint16ScaledFrom1UnsignedBytes(const uint8_t* bytes, int* index, int16_t min, uint16_t divisor);

//! Decode a uint16_t from a byte stream by inverse integer scaling from 1 signed byte.
uint16_t uint16ScaledFrom1SignedBytes(const uint8_t* bytes, int* index, uint16_t divisor);

//! Compute a int16_t using inverse integer scaling from the base integer type used for bitfields.
int16_t int16ScaledFromBitfield(unsigned int value, int16_t min, uint16_t divisor);

//! Decode a int16_t from a byte stream by inverse integer scaling from 2 unsigned bytes in big endian order.
int16_t int16ScaledFrom2UnsignedBeBytes(const uint8_t* bytes, int* index, int16_t min, uint16_t divisor);

//! Decode a int16_t from a byte stream by inverse integer scaling from 2 unsigned bytes in little endian order.
int16_t int16ScaledFrom2UnsignedLeBytes(const uint8_t* bytes, int* index, int16_t min, uint16_t divisor);

//! Decode a int16_t from a byte stream by inverse integer scaling from 2 signed bytes in big endian order.
int16_t int16ScaledFrom2SignedBeBytes(const uint8_t* bytes, int* index, uint16_t divisor);

//! Decode a int16_t from a byte stream by inverse integer scaling from 2 signed bytes in little endian order.
int16_t int16ScaledFrom2SignedLeBytes(const uint8_t* bytes, int* index, uint16_t divisor);

//! Decode a int16_t from a byte stream by inverse integer scaling from 1 unsigned byte.
int16_t int16ScaledFrom1UnsignedBytes(const uint8_t* bytes, int* index, int16_t min, uint16_t divisor);

//! Decode a int16_t from a byte stream by inverse integer scaling from 1 signed byte.
int16_t int16ScaledFrom1SignedBytes(const uint8_t* bytes, int* index, uint16_t divisor);

//! Compute a uint8_t using inverse integer scaling from the base integer type used for bitfields.
uint8_t uint8ScaledFromBitfield(unsigned int value, int8_t min, uint8_t divisor);

//! Decode a uint8_t from a byte stream by inverse integer scaling from 1 unsigned byte.
uint8_t uint8ScaledFrom1UnsignedBytes(const uint8_t* bytes, int* index, int8_t min, uint8_t divisor);

//! Decode a uint8_t from a byte stream by inverse integer scaling from 1 signed byte.
uint8_t uint8ScaledFrom1SignedBytes(const uint8_t* bytes, int* index, uint8_t divisor);

//! Compute a int8_t using inverse integer scaling from the base integer type used for bitfields.
int8_t int8ScaledFromBitfield(unsigned int value, int8_t min, uint8_t divisor);

//! Decode a int8_t from a byte stream by inverse integer scaling from 1 unsigned byte.
int8_t int8ScaledFrom1UnsignedBytes(const uint8_t* bytes, int* index, int8_t min, uint8_t divisor);

//! Decode a int8_t from a byte stream by inverse integer scaling from 1 signed byte.
int8_t int8ScaledFrom1SignedBytes(const uint8_t* bytes, int* index, uint8_t divisor);

#ifdef __cplusplus
}
#endif
#endif // _SCALEDDECODE_H
