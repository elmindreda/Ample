
#include <verse.h>

#include <Ample.h>

namespace verse
{
  namespace ample
  {

//---------------------------------------------------------------------

BaseVertex::BaseVertex(void)
{
}

BaseVertex::BaseVertex(real64 sx, real64 sy, real64 sz):
  x(sx),
  y(sy),
  z(sz)
{
}

bool BaseVertex::isValid(void) const
{
  return x != V_REAL64_MAX || y != V_REAL64_MAX || z != V_REAL64_MAX;
}

void BaseVertex::set(real64 sx, real64 sy, real64 sz)
{
  x = sx;
  y = sy;
  z = sz;
}

void BaseVertex::setInvalid(void)
{
  x = V_REAL64_MAX;
  y = V_REAL64_MAX;
  z = V_REAL64_MAX;
}

//---------------------------------------------------------------------

BasePolygon::BasePolygon(void)
{
}

BasePolygon::BasePolygon(uint32 v0, uint32 v1, uint32 v2, uint32 v3)
{
  mIndices[0] = v0;
  mIndices[1] = v1;
  mIndices[2] = v2;
  mIndices[3] = v3;
}

void BasePolygon::set(uint32 v0, uint32 v1, uint32 v2, uint32 v3)
{
  mIndices[0] = v0;
  mIndices[1] = v1;
  mIndices[2] = v2;
  mIndices[3] = v3;
}

void BasePolygon::setInvalid(void)
{
  mIndices[0] = INVALID_VERTEX_ID;
  mIndices[1] = INVALID_VERTEX_ID;
  mIndices[2] = INVALID_VERTEX_ID;
  mIndices[3] = INVALID_VERTEX_ID;
}

//---------------------------------------------------------------------

Block::Block(void):
  mItemCount(0),
  mItemSize(1),
  mGrain(0),
  mData(NULL)
{
}

Block::Block(const Block& source):
  mItemCount(0),
  mItemSize(1),
  mGrain(0),
  mData(NULL)        
{
  operator = (source);
}

Block::~Block(void)
{
  release();
}

void Block::resize(size_t count)
{
  if (count > 0)
  {
    size_t size;

    if (mGrain != 0)
      count = mGrain * ((count + mGrain - 1) / mGrain);
      
    if (mData)
    {
      uint8* data = new uint8 [count * mItemSize];

      if (count > mItemCount)
	count = mItemCount;

      std::memcpy(data, mData, count * mItemSize);

      delete [] mData;
      mData = data;
    }
    else
      mData = new uint8 [count * mItemSize];

    mItemCount = count;
  }
  else
    release();
}
  
void Block::reserve(size_t count)
{
  if (mItemCount < count)
    resize(count);
}
  
void Block::release(void)
{
  delete [] mData;
  mData = NULL;
  mItemCount = 0;
}

Block::operator void* (void)
{
  return mData;
}

Block::operator const void* (void) const
{
  return mData;
}

Block& Block::operator = (const Block& source)
{
  delete mData;

  mGrain = source.mGrain;
  mItemSize = source.mItemSize;

  release();
  resize(source.mItemCount);

  std::memcpy(mData, source.mData, mItemCount * mItemSize);
  return *this;
}

void* Block::getItem(size_t index)
{
  if (index >= mItemCount)
    return NULL;

  return mData + index * mItemSize;
}

const void* Block::getItem(size_t index) const
{
  if (index >= mItemCount)
    return NULL;

  return mData + index * mItemSize;
}

void Block::setItem(void* item, size_t index)
{
  reserve(index + 1);
  std::memcpy(mData + index * mItemSize, item, mItemSize);
}

size_t Block::getItemSize(void) const
{
  return mItemSize;
}

void Block::setItemSize(size_t size)
{
  release();
  mItemSize = size;
}

size_t Block::getItemCount(void) const
{
  return mItemCount;
}

size_t Block::getGranularity(void) const
{
  return mGrain;
}

void Block::setGranularity(size_t grain)
{
  mGrain = grain;
  reserve(mItemCount);
}

//---------------------------------------------------------------------

Versioned::Versioned(void):
  mStructVersion(0),
  mDataVersion(0)
{
}

unsigned int Versioned::getDataVersion(void) const
{
  return mDataVersion;
}

unsigned int Versioned::getStructureVersion(void) const
{
  return mStructVersion;
}

void Versioned::updateData(void)
{
  mDataVersion++;
}

void Versioned::updateStructure(void)
{
  mStructVersion++;
  mDataVersion++;
}

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/

