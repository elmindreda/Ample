
#include <verse.h>

#include <Ample.h>

namespace verse
{
  namespace ample
  {

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

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/

