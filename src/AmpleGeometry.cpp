
#include <verse.h>

#include <Ample.h>

namespace verse
{
  namespace ample
  {

//---------------------------------------------------------------------

struct Slot
{
  union
  {
    real64 real[4];
    uint32 uint[4];
    uint8 byte[4];
  };
};

//---------------------------------------------------------------------

void GeometryLayer::destroy(void)
{
  mNode.getSession().push();
  verse_send_g_layer_destroy(mNode.getID(), mID);
  mNode.getSession().pop();
}

void GeometryLayer::deleteSlot(uint32 slotID)
{
  mNode.getSession().push();
  if (isVertex())
    verse_send_g_vertex_delete_real64(mNode.getID(), slotID);
  else
    verse_send_g_polygon_delete(mNode.getID(), slotID);
  mNode.getSession().pop();
}

bool GeometryLayer::isVertex(void) const
{
  switch (mType)
  {
    case VN_G_LAYER_VERTEX_XYZ:
    case VN_G_LAYER_VERTEX_UINT32:
    case VN_G_LAYER_VERTEX_REAL:
      return true;
  }

  return false;
}

bool GeometryLayer::isPolygon(void) const
{
  switch (mType)
  {
    case VN_G_LAYER_POLYGON_CORNER_UINT32:
    case VN_G_LAYER_POLYGON_CORNER_REAL:
    case VN_G_LAYER_POLYGON_FACE_UINT8:
    case VN_G_LAYER_POLYGON_FACE_UINT32:
    case VN_G_LAYER_POLYGON_FACE_REAL:
      return true;
  }

  return false;
}

VLayerID GeometryLayer::getID(void) const
{
  return mID;
}

void GeometryLayer::setName(const std::string& name)
{
  mNode.getSession().push();
  verse_send_g_layer_create(mNode.getID(), mID, name.c_str(), mType, mDefaultInt, mDefaultReal);
  mNode.getSession().pop();
}

const std::string& GeometryLayer::getName(void) const
{
  return mName;
}

unsigned int GeometryLayer::getSlotSize(void) const
{
  return getTypeSize(mType);
}

VNGLayerType GeometryLayer::getType(void) const
{
  return mType;
}
  
bool GeometryLayer::getSlot(uint32 index, void* data) const
{
  if (isVertex())
  {
    if (!mNode.isVertex(index))
      return false;
  }
  else
  {
    if (!mNode.isPolygon(index))
      return false;
  }
  
  unsigned int slotSize = getTypeSize(mType);
  off_t offset = index * slotSize;

  bool defaults = offset + slotSize > mData.getCount();

  Slot* sourceSlot = reinterpret_cast<Slot*>(mData.getItems() + offset);
  Slot* targetSlot = reinterpret_cast<Slot*>(data);

  switch (mType)
  {
    case VN_G_LAYER_VERTEX_XYZ:
    case VN_G_LAYER_VERTEX_REAL:
    case VN_G_LAYER_POLYGON_CORNER_REAL:
    case VN_G_LAYER_POLYGON_FACE_REAL:
    {
      for (unsigned int i = 0;  i < getTypeElementCount(mType);  i++)
      {
	if (defaults)
	  targetSlot->real[i] = mDefaultReal;
	else
	  targetSlot->real[i] = sourceSlot->real[i];
      }
      break;
    }

    case VN_G_LAYER_VERTEX_UINT32:
    case VN_G_LAYER_POLYGON_CORNER_UINT32:
    case VN_G_LAYER_POLYGON_FACE_UINT32:
    {
      for (unsigned int i = 0;  i < getTypeElementCount(mType);  i++)
      {
	if (defaults)
	  targetSlot->uint[i] = mDefaultInt;
	else
	  targetSlot->uint[i] = sourceSlot->uint[i];
      }
      break;
    }
    
    case VN_G_LAYER_POLYGON_FACE_UINT8:
    {
      for (unsigned int i = 0;  i < getTypeElementCount(mType);  i++)
      {
	if (defaults)
	  targetSlot->byte[i] = mDefaultInt;
	else
	  targetSlot->byte[i] = sourceSlot->byte[i];
      }
      break;
    }
  }

  return true;
}

void GeometryLayer::setSlot(uint32 index, const void* data)
{
  mNode.getSession().push();

  switch (mType)
  {
    case VN_G_LAYER_VERTEX_XYZ:
    {
      const real64* values = reinterpret_cast<const real64*>(data);
      verse_send_g_vertex_set_xyz_real64(mNode.getID(), mID, index, values[0], values[1], values[2]);
      break;
    }

    case VN_G_LAYER_VERTEX_UINT32:
    {
      const uint32* value = reinterpret_cast<const uint32*>(data);
      verse_send_g_vertex_set_uint32(mNode.getID(), mID, index, *value);
      break;
    }

    case VN_G_LAYER_VERTEX_REAL:
    {
      const real64* value = reinterpret_cast<const real64*>(data);
      verse_send_g_vertex_set_real64(mNode.getID(), mID, index, *value);
      break;
    }
    
    case VN_G_LAYER_POLYGON_CORNER_UINT32:
    {
      const uint32* values = reinterpret_cast<const uint32*>(data);
      verse_send_g_polygon_set_corner_uint32(mNode.getID(), mID, index, values[0], values[1], values[2], values[3]);
      break;
    }
    
    case VN_G_LAYER_POLYGON_CORNER_REAL:
    {
      const real64* values = reinterpret_cast<const real64*>(data);
      verse_send_g_polygon_set_corner_real64(mNode.getID(), mID, index, values[0], values[1], values[2], values[3]);
      break;
    }
    
    case VN_G_LAYER_POLYGON_FACE_UINT8:
    {
      const uint8* value = reinterpret_cast<const uint8*>(data);
      verse_send_g_polygon_set_face_uint8(mNode.getID(), mID, index, *value);
      break;
    }

    case VN_G_LAYER_POLYGON_FACE_UINT32:
    {
      const uint32* value = reinterpret_cast<const uint32*>(data);
      verse_send_g_polygon_set_face_uint32(mNode.getID(), mID, index, *value);
      break;
    }

    case VN_G_LAYER_POLYGON_FACE_REAL:
    {
      const real64* value = reinterpret_cast<const real64*>(data);
      verse_send_g_polygon_set_face_real64(mNode.getID(), mID, index, *value);
      break;
    }
  }

  mNode.getSession().pop();
}

uint32 GeometryLayer::getDefaultInt(void) const
{
  return mDefaultInt;
}

real64 GeometryLayer::getDefaultReal(void) const
{
  return mDefaultReal;
}

GeometryNode& GeometryLayer::getNode(void) const
{
  return mNode;
}

GeometryLayer::GeometryLayer(VLayerID ID, const std::string& name, VNGLayerType type, GeometryNode& node, uint32 defaultInt, real64 defaultReal):
  mID(ID),
  mName(name),
  mType(type),
  mNode(node),
  mDefaultInt(defaultInt),
  mDefaultReal(defaultReal)
{
}

void GeometryLayer::reserve(unsigned int count)
{
  unsigned int slotSize = getTypeSize(mType);
  
  if (mData.getCount() < count * slotSize)
  {
    unsigned int extra = count - mData.getCount() / slotSize;

    mData.reserve(count * slotSize);

    for (unsigned int i = 0;  i < extra;  i++)
    {
    }
  }
}

unsigned int GeometryLayer::getTypeSize(VNGLayerType type)
{
  switch (type)
  {
    case VN_G_LAYER_VERTEX_XYZ:
      return sizeof(real64) * 3;
    case VN_G_LAYER_VERTEX_UINT32:
      return sizeof(uint32);
    case VN_G_LAYER_VERTEX_REAL:
      return sizeof(real64);
    case VN_G_LAYER_POLYGON_CORNER_UINT32:
      return sizeof(uint32) * 4;
    case VN_G_LAYER_POLYGON_CORNER_REAL:
      return sizeof(real64) * 4;
    case VN_G_LAYER_POLYGON_FACE_UINT8:
      return sizeof(uint8);
    case VN_G_LAYER_POLYGON_FACE_UINT32:
      return sizeof(uint32);
    case VN_G_LAYER_POLYGON_FACE_REAL:
      return sizeof(real64);
  }
}

//---------------------------------------------------------------------

void GeometryLayerObserver::onSetSlot(GeometryLayer& layer, uint16 slotID, const void* data)
{
}

void GeometryLayerObserver::onSetName(GeometryLayer& layer, const std::string& name)
{
}

//---------------------------------------------------------------------

void GeometryNode::createLayer(const std::string& name, VNGLayerType type, uint32 defaultInt, real64 defaultReal)
{
  getSession().push();
  verse_send_g_layer_create(getID(), ~0, name.c_str(), type, defaultInt, defaultReal);
  getSession().pop();
}

GeometryLayer* GeometryNode::getLayerByID(VLayerID ID)
{
  for (LayerList::iterator i = mLayers.begin();  i != mLayers.end();  i++)
    if ((*i)->getID() == ID)
      return *i;

  return NULL;
}

const GeometryLayer* GeometryNode::getLayerByID(VLayerID ID) const
{
  for (LayerList::const_iterator i = mLayers.begin();  i != mLayers.end();  i++)
    if ((*i)->getID() == ID)
      return *i;

  return NULL;
}

GeometryLayer* GeometryNode::getLayerByIndex(unsigned int index)
{
  return mLayers[index];
}

const GeometryLayer* GeometryNode::getLayerByIndex(unsigned int index) const
{
  return mLayers[index];
}

GeometryLayer* GeometryNode::getLayerByName(const std::string& name)
{
  for (LayerList::iterator i = mLayers.begin();  i != mLayers.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

const GeometryLayer* GeometryNode::getLayerByName(const std::string& name) const
{
  for (LayerList::const_iterator i = mLayers.begin();  i != mLayers.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

bool GeometryNode::isVertex(uint32 index) const
{
  if (index >= mValidVertices.size())
    return false;

  return mValidVertices[index];
}

bool GeometryNode::isPolygon(uint32 index) const
{
  if (index >= mValidPolygons.size())
    return false;

  return mValidPolygons[index];
}

bool GeometryNode::getVertex(uint32 index, void* data) const
{
  uint8* target = reinterpret_cast<uint8*>(data);

  for (LayerList::const_iterator i = mLayers.begin();  i != mLayers.end();  i++)
  {
    GeometryLayer* layer = *i;
    if (layer->isVertex())
    {
      layer->getSlot(index, target);
      target += layer->getSlotSize();
    }
  }
}

bool GeometryNode::getPolygon(uint32 index, void* data) const
{
  uint8* target = reinterpret_cast<uint8*>(data);

  for (LayerList::const_iterator i = mLayers.begin();  i != mLayers.end();  i++)
  {
    GeometryLayer* layer = *i;
    if (layer->isPolygon())
    {
      layer->getSlot(index, target);
      target += layer->getSlotSize();
    }
  }
}

unsigned int GeometryNode::getVertexSize(void) const
{
  unsigned int size = 0;

  for (LayerList::const_iterator i = mLayers.begin();  i != mLayers.end();  i++)
  {
    if ((*i)->isVertex())
      size += (*i)->getSlotSize();
  }

  return size;
}

unsigned int GeometryNode::getPolygonSize(void) const
{
  unsigned int size = 0;

  for (LayerList::const_iterator i = mLayers.begin();  i != mLayers.end();  i++)
  {
    if ((*i)->isPolygon())
      size += (*i)->getSlotSize();
  }

  return size;
}

GeometryNode::GeometryNode(VNodeID ID, VNodeOwner owner, Session& session):
  Node(ID, V_NT_GEOMETRY, owner, session)
{
}

GeometryNode::~GeometryNode(void)
{
  for (LayerList::iterator i = mLayers.begin();  i != mLayers.end();  i++)
    delete *i;
}

//---------------------------------------------------------------------

void GeometryNodeObserver::onCreateLayer(GeometryNode& node, GeometryLayer& layer)
{
}

void GeometryNodeObserver::onDestroyLayer(GeometryNode& node, GeometryLayer& layer)
{
}

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/

