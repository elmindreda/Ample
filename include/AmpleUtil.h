///////////////////////////////////////////////////////////////////////
// (Modified from the) Wendy core library
// Copyright (C) 2004 Camilla Berglund <elmindreda@users.sourceforge.net>
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

namespace verse
{
  namespace ample
  {

//---------------------------------------------------------------------

/*! Basic heap memory block container.
 *  Contains objects of type T.
 *  Contained objects need to have public copy constructors and assignment operators.
 */
template <typename T>
class Block
{
public:
  /*! Default constructor.
   *  @param count The number of initial items.
   */
  inline explicit Block(unsigned int count = 0);
  /*! Constructor. Copies the specified number of items into this block
   *  @param items [in] The items to copy into this block.
   *  @param count The number of initial items.
   */
  inline Block(const T* items, unsigned int count);
  /*! Copy constructor.
   *  @remarks Performs a deep copy of data items.
   */
  inline Block(const Block<T>& source);
  /*! Destructor.
   */
  inline ~Block(void);
  /*! Copies the specified number of items from this block, starting at the specified index.
   *  @param items [in] The target buffer for the items to be copied.
   *  @param count [in] The number of items to copy.
   *  @param index [in] The desired index, within this datablock, of the first item.
   */
  inline void copyTo(T* items, unsigned int count, unsigned int index = 0) const;
  /*! Copies the specified number of items into this block, starting at the specified index.
   *  @param items [in] The items to copy into this block.
   *  @param count [in] The number of items to copy.
   *  @param index [in] The desired index, within this datablock, of the first item.
   */
  inline void copyFrom(const T* items, unsigned int count, unsigned int index = 0);
  /*! Changes the number of items in this data block.
   *  @param count [in] The number of data items to create.
   *  @remarks The resize is performed with new and element assignment, NOT with realloc.
   */
  inline void resize(unsigned int count);
  /*! Assures that this block has at least the specified number of items.
   *  @param count [in] The minimum desired capacity.
   */
  inline void reserve(unsigned int count);
  /*! Releases the currently created data block.
   */
  inline void release(void);
  /*! Cast operator.
   */
  inline operator T* (void);
  /*! Cast operator.
   */
  inline operator const T* (void) const;
  /*! Assignment operator.
   *  @remarks Performs a deep copy of data items.
   */
  inline Block<T>& operator = (const Block<T>& source);
  /*! @return The number of data items.
   */
  inline unsigned int getCount(void) const;
  /*! @return The allocation granularity, or zero if granularity is disabled.
   */
  inline unsigned int getGrain(void) const;
  /*! Sets or disables the allocation granularity.
   *  @param grain [in] The desired allocation granularity, or zero to disable.
   */
  inline void setGrain(unsigned int grain);
  /*! @return The item at the specified index.
   */
  T& getItem(unsigned int index);
  /*! @return The item at the specified index.
    */
  const T& getItem(unsigned int index) const;
  /*! @return The first item.
   */
  inline T* getItems(void);
  /*! @return The first item.
   */
  inline const T* getItems(void) const;
private:
  unsigned int m_count;
  unsigned int m_grain;
  T* m_items;
};

//---------------------------------------------------------------------

typedef Block<uint8> ByteBlock;

//---------------------------------------------------------------------

template <typename T>
inline Block<T>::Block(unsigned int count):
  m_count(0),
  m_grain(0),
  m_items(NULL)
{
  resize(count);
}

template <typename T>
inline Block<T>::Block(const T* items, unsigned int count):
  m_count(0),
  m_grain(0),
  m_items(NULL)
{
  copyFrom(items, count);
}

template <typename T>
inline Block<T>::Block(const Block<T>& source):
  m_count(0),
  m_grain(0),
  m_items(NULL)
{
  operator = (source);
}

template <typename T>
inline Block<T>::~Block(void)
{
  release();
}

template <typename T>
inline void Block<T>::copyTo(T* items, unsigned int count, unsigned int index) const
{
  for (unsigned int i = 0;  i < count;  i++)
    items[i] = m_items[i + index];
}

template <typename T>
inline void Block<T>::copyFrom(const T* items, unsigned int count, unsigned int index)
{
  reserve(count + index);
  
  for (unsigned int i = 0;  i < count;  i++)
    m_items[i + index] = items[i];
}

template <typename T>
inline void Block<T>::resize(unsigned int count)
{
  if (count == 0)
    release();
  else
  {
    if (m_grain != 0)
      count = m_grain * ((count + m_grain - 1) / m_grain);
    
    if (m_count == 0)
      m_items  = new T [count];
    else
    {
      T* buffer = new T [count];
      
      if (count > m_count)
        count = m_count;
      
      copyTo(buffer, count);
      
      delete [] m_items;
      m_items = buffer;
    }
    
    m_count = count;
  }
}

template <typename T>
inline void Block<T>::reserve(unsigned int count)
{
  if (count > m_count)
    resize(count);
}

template <typename T>
inline void Block<T>::release(void)
{
  if (m_count > 0)
  {
    delete [] m_items;
    m_items = NULL;
    
    m_count = 0;
  }
}

template <typename T>
inline Block<T>::operator T* (void)
{
  return m_items;
}

template <typename T>
inline Block<T>::operator const T* (void) const
{
  return m_items;
}

template <typename T>
inline Block<T>& Block<T>::operator = (const Block<T>& source)
{
  if (source.m_count == 0)
    release();
  else
  {
    resize(source.m_count);
    copyFrom(source.m_items, source.m_count);
  }
  
  return *this;
}

template <typename T>
inline unsigned int Block<T>::getCount(void) const
{
  return m_count;
}

template <typename T>
inline unsigned int Block<T>::getGrain(void) const
{
  return m_grain;
}

template <typename T>
inline void Block<T>::setGrain(unsigned int grain)
{
  m_grain = grain;
}

template <typename T>
inline T* Block<T>::getItems(void)
{
  return m_items;
}

template <typename T>
inline const T* Block<T>::getItems(void) const
{
  return m_items;
}

//---------------------------------------------------------------------

class Vector3
{
public:
  inline Vector3(void);
  inline Vector3(float sx, float sy, float sz);
  inline float length(void) const;
  inline float lengthSquared(void) const;
  inline float dotProduct(const Vector3& vector) const;
  inline Vector3 crossProduct(const Vector3& vector) const;
  inline void scaleBy(float factor);
  inline void scaleTo(float length);
  inline void mirrorBy(const Vector3& vector);
  inline void invert(void);
  inline void normalize(void);
  inline operator float* (void);
  inline operator const float* (void) const;
  inline Vector3 operator - (void) const;
  inline Vector3 operator + (float value) const;
  inline Vector3 operator - (float value) const;
  inline Vector3 operator * (float value) const;
  inline Vector3 operator / (float value) const;
  inline Vector3 operator += (float value);
  inline Vector3& operator -= (float value);
  inline Vector3& operator *= (float value);
  inline Vector3& operator /= (float value);
  inline Vector3 operator + (const Vector3& vector) const;
  inline Vector3 operator - (const Vector3& vector) const;
  inline Vector3 operator * (const Vector3& vector) const;
  inline Vector3 operator / (const Vector3& vector) const;
  inline Vector3 operator += (const Vector3& vector);
  inline Vector3& operator -= (const Vector3& vector);
  inline Vector3& operator *= (const Vector3& vector);
  inline Vector3& operator /= (const Vector3& vector);
  inline bool operator == (const Vector3& vector) const;
  inline bool operator != (const Vector3& vector) const;
  inline void set(float sx, float sy, float sz);
  float x;
  float y;
  float z;
};


//---------------------------------------------------------------------

inline Vector3::Vector3(void)
{
}

inline Vector3::Vector3(float sx, float sy, float sz):
  x(sx),
  y(sy),
  z(sz)
{
}

inline float Vector3::length(void) const
{
  return sqrtf(x * x + y * y + z * z);
}

inline float Vector3::lengthSquared(void) const
{
  return x * x + y * y + z * z;
}

inline float Vector3::dotProduct(const Vector3& vector) const
{
  return x * vector.x + y * vector.y + z * vector.z;
}

inline Vector3 Vector3::crossProduct(const Vector3& vector) const
{
  return Vector3(vector.z * y - vector.y * z, vector.x * z - vector.z * x, vector.y * x - vector.x * y);
}

inline void Vector3::scaleBy(float factor)
{
  x *= factor;
  y *= factor;
  z *= factor;
}

inline void Vector3::scaleTo(float len)
{
  const float scale = len / length();
  
  x *= scale;
  y *= scale;
  z *= scale;
}

inline void Vector3::mirrorBy(const Vector3& vector)
{
  const float scale = 2.f * (x * vector.x + y * vector.y + z * vector.z);
  
  x = vector.x * scale - x;
  y = vector.y * scale - y;
  z = vector.z * scale - z;
}

inline void Vector3::invert(void)
{
  x = -x;
  y = -y;
  z = -z;
}

inline void Vector3::normalize(void)
{
  const float scale = 1.f / length();
  
  x *= scale;
  y *= scale;
  z *= scale;
}

inline Vector3::operator float* (void)
{
  return &(x);
}

inline Vector3::operator const float* (void) const
{
  return &(x);
}

inline Vector3 Vector3::operator - (void) const
{
  return Vector3(-x, -y, -z);
}

inline Vector3 Vector3::operator + (float value) const
{
  return Vector3(x + value, y + value, z + value);
}

inline Vector3 Vector3::operator - (float value) const
{
  return Vector3(x - value, y - value, z - value);
}

inline Vector3 Vector3::operator * (float value) const
{
  return Vector3(x * value, y * value, z * value);
}

inline Vector3 Vector3::operator / (float value) const
{
  return Vector3(x / value, y / value, z / value);
}

inline Vector3 Vector3::operator += (float value)
{
  x += value;
  y += value;
  z += value;
  return *this;
}

inline Vector3& Vector3::operator -= (float value)
{
  x -= value;
  y -= value;
  z -= value;
  return *this;
}

inline Vector3& Vector3::operator *= (float value)
{
  x *= value;
  y *= value;
  z *= value;
  return *this;
}

inline Vector3& Vector3::operator /= (float value)
{
  x /= value;
  y /= value;
  z /= value;
  return *this;
}

inline Vector3 Vector3::operator + (const Vector3& vector) const
{
  return Vector3(x + vector.x, y + vector.y, z + vector.z);
}

inline Vector3 Vector3::operator - (const Vector3& vector) const
{
  return Vector3(x - vector.x, y - vector.y, z - vector.z);
}

inline Vector3 Vector3::operator * (const Vector3& vector) const
{
  return Vector3(x * vector.x, y * vector.y, z * vector.z);
}

inline Vector3 Vector3::operator / (const Vector3& vector) const
{
  return Vector3(x / vector.x, y / vector.y, z / vector.z);
}

inline Vector3 Vector3::operator += (const Vector3& vector)
{
  x += vector.x;
  y += vector.y;
  z += vector.z;
  return *this;
}

inline Vector3& Vector3::operator -= (const Vector3& vector)
{
  x -= vector.x;
  y -= vector.y;
  z -= vector.z;
  return *this;
}

inline Vector3& Vector3::operator *= (const Vector3& vector)
{
  x *= vector.x;
  y *= vector.y;
  z *= vector.z;
  return *this;
}

inline Vector3& Vector3::operator /= (const Vector3& vector)
{
  x /= vector.x;
  y /= vector.y;
  z /= vector.z;
  return *this;
}

inline bool Vector3::operator == (const Vector3& vector) const
{
  return x == vector.x && y == vector.y && z == vector.z;
}

inline bool Vector3::operator != (const Vector3& vector) const
{
  return x != vector.x || y != vector.y || z != vector.z;
}

inline void Vector3::set(float sx, float sy, float sz)
{
  x = sx;
  y = sy;
  z = sz;
}	


//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/

///////////////////////////////////////////////////////////////////////
#endif /*AMPLEUTIL_H*/
///////////////////////////////////////////////////////////////////////
