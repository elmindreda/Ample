
#include <verse.h>

#include <Ample.h>

namespace verse
{
  namespace ample
  {

//---------------------------------------------------------------------

MethodParam::MethodParam(const std::string& name, VNOParamType type):
  mName(name),
  mType(type)
{
}

size_t MethodParam::getSize(void) const
{
  switch (mType)
  {
    case VN_O_METHOD_PTYPE_INT8:
    case VN_O_METHOD_PTYPE_UINT8:
      return sizeof(uint8);
    case VN_O_METHOD_PTYPE_INT16:
    case VN_O_METHOD_PTYPE_UINT16:
      return sizeof(uint16);
    case VN_O_METHOD_PTYPE_INT32:
    case VN_O_METHOD_PTYPE_UINT32:
      return sizeof(uint32);
    case VN_O_METHOD_PTYPE_REAL32:
      return sizeof(real32);
    case VN_O_METHOD_PTYPE_REAL64:
      return sizeof(real64);
    case VN_O_METHOD_PTYPE_REAL32_VEC2:
      return sizeof(real32) * 2;
    case VN_O_METHOD_PTYPE_REAL32_VEC3:
      return sizeof(real32) * 3;
    case VN_O_METHOD_PTYPE_REAL32_VEC4:
      return sizeof(real32) * 4;
    case VN_O_METHOD_PTYPE_REAL64_VEC2:
      return sizeof(real64) * 2;
    case VN_O_METHOD_PTYPE_REAL64_VEC3:
      return sizeof(real64) * 3;
    case VN_O_METHOD_PTYPE_REAL64_VEC4:
      return sizeof(real64) * 4;
    case VN_O_METHOD_PTYPE_REAL32_MAT4:
      return sizeof(real32) * 4;
    case VN_O_METHOD_PTYPE_REAL32_MAT9:
      return sizeof(real32) * 9;
    case VN_O_METHOD_PTYPE_REAL32_MAT16:
      return sizeof(real32) * 16;
    case VN_O_METHOD_PTYPE_REAL64_MAT4:
      return sizeof(real64) * 4;
    case VN_O_METHOD_PTYPE_REAL64_MAT9:
      return sizeof(real64) * 9;
    case VN_O_METHOD_PTYPE_REAL64_MAT16:
      return sizeof(real64) * 16;
    case VN_O_METHOD_PTYPE_STRING:
      return 500;
    case VN_O_METHOD_PTYPE_NODE:
      return sizeof(VNodeID);
    case VN_O_METHOD_PTYPE_LAYER:
      return sizeof(VLayerID);
  }
}

//---------------------------------------------------------------------

void Method::destroy(void)
{
  mGroup.getNode().getSession().push();
  verse_send_o_method_destroy(mGroup.getNode().getID(), mGroup.getID(), mID);
  mGroup.getNode().getSession().pop();
}

bool Method::call(const MethodArgumentList& arguments, VNodeID senderID)
{
  if (arguments.size() != mParams.size())
    return false;

  size_t size = 0;
  for (unsigned int i = 0;  i < mParams.size();  i++)
    size += mParams[i].getSize();

  char* data = new char [size];

  off_t offset = 0;
  for (unsigned int i = 0;  i < mParams.size();  i++)
    memcpy(data + offset, &arguments[i], mParams[i].getSize());

  mGroup.getNode().getSession().push();
  //verse_send_o_method_call(mGroup.getNode().getID(), mGroup.getID(), mID, senderID, 
  mGroup.getNode().getSession().pop();
}

uint16 Method::getID(void) const
{
  return mID;
}

const std::string& Method::getName(void) const
{
  return mName;
}

uint8 Method::getParamCount(void) const
{
  mParams.size();
}

const MethodParam& Method::getParam(uint8 index)
{
  return mParams[index];
}

MethodGroup& Method::getGroup(void) const
{
  return mGroup;
}

Method::Method(uint16 ID, const std::string& name, MethodGroup& group):
  mID(ID),
  mName(name),
  mGroup(group)
{
}

void Method::initialize(void)
{
  verse_callback_set((void*) verse_send_o_method_call,
                     (void*) receiveMethodCall,
                     NULL);
}

void Method::receiveMethodCall(void* user, VNodeID nodeID, uint16 groupID, uint16 methodID, VNodeID senderID, const VNOPackedParams* arguments)
{
  Session* session = Session::getCurrent();

  ObjectNode* node = dynamic_cast<ObjectNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  MethodGroup* group = node->getMethodGroupByID(groupID);
  if (!group)
    return;

  Method* method = group->getMethodByID(methodID);
  if (!method)
    return;

  // TODO: Unpack arguments.

  MethodArgumentList unpackedArguments;

  const ObserverList& observers = method->getObservers();
  for (ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
    (*observer)->onCall(*method, unpackedArguments);
}

//---------------------------------------------------------------------

void MethodObserver::onCall(Method& method, const MethodArgumentList& arguments)
{
}

void MethodObserver::onDestroy(Method& method)
{
}

//---------------------------------------------------------------------

void MethodGroup::destroy(void)
{
  mNode.getSession().push();
  verse_send_o_method_group_destroy(mNode.getID(), mID);
  mNode.getSession().pop();
}

void MethodGroup::createMethod(const std::string& name, const MethodParamList& parameters)
{
  const char** names;
  VNOParamType* types;

  names = (const char**) new char* [parameters.size()];
  types = new VNOParamType [parameters.size()];

  for (unsigned int i = 0;  i < parameters.size();  i++)
  {
    types[i] = parameters[i].mType;
    names[i] = parameters[i].mName.c_str();
  }

  mNode.getSession().push();
  verse_send_o_method_create(mNode.getID(), mID, (uint16) ~0, name.c_str(), parameters.size(), types, names);
  mNode.getSession().pop();

  delete types;
  delete names;
}

Method* MethodGroup::getMethodByID(uint16 methodID)
{
  for (MethodList::iterator i = mMethods.begin();  i != mMethods.end();  i++)
    if ((*i)->getID() == methodID)
      return *i;

  return NULL;
}

const Method* MethodGroup::getMethodByID(uint16 methodID) const
{
  for (MethodList::const_iterator i = mMethods.begin();  i != mMethods.end();  i++)
    if ((*i)->getID() == methodID)
      return *i;

  return NULL;
}

Method* MethodGroup::getMethodByIndex(unsigned int index)
{
  return mMethods[index];
}

const Method* MethodGroup::getMethodByIndex(unsigned int index) const
{
  return mMethods[index];
}

Method* MethodGroup::getMethodByName(const std::string& name)
{
  for (MethodList::iterator i = mMethods.begin();  i != mMethods.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

const Method* MethodGroup::getMethodByName(const std::string& name) const
{
  for (MethodList::const_iterator i = mMethods.begin();  i != mMethods.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

unsigned int MethodGroup::getMethodCount(void) const
{
  return mMethods.size();
}

const uint16 MethodGroup::getID(void) const
{
  return mID;
}

const std::string& MethodGroup::getName(void) const
{
  return mName;
}

ObjectNode& MethodGroup::getNode(void) const
{
  return mNode;
}

MethodGroup::MethodGroup(uint16 ID, const std::string& name, ObjectNode& node):
  mID(ID),
  mName(name),
  mNode(node)
{
}

MethodGroup::~MethodGroup(void)
{
  while (mMethods.size())
  {
    delete mMethods.back();
    mMethods.pop_back();
  }
}

void MethodGroup::initialize(void)
{
  Method::initialize();

  verse_callback_set((void*) verse_send_o_method_create,
                     (void*) receiveMethodCreate,
                     NULL);
  verse_callback_set((void*) verse_send_o_method_destroy,
                     (void*) receiveMethodDestroy,
                     NULL);
}

void MethodGroup::receiveMethodCreate(void* user, VNodeID nodeID, uint16 groupID, uint16 methodID, const char* name, uint8 paramCount, const VNOParamType* paramTypes, const char** paramNames)
{
  Session* session = Session::getCurrent();

  ObjectNode* node = dynamic_cast<ObjectNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  MethodGroup* group = node->getMethodGroupByID(groupID);
  if (!group)
    return;

  Method* method = group->getMethodByID(methodID);
  if (method)
  {
    if (method->mName != name)
    {
      // TODO: Notify name change.
    }

    // NOTE: This is bad. Don't do this.
    receiveMethodDestroy(user, nodeID, groupID, methodID);
  }
  else
  {
    method = new Method(methodID, name, *group);

    for (unsigned int i = 0;  i < paramCount;  i++)
      method->mParams.push_back(MethodParam(paramNames[i], paramTypes[i]));

    group->mMethods.push_back(method);
    group->updateStructure();

    const MethodGroup::ObserverList& observers = group->getObservers();
    for (MethodGroup::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      (*i)->onCreateMethod(*group, *method);
  }
}

void MethodGroup::receiveMethodDestroy(void* user, VNodeID nodeID, uint16 groupID, uint16 methodID)
{
  Session* session = Session::getCurrent();

  ObjectNode* node = dynamic_cast<ObjectNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  MethodGroup* group = node->getMethodGroupByID(groupID);
  if (!group)
    return;

  MethodList& methods = group->mMethods;
  for (MethodList::iterator method = methods.begin();  method != methods.end();  method++)
  {
    if ((*method)->getID() == methodID)
    {
      // Notify method observers.
      {
        const Method::ObserverList& observers = (*method)->getObservers();
        for (Method::ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
          (*observer)->onDestroy(*(*method));
      }

      // Notify method group observers.
      {
        const ObserverList& observers = group->getObservers();
        for (ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
          (*observer)->onDestroyMethod(*group, *(*method));
      }

      delete *method;
      methods.erase(method);

      group->updateStructure();
      break;
    }
  }
}

//---------------------------------------------------------------------

void MethodGroupObserver::onCreateMethod(MethodGroup& group, Method& method)
{
}

void MethodGroupObserver::onDestroyMethod(MethodGroup& group, Method& method)
{
}

void MethodGroupObserver::onSetName(MethodGroup& group, const std::string& name)
{
}

void MethodGroupObserver::onDestroy(MethodGroup& group)
{
}

//---------------------------------------------------------------------

void Link::destroy(void)
{
  mNode.getSession().push();
  verse_send_o_link_destroy(mNode.getID(), mID);
  mNode.getSession().pop();
}

uint16 Link::getID(void) const
{
  return mID;
}

VNodeID Link::getLinkedNodeID(void) const
{
  return mNodeID;
}

VNodeID Link::getTargetNodeID(void) const
{
  return mTargetID;
}

const std::string& Link::getName(void) const
{
  return mName;
}

ObjectNode& Link::getNode(void) const
{
  return mNode;
}

Link::Link(uint16 ID, const std::string& name, ObjectNode& node):
  mID(ID),
  mName(name),
  mNode(node)
{
}

//---------------------------------------------------------------------

void LinkObserver::onDestroy(Link& link)
{
}

//---------------------------------------------------------------------

void ObjectNode::createMethodGroup(const std::string& name)
{
  getSession().push();
  verse_send_o_method_group_create(getID(), (uint16) ~0, name.c_str());
  getSession().pop();
}

void ObjectNode::createLink(const std::string& name, VNodeID nodeID, VNodeID targetID)
{
  getSession().push();
  verse_send_o_link_set(getID(), 0, nodeID, name.c_str(), targetID);
  getSession().pop();
}

MethodGroup* ObjectNode::getMethodGroupByID(uint16 groupID)
{
  for (MethodGroupList::iterator i = mGroups.begin();  i != mGroups.end();  i++)
    if ((*i)->getID() == groupID)
      return *i;

  return NULL;
}

const MethodGroup* ObjectNode::getMethodGroupByID(uint16 groupID) const
{
  for (MethodGroupList::const_iterator i = mGroups.begin();  i != mGroups.end();  i++)
    if ((*i)->getID() == groupID)
      return *i;

  return NULL;
}

MethodGroup* ObjectNode::getMethodGroupByIndex(unsigned int index)
{
  return mGroups[index];
}

const MethodGroup* ObjectNode::getMethodGroupByIndex(unsigned int index) const
{
  return mGroups[index];
}

MethodGroup* ObjectNode::getMethodGroupByName(const std::string& name)
{
  for (MethodGroupList::iterator i = mGroups.begin();  i != mGroups.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

const MethodGroup* ObjectNode::getMethodGroupByName(const std::string& name) const
{
  for (MethodGroupList::const_iterator i = mGroups.begin();  i != mGroups.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

unsigned int ObjectNode::getMethodGroupCount(void) const
{
  return mGroups.size();
}

Link* ObjectNode::getLinkByID(uint16 linkID)
{
  for (LinkList::iterator i = mLinks.begin();  i != mLinks.end();  i++)
    if ((*i)->getID() == linkID)
      return *i;

  return NULL;
}

const Link* ObjectNode::getLinkByID(uint16 linkID) const
{
  for (LinkList::const_iterator i = mLinks.begin();  i != mLinks.end();  i++)
    if ((*i)->getID() == linkID)
      return *i;

  return NULL;
}

Link* ObjectNode::getLinkByIndex(unsigned int index)
{
  return mLinks[index];
}

const Link* ObjectNode::getLinkByIndex(unsigned int index) const
{
  return mLinks[index];
}

Link* ObjectNode::getLinkByName(const std::string& name)
{
  for (LinkList::iterator i = mLinks.begin();  i != mLinks.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

const Link* ObjectNode::getLinkByName(const std::string& name) const
{
  for (LinkList::const_iterator i = mLinks.begin();  i != mLinks.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

unsigned int ObjectNode::getLinkCount(void) const
{
  return mLinks.size();
}

const Vector3d& ObjectNode::getPosition(void) const
{
  return mTranslation.mPosition;
}

void ObjectNode::setPosition(const Vector3d& position)
{
  mTranslationCache.mPosition = position;

  getSession().push();
  verse_send_o_transform_pos_real64(getID(),
                                    mTranslationCache.mSeconds,
                                    mTranslationCache.mFraction,
                                    mTranslationCache.mPosition,
                                    mTranslationCache.mSpeed,
                                    mTranslationCache.mAccel,
                                    mTranslationCache.mDragNormal,
                                    mTranslationCache.mDrag);
  getSession().pop();
}

const Vector3d& ObjectNode::getSpeed(void) const
{
  return mTranslation.mSpeed;
}

void ObjectNode::setSpeed(const Vector3d& speed)
{
  mTranslationCache.mSpeed = speed;

  getSession().push();
  verse_send_o_transform_pos_real64(getID(),
                                    mTranslationCache.mSeconds,
                                    mTranslationCache.mFraction,
                                    mTranslationCache.mPosition,
                                    mTranslationCache.mSpeed,
                                    mTranslationCache.mAccel,
                                    mTranslationCache.mDragNormal,
                                    mTranslationCache.mDrag);
  getSession().pop();
}

const Vector3d& ObjectNode::getAccel(void) const
{
  return mTranslation.mAccel;
}

void ObjectNode::setAccel(const Vector3d& accel)
{
  mTranslationCache.mAccel = accel;

  getSession().push();
  verse_send_o_transform_pos_real64(getID(),
                                    mTranslationCache.mSeconds,
                                    mTranslationCache.mFraction,
                                    mTranslationCache.mPosition,
                                    mTranslationCache.mSpeed,
                                    mTranslationCache.mAccel,
                                    mTranslationCache.mDragNormal,
                                    mTranslationCache.mDrag);
  getSession().pop();
}

const Quaternion64& ObjectNode::getRotation(void) const
{
  return mRotation.mRotation;
}

void ObjectNode::setRotation(const Quaternion64& rotation)
{
  mRotationCache.mRotation = rotation;

  getSession().push();
  verse_send_o_transform_rot_real64(getID(),
                                    mRotationCache.mSeconds,
                                    mRotationCache.mFraction,
                                    &mRotationCache.mRotation,
                                    &mRotationCache.mSpeed,
                                    &mRotationCache.mAccel,
                                    &mRotationCache.mDragNormal,
                                    mRotationCache.mDrag);
  getSession().pop();
}

const Quaternion64& ObjectNode::getRotationSpeed(void) const
{
  return mRotation.mSpeed;
}

void ObjectNode::setRotationSpeed(const Quaternion64& speed)
{
  mRotationCache.mSpeed = speed;

  getSession().push();
  verse_send_o_transform_rot_real64(getID(),
                                    mRotationCache.mSeconds,
                                    mRotationCache.mFraction,
                                    &mRotationCache.mRotation,
                                    &mRotationCache.mSpeed,
                                    &mRotationCache.mAccel,
                                    &mRotationCache.mDragNormal,
                                    mRotationCache.mDrag);
  getSession().pop();
}

const Quaternion64& ObjectNode::getRotationAccel(void) const
{
  return mRotation.mAccel;
}

void ObjectNode::setRotationAccel(const Quaternion64& accel)
{
  mRotationCache.mAccel = accel;

  getSession().push();
  verse_send_o_transform_rot_real64(getID(),
                                    mRotationCache.mSeconds,
                                    mRotationCache.mFraction,
                                    &mRotationCache.mRotation,
                                    &mRotationCache.mSpeed,
                                    &mRotationCache.mAccel,
                                    &mRotationCache.mDragNormal,
                                    mRotationCache.mDrag);
  getSession().pop();
}

const Vector3d& ObjectNode::ObjectNode::getScale(void) const
{
  return mScale;
}

void ObjectNode::setScale(const Vector3d& scale)
{
  getSession().push();
  verse_send_o_transform_scale_real64(getID(), scale.x, scale.y, scale.z);
  getSession().pop();
}

ObjectNode::ObjectNode(VNodeID ID, VNodeOwner owner, Session& session):
  Node(ID, V_NT_OBJECT, owner, session)
{
  verse_send_o_transform_subscribe(getID(), VN_FORMAT_REAL64);
}

ObjectNode::~ObjectNode(void)
{
  while (mLinks.size())
  {
    delete mLinks.back();
    mLinks.pop_back();
  }

  while (mGroups.size())
  {
    delete mGroups.back();
    mGroups.pop_back();
  }
}

void ObjectNode::initialize(void)
{
  MethodGroup::initialize();

  verse_callback_set((void*) verse_send_o_transform_pos_real32,
                     (void*) receiveTransformPosReal32,
                     NULL);
  verse_callback_set((void*) verse_send_o_transform_rot_real32,
                     (void*) receiveTransformRotReal32,
                     NULL);
  verse_callback_set((void*) verse_send_o_transform_scale_real32,
                     (void*) receiveTransformScaleReal32,
                     NULL);
  verse_callback_set((void*) verse_send_o_transform_pos_real64,
                     (void*) receiveTransformPosReal64,
                     NULL);
  verse_callback_set((void*) verse_send_o_transform_rot_real64,
                     (void*) receiveTransformRotReal64,
                     NULL);
  verse_callback_set((void*) verse_send_o_transform_scale_real64,
                     (void*) receiveTransformScaleReal64,
                     NULL);
  verse_callback_set((void*) verse_send_o_light_set,
                     (void*) receiveLightSet,
                     NULL);
  verse_callback_set((void*) verse_send_o_link_set,
                     (void*) receiveLinkSet,
                     NULL);
  verse_callback_set((void*) verse_send_o_link_destroy,
                     (void*) receiveLinkDestroy,
                     NULL);
  verse_callback_set((void*) verse_send_o_method_group_create,
                     (void*) receiveMethodGroupCreate,
                     NULL);
  verse_callback_set((void*) verse_send_o_method_group_destroy,
                     (void*) receiveMethodGroupDestroy,
                     NULL);
  verse_callback_set((void*) verse_send_o_anim_run,
                     (void*) receiveAnimRun,
                     NULL);
}

void ObjectNode::receiveTransformPosReal32(void* user,
                                           VNodeID nodeID,
                                           uint32 seconds,
                                           uint32 fraction,
                                           const real32* position,
                                           const real32* speed,
                                           const real32* accel,
                                           const real32* dragNormal,
                                           real32 drag)
{
}

void ObjectNode::receiveTransformRotReal32(void* user,
                                           VNodeID nodeID,
                                           uint32 seconds,
                                           uint32 fraction,
                                           const VNQuat32* rotation,
                                           const VNQuat32* speed,
                                           const VNQuat32* accelerate,
                                           const VNQuat32* dragNormal,
                                           real32 drag)
{
}

void ObjectNode::receiveTransformScaleReal32(void* user,
                                             VNodeID nodeID,
                                             real32 scaleX,
                                             real32 scaleY,
                                             real32 scaleZ)
{
}

void ObjectNode::receiveTransformPosReal64(void* user,
                                           VNodeID nodeID,
                                           uint32 seconds,
                                           uint32 fraction,
                                           const real64* position,
                                           const real64* speed,
                                           const real64* accel,
                                           const real64* dragNormal,
                                           real64 drag)
{
  Session* session = Session::getCurrent();

  ObjectNode* node = dynamic_cast<ObjectNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  node->mTranslation.mPosition.set(position[0], position[1], position[2]);
  node->mTranslation.mSpeed.set(speed[0], speed[1], speed[2]);
  node->mTranslation.mAccel.set(accel[0], accel[1], accel[2]);
  node->mTranslation.mDragNormal.set(dragNormal[0], dragNormal[1], dragNormal[2]);

  node->mTranslation.mSeconds = seconds;
  node->mTranslation.mFraction = fraction;
  node->mTranslation.mDrag = drag;

  node->mTranslationCache = node->mTranslation;
}

void ObjectNode::receiveTransformRotReal64(void* user,
                                           VNodeID nodeID,
                                           uint32 seconds,
                                           uint32 fraction,
                                           const VNQuat64* rotation,
                                           const VNQuat64* speed,
                                           const VNQuat64* accel,
                                           const VNQuat64* dragNormal,
                                           real64 drag)
{
  Session* session = Session::getCurrent();

  ObjectNode* node = dynamic_cast<ObjectNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  node->mRotation.mRotation = *rotation;
  node->mRotation.mSpeed = *speed;
  node->mRotation.mAccel = *accel;
  node->mRotation.mDragNormal = *dragNormal;

  node->mRotation.mSeconds = seconds;
  node->mRotation.mFraction = fraction;
  node->mRotation.mDrag = drag;

  node->mRotationCache = node->mRotation;
}

void ObjectNode::receiveTransformScaleReal64(void* user,
                                             VNodeID nodeID,
                                             real64 scaleX,
                                             real64 scaleY,
                                             real64 scaleZ)
{
  Session* session = Session::getCurrent();

  ObjectNode* node = dynamic_cast<ObjectNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  node->mScale.set(scaleX, scaleY, scaleZ);
}

void ObjectNode::receiveLightSet(void* user,
                                 VNodeID nodeID,
                                 real64 lightR,
                                 real64 lightG,
                                 real64 lightB)
{
}

void ObjectNode::receiveLinkSet(void* user,
                                VNodeID nodeID,
                                uint16 linkID,
                                VNodeID linkedNodeID,
                                const char* name,
                                uint32 targetID)
{
  Session* session = Session::getCurrent();

  ObjectNode* node = dynamic_cast<ObjectNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  Link* link = node->getLinkByID(linkID);
  if (link)
  {
    if (link->mName != name)
    {
      // TODO: Notify name change.
    }

    // NOTE: This is bad. Don't do this.
    receiveLinkDestroy(user, nodeID, linkID);
  }

  link = new Link(linkID, name, *node);
  node->mLinks.push_back(link);
  node->updateStructure();

  const ObserverList& observers = node->getObservers();
  for (ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
  {
    if (ObjectNodeObserver* observer = dynamic_cast<ObjectNodeObserver*>(*i))
      observer->onCreateLink(*node, *link);
  }
}

void ObjectNode::receiveLinkDestroy(void* user, VNodeID nodeID, uint16 linkID)
{
  Session* session = Session::getCurrent();

  ObjectNode* node = dynamic_cast<ObjectNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  for (LinkList::iterator link = node->mLinks.begin();  link != node->mLinks.end();  link++)
  {
    if ((*link)->getID() == linkID)
    {
      // Notify link observers.
      {
        const Link::ObserverList& observers = (*link)->getObservers();
        for (Link::ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
          (*observer)->onDestroy(*(*link));
      }

      // Notify node observers.
      {
        const ObserverList& observers = node->getObservers();
        for (ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
        {
          if (ObjectNodeObserver* observer = dynamic_cast<ObjectNodeObserver*>(*i))
            observer->onDestroyLink(*node, *(*link));
        }
      }

      delete *link;
      node->mLinks.erase(link);

      node->updateStructure();
      break;
    }
  }
}

void ObjectNode::receiveMethodGroupCreate(void* user, VNodeID nodeID, uint16 groupID, const char* name)
{
  Session* session = Session::getCurrent();

  ObjectNode* node = dynamic_cast<ObjectNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  MethodGroup* group = node->getMethodGroupByID(groupID);
  if (group)
  {
    const MethodGroup::ObserverList& observers = group->getObservers();
    for (MethodGroup::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      (*i)->onSetName(*group, name);
      
    group->mName = name;
    group->updateData();
  }
  else
  {
    group = new MethodGroup(groupID, name, *node);
    node->mGroups.push_back(group);
    node->updateStructure();
    
    const ObserverList& observers = node->getObservers();
    for (ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    {
      if (ObjectNodeObserver* observer = dynamic_cast<ObjectNodeObserver*>(*i))
        observer->onCreateMethodGroup(*node, *group);
    }
    
    verse_send_o_method_group_subscribe(node->getID(), groupID);
  }
}

void ObjectNode::receiveMethodGroupDestroy(void* user, VNodeID nodeID, uint16 groupID)
{
  Session* session = Session::getCurrent();

  ObjectNode* node = dynamic_cast<ObjectNode*>(session->getNodeByID(nodeID));
  if (!node)
    return;

  MethodGroupList& groups = node->mGroups;
  for (MethodGroupList::iterator group = groups.begin();  group != groups.end();  group++)
  {
    if ((*group)->getID() == groupID)
    {
      // Notify method group observers.
      {
        const MethodGroup::ObserverList& observers = (*group)->getObservers();
        for (MethodGroup::ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
          (*observer)->onDestroy(**group);
      }
      
      // Notify node observers.
      {
        const Node::ObserverList& observers = node->getObservers();
        for (Node::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
        {
          if (ObjectNodeObserver* observer = dynamic_cast<ObjectNodeObserver*>(*i))
            observer->onDestroyMethodGroup(*node, **group);
        }
      }
      
      delete *group;
      groups.erase(group);

      node->updateStructure();
      break;
    }
  }
}

void ObjectNode::receiveAnimRun(void* user, VNodeID nodeID, uint16 linkID, uint32 seconds, uint32 fraction, real64 pos, real64 speed, real64 accel, real64 scale, real64 scaleSpeed)
{
}

//---------------------------------------------------------------------

void ObjectNodeObserver::onCreateMethodGroup(ObjectNode& node, MethodGroup& group)
{
}

void ObjectNodeObserver::onDestroyMethodGroup(ObjectNode& node, MethodGroup& group)
{
}

void ObjectNodeObserver::onCreateLink(ObjectNode& node, Link& link)
{
}

void ObjectNodeObserver::onDestroyLink(ObjectNode& node, Link& link)
{
}

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/



