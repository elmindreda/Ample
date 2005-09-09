
#include <verse.h>

#include "Ample.h"

namespace verse
{
  namespace ample
  {

//---------------------------------------------------------------------

void Node::destroy(void)
{
  mSession.push();
  verse_send_node_destroy(mID);
  mSession.pop();
}

void Node::createTagGroup(const std::string& name)
{
  mSession.push();
  verse_send_tag_group_create(mID, 0, name.c_str());
  mSession.pop();
}

TagGroup* Node::getTagGroupByID(uint16 ID)
{
  for (TagGroupList::iterator i = mGroups.begin();  i != mGroups.end();  i++)
    if ((*i)->getID() == ID)
      return *i;

  return NULL;
}

const TagGroup* Node::getTagGroupByID(uint16 ID) const
{
  for (TagGroupList::const_iterator i = mGroups.begin();  i != mGroups.end();  i++)
    if ((*i)->getID() == ID)
      return *i;

  return NULL;
}

TagGroup* Node::getTagGroupByIndex(unsigned int index)
{
  return mGroups[index];
}

const TagGroup* Node::getTagGroupByIndex(unsigned int index) const
{
  return mGroups[index];
}

TagGroup* Node::getTagGroupByName(const std::string& name)
{
  for (TagGroupList::iterator i = mGroups.begin();  i != mGroups.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

const TagGroup* Node::getTagGroupByName(const std::string& name) const
{
  for (TagGroupList::const_iterator i = mGroups.begin();  i != mGroups.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

unsigned int Node::getTagGroupCount(void) const
{
  return mGroups.size();
}

bool Node::isMine(void) const
{
  return mOwner == VN_OWNER_MINE;
}

VNodeID Node::getID(void) const
{
  return mID;
}

VNodeType Node::getType(void) const
{
  return mType;
}
  
const std::string& Node::getName(void) const
{
  return mName;
}

void Node::setName(const std::string& name)
{
  mSession.push();
  verse_send_node_name_set(mID, name.c_str());
  mSession.pop();
}

Session& Node::getSession(void) const
{
  return mSession;
}

Node::Node(VNodeID ID, VNodeType type, VNodeOwner owner, Session& session):
  mID(ID),
  mType(type),
  mOwner(owner),
  mSession(session)
{
}

Node::~Node(void)
{
  while (mGroups.size())
  {
    delete mGroups.back();
    mGroups.pop_back();
  }
}

void Node::initialize(void)
{
  TagGroup::initialize();

  verse_callback_set((void*) verse_send_node_name_set,
                     (void*) receiveNodeNameSet,
                     NULL);
  verse_callback_set((void*) verse_send_tag_group_create,
                     (void*) receiveTagGroupCreate,
                     NULL);
  verse_callback_set((void*) verse_send_tag_group_destroy,
                     (void*) receiveTagGroupDestroy,
                     NULL);
}

void Node::receiveNodeNameSet(void* user, VNodeID ID, const char* name)
{
  Session* session = Session::getCurrent();

  Node* node = session->getNodeByID(ID);
  if (!node)
    return;
  
  const Node::ObserverList& observers = node->getObservers();
  for (Node::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onSetName(*node, name);
  
  node->mName = name;
  node->updateData();
}

void Node::receiveTagGroupCreate(void* user, VNodeID ID, uint16 groupID, const char* name)
{
  Session* session = Session::getCurrent();

  Node* node = session->getNodeByID(ID);
  if (!node)
    return;

  TagGroup* group = node->getTagGroupByID(groupID);
  if (group)
  {
    const TagGroup::ObserverList& observers = group->getObservers();
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
    
    const Node::ObserverList& observers = node->getObservers();
    for (Node::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      (*i)->onCreateTagGroup(*node, *group);
    
    verse_send_tag_group_subscribe(node->getID(), groupID);
  }
}

void Node::receiveTagGroupDestroy(void* user, VNodeID ID, uint16 groupID)
{
  Session* session = Session::getCurrent();

  Node* node = session->getNodeByID(ID);
  if (!node)
    return;

  Node::TagGroupList& groups = node->mGroups;
  for (Node::TagGroupList::iterator group = groups.begin();  group != groups.end();  group++)
  {
    if ((*group)->getID() == groupID)
    {
      // Notify tag group observers.
      {
        const TagGroup::ObserverList& observers = (*group)->getObservers();
        for (TagGroup::ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
          (*observer)->onDestroy(*(*group));
      }
      
      // Notify node observers.
      {
        const Node::ObserverList& observers = node->getObservers();
        for (Node::ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
          (*observer)->onDestroyTagGroup(*node, *(*group));
      }
      
      delete *group;
      groups.erase(group);

      node->updateStructure();
      break;
    }
  }
}

//---------------------------------------------------------------------

void NodeObserver::onCreateTagGroup(Node& node, TagGroup& group)
{
}

void NodeObserver::onDestroyTagGroup(Node& node, TagGroup& group)
{
}

void NodeObserver::onSetName(Node& node, const std::string& name)
{
}

void NodeObserver::onDestroy(Node& node)
{
}

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/

