//---------------------------------------------------------------------
// Simple C++ retained mode library for Verse
// Copyright (c) PDC, KTH
// Written by Camilla Berglund <elmindreda@elmindreda.org>
//---------------------------------------------------------------------

#include <verse.h>

#include "Ample.h"

namespace verse
{
  namespace ample
  {

//---------------------------------------------------------------------

void TextBuffer::replaceRange(uint32 position, uint32 length, const std::string& text)
{
  Session& session = mNode.getSession();

  session.push();
  verse_send_t_text_set(mNode.getID(), mID, position, length, text.c_str());
  session.pop();
}

void TextBuffer::destroy(void)
{
  Session& session = mNode.getSession();

  session.push();
  verse_send_t_buffer_destroy(mNode.getID(), mID);
  session.pop();
}

VBufferID TextBuffer::getID(void) const
{
  return mID;
}

const std::string& TextBuffer::getName(void) const
{
  return mName;
}

void TextBuffer::setName(const std::string& name)
{
  Session& session = mNode.getSession();

  session.push();
  verse_send_t_buffer_create(mNode.getID(), mID, name.c_str());
  session.pop();
}

const std::string& TextBuffer::getText(void) const
{
  return mText;
}

void TextBuffer::setText(const std::string& text)
{
  Session& session = mNode.getSession();

  session.push();
  verse_send_t_text_set(mNode.getID(), mID, 0, mText.size(), text.c_str());
  session.pop();
}

TextNode& TextBuffer::getNode(void) const
{
  return mNode;
}

TextBuffer::TextBuffer(VBufferID ID, const std::string& name, TextNode& node):
  mID(ID),
  mName(name),
  mNode(node)
{
}

void TextBuffer::initialize(void)
{
  verse_callback_set((void*) verse_send_t_text_set,
                     (void*) receiveTextBufferSet,
                     NULL);
}

void TextBuffer::receiveTextBufferSet(void* user, VNodeID ID, VBufferID bufferID, uint32 position, uint32 length, const char* text)
{
  Session* session = Session::getCurrent();

  TextNode* node = dynamic_cast<TextNode*>(session->getNodeByID(ID));
  if (!node)
    return;

  TextBuffer* buffer = node->getBufferByID(bufferID);
  if (!buffer)
    return;

  const TextBuffer::ObserverList& observers = buffer->getObservers();
  for (TextBuffer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onReplaceRange(*buffer, position, length, text);

  buffer->mText.replace(position, length, text);
  buffer->updateDataVersion();
}

//---------------------------------------------------------------------

void TextBufferObserver::onReplaceRange(TextBuffer& buffer, uint32 position, uint32 length, const std::string& text)
{
}

void TextBufferObserver::onSetName(TextBuffer& buffer, const std::string& name)
{
}

void TextBufferObserver::onDestroy(TextBuffer& buffer)
{
}

//---------------------------------------------------------------------

void TextNode::createBuffer(const std::string& name)
{
  getSession().push();
  verse_send_t_buffer_create(getID(), ~1, name.c_str());
  getSession().pop();
}

TextBuffer* TextNode::getBufferByID(VBufferID ID)
{
  for (BufferList::iterator i = mBuffers.begin();  i != mBuffers.end();  i++)
    if ((*i)->getID() == ID)
      return *i;

  return NULL;
}

const TextBuffer* TextNode::getBufferByID(VBufferID ID) const
{
  for (BufferList::const_iterator i = mBuffers.begin();  i != mBuffers.end();  i++)
    if ((*i)->getID() == ID)
      return *i;

  return NULL;
}

TextBuffer* TextNode::getBufferByIndex(unsigned int index)
{
  return mBuffers[index];
}

const TextBuffer* TextNode::getBufferByIndex(unsigned int index) const
{
  return mBuffers[index];
}

TextBuffer* TextNode::getBufferByName(const std::string& name)
{
  for (BufferList::iterator i = mBuffers.begin();  i != mBuffers.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

const TextBuffer* TextNode::getBufferByName(const std::string& name) const
{
  for (BufferList::const_iterator i = mBuffers.begin();  i != mBuffers.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

unsigned int TextNode::getBufferCount(void) const
{
  return mBuffers.size();
}

const std::string& TextNode::getLanguage(void) const
{
  return mLanguage;
}

void TextNode::setLanguage(const std::string& language)
{
  getSession().push();
  verse_send_t_language_set(getID(), language.c_str());
  getSession().pop();
}

TextNode::TextNode(VNodeID ID, VNodeOwner owner, Session& session):
  Node(ID, V_NT_TEXT, owner, session)
{
}

TextNode::~TextNode(void)
{
  for (BufferList::iterator i = mBuffers.begin();  i != mBuffers.end();  i++)
    delete *i;
}

void TextNode::initialize(void)
{
  TextBuffer::initialize();

  verse_callback_set((void*) verse_send_t_language_set,
                     (void*) receiveNodeLanguageSet,
                     NULL);
  verse_callback_set((void*) verse_send_t_buffer_create,
                     (void*) receiveTextBufferCreate,
                     NULL);
  verse_callback_set((void*) verse_send_t_buffer_destroy,
                     (void*) receiveTextBufferDestroy,
                     NULL);
}

void TextNode::receiveNodeLanguageSet(void* user, VNodeID ID, const char* language)
{
  Session* session = Session::getCurrent();

  TextNode* node = dynamic_cast<TextNode*>(session->getNodeByID(ID));
  if (!node)
    return;

  const Node::ObserverList& observers = node->getObservers();
  for (Node::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
  {
    if (TextNodeObserver* observer = dynamic_cast<TextNodeObserver*>(*i))
      observer->onSetLanguage(*node, language);
  }

  node->mLanguage = language;
  node->updateDataVersion();
}

void TextNode::receiveTextBufferCreate(void* user, VNodeID ID, VBufferID bufferID, const char* name)
{
  Session* session = Session::getCurrent();

  TextNode* node = dynamic_cast<TextNode*>(session->getNodeByID(ID));
  if (!node)
    return;

  TextBuffer* buffer = node->getBufferByID(bufferID);
  if (buffer)
  {
    if (buffer->mName != name)
    {
      const TextBuffer::ObserverList& observers = buffer->getObservers();
      for (TextBuffer::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
	(*i)->onSetName(*buffer, name);

      buffer->mName = name;
      buffer->updateDataVersion();
    }
  }
  else
  {
    buffer = new TextBuffer(bufferID, name, *node);
    node->mBuffers.push_back(buffer);
    node->updateStructureVersion();

    const Node::ObserverList& observers = node->getObservers();
    for (Node::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    {
      if (TextNodeObserver* observer = dynamic_cast<TextNodeObserver*>(*i))
	observer->onCreateBuffer(*node, *buffer);
    }

    verse_send_t_buffer_subscribe(node->getID(), bufferID);
  }
}

void TextNode::receiveTextBufferDestroy(void* user, VNodeID ID, VBufferID bufferID)
{
  Session* session = Session::getCurrent();

  TextNode* node = dynamic_cast<TextNode*>(session->getNodeByID(ID));
  if (!node)
    return;

  TextNode::BufferList& buffers = node->mBuffers;
  for (TextNode::BufferList::iterator buffer = buffers.begin();  buffer != buffers.end();  buffer++)
  {
    if ((*buffer)->getID() == bufferID)
    {
      // Notify text buffer observers.
      {
        const TextBuffer::ObserverList& observers = (*buffer)->getObservers();
        for (TextBuffer::ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
          (*observer)->onDestroy(*(*buffer));
      }

      // Notify text node observers.
      const TextNode::ObserverList& observers = node->getObservers();
      for (TextNode::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      {
	if (TextNodeObserver* observer = dynamic_cast<TextNodeObserver*>(*i))
	  observer->onDestroyBuffer(*node, *(*buffer));
      }

      delete *buffer;
      buffers.erase(buffer);

      node->updateStructureVersion();
      break;
    }
  }
}

//---------------------------------------------------------------------

void TextNodeObserver::onCreateBuffer(TextNode& node, TextBuffer& buffer)
{
}

void TextNodeObserver::onDestroyBuffer(TextNode& node, TextBuffer& buffer)
{
}

void TextNodeObserver::onSetLanguage(TextNode& node, const std::string& language)
{
}

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/

