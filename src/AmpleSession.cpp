
#include <new>

#include <verse.h>

#include "Ample.h"

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

void Session::push(void)
{
  msStack.push(this);
  msCurrent = this;
  verse_session_set(mInternal);
}

void Session::pop(void)
{
  msStack.pop();
  if (msStack.empty())
    msCurrent = getCurrent();
  else
  {
    msCurrent = msStack.top();
    verse_session_set(msCurrent->mInternal);
  }
}

void Session::terminate(const std::string& byebye)
{
  push();
  verse_send_connect_terminate(mAddress.c_str(), byebye.c_str());
  pop();
}

void Session::createNode(const std::string& name, VNodeType type)
{
  push();
  verse_send_node_create(~1, type, VN_OWNER_MINE);
  pop();

  mPending.push_back(PendingNode(name, type));
}

Session::State Session::getState(void) const
{
  return mState;
}

Node* Session::getNodeByID(VNodeID ID)
{
  for (NodeList::iterator i = mNodes.begin();  i != mNodes.end();  i++)
    if ((*i)->getID() == ID)
      return *i;

  return NULL;
}

const Node* Session::getNodeByID(VNodeID ID) const
{
  for (NodeList::const_iterator i = mNodes.begin();  i != mNodes.end();  i++)
    if ((*i)->getID() == ID)
      return *i;

  return NULL;
}

Node* Session::getNodeByIndex(unsigned int index)
{
  return mNodes[index];
}

const Node* Session::getNodeByIndex(unsigned int index) const
{
  return mNodes[index];
}

