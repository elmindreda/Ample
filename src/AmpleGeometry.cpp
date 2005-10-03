
#include <verse.h>

#include <Ample.h>

namespace verse
{
  namespace ample
  {

//---------------------------------------------------------------------

namespace
{

struct Slot
{
  union
  {
    real64 real[4];
    uint32 uint[4];
    uint8 byte[4];
  };
};

template <typename T>
inline void copySlot(T* target, const T* source, size_t count, T defaultValue = 0, bool useDefault = false)
{
  while (count--)
  {
    if (useDefault)
      *target++ = defaultValue;
    else
      *target++ = *source++;
  }
}

}

//---------------------------------------------------------------------

void GeometryLayer::destroy(void)
{
  mNode.getSession().push();
  verse_send_g_layer_destroy(mNode.getID(), mID);
  mNode.getSession().pop();
}

VLayerID GeometryLayer::getID(void) const
{
  return mID;
}

GeometryLayer::Stack GeometryLayer::getStack(void) const
{
  return mStack;
}

VNRealFormat GeometryLayer::getRealFormat(void) const
{
  return mFormat;
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
  return mData.getItemSize();
}

VNGLayerType GeometryLayer::getType(void) const
{
  return mType;
}
  
bool GeometryLayer::getSlot(uint32 slotID, void* data) const
{
  if (mStack == VERTEX)
  {
    if (!mNode.isVertex(slotID))
      return false;
  }
  else
  {
    if (!mNode.isPolygon(slotID))
      return false;
  }
  
  bool defaults = (slotID >= mData.getItemCount());

  Slot* targetSlot = reinterpret_cast<Slot*>(data);
  const Slot* sourceSlot = reinterpret_cast<const Slot*>(mData.getItem(slotID));

  switch (mType)
  {
    case VN_G_LAYER_VERTEX_XYZ:
    case VN_G_LAYER_VERTEX_REAL:
    case VN_G_LAYER_POLYGON_CORNER_REAL:
    case VN_G_LAYER_POLYGON_FACE_REAL:
    {
      copySlot(targetSlot->real,
               sourceSlot->real,
	       getTypeElementCount(mType),
	       mDefaultReal,
	       defaults);
      break;
    }

    case VN_G_LAYER_VERTEX_UINT32:
    case VN_G_LAYER_POLYGON_CORNER_UINT32:
    case VN_G_LAYER_POLYGON_FACE_UINT32:
    {
      copySlot(targetSlot->uint,
               sourceSlot->uint,
	       getTypeElementCount(mType),
	       mDefaultInt,
	       defaults);
      break;
    }
    
    case VN_G_LAYER_POLYGON_FACE_UINT8:
    {
      copySlot(targetSlot->byte,
               sourceSlot->byte,
	       getTypeElementCount(mType),
	       (uint8) mDefaultInt,
	       defaults);
      break;
    }
  }

  return true;
}

void GeometryLayer::setSlot(uint32 slotID, const void* data)
{
  mNode.getSession().push();

  const Slot* slot = reinterpret_cast<const Slot*>(data);

  switch (mType)
  {
    case VN_G_LAYER_VERTEX_XYZ:
    {
      verse_send_g_vertex_set_xyz_real64(mNode.getID(),
                                         mID,
					 slotID,
					 slot->real[0],
					 slot->real[1],
					 slot->real[2]);
      break;
    }

    case VN_G_LAYER_VERTEX_UINT32:
    {
      verse_send_g_vertex_set_uint32(mNode.getID(),
                                     mID,
				     slotID,
				     slot->uint[0]);
      break;
    }

    case VN_G_LAYER_VERTEX_REAL:
    {
      verse_send_g_vertex_set_real64(mNode.getID(),
                                     mID,
				     slotID,
				     slot->real[0]);
      break;
    }
    
    case VN_G_LAYER_POLYGON_CORNER_UINT32:
    {
      verse_send_g_polygon_set_corner_uint32(mNode.getID(),
					     mID,
					     slotID,
					     slot->uint[0],
					     slot->uint[1],
					     slot->uint[2],
					     slot->uint[3]);
      break;
    }
    
    case VN_G_LAYER_POLYGON_CORNER_REAL:
    {
      verse_send_g_polygon_set_corner_real64(mNode.getID(),
                                             mID,
					     slotID,
					     slot->real[0],
					     slot->real[1],
					     slot->real[2],
					     slot->real[3]);
      break;
    }
    
    case VN_G_LAYER_POLYGON_FACE_UINT8:
    {
      verse_send_g_polygon_set_face_uint8(mNode.getID(),
                                          mID,
					  slotID,
					  slot->byte[0]);
      break;
    }

    case VN_G_LAYER_POLYGON_FACE_UINT32:
    {
      verse_send_g_polygon_set_face_uint32(mNode.getID(),
                                           mID,
					   slotID,
					   slot->uint[0]);
      break;
    }

    case VN_G_LAYER_POLYGON_FACE_REAL:
    {
      verse_send_g_polygon_set_face_real64(mNode.getID(),
                                           mID,
					   slotID,
					   slot->real[0]);
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
  mData.setItemSize(getTypeSize(mType));
  mData.setGranularity(1024);

  switch (mType)
  {
    case VN_G_LAYER_VERTEX_XYZ:
    case VN_G_LAYER_VERTEX_UINT32:
    case VN_G_LAYER_VERTEX_REAL:
    {
      mStack = VERTEX;
      break;
    }

    default:
    {
      mStack = POLYGON;
      break;
    }
  }
}

void GeometryLayer::reserve(size_t slotCount)
{
  if (slotCount <= mData.getItemCount())
    return;

  size_t baseCount = mData.getItemCount();

  mData.reserve(slotCount);

  for (unsigned int i = baseCount;  i < mData.getItemCount();  i++)
  {
    Slot* targetSlot = reinterpret_cast<Slot*>(mData.getItem(i));

    switch (mType)
    {
      case VN_G_LAYER_VERTEX_XYZ:
      {
	copySlot(targetSlot->real,
		 (real64*) NULL,
		 getTypeElementCount(mType),
		 ((getID() == 0) ? V_REAL64_MAX : mDefaultReal),
		 true);
	break;
      }

      case VN_G_LAYER_VERTEX_REAL:
      case VN_G_LAYER_POLYGON_CORNER_REAL:
      case VN_G_LAYER_POLYGON_FACE_REAL:
      {
	copySlot(targetSlot->real,
		 (real64*) NULL,
		 getTypeElementCount(mType),
		 mDefaultReal,
		 true);
	break;
      }

      case VN_G_LAYER_VERTEX_UINT32:
      case VN_G_LAYER_POLYGON_CORNER_UINT32:
      case VN_G_LAYER_POLYGON_FACE_UINT32:
      {
	copySlot(targetSlot->uint,
		 (uint32*) NULL,
		 getTypeElementCount(mType),
		 ((getID() == 1) ? INVALID_VERTEX_ID : mDefaultInt),
		 true);
	break;
      }
      
      case VN_G_LAYER_POLYGON_FACE_UINT8:
      {
	copySlot(targetSlot->byte,
		 (uint8*) NULL,
		 getTypeElementCount(mType),
		 (uint8) mDefaultInt,
		 true);
	break;
      }
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

unsigned int GeometryLayer::getTypeElementCount(VNGLayerType type)
{
  switch (type)
  {
    case VN_G_LAYER_VERTEX_UINT32:
    case VN_G_LAYER_VERTEX_REAL:
    case VN_G_LAYER_POLYGON_FACE_UINT8:
    case VN_G_LAYER_POLYGON_FACE_UINT32:
    case VN_G_LAYER_POLYGON_FACE_REAL:
      return 1;
    case VN_G_LAYER_VERTEX_XYZ:
      return 3;
    case VN_G_LAYER_POLYGON_CORNER_UINT32:
    case VN_G_LAYER_POLYGON_CORNER_REAL:
      return 4;
  }
}

void GeometryLayer::initialize(void)
{
  verse_callback_set((void*) verse_send_g_vertex_set_xyz_real32,
                     (void*) receiveVertexSetXyzReal32,
                     NULL);
  verse_callback_set((void*) verse_send_g_vertex_delete_real32,
                     (void*) receiveVertexDeleteReal32,
                     NULL);
  verse_callback_set((void*) verse_send_g_vertex_set_xyz_real64,
                     (void*) receiveVertexSetXyzReal64,
                     NULL);
  verse_callback_set((void*) verse_send_g_vertex_delete_real64,
                     (void*) receiveVertexDeleteReal64,
                     NULL);
  verse_callback_set((void*) verse_send_g_vertex_set_uint32,
                     (void*) receiveVertexSetUint32,
                     NULL);
  verse_callback_set((void*) verse_send_g_vertex_set_real64,
                     (void*) receiveVertexSetReal64,
                     NULL);
  verse_callback_set((void*) verse_send_g_vertex_set_real32,
                     (void*) receiveVertexSetReal32,
                     NULL);
  verse_callback_set((void*) verse_send_g_polygon_set_corner_uint32,
                     (void*) receivePolygonSetCornerUint32,
                     NULL);
  verse_callback_set((void*) verse_send_g_polygon_delete,
                     (void*) receivePolygonDelete,
                     NULL);
  verse_callback_set((void*) verse_send_g_polygon_set_corner_real64,
                     (void*) receivePolygonSetCornerReal64,
                     NULL);
  verse_callback_set((void*) verse_send_g_polygon_set_corner_real32,
                     (void*) receivePolygonSetCornerReal32,
                     NULL);
  verse_callback_set((void*) verse_send_g_polygon_set_face_uint8,
                     (void*) receivePolygonSetFaceUint8,
                     NULL);
  verse_callback_set((void*) verse_send_g_polygon_set_face_uint32,
                     (void*) receivePolygonSetFaceUint32,
                     NULL);
  verse_callback_set((void*) verse_send_g_polygon_set_face_real64,
                     (void*) receivePolygonSetFaceReal64,
                     NULL);
  verse_callback_set((void*) verse_send_g_polygon_set_face_real32,
                     (void*) receivePolygonSetFaceReal32,
                     NULL);
}

void GeometryLayer::receiveVertexSetXyzReal32(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, real32 x, real32 y, real32 z)
{
}

void GeometryLayer::receiveVertexDeleteReal32(void* user, VNodeID nodeID, uint32 vertexID)
{
}

void GeometryLayer::receiveVertexSetXyzReal64(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, real64 x, real64 y, real64 z)
{
  Session* session = Session::getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_VERTEX_XYZ)
    return;
  
  BaseVertex vertex(x, y, z);

  const GeometryLayer::ObserverList& observers = layer->getObservers();
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, vertexID, &vertex);

  bool created = (layerID == 0 && !node->isVertex(vertexID));

  if (!created)
  {
    const GeometryNode::ObserverList& observers = node->getObservers();
    for (GeometryNode::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    {
      if (GeometryNodeObserver* observer = dynamic_cast<GeometryNodeObserver*>(*i))
	observer->onChangeBaseVertex(*node, vertexID, vertex);
    }
  }

  layer->reserve(vertexID + 1);

  Slot& targetSlot = *reinterpret_cast<Slot*>(layer->mData.getItem(vertexID));
  targetSlot.real[0] = vertex.x;
  targetSlot.real[1] = vertex.y;
  targetSlot.real[2] = vertex.z;

  if (created)
  {
    node->mVertexCount++;
    if (node->mHighestVertexID == INVALID_VERTEX_ID || vertexID > node->mHighestVertexID)
      node->mHighestVertexID = vertexID;

    layer->updateStructure();

    const GeometryNode::ObserverList& observers = node->getObservers();
    for (GeometryNode::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    {
      if (GeometryNodeObserver* observer = dynamic_cast<GeometryNodeObserver*>(*i))
	observer->onCreateVertex(*node, vertexID, vertex);
    }
  }
  else
    layer->updateData();
}

void GeometryLayer::receiveVertexDeleteReal64(void* user, VNodeID nodeID, uint32 vertexID)
{
  Session* session = Session::getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node || !node->mBaseVertexLayer)
    return;

  BaseVertex* vertex = reinterpret_cast<BaseVertex*>(node->mBaseVertexLayer->mData.getItem(vertexID));
  if (!vertex || !vertex->isValid())
    return;

  const GeometryNode::ObserverList& observers = node->getObservers();
  for (GeometryNode::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
  {
    if (GeometryNodeObserver* observer = dynamic_cast<GeometryNodeObserver*>(*i))
      observer->onDeleteVertex(*node, vertexID);
  }

  vertex->setInvalid();

  if (vertexID == node->mHighestVertexID)
  {
    BaseVertex* vertices = reinterpret_cast<BaseVertex*>(node->mBaseVertexLayer->mData.getItems());

    uint32 index = vertexID;
    while (index--)
    {
      if (vertices[index].isValid())
      {
	node->mHighestVertexID = index;
	break;
      }
    }

    if (node->mHighestVertexID != index)
      node->mHighestVertexID = INVALID_VERTEX_ID;
  }

  node->mVertexCount--;
  node->mBaseVertexLayer->updateStructure();
}

void GeometryLayer::receiveVertexSetUint32(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, uint32 value)
{
  Session* session = Session::getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_VERTEX_UINT32)
    return;
  
  const GeometryLayer::ObserverList& observers = layer->getObservers();
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, vertexID, &value);

  layer->reserve(vertexID + 1);
  layer->updateData();

  Slot& targetSlot = *reinterpret_cast<Slot*>(layer->mData.getItem(vertexID));
  targetSlot.uint[0] = value;
}

void GeometryLayer::receiveVertexSetReal64(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, real64 value)
{
  Session* session = Session::getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_VERTEX_REAL)
    return;
  
  const GeometryLayer::ObserverList& observers = layer->getObservers();
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, vertexID, &value);

  layer->reserve(vertexID + 1);
  layer->updateData();

  Slot& targetSlot = *reinterpret_cast<Slot*>(layer->mData.getItem(vertexID));
  targetSlot.real[0] = value;
}

void GeometryLayer::receiveVertexSetReal32(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, real32 value)
{
}

void GeometryLayer::receivePolygonSetCornerUint32(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, uint32 v0, uint32 v1, uint32 v2, uint32 v3)
{
  Session* session = Session::getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_POLYGON_CORNER_UINT32)
    return;
  
  BasePolygon polygon(v0, v1, v2, v3);

  const GeometryLayer::ObserverList& observers = layer->getObservers();
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, polygonID, &polygon);

