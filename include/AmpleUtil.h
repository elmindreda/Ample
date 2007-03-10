///////////////////////////////////////////////////////////////////////
// (Modified from the) Wendy core library
// Copyright (C) 2004 Camilla Berglund <camilla.berglund@gmail.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any
// purpose, including commercial applications, and to alter it and
// redistribute it freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you
//     must not claim that you wrote the original software. If you use
//     this software in a product, an acknowledgment in the product
//     documentation would be appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and
//     must not be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source
//     distribution.
//
///////////////////////////////////////////////////////////////////////
#ifndef AMPLEUTIL_H
#define AMPLEUTIL_H
///////////////////////////////////////////////////////////////////////

#include <math.h>

#ifdef _WIN32

#undef min
#undef max

float fminf(float x, float y);
float fmaxf(float x, float y);

#endif /*_WIN32*/

///////////////////////////////////////////////////////////////////////

namespace verse
{
  namespace ample
  {

//---------------------------------------------------------------------

template <typename T>
class Vector3
{
public:
  inline Vector3(void);
  inline Vector3(T sx, T sy, T sz);
  inline T length(void) const;
  inline T lengthSquared(void) const;
  inline T dotProduct(const Vector3<T>& vector) const;
  inline Vector3 crossProduct(const Vector3<T>& vector) const;
  inline void scaleBy(T factor);
  inline void scaleTo(T length);
  inline void mirrorBy(const Vector3<T>& vector);
  inline void invert(void);
  inline void normalize(void);
  inline operator T* (void);
  inline operator const T* (void) const;
  inline Vector3 operator - (void) const;
  inline Vector3 operator + (T value) const;
  inline Vector3 operator - (T value) const;
  inline Vector3 operator * (T value) const;
  inline Vector3 operator / (T value) const;
  inline Vector3 operator += (T value);
  inline Vector3<T>& operator -= (T value);
  inline Vector3<T>& operator *= (T value);
  inline Vector3<T>& operator /= (T value);
  inline Vector3 operator + (const Vector3<T>& vector) const;
  inline Vector3 operator - (const Vector3<T>& vector) const;
  inline Vector3 operator * (const Vector3<T>& vector) const;
  inline Vector3 operator / (const Vector3<T>& vector) const;
  inline Vector3 operator += (const Vector3<T>& vector);
  inline Vector3<T>& operator -= (const Vector3<T>& vector);
  inline Vector3<T>& operator *= (const Vector3<T>& vector);
  inline Vector3<T>& operator /= (const Vector3<T>& vector);
  inline bool operator == (const Vector3<T>& vector) const;
  inline bool operator != (const Vector3<T>& vector) const;
  inline void set(T sx, T sy, T sz);
  T x;
  T y;
  T z;
};

//---------------------------------------------------------------------

typedef Vector3<real32> Vector3f;
typedef Vector3<real64> Vector3d;

//---------------------------------------------------------------------

class ColorRGB
{
public:
  inline ColorRGB(void);
  inline ColorRGB(real64 sr, real64 sg, real64 sb);
  inline void clamp(void);
  inline ColorRGB min(const ColorRGB& color) const;
  inline ColorRGB max(const ColorRGB& color) const;
  inline operator real64* (void);
  inline operator const real64* (void) const;
  inline ColorRGB operator - (void) const;
  inline ColorRGB operator + (real64 value) const;
  inline ColorRGB operator - (real64 value) const;
  inline ColorRGB operator * (real64 value) const;
  inline ColorRGB operator / (real64 value) const;
  inline ColorRGB operator += (real64 value);
  inline ColorRGB& operator -= (real64 value);
  inline ColorRGB& operator *= (real64 value);
  inline ColorRGB& operator /= (real64 value);
  inline ColorRGB operator + (const ColorRGB& color) const;
  inline ColorRGB operator - (const ColorRGB& color) const;
  inline ColorRGB operator * (const ColorRGB& color) const;
  inline ColorRGB operator / (const ColorRGB& color) const;
  inline ColorRGB operator += (const ColorRGB& color);
  inline ColorRGB& operator -= (const ColorRGB& color);
  inline ColorRGB& operator *= (const ColorRGB& color);
  inline ColorRGB& operator /= (const ColorRGB& color);
  inline bool operator == (const ColorRGB& color) const;
  inline bool operator != (const ColorRGB& color) const;
  inline real64 getLength(void) const;
  inline real64 getSquaredLength(void) const;
  inline void setDefaults(void);
  inline void set(real64 sr, real64 sg, real64 sb);
  real64 r;
  real64 g;
  real64 b;
  static const ColorRGB WHITE;
  static const ColorRGB BLACK;
};

//---------------------------------------------------------------------

template <typename T>
inline Vector3<T>::Vector3(void)
{
}

template <typename T>
inline Vector3<T>::Vector3(T sx, T sy, T sz):
  x(sx),
  y(sy),
  z(sz)
{
}

template <typename T>
inline T Vector3<T>::length(void) const
{
  return sqrtf(x * x + y * y + z * z);
}

template <typename T>
inline T Vector3<T>::lengthSquared(void) const
{
  return x * x + y * y + z * z;
}

template <typename T>
inline T Vector3<T>::dotProduct(const Vector3<T>& vector) const
{
  return x * vector.x + y * vector.y + z * vector.z;
}

template <typename T>
inline Vector3<T> Vector3<T>::crossProduct(const Vector3<T>& vector) const
{
  return Vector3(vector.z * y - vector.y * z, vector.x * z - vector.z * x, vector.y * x - vector.x * y);
}

template <typename T>
inline void Vector3<T>::scaleBy(T factor)
{
  x *= factor;
  y *= factor;
  z *= factor;
}

template <typename T>
inline void Vector3<T>::scaleTo(T len)
{
  const T scale = len / length();
  
  x *= scale;
  y *= scale;
  z *= scale;
}

template <typename T>
inline void Vector3<T>::mirrorBy(const Vector3<T>& vector)
{
  const T scale = 2.f * (x * vector.x + y * vector.y + z * vector.z);
  
  x = vector.x * scale - x;
  y = vector.y * scale - y;
  z = vector.z * scale - z;
}

template <typename T>
inline void Vector3<T>::invert(void)
{
  x = -x;
  y = -y;
  z = -z;
}

template <typename T>
inline void Vector3<T>::normalize(void)
{
  const T scale = 1.f / length();
  
  x *= scale;
  y *= scale;
  z *= scale;
}

template <typename T>
inline Vector3<T>::operator T* (void)
{
  return &x;
}

template <typename T>
inline Vector3<T>::operator const T* (void) const
{
  return &x;
}

template <typename T>
inline Vector3<T> Vector3<T>::operator - (void) const
{
  return Vector3(-x, -y, -z);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator + (T value) const
{
  return Vector3(x + value, y + value, z + value);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator - (T value) const
{
  return Vector3(x - value, y - value, z - value);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator * (T value) const
{
  return Vector3(x * value, y * value, z * value);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator / (T value) const
{
  return Vector3(x / value, y / value, z / value);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator += (T value)
{
  x += value;
  y += value;
  z += value;
  return *this;
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator -= (T value)
{
  x -= value;
  y -= value;
  z -= value;
  return *this;
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator *= (T value)
{
  x *= value;
  y *= value;
  z *= value;
  return *this;
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator /= (T value)
{
  x /= value;
  y /= value;
  z /= value;
  return *this;
}

template <typename T>
inline Vector3<T> Vector3<T>::operator + (const Vector3<T>& vector) const
{
  return Vector3<T>(x + vector.x, y + vector.y, z + vector.z);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator - (const Vector3<T>& vector) const
{
  return Vector3<T>(x - vector.x, y - vector.y, z - vector.z);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator * (const Vector3<T>& vector) const
{
  return Vector3<T>(x * vector.x, y * vector.y, z * vector.z);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator / (const Vector3<T>& vector) const
{
  return Vector3<T>(x / vector.x, y / vector.y, z / vector.z);
}

template <typename T>
inline Vector3<T> Vector3<T>::operator += (const Vector3<T>& vector)
{
  x += vector.x;
  y += vector.y;
  z += vector.z;
  return *this;
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator -= (const Vector3<T>& vector)
{
  x -= vector.x;
  y -= vector.y;
  z -= vector.z;
  return *this;
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator *= (const Vector3<T>& vector)
{
  x *= vector.x;
  y *= vector.y;
  z *= vector.z;
  return *this;
}

template <typename T>
inline Vector3<T>& Vector3<T>::operator /= (const Vector3<T>& vector)
{
  x /= vector.x;
  y /= vector.y;
  z /= vector.z;
  return *this;
}

template <typename T>
inline bool Vector3<T>::operator == (const Vector3<T>& vector) const
{
  return x == vector.x && y == vector.y && z == vector.z;
}

template <typename T>
inline bool Vector3<T>::operator != (const Vector3<T>& vector) const
{
  return x != vector.x || y != vector.y || z != vector.z;
}

template <typename T>
inline void Vector3<T>::set(T sx, T sy, T sz)
{
  x = sx;
  y = sy;
  z = sz;
}	

//---------------------------------------------------------------------

inline ColorRGB::ColorRGB(void)
{
}

inline ColorRGB::ColorRGB(real64 sr, real64 sg, real64 sb):
  r(sr),
  g(sg),
  b(sb)
{
}

inline void ColorRGB::clamp(void)
{
  if (r > 1.f)
    r = 1.f;
  else if (r < 0.f)
    r = 0.f;
  
  if (g > 1.f)
    g = 1.f;
  else if (g < 0.f)
    g = 0.f;
  
  if (b > 1.f)
    b = 1.f;
  else if (b < 0.f)
    b = 0.f;
}

inline ColorRGB ColorRGB::min(const ColorRGB& color) const
{
  return ColorRGB(fminf(r, color.r), fminf(g, color.g), fminf(b, color.b));
}

inline ColorRGB ColorRGB::max(const ColorRGB& color) const
{
  return ColorRGB(fmaxf(r, color.r), fmaxf(g, color.g), fmaxf(b, color.b));
}

inline ColorRGB::operator real64* (void)
{
  return &(r);
}

inline ColorRGB::operator const real64* (void) const
{
  return &(r);
}

inline ColorRGB ColorRGB::operator - (void) const
{
  return ColorRGB(-r, -g, -b);
}

inline ColorRGB ColorRGB::operator + (real64 value) const
{
  return ColorRGB(r + value, g + value, b + value);
}

inline ColorRGB ColorRGB::operator - (real64 value) const
{
  return ColorRGB(r - value, g - value, b - value);
}

inline ColorRGB ColorRGB::operator * (real64 value) const
{
  return ColorRGB(r * value, g * value, b * value);
}

inline ColorRGB ColorRGB::operator / (real64 value) const
{
  return ColorRGB(r / value, g / value, b / value);
}

inline ColorRGB ColorRGB::operator += (real64 value)
{
  r += value;
  g += value;
  b += value;
  return *this;
}

inline ColorRGB& ColorRGB::operator -= (real64 value)
{
  r -= value;
  g -= value;
  b -= value;
  return *this;
}

inline ColorRGB& ColorRGB::operator *= (real64 value)
{
  r *= value;
  g *= value;
  b *= value;
  return *this;
}

inline ColorRGB& ColorRGB::operator /= (real64 value)
{
  r /= value;
  g /= value;
  b /= value;
  return *this;
}

inline ColorRGB ColorRGB::operator + (const ColorRGB& color) const
{
  return ColorRGB(r + color.r, g + color.g, b + color.b);
}

inline ColorRGB ColorRGB::operator - (const ColorRGB& color) const
{
  return ColorRGB(r - color.r, g - color.g, b - color.b);
}

inline ColorRGB ColorRGB::operator * (const ColorRGB& color) const
{
  return ColorRGB(r * color.r, g * color.g, b * color.b);
}

inline ColorRGB ColorRGB::operator / (const ColorRGB& color) const
{
  return ColorRGB(r / color.r, g / color.g, b / color.b);
}

inline ColorRGB ColorRGB::operator += (const ColorRGB& color)
{
  r += color.r;
  g += color.g;
  b += color.b;
  return *this;
}

inline ColorRGB& ColorRGB::operator -= (const ColorRGB& color)
{
  r -= color.r;
  g -= color.g;
  b -= color.b;
  return *this;
}

inline ColorRGB& ColorRGB::operator *= (const ColorRGB& color)
{
  r *= color.r;
  g *= color.g;
  b *= color.b;
  return *this;
}

inline ColorRGB& ColorRGB::operator /= (const ColorRGB& color)
{
  r /= color.r;
  g /= color.g;
  b /= color.b;
  return *this;
}

inline bool ColorRGB::operator == (const ColorRGB& color) const
{
  return r == color.r && g == color.g && b == color.b;
}

inline bool ColorRGB::operator != (const ColorRGB& color) const
{
  return r != color.r || g != color.g || b != color.b;
}

inline real64 ColorRGB::getLength(void) const
{
  return r * r + g * g + b * b;
}

inline real64 ColorRGB::getSquaredLength(void) const
{
  return sqrtf(r * r + g * g + b * b);
}

inline void ColorRGB::setDefaults(void)
{
  r = 0.f;
  g = 0.f;
  b = 0.f;
}

inline void ColorRGB::set(real64 sr, real64 sg, real64 sb)
{
  r = sr;
  g = sg;
  b = sb;
}

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/

///////////////////////////////////////////////////////////////////////
#endif /*AMPLEUTIL_H*/
///////////////////////////////////////////////////////////////////////
