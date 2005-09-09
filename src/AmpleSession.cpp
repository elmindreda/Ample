
#include <new>

#include <verse.h>

#include "Ample.h"

namespace verse
{
  namespace ample
  {

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

void Session::release(void)
{
  if (mState == TERMINATED)
    mState = RELEASED;
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
    Node::initialize();
    TextNode::initialize();
    GeometryNode::initialize();
    ObjectNode::initialize();

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
  for (SessionList::iterator session = msSessions.begin();  session != msSessions.end();  )
  {
    if ((*session)->mState == RELEASED)
    {
      const ObserverList& observers = (*session)->getObservers();
      for (ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
        (*observer)->onDestroy(*(*session));

      SessionList::iterator released = session++;
      msSessions.erase(released);
    }
    else
    {
      if ((*session)->mState == CONNECTING || (*session)->mState == CONNECTED)
      {
        (*session)->push();
        verse_callback_update(microseconds);
        (*session)->pop();
      }

      session++;
    }
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

Session* Session::getByIndex(unsigned int index)
{
  SessionList::iterator i = msSessions.begin();
  std::advance(i, index);
  return *i;
}

unsigned int Session::getCount(void)
{
  return msSessions.size();
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

  const ObserverList& observers = session->getObservers();
  for (ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onAccept(*session);

  // Subscribe to all node types
  verse_send_node_index_subscribe(session->mTypeMask);
}

void Session::receiveTerminate(void* user, const char* address, const char* byebye)
{
  Session* session = getCurrent();

  const ObserverList& observers = session->getObservers();
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
      node = new ObjectNode(ID, owner, *session); 
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

  const ObserverList& observers = session->getObservers();
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
      // Notify node observers.
      {
        const Node::ObserverList& observers = (*node)->getObservers();
        for (Node::ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
          (*observer)->onDestroy(*(*node));
      }

      // Notify session observers.
      {
        const ObserverList& observers = session->getObservers();
        for (ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
          (*observer)->onDestroyNode(*session, *(*node));
      }

      delete *node;
      nodes.erase(node);

      session->updateStructure();
      break;
    }
  }
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

void SessionObserver::onDestroy(Session& session)
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

