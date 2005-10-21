//---------------------------------------------------------------------
// Simple C++ retained mode library for Verse
// Copyright (c) PDC, KTH
// Written by Camilla Berglund <clb@kth.se>
//---------------------------------------------------------------------

#include <verse.h>

#include <Ample.h>

namespace verse
{
  namespace ample
  {

//---------------------------------------------------------------------

void BitmapLayer::destroy(void)
{
  // TODO: The code.
}

VLayerID BitmapLayer::getID(void) const
{
  return mID;
}

const std::string& BitmapLayer::getName(void) const
{
  return mName;
}

BitmapNode& BitmapLayer::getNode(void) const
{
  return mNode;
}

BitmapLayer::BitmapLayer(VLayerID ID, const std::string& name, BitmapNode& node):
  mID(ID),
  mName(name),
  mNode(node)
{
  // TODO: The code.
}

//---------------------------------------------------------------------

void BitmapLayerObserver::onSetName(BitmapLayer& layer, const std::string name)
{
}

void BitmapLayerObserver::onDestroy(BitmapLayer& layer)
{
}

//---------------------------------------------------------------------

BitmapLayer* BitmapNode::getLayerByID(VLayerID ID)
{
  for (LayerList::iterator i = mLayers.begin();  i != mLayers.end();  i++)
    if ((*i)->getID() == ID)
      return *i;

  return NULL;
}

const BitmapLayer* BitmapNode::getLayerByID(VLayerID ID) const
{
  for (LayerList::const_iterator i = mLayers.begin();  i != mLayers.end();  i++)
    if ((*i)->getID() == ID)
      return *i;

  return NULL;
}

BitmapLayer* BitmapNode::getLayerByIndex(unsigned int index)
{
  return mLayers[index];
}

const BitmapLayer* BitmapNode::getLayerByIndex(unsigned int index) const
{
  return mLayers[index];
}

BitmapLayer* BitmapNode::getLayerByName(const std::string& name)
{
  for (LayerList::iterator i = mLayers.begin();  i != mLayers.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

const BitmapLayer* BitmapNode::getLayerByName(const std::string& name) const
{
  for (LayerList::const_iterator i = mLayers.begin();  i != mLayers.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

void BitmapNode::setDimensions(uint16 width, uint16 height, uint16 depth)
{
  // TODO: The code.
}

uint16 BitmapNode::getWidth(void) const
{
  return mWidth;
}

uint16 BitmapNode::getHeight(void) const
{
  return mHeight;
}

uint16 BitmapNode::getDepth(void) const
{
  return mDepth;
}

BitmapNode::BitmapNode(VNodeID ID, VNodeOwner owner, Session& session):
  Node(ID, V_NT_BITMAP, owner, session)
{
}

BitmapNode::~BitmapNode(void)
{
  while (mLayers.size())
  {
    delete mLayers.back();
    mLayers.pop_back();
  }
}

//---------------------------------------------------------------------

void BitmapNodeObserver::onSetDimensions(BitmapNode& node, uint16 width, uint16 height, uint16 depth)
{
}

void BitmapNodeObserver::onCreateLayer(BitmapNode& node, BitmapLayer& layer)
{
}

void BitmapNodeObserver::onDestroyLayer(BitmapNode& node, BitmapLayer& layer)
{
}

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/

