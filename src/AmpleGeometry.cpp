
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
inline void copySlot(T* target, const T* source, size_t count, T defaultValue, bool useDefault = false)
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
  return getTypeSize(mType);
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
  if (slotCount > mData.getItemCount())
  {
    size_t baseCount = mData.getItemCount();

    mData.reserve(slotCount);

    for (unsigned int i = baseCount;  i < slotCount;  i++)
    {
      Slot* targetSlot = reinterpret_cast<Slot*>(mData.getItem(i));

      switch (mType)
      {
	case VN_G_LAYER_VERTEX_XYZ:
	{
	  copySlot(targetSlot->real,
		   (real64*) NULL,
		   getTypeElementCount(mType),
		   V_REAL64_MAX,
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
		   mDefaultInt,
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

//---------------------------------------------------------------------

void GeometryLayerObserver::onSetSlot(GeometryLayer& layer, uint32 slotID, const void* data)
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

bool GeometryNode::isVertex(uint32 vertexID) const
{
  if (!mBaseVertexLayer)
    return false;

  const BaseVertex* vertex = reinterpret_cast<BaseVertex*>(mBaseVertexLayer->mData.getItem(vertexID));
  if (!vertex)
    return false;

  if (vertex->x == V_REAL64_MAX && vertex->y == V_REAL64_MAX && vertex->z == V_REAL64_MAX)
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
  if (!result)
    return false;

  if (result->x == V_REAL64_MAX && result->y == V_REAL64_MAX && result->z == V_REAL64_MAX)
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

GeometryNode::GeometryNode(VNodeID ID, VNodeOwner owner, Session& session):
  Node(ID, V_NT_GEOMETRY, owner, session),
  mBaseVertexLayer(NULL),
  mBasePolygonLayer(NULL)
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

//---------------------------------------------------------------------

void GeometryNodeObserver::onCreateVertex(GeometryNode& node, uint32 vertexID, const BaseVertex& vertex)
{
}

void GeometryNodeObserver::onDeleteVertex(GeometryNode& node, uint32 vertexID)
{
}
  
void GeometryNodeObserver::onCreatePolygon(GeometryNode& node, uint32 polygonID, const BasePolygon& polygon)
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