  bool created = (layerID == 1 && !node->isPolygon(polygonID));

  if (!created)
  {
    const GeometryNode::ObserverList& observers = node->getObservers();
    for (GeometryNode::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    {
      if (GeometryNodeObserver* observer = dynamic_cast<GeometryNodeObserver*>(*i))
	observer->onChangeBasePolygon(*node, polygonID, polygon);
    }
  }

  layer->reserve(polygonID + 1);

  Slot& targetSlot = *reinterpret_cast<Slot*>(layer->mData.getItem(polygonID));
  copySlot(targetSlot.uint, polygon.mIndices, 4);

  if (created)
  {
    node->mPolygonCount++;
    if (node->mHighestPolygonID == INVALID_POLYGON_ID || polygonID > node->mHighestPolygonID)
      node->mHighestPolygonID = polygonID;

    layer->updateStructure();

    const GeometryNode::ObserverList& observers = node->getObservers();
    for (GeometryNode::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    {
      if (GeometryNodeObserver* observer = dynamic_cast<GeometryNodeObserver*>(*i))
	observer->onCreatePolygon(*node, polygonID, polygon);
    }
  }
  else
    layer->updateData();
}

void GeometryLayer::receivePolygonDelete(void* user, VNodeID nodeID, uint32 polygonID)
{
  Session* session = Session::getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node || !node->mBasePolygonLayer)
    return;

  BasePolygon* polygon = reinterpret_cast<BasePolygon*>(node->mBasePolygonLayer->mData.getItem(polygonID));
  if (!polygon)
    return;

  const GeometryNode::ObserverList& observers = node->getObservers();
  for (GeometryNode::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
  {
    if (GeometryNodeObserver* observer = dynamic_cast<GeometryNodeObserver*>(*i))
      observer->onDeletePolygon(*node, polygonID);
  }

  polygon->setInvalid();

  if (polygonID == node->mHighestPolygonID)
  {
    BasePolygon* polygons = reinterpret_cast<BasePolygon*>(node->mBasePolygonLayer->mData.getItems());

    uint32 index = polygonID;
    while (index--)
    {
      if (polygons[index].isValid())
      {
	node->mHighestPolygonID = index;
	break;
      }
    }

    if (node->mHighestPolygonID != index)
      node->mHighestPolygonID = INVALID_POLYGON_ID;
  }

  node->mPolygonCount--;
  node->mBasePolygonLayer->updateStructure();
}

void GeometryLayer::receivePolygonSetCornerReal64(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, real64 v0, real64 v1, real64 v2, real64 v3)
{
  Session* session = Session::getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_POLYGON_CORNER_REAL)
    return;
  
  Slot sourceSlot;
  sourceSlot.real[0] = v0;
  sourceSlot.real[1] = v1;
  sourceSlot.real[2] = v2;
  sourceSlot.real[3] = v3;

  const GeometryLayer::ObserverList& observers = layer->getObservers();
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, polygonID, &sourceSlot);

