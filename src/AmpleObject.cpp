
#include <verse.h>

#include <Ample.h>

namespace verse
{
  namespace ample
  {

//---------------------------------------------------------------------

void Method::destroy(void)
{
  mGroup.getNode().getSession().push();
  verse_send_o_method_destroy(mGroup.getNode().getID(), mGroup.getID(), mID);
  mGroup.getNode().getSession().pop();
}

void Method::call(const MethodArgumentList& arguments)
{
  //verse_send_o_method_call(mGroup.getNode().getID(), mGroup.getID(), mID, 0, 
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

//---------------------------------------------------------------------

void MethodGroupObserver::onCreateMethod(MethodGroup& group, Method& method)
{
}

void MethodGroupObserver::onDestroyMethod(MethodGroup& group, Method& method)
{
}

void MethodGroupObserver::onDestroy(MethodGroup& group)
{
}

//---------------------------------------------------------------------

void Link::destroy(void)
{
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

Link::Link(uint16 ID, ObjectNode& node):
  mID(ID),
  mNode(node)
{
}

//---------------------------------------------------------------------

void ObjectNode::createMethodGroup(const std::string& name)
{
  getSession().push();
  verse_send_o_method_group_create(getID(), (uint16) ~0, name.c_str());
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

const Vector3d& ObjectNode::getScale(void) const
{
  return mScale;
}

void ObjectNode::setScale(const Vector3d& scale)
{
}

ObjectNode::ObjectNode(VNodeID ID, VNodeOwner owner, Session& session):
  Node(ID, V_NT_OBJECT, owner, session)
{
}

ObjectNode::~ObjectNode(void)
{
  while (mGroups.size())
  {
    delete mGroups.back();
    mGroups.pop_back();
  }
}

//---------------------------------------------------------------------

void ObjectNodeObserver::onCreateMethodGroup(ObjectNode& node, MethodGroup& group)
{
}

void ObjectNodeObserver::onDestroyMethodGroup(ObjectNode& node, MethodGroup& group)
{
}

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/

