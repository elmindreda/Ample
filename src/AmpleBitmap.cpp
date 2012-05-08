//---------------------------------------------------------------------
// Simple C++ retained mode library for Verse
// Copyright (c) PDC, KTH
// Written by Camilla Berglund <elmindreda@elmindreda.org>
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
  getNode().getSession().push();
  verse_send_b_layer_destroy(getNode().getID(), mID);
  getNode().getSession().pop();
}

void BitmapLayer::setTile(uint16 tileX, uint16 tileY, uint16 z, const VNBTile& tile)
{
  getNode().getSession().push();
  verse_send_b_tile_set(getNode().getID(), mID, tileX, tileY, z, mType, &tile);
  getNode().getSession().pop();
}

VLayerID BitmapLayer::getID(void) const
{
  return mID;
}

const std::string& BitmapLayer::getName(void) const
{
  return mName;
}

VNBLayerType BitmapLayer::getType(void) const
{
  return mType;
}

BitmapNode& BitmapLayer::getNode(void) const
{
  return mNode;
}

BitmapLayer::BitmapLayer(VLayerID ID, const std::string& name, BitmapNode& node, VNBLayerType type):
  mID(ID),
  mName(name),
  mNode(node),
  mType(type)
{
}

void BitmapLayer::initialize(void)
{
  verse_callback_set((void*) verse_send_b_tile_set,
                     (void*) receiveTileSet,
		     NULL);
}

void BitmapLayer::receiveTileSet(void* user,
                                 VNodeID nodeID,
                                 VLayerID layerID,
				 uint16 tileX,
				 uint16 tileY,
				 uint16 z,
				 VNBLayerType type,
				 const VNBTile* data)
{
  Session* session = Session::getCurrent();

  BitmapNode* node = dynamic_cast<BitmapNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  BitmapLayer* layer = node->getLayerByID(layerID);
  if (!layer)
    return;

  if (layer->mType != type)
    return;

  // TODO: Check bounds.
  // TODO: Notify observers.
  // TODO: Blit tile.
}

//---------------------------------------------------------------------

void BitmapLayerObserver::setTile(BitmapLayer& layer, uint16 tileX, uint16 tileY, uint16 z, const VNBTile& tile)
{
}

void BitmapLayerObserver::onSetType(BitmapLayer& layer, VNBLayerType type)
{
}

void BitmapLayerObserver::onSetName(BitmapLayer& layer, const std::string name)
{
}

void BitmapLayerObserver::onDestroy(BitmapLayer& layer)
{
}

//---------------------------------------------------------------------

void BitmapNode::createLayer(const std::string& name, VNBLayerType type)
{
  getSession().push();
  verse_send_b_layer_create(getID(), (VLayerID) ~0, name.c_str(), type);
  getSession().pop();
}

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
  getSession().push();
  verse_send_b_dimensions_set(getID(), width, height, depth);
  getSession().pop();
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

unsigned int BitmapNode::getDimensionCount(void) const
{
  if (mDepth > 1)
    return 3;
  if (mHeight > 1)
    return 2;
  return 1;
}

BitmapNode::BitmapNode(VNodeID ID, VNodeOwner owner, Session& session):
  Node(ID, V_NT_BITMAP, owner, session),
  mWidth(0),
  mHeight(0),
  mDepth(0)
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

void BitmapNode::initialize(void)
{
  BitmapLayer::initialize();

  verse_callback_set((void*) verse_send_b_dimensions_set,
                     (void*) receiveDimensionsSet,
		     NULL);
  verse_callback_set((void*) verse_send_b_layer_create,
                     (void*) receiveLayerCreate,
		     NULL);
  verse_callback_set((void*) verse_send_b_layer_destroy,
                     (void*) receiveLayerDestroy,
		     NULL);
}

void BitmapNode::receiveDimensionsSet(void* user,
                                      VNodeID nodeID,
				      uint16 width,
				      uint16 height,
				      uint16 depth)
{
  Session* session = Session::getCurrent();

  BitmapNode* node = dynamic_cast<BitmapNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  // TODO: The code.

  node->mWidth = width;
  node->mHeight = height;
  node->mDepth = depth;
  node->updateDataVersion();
}

void BitmapNode::receiveLayerCreate(void* user,
                                    VNodeID nodeID,
				    VLayerID layerID,
				    const char* name,
				    VNBLayerType type)
{
  Session* session = Session::getCurrent();

  BitmapNode* node = dynamic_cast<BitmapNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  BitmapLayer* layer = node->getLayerByID(layerID);
  if (layer)
  {
    if (layer->mName != name)
    {
      const BitmapLayer::ObserverList& observers = layer->getObservers();
      for (BitmapLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
	(*i)->onSetName(*layer, name);

      layer->mName = name;
      layer->updateDataVersion();
    }

    if (type != layer->mType)
    {
      const BitmapLayer::ObserverList& observers = layer->getObservers();
      for (BitmapLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
	(*i)->onSetType(*layer, type);

      // TODO: Convert layer data.

      layer->mType = type;
      layer->updateDataVersion();
    }
  }
  else
  {
    layer = new BitmapLayer(layerID, name, *node, type);

    const BitmapNode::ObserverList& observers = node->getObservers();
    for (BitmapNode::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    {
      if (BitmapNodeObserver* observer = dynamic_cast<BitmapNodeObserver*>(*i))
        observer->onCreateLayer(*node, *layer);
    }

    node->mLayers.push_back(layer);
    node->updateStructureVersion();

    verse_send_b_layer_subscribe(nodeID, layerID, 0);
  }
}

void BitmapNode::receiveLayerDestroy(void* user, VNodeID nodeID, VLayerID layerID)
{
  Session* session = Session::getCurrent();

  BitmapNode* node = dynamic_cast<BitmapNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  BitmapNode::LayerList& layers = node->mLayers;
  for (BitmapNode::LayerList::iterator layer = layers.begin();  layer != layers.end();  layer++)
  {
    if ((*layer)->getID() == layerID)
    {
      // Notify layer observers.
      {
        const BitmapLayer::ObserverList& observers = (*layer)->getObservers();
        for (BitmapLayer::ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
          (*observer)->onDestroy(*(*layer));
      }

      // Notify node observers.
      const BitmapNode::ObserverList& observers = node->getObservers();
      for (BitmapNode::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      {
	if (BitmapNodeObserver* observer = dynamic_cast<BitmapNodeObserver*>(*i))
	  observer->onDestroyLayer(*node, *(*layer));
      }

      delete *layer;
      layers.erase(layer);

      node->updateStructureVersion();
      break;
    }
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