Node* Session::getNodeByName(const std::string& name)
{
  for (NodeList::iterator i = mNodes.begin();  i != mNodes.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

const Node* Session::getNodeByName(const std::string& name) const
{
  for (NodeList::const_iterator i = mNodes.begin();  i != mNodes.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

Node* Session::getAvatarNode(void)
{
  return getNodeByID(mAvatarID);
}

const Node* Session::getAvatarNode(void) const
{
  return getNodeByID(mAvatarID);
}

unsigned int Session::getNodeCount(void) const
{
  return mNodes.size();
}

const std::string& Session::getAddress(void) const
{
  return mAddress;
}

Session* Session::create(const std::string& address,
                         const std::string& username,
			 const std::string& password,
			 unsigned int typeMask)
{
  if (!msInitialized)
  {
    verse_callback_set((void*) verse_send_connect_accept,
		       (void*) receiveAccept,
		       NULL);
    verse_callback_set((void*) verse_send_connect_terminate,
		       (void*) receiveTerminate,
		       NULL);
    verse_callback_set((void*) verse_send_node_create,
		       (void*) receiveNodeCreate,
		       NULL);
    verse_callback_set((void*) verse_send_node_destroy,
		       (void*) receiveNodeDestroy,
		       NULL);
    verse_callback_set((void*) verse_send_node_name_set,
		       (void*) receiveNodeNameSet,
		       NULL);
    verse_callback_set((void*) verse_send_tag_group_create,
                       (void*) receiveTagGroupCreate,
		       NULL);
    verse_callback_set((void*) verse_send_tag_group_destroy,
		       (void*) receiveTagGroupDestroy,
		       NULL);
    verse_callback_set((void*) verse_send_tag_create,
		       (void*) receiveTagCreate,
		       NULL);
    verse_callback_set((void*) verse_send_tag_destroy,
		       (void*) receiveTagDestroy,
		       NULL);
    verse_callback_set((void*) verse_send_t_set_language,
		       (void*) receiveNodeLanguageSet,
		       NULL);
    verse_callback_set((void*) verse_send_t_buffer_create,
		       (void*) receiveTextBufferCreate,
		       NULL);
    verse_callback_set((void*) verse_send_t_buffer_destroy,
		       (void*) receiveTextBufferDestroy,
		       NULL);
    verse_callback_set((void*) verse_send_t_text_set,
		       (void*) receiveTextBufferSet,
		       NULL);
    verse_callback_set((void*) verse_send_g_layer_create,
		       (void*) receiveGeometryLayerCreate,
		       NULL);
    verse_callback_set((void*) verse_send_g_layer_destroy,
		       (void*) receiveGeometryLayerDestroy,
		       NULL);
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

    msInitialized = true;
  }

  if (Session* session = find(address))
  {
    if (session->mState == TERMINATED)
      delete session;
    else
      return session;
  }

  VSession internal = verse_send_connect(username.c_str(), password.c_str(), address.c_str(), NULL);

  // TODO: Insert address translation here?

  Session* session = new Session(address, username, internal);

  if (typeMask)
    session->mTypeMask = typeMask;
  else
    session->mTypeMask = (1 << V_NT_NUM_TYPES) - 1;

  session->mAddress = address;

  return session;
}

Session* Session::find(const std::string& address)
{
  for (SessionList::iterator i = msSessions.begin();  i != msSessions.end();  i++)
  {
    if ((*i)->mAddress == address)
      return *i;
  }

  return NULL;
}

void Session::update(uint32 microseconds)
{
  for (SessionList::iterator i = msSessions.begin();  i != msSessions.end();  i++)
  {
    (*i)->push();
    verse_callback_update(microseconds);
    (*i)->pop();
  }
}

void Session::terminateAll(const std::string& byebye)
{
  for (SessionList::iterator i = msSessions.begin();  i != msSessions.end();  i++)
    (*i)->terminate(byebye);
}

Session* Session::getCurrent(void)
{
  if (!msCurrent)
  {
    VSession internal = verse_session_get();

    for (SessionList::iterator i = msSessions.begin();  i != msSessions.end();  i++)
    {
      if ((*i)->mInternal == internal)
      {
	msCurrent = *i;
	break;
      }
    }
  }
  
  return msCurrent;
}

Session::Session(const std::string& address, const std::string& username, VSession internal):
  mAddress(address),
  mUserName(username),
  mInternal(internal),
  mAvatarID(0xffffffff),
  mState(CONNECTING)
{
  msSessions.push_back(this);
}

Session::~Session(void)
{
  msSessions.remove(this);
}

void Session::receiveAccept(void* user, VNodeID avatarID, const char* address, uint8* hostID)
{
  Session* session = getCurrent();

  session->mAvatarID = avatarID;
  session->mState = CONNECTED;
  session->updateData();

  const ObserverList& observers = session->mObservers;
  for (ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onAccept(*session);

  // Subscribe to all node types
  verse_send_node_index_subscribe(session->mTypeMask);
}

void Session::receiveTerminate(void* user, const char* address, const char* byebye)
{
  Session* session = getCurrent();

  const ObserverList& observers = session->mObservers;
  for (ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onTerminate(*session, byebye);

  session->mState = TERMINATED;
  session->updateData();
}

void Session::receiveNodeCreate(void* user, VNodeID ID, VNodeType type, VNodeOwner owner)
{
  Session* session = getCurrent();

  // NOTE: This is bad. Don't do this.
  receiveNodeDestroy(user, ID);

  Node* node;
  
  switch (type)
  {
    case V_NT_OBJECT:
      node = new Node(ID, type, owner, *session); 
      break;
    case V_NT_GEOMETRY:
      node = new GeometryNode(ID, owner, *session); 
      break;
    case V_NT_MATERIAL:
      node = new Node(ID, type, owner, *session); 
      break;
    case V_NT_BITMAP:
      node = new Node(ID, type, owner, *session); 
      break;
    case V_NT_TEXT:
      node = new TextNode(ID, owner, *session); 
      break;
    case V_NT_CURVE:
      node = new Node(ID, type, owner, *session); 
      break;
    case V_NT_AUDIO:
      node = new Node(ID, type, owner, *session); 
      break;
  }

  if (!node)
    return;
  
  session->mNodes.push_back(node);
  session->updateStructure();

  if (owner == VN_OWNER_MINE)
  {
    for (PendingList::iterator i = session->mPending.begin();  i != session->mPending.end();  i++)
    {
      if ((*i).mType == type)
      {
	verse_send_node_name_set(ID, (*i).mName.c_str());
	session->mPending.erase(i);
	break;
      }
    }
  }

  const ObserverList& observers = session->mObservers;
  for (ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onCreateNode(*session, *node);

  verse_send_node_subscribe(ID);
}

void Session::receiveNodeDestroy(void* user, VNodeID ID)
{
  Session* session = getCurrent();
  
  NodeList& nodes = session->mNodes;
  for (NodeList::iterator node = nodes.begin();  node != nodes.end();  node++)
  {
    if ((*node)->getID() == ID)
    {
      const ObserverList& observers = session->mObservers;
      for (ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
	(*observer)->onDestroyNode(*session, *(*node));

      delete *node;
      nodes.erase(node);

      session->updateStructure();
      break;
    }
  }
}

void Session::receiveNodeNameSet(void* user, VNodeID ID, const char* name)
{
  Session* session = getCurrent();

  Node* node = session->getNodeByID(ID);
  if (!node)
    return;
  
  const Node::ObserverList& observers = node->mObservers;
  for (Node::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetName(*node, name);
  
  node->mName = name;
  node->updateData();
}

void Session::receiveTagGroupCreate(void* user, VNodeID ID, uint16 groupID, const char* name)
{
  Session* session = getCurrent();

  Node* node = session->getNodeByID(ID);
  if (!node)
    return;

  TagGroup* group = node->getTagGroupByID(groupID);
  if (group)
  {
    const TagGroup::ObserverList& observers = group->mObservers;
    for (TagGroup::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      (*i)->onSetName(*group, name);
      
    group->mName = name;
    group->updateData();
  }
  else
  {
    group = new TagGroup(groupID, name, *node);
    node->mGroups.push_back(group);
    node->updateStructure();
    
    const Node::ObserverList& observers = node->mObservers;
    for (Node::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      (*i)->onCreateTagGroup(*node, *group);
    
    verse_send_tag_group_subscribe(node->getID(), groupID);
  }
}

void Session::receiveTagGroupDestroy(void* user, VNodeID ID, uint16 groupID)
{
  Session* session = getCurrent();

  Node* node = session->getNodeByID(ID);
  if (!node)
    return;

  Node::TagGroupList& groups = node->mGroups;
  for (Node::TagGroupList::iterator group = groups.begin();  group != groups.end();  group++)
  {
    if ((*group)->getID() == groupID)
    {
      const Node::ObserverList& observers = node->mObservers;
      for (Node::ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
	(*observer)->onDestroyTagGroup(*node, *(*group));
      
      delete *group;
      groups.erase(group);

      node->updateStructure();
      break;
    }
  }
}

void Session::receiveTagCreate(void* user, VNodeID ID, uint16 groupID, uint16 tagID, const char* name, VNTagType type, const VNTag* value)
{
  Session* session = getCurrent();

  Node* node = session->getNodeByID(ID);
  if (!node)
    return;

  TagGroup* group = node->getTagGroupByID(groupID);
  if (!group)
    return;

  Tag* tag = group->getTagByID(tagID);
  if (tag)
  {
    if (tag->mName != name)
    {
      // TODO: Notify name change.
    }

    if (tag->mType != type)
    {
      // TODO: Notify type change.
    }

    // TODO: Compare values.

    const Tag::ObserverList& observers = tag->mObservers;
    for (Tag::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      (*i)->onChange(*tag);
    
    tag->mName = name;
    tag->mType = type;
    tag->mValue = *value;
    tag->updateData();
  }
  else
  {
    tag = new Tag(tagID, name, type, *value, *group);
    group->mTags.push_back(tag);
    group->updateStructure();

    const TagGroup::ObserverList& observers = group->mObservers;
    for (TagGroup::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      (*i)->onCreateTag(*group, *tag);
  }
}

void Session::receiveTagDestroy(void* user, VNodeID ID, uint16 groupID, uint16 tagID)
{
  Session* session = getCurrent();

  Node* node = session->getNodeByID(ID);
  if (!node)
    return;

  TagGroup* group = node->getTagGroupByID(groupID);
  if (!group)
    return;

  TagGroup::TagList& tags = group->mTags;
  for (TagGroup::TagList::iterator tag = tags.begin();  tag != tags.end();  tag++)
  {
    if ((*tag)->getID() == tagID)
    {
      const TagGroup::ObserverList& observers = group->mObservers;
      for (TagGroup::ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
	(*observer)->onDestroyTag(*group, *(*tag));

      delete *tag;
      tags.erase(tag);

      group->updateStructure();
      break;
    }
  }
}

void Session::receiveNodeLanguageSet(void* user, VNodeID ID, const char* language)
{
  Session* session = getCurrent();

  TextNode* node = dynamic_cast<TextNode*>(session->getNodeByID(ID));
  if (!node)
    return;

  const Node::ObserverList& observers = node->mObservers;
  for (Node::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
  {
    if (TextNodeObserver* observer = dynamic_cast<TextNodeObserver*>(*i))
      observer->onSetLanguage(*node, language);
  }
  
  node->mLanguage = language;
  node->updateData();
}

void Session::receiveTextBufferCreate(void* user, VNodeID ID, VBufferID bufferID, const char* name)
{
  Session* session = getCurrent();

  TextNode* node = dynamic_cast<TextNode*>(session->getNodeByID(ID));
  if (!node)
    return;

  TextBuffer* buffer = node->getBufferByID(bufferID);
  if (buffer)
  {
    const TextBuffer::ObserverList& observers = buffer->mObservers;
    for (TextBuffer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      (*i)->onSetName(*buffer, name);
    
    buffer->mName = name;
    buffer->updateData();
  }
  else
  {
    buffer = new TextBuffer(bufferID, name, *node);
    node->mBuffers.push_back(buffer);
    node->updateStructure();

    const Node::ObserverList& observers = node->mObservers;
    for (Node::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    {
      if (TextNodeObserver* observer = dynamic_cast<TextNodeObserver*>(*i))
	observer->onCreateBuffer(*node, *buffer);
    }
  
    verse_send_t_buffer_subscribe(node->getID(), bufferID);
  }
}

void Session::receiveTextBufferDestroy(void* user, VNodeID ID, VBufferID bufferID)
{
  Session* session = getCurrent();

  TextNode* node = dynamic_cast<TextNode*>(session->getNodeByID(ID));
  if (!node)
    return;

  TextNode::BufferList& buffers = node->mBuffers;
  for (TextNode::BufferList::iterator buffer = buffers.begin();  buffer != buffers.end();  buffer++)
  {
    if ((*buffer)->getID() == bufferID)
    {
      const TextNode::ObserverList& observers = node->mObservers;
      for (TextNode::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      {
	if (TextNodeObserver* observer = dynamic_cast<TextNodeObserver*>(*i))
	  observer->onDestroyBuffer(*node, *(*buffer));
      }

      delete *buffer;
      buffers.erase(buffer);
      
      node->updateStructure();
      break;
    }
  }
}

void Session::receiveTextBufferSet(void* user, VNodeID ID, VBufferID bufferID, uint32 position, uint32 length, const char* text)
{
  Session* session = getCurrent();

  TextNode* node = dynamic_cast<TextNode*>(session->getNodeByID(ID));
  if (!node)
    return;

  TextBuffer* buffer = node->getBufferByID(bufferID);
  if (!buffer)
    return;

  const TextBuffer::ObserverList& observers = buffer->mObservers;
  for (TextBuffer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onReplaceRange(*buffer, position, length, text);

  buffer->mText.replace(position, length, text);
  buffer->updateData();
}

void Session::receiveGeometryLayerCreate(void* data, VNodeID ID, VLayerID layerID, const char* name, VNGLayerType type, uint32 defaultInt, real64 defaultReal)
{
  Session* session = getCurrent();

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

      const GeometryLayer::ObserverList& observers = layer->mObservers;
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

    const Node::ObserverList& observers = node->mObservers;
    for (Node::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    {
      if (GeometryNodeObserver* observer = dynamic_cast<GeometryNodeObserver*>(*i))
	observer->onCreateLayer(*node, *layer);
    }

    verse_send_g_layer_subscribe(ID, layerID, VN_FORMAT_REAL64);
  }
}

void Session::receiveGeometryLayerDestroy(void* data, VNodeID ID, VLayerID layerID)
{
  Session* session = getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(ID));
  if (!node)
    return;

  GeometryNode::LayerList& layers = node->mLayers;
  for (GeometryNode::LayerList::iterator layer = layers.begin();  layer != layers.end();  layer++)
  {
    if ((*layer)->getID() == layerID)
    {
      const GeometryNode::ObserverList& observers = node->mObservers;
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

void Session::receiveVertexSetXyzReal32(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, real32 x, real32 y, real32 z)
{
}

void Session::receiveVertexDeleteReal32(void* user, VNodeID nodeID, uint32 vertexID)
{
}

void Session::receiveVertexSetXyzReal64(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, real64 x, real64 y, real64 z)
{
  Session* session = getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_VERTEX_XYZ)
    return;
  
  BaseVertex vertex(x, y, z);

  const GeometryLayer::ObserverList& observers = layer->mObservers;
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, vertexID, &vertex);

  bool created = (layerID == 0 && !node->isVertex(vertexID));

  if (!created)
  {
    const GeometryNode::ObserverList& observers = node->mObservers;
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
    if (vertexID > node->mHighestVertexID)
      node->mHighestVertexID = vertexID;

    layer->updateStructure();

    const GeometryNode::ObserverList& observers = node->mObservers;
    for (GeometryNode::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    {
      if (GeometryNodeObserver* observer = dynamic_cast<GeometryNodeObserver*>(*i))
	observer->onCreateVertex(*node, vertexID, vertex);
    }
  }
  else
    layer->updateData();
}

void Session::receiveVertexDeleteReal64(void* user, VNodeID nodeID, uint32 vertexID)
{
  Session* session = getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node || !node->mBaseVertexLayer)
    return;

  BaseVertex* vertex = reinterpret_cast<BaseVertex*>(node->mBaseVertexLayer->mData.getItem(vertexID));
  if (!vertex || !vertex->isValid())
    return;

  const GeometryNode::ObserverList& observers = node->mObservers;
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

    if (!index)
      node->mHighestVertexID = INVALID_VERTEX_ID;
  }

  node->mVertexCount--;
  node->mBaseVertexLayer->updateStructure();
}

void Session::receiveVertexSetUint32(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, uint32 value)
{
  Session* session = getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_VERTEX_UINT32)
    return;
  
  const GeometryLayer::ObserverList& observers = layer->mObservers;
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, vertexID, &value);

  layer->reserve(vertexID + 1);
  layer->updateData();

  Slot& targetSlot = *reinterpret_cast<Slot*>(layer->mData.getItem(vertexID));
  targetSlot.uint[0] = value;
}

void Session::receiveVertexSetReal64(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, real64 value)
{
  Session* session = getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_VERTEX_REAL)
    return;
  
  const GeometryLayer::ObserverList& observers = layer->mObservers;
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, vertexID, &value);

  layer->reserve(vertexID + 1);
  layer->updateData();

  Slot& targetSlot = *reinterpret_cast<Slot*>(layer->mData.getItem(vertexID));
  targetSlot.real[0] = value;
}

void Session::receiveVertexSetReal32(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, real32 value)
{
}

void Session::receivePolygonSetCornerUint32(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, uint32 v0, uint32 v1, uint32 v2, uint32 v3)
{
  Session* session = getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_POLYGON_CORNER_UINT32)
    return;
  
  BasePolygon polygon(v0, v1, v2, v3);

  const GeometryLayer::ObserverList& observers = layer->mObservers;
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, polygonID, &polygon);

  bool created = (layerID == 1 && node->isPolygon(polygonID));

  if (!created)
  {
    const GeometryNode::ObserverList& observers = node->mObservers;
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
    if (polygonID > node->mHighestPolygonID)
      node->mHighestPolygonID = polygonID;

    layer->updateStructure();

    const GeometryNode::ObserverList& observers = node->mObservers;
    for (GeometryNode::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    {
      if (GeometryNodeObserver* observer = dynamic_cast<GeometryNodeObserver*>(*i))
	observer->onCreatePolygon(*node, polygonID, polygon);
    }
  }
  else
    layer->updateData();
}

void Session::receivePolygonDelete(void* user, VNodeID nodeID, uint32 polygonID)
{
  Session* session = getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node || !node->mBasePolygonLayer)
    return;

  BasePolygon* polygon = reinterpret_cast<BasePolygon*>(node->mBasePolygonLayer->mData.getItem(polygonID));
  if (!polygon)
    return;

  const GeometryNode::ObserverList& observers = node->mObservers;
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

    if (!index)
      node->mHighestPolygonID = INVALID_POLYGON_ID;
  }

  node->mPolygonCount--;
  node->mBasePolygonLayer->updateStructure();
}

void Session::receivePolygonSetCornerReal64(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, real64 v0, real64 v1, real64 v2, real64 v3)
{
  Session* session = getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_POLYGON_CORNER_UINT32)
    return;
  
  Slot sourceSlot;
  sourceSlot.real[0] = v0;
  sourceSlot.real[1] = v1;
  sourceSlot.real[2] = v2;
  sourceSlot.real[3] = v3;

  const GeometryLayer::ObserverList& observers = layer->mObservers;
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, polygonID, &sourceSlot);

  layer->reserve(polygonID + 1);
  layer->updateData();

  Slot& targetSlot = *reinterpret_cast<Slot*>(layer->mData.getItem(polygonID));
  copySlot(targetSlot.real, sourceSlot.real, 4);
}

void Session::receivePolygonSetCornerReal32(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, real32 v0, real32 v1, real32 v2, real32 v3)
{
}

void Session::receivePolygonSetFaceUint8(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, uint8 value)
{
  Session* session = getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_POLYGON_FACE_UINT8)
    return;
  
  const GeometryLayer::ObserverList& observers = layer->mObservers;
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, polygonID, &value);

  layer->reserve(polygonID + 1);
  layer->updateData();

  Slot& targetSlot = *reinterpret_cast<Slot*>(layer->mData.getItem(polygonID));
  targetSlot.byte[0] = value;
}

void Session::receivePolygonSetFaceUint32(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, uint32 value)
{
  Session* session = getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_POLYGON_FACE_UINT32)
    return;
  
  const GeometryLayer::ObserverList& observers = layer->mObservers;
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, polygonID, &value);

  layer->reserve(polygonID + 1);
  layer->updateData();

  Slot& targetSlot = *reinterpret_cast<Slot*>(layer->mData.getItem(polygonID));
  targetSlot.uint[0] = value;
}

void Session::receivePolygonSetFaceReal64(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, real64 value)
{
  Session* session = getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  GeometryLayer* layer = node->getLayerByID(layerID);
  if (!layer || layer->getType() != VN_G_LAYER_POLYGON_FACE_REAL)
    return;
  
  const GeometryLayer::ObserverList& observers = layer->mObservers;
  for (GeometryLayer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetSlot(*layer, polygonID, &value);

  layer->reserve(polygonID + 1);
  layer->updateData();

  Slot& targetSlot = *reinterpret_cast<Slot*>(layer->mData.getItem(polygonID));
  targetSlot.real[0] = value;
}

void Session::receivePolygonSetFaceReal32(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, real32 value)
{
}

void Session::receiveCreaseSetVertex(void* user, VNodeID nodeID, const char *layer, uint32 def_crease)
{
  Session* session = getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  node->mVertexCreases = layer;
  node->mVertexDefaultCrease = def_crease;
}

void Session::receiveCreaseSetEdge(void* user, VNodeID nodeID, const char *layer, uint32 def_crease)
{
  Session* session = getCurrent();

  GeometryNode* node = dynamic_cast<GeometryNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  node->mEdgeCreases = layer;
  node->mEdgeDefaultCrease = def_crease;
}

void Session::receiveBoneCreate(void* user, VNodeID nodeID, uint16 bone_id, const char *weight, const char *reference, uint32 parent, real64 pos_x, real64 pos_y, real64 pos_z, real64 rot_x, real64 rot_y, real64 rot_z, real64 rot_w)
{
  // TODO: Design bone management.
}

void Session::receiveBoneDestroy(void* user, VNodeID nodeID, uint16 bone_id)
{
  // TODO: Design bone management.
}

Session::SessionList Session::msSessions;

Session::SessionStack Session::msStack;

Session* Session::msCurrent = NULL;

bool Session::msInitialized = false;

//---------------------------------------------------------------------

Session::PendingNode::PendingNode(const std::string& name, VNodeType type):
  mName(name),
  mType(type)
{
}

//---------------------------------------------------------------------

void SessionObserver::onAccept(Session& session)
{
}

void SessionObserver::onTerminate(Session& session, const std::string& byebye)
{
}

void SessionObserver::onCreateNode(Session& session, Node& node)
{
}

void SessionObserver::onDestroyNode(Session& session, Node& node)
{
}

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/