  layer->reserve(polygonID + 1);
  layer->updateData();

  Slot& targetSlot = *reinterpret_cast<Slot*>(layer->mData.getItem(polygonID));
  copySlot(targetSlot.real, sourceSlot.real, 4);
}

void GeometryLayer::receivePolygonSetCornerReal32(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, real32 v0, real32 v1, real32 v2, real32 v3)
{
}

void GeometryLayer::receivePolygonSetFaceUint8(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, uint8 value)
{
  Session* session = Session::getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_POLYGON_FACE_UINT8)
    return;
  
  const GeometryLayer::ObserverList& observers = layer->getObservers();
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, polygonID, &value);

  layer->reserve(polygonID + 1);
  layer->updateData();

  Slot& targetSlot = *reinterpret_cast<Slot*>(layer->mData.getItem(polygonID));
  targetSlot.byte[0] = value;
}

void GeometryLayer::receivePolygonSetFaceUint32(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, uint32 value)
{
  Session* session = Session::getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_POLYGON_FACE_UINT32)
    return;
  
  const GeometryLayer::ObserverList& observers = layer->getObservers();
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, polygonID, &value);

  layer->reserve(polygonID + 1);
  layer->updateData();

  Slot& targetSlot = *reinterpret_cast<Slot*>(layer->mData.getItem(polygonID));
  targetSlot.uint[0] = value;
}

void GeometryLayer::receivePolygonSetFaceReal64(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, real64 value)
{
  Session* session = Session::getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_POLYGON_FACE_REAL)
    return;
  
  const GeometryLayer::ObserverList& observers = layer->getObservers();
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, polygonID, &value);

  layer->reserve(polygonID + 1);
  layer->updateData();

  Slot& targetSlot = *reinterpret_cast<Slot*>(layer->mData.getItem(polygonID));
  targetSlot.real[0] = value;
}

void GeometryLayer::receivePolygonSetFaceReal32(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, real32 value)
{
}

//---------------------------------------------------------------------

void GeometryLayerObserver::onSetSlot(GeometryLayer& layer, uint32 slotID, const void* data)
{
}

void GeometryLayerObserver::onSetName(GeometryLayer& layer, const std::string& name)
{
}

void GeometryLayerObserver::onDestroy(GeometryLayer& layer)
{
}

//---------------------------------------------------------------------

void GeometryNode::createLayer(const std::string& name, VNGLayerType type, uint32 defaultInt, real64 defaultReal)
{
  getSession().push();
  verse_send_g_layer_create(getID(), 0xffffffff, name.c_str(), type, defaultInt, defaultReal);
  getSession().pop();
}

bool GeometryNode::getBaseMesh(BaseMesh& mesh)
{
  if (!mBaseVertexLayer || !mBasePolygonLayer)
    return false;

  // Retrieve all valid polygons.

  for (uint32 i = 0;  i < mBasePolygonLayer->mData.getItemCount();  i++)
  {
    BasePolygon polygon;

    if (getBasePolygon(i, polygon))
    {
      if (!isVertex(polygon.mIndices[3]))
	polygon.mIndices[3] = INVALID_VERTEX_ID;
      mesh.mPolygons.push_back(polygon);
    }
  }

  if (!mesh.mPolygons.size())
    return false;

  // Remap vertex indices.

  VertexIndexMap indices;

  for (BaseMesh::PolygonList::iterator polygon = mesh.mPolygons.begin();  polygon != mesh.mPolygons.end();  polygon++)
  {
    for (unsigned int i = 0;  i < 4;  i++)
    {
      uint32 index = (*polygon).mIndices[i];
      if (index == INVALID_VERTEX_ID)
	break;

      VertexIndexMap::iterator mapper = indices.find(index);
      if (mapper == indices.end())
      {
	const uint32 value = indices.size();
	indices[index] = value;
	mapper = indices.find(index);
      }

      (*polygon).mIndices[i] = (*mapper).second;
    }
  }

  // Copy vertices into new indices.

  mesh.mVertices.resize(indices.size());
  
  for (VertexIndexMap::const_iterator i = indices.begin();  i != indices.end();  i++)
    getBaseVertex((*i).first, mesh.mVertices[(*i).second]);

  return true;
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

bool GeometryNode::isVertex(uint32 vertexID) const
{
  if (!mBaseVertexLayer)
    return false;

  const BaseVertex* vertex = reinterpret_cast<BaseVertex*>(mBaseVertexLayer->mData.getItem(vertexID));
  if (!vertex || !vertex->isValid())
    return false;

  return true;
}

bool GeometryNode::isPolygon(uint32 polygonID) const
{
  if (!mBasePolygonLayer)
    return false;

  const BasePolygon* polygon = reinterpret_cast<BasePolygon*>(mBasePolygonLayer->mData.getItem(polygonID));
  if (!polygon)
    return false;

  for (unsigned int i = 0;  i < 3;  i++)
  {
    if (!isVertex(polygon->mIndices[i]))
      return false;
  }

  return true;
}

bool GeometryNode::getBaseVertex(uint32 vertexID, BaseVertex& vertex) const
{
  if (!mBaseVertexLayer)
    return false;

  const BaseVertex* result = reinterpret_cast<BaseVertex*>(mBaseVertexLayer->mData.getItem(vertexID));
  if (!result || !result->isValid())
    return false;

  vertex = *result;
  return true;
}

bool GeometryNode::getBasePolygon(uint32 polygonID, BasePolygon& polygon) const
{
  if (!mBasePolygonLayer)
    return false;

  const BasePolygon* result = reinterpret_cast<BasePolygon*>(mBasePolygonLayer->mData.getItem(polygonID));
  if (!result)
    return false;

  for (unsigned int i = 0;  i < 3;  i++)
  {
    if (!isVertex(result->mIndices[i]))
      return false;
  }

  polygon = *result;
  return true;
}

void GeometryNode::setBaseVertex(uint32 vertexID, const BaseVertex& vertex)
{
  getSession().push();
  verse_send_g_vertex_set_xyz_real64(getID(),
                                     0,
				     vertexID,
				     vertex.x,
				     vertex.y,
				     vertex.z);
  getSession().pop();
}

void GeometryNode::setBasePolygon(uint32 polygonID, const BasePolygon& polygon)
{
  getSession().push();
  verse_send_g_polygon_set_corner_uint32(getID(),
                                         1,
					 polygonID,
					 polygon.mIndices[0],
					 polygon.mIndices[1],
					 polygon.mIndices[2],
					 polygon.mIndices[3]);
  getSession().pop();
}

void GeometryNode::deleteVertex(uint32 vertexID)
{
  getSession().push();
  verse_send_g_vertex_delete_real64(getID(), vertexID);
  getSession().pop();
}

void GeometryNode::deletePolygon(uint32 polygonID)
{
  getSession().push();
  verse_send_g_polygon_delete(getID(), polygonID);
  getSession().pop();
}

size_t GeometryNode::getVertexSize(void) const
{
  unsigned int size = 0;

  for (LayerList::const_iterator i = mLayers.begin();  i != mLayers.end();  i++)
  {
    if ((*i)->getStack() == GeometryLayer::VERTEX)
      size += (*i)->getSlotSize();
  }

  return size;
}

size_t GeometryNode::getPolygonSize(void) const
{
  unsigned int size = 0;

  for (LayerList::const_iterator i = mLayers.begin();  i != mLayers.end();  i++)
  {
    if ((*i)->getStack() == GeometryLayer::POLYGON)
      size += (*i)->getSlotSize();
  }

  return size;
}

GeometryLayer* GeometryNode::getVertexCreaseLayer(void)
{
  GeometryLayer* layer = getLayerByName(mVertexCreases);
  if (!layer || layer->getType() != VN_G_LAYER_VERTEX_UINT32)
    return NULL;

  return layer;
}

const std::string& GeometryNode::getVertexCreaseLayerName(void) const
{
  return mVertexCreases;
}

GeometryLayer* GeometryNode::getEdgeCreaseLayer(void)
{
  GeometryLayer* layer = getLayerByName(mEdgeCreases);
  if (!layer || layer->getType() != VN_G_LAYER_POLYGON_CORNER_UINT32)
    return NULL;

  return layer;
}

const std::string& GeometryNode::getEdgeCreaseLayerName(void) const
{
  return mEdgeCreases;
}

uint32 GeometryNode::getHighestVertexID(void) const
{
  return mHighestVertexID;
}

uint32 GeometryNode::getHighestPolygonID(void) const
{
  return mHighestPolygonID;
}

uint32 GeometryNode::getVertexCount(void) const
{
  return mVertexCount;
}

uint32 GeometryNode::getPolygonCount(void) const
{
  return mPolygonCount;
}

GeometryNode::GeometryNode(VNodeID ID, VNodeOwner owner, Session& session):
  Node(ID, V_NT_GEOMETRY, owner, session),
  mBaseVertexLayer(NULL),
  mBasePolygonLayer(NULL),
  mVertexDefaultCrease(0),
  mEdgeDefaultCrease(0),
  mHighestVertexID(INVALID_VERTEX_ID),
  mHighestPolygonID(INVALID_POLYGON_ID),
  mVertexCount(0),
  mPolygonCount(0)
{
}

GeometryNode::~GeometryNode(void)
{
  while (mLayers.size())
  {
    delete mLayers.back();
    mLayers.pop_back();
  }
}

void GeometryNode::initialize(void)
{
  GeometryLayer::initialize();

  verse_callback_set((void*) verse_send_g_layer_create,
                     (void*) receiveGeometryLayerCreate,
                     NULL);
  verse_callback_set((void*) verse_send_g_layer_destroy,
                     (void*) receiveGeometryLayerDestroy,
                     NULL);
  verse_callback_set((void*) verse_send_g_crease_set_vertex,
                     (void*) receiveCreaseSetVertex,
                     NULL);
  verse_callback_set((void*) verse_send_g_crease_set_edge,
                     (void*) receiveCreaseSetEdge,
                     NULL);
  verse_callback_set((void*) verse_send_g_bone_create,
                     (void*) receiveBoneCreate,
                     NULL);
  verse_callback_set((void*) verse_send_g_bone_destroy,
                     (void*) receiveBoneDestroy,
                     NULL);
}

void GeometryNode::receiveGeometryLayerCreate(void* data, VNodeID ID, VLayerID layerID, const char* name, VNGLayerType type, uint32 defaultInt, real64 defaultReal)
{
  Session* session = Session::getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(ID));
  if (!node)
    return;

  // TODO: Refactor the lot.

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (layer)
  {
    if (layer->getType() == type)
    {
      // This was a "change name" command.
      // NOTE: We don't care about announcing changes in default values. Who does?

      const GeometryLayer::ObserverList& observers = layer->getObservers();
      for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
	(*i)->onSetName(*layer, name);

      layer->mDefaultInt = defaultInt;
      layer->mDefaultReal = defaultReal;
      layer->mName = name;
      layer->updateData();
    }
    else
    {
      // This was a "change type" command
      // Discard the current layer and re-subscribe.
      
      // NOTE: This is bad. Don't do this.
      receiveGeometryLayerDestroy(data, ID, layerID);

      // NOTE: This is bad. Don't do this.
      receiveGeometryLayerCreate(data, ID, layerID, name, type, defaultInt, defaultReal);
    }
  }
  else
  {
    // No previous local layer object existed, so this is a create command.

    layer = new GeometryLayer(layerID, name, type, *node, defaultInt, defaultReal);
    node->mLayers.push_back(layer);
    node->updateStructure();

    if (layer->getID() == 0)
      node->mBaseVertexLayer = layer;
    else if (layer->getID() == 1)
      node->mBasePolygonLayer = layer;

    const Node::ObserverList& observers = node->getObservers();
    for (Node::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    {
      if (GeometryNodeObserver* observer = dynamic_cast<GeometryNodeObserver*>(*i))
	observer->onCreateLayer(*node, *layer);
    }

    verse_send_g_layer_subscribe(ID, layerID, VN_FORMAT_REAL64);
  }
}

void GeometryNode::receiveGeometryLayerDestroy(void* data, VNodeID ID, VLayerID layerID)
{
  Session* session = Session::getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(ID));
  if (!node)
    return;

  GeometryNode::LayerList& layers = node->mLayers;
  for (GeometryNode::LayerList::iterator layer = layers.begin();  layer != layers.end();  layer++)
  {
    if ((*layer)->getID() == layerID)
    {
      // Notify geometry layer observers.
      {
        const GeometryLayer::ObserverList& observers = (*layer)->getObservers();
        for (GeometryLayer::ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
          (*observer)->onDestroy(*(*layer));
      }

      // Notify geometry node observers.
      const GeometryNode::ObserverList& observers = node->getObservers();
      for (GeometryNode::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      {
	if (GeometryNodeObserver* observer = dynamic_cast<GeometryNodeObserver*>(*i))
	  observer->onDestroyLayer(*node, *(*layer));
      }
      
      delete *layer;
      layers.erase(layer);

      node->updateStructure();
      break;
    }
  }
}

void GeometryNode::receiveCreaseSetVertex(void* user, VNodeID nodeID, const char *layer, uint32 def_crease)
{
  Session* session = Session::getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  node->mVertexCreases = layer;
  node->mVertexDefaultCrease = def_crease;
}

void GeometryNode::receiveCreaseSetEdge(void* user, VNodeID nodeID, const char *layer, uint32 def_crease)
{
  Session* session = Session::getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  node->mEdgeCreases = layer;
  node->mEdgeDefaultCrease = def_crease;
}

void GeometryNode::receiveBoneCreate(void* user, VNodeID nodeID, uint16 bone_id, const char *weight, const char *reference, uint32 parent, real64 pos_x, real64 pos_y, real64 pos_z, real64 rot_x, real64 rot_y, real64 rot_z, real64 rot_w)
{
  // TODO: Design bone management.
}

void GeometryNode::receiveBoneDestroy(void* user, VNodeID nodeID, uint16 bone_id)
{
  // TODO: Design bone management.
}

//---------------------------------------------------------------------

void GeometryNodeObserver::onCreateVertex(GeometryNode& node, uint32 vertexID, const BaseVertex& vertex)
{
}

void GeometryNodeObserver::onChangeBaseVertex(GeometryNode& node, uint32 vertexID, const BaseVertex& vertex)
{
}

void GeometryNodeObserver::onDeleteVertex(GeometryNode& node, uint32 vertexID)
{
}
  
void GeometryNodeObserver::onCreatePolygon(GeometryNode& node, uint32 polygonID, const BasePolygon& polygon)
{
}

void GeometryNodeObserver::onChangeBasePolygon(GeometryNode& node, uint32 polygonID, const BasePolygon& polygon)
{
}

void GeometryNodeObserver::onDeletePolygon(GeometryNode& node, uint32 polygonID)
{
}

void GeometryNodeObserver::onCreateLayer(GeometryNode& node, GeometryLayer& layer)
{
}

void GeometryNodeObserver::onDestroyLayer(GeometryNode& node, GeometryLayer& layer)
{
}

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/

