//---------------------------------------------------------------------
// Simple C++ retained mode library for Verse
// Copyright (c) PDC, KTH
// Written by Camilla Berglund <clb@kth.se>
//---------------------------------------------------------------------

#include <verse.h>

#include "Ample.h"

namespace verse
{
  namespace ample
  {

//---------------------------------------------------------------------

void Tag::destroy(void)
{
  Session& session = mGroup.getNode().getSession();

  session.push();
  verse_send_tag_destroy(mGroup.getNode().getID(), mGroup.getID(), mID);
  session.pop();
}

uint16 Tag::getID(void) const
{
  return mID;
}

const std::string& Tag::getName(void) const
{
  return mName;
}

void Tag::setName(const std::string& name)
{
  Session& session = mGroup.getNode().getSession();

  session.push();
  verse_send_tag_create(mGroup.getNode().getID(), mGroup.getID(), mID,
			name.c_str(), mType, &mValue);
  session.pop();
}

VNTagType Tag::getType(void) const
{
  return mType;
}

void Tag::setType(VNTagType type, const VNTag& value)
{
  Session& session = mGroup.getNode().getSession();

  session.push();
  verse_send_tag_create(mGroup.getNode().getID(), mGroup.getID(), mID,
			mName.c_str(), type, const_cast<VNTag*>(&value));
  session.pop();
}

const VNTag& Tag::getValue(void) const
{
  return mValue;
}

void Tag::setValue(const VNTag& value)
{
  Session& session = mGroup.getNode().getSession();

  session.push();
  verse_send_tag_create(mGroup.getNode().getID(), mGroup.getID(), mID,
			mName.c_str(), mType, const_cast<VNTag*>(&value));
  session.pop();
}

TagGroup& Tag::getGroup(void) const
{
  return mGroup;
}

Tag::Tag(uint16 ID, const std::string& name, VNTagType type, const VNTag& value, TagGroup& group):
  mID(ID),
  mName(name),
  mType(type),
  mValue(value),
  mGroup(group)
{
}

//---------------------------------------------------------------------

void TagObserver::onSetType(Tag& tag, VNTagType type, const VNTag& value)
{
}

void TagObserver::onSetValue(Tag& tag, const VNTag& value)
{
}

void TagObserver::onSetName(Tag& tag, const std::string name)
{
}

void TagObserver::onDestroy(Tag& tag)
{
}

//---------------------------------------------------------------------

void TagGroup::destroy(void)
{
  Session& session = mNode.getSession();

  session.push();
  verse_send_tag_group_destroy(mNode.getID(), mID);
  session.pop();
}

void TagGroup::createTag(const std::string& name, VNTagType type, const VNTag& value)
{
  Session& session = mNode.getSession();

  session.push();
  verse_send_tag_create(mNode.getID(), mID, ~0, name.c_str(), type,
                        const_cast<VNTag*>(&value));
  session.pop();
}

uint16 TagGroup::getID(void) const
{
  return mID;
}

const std::string& TagGroup::getName(void) const
{
  return mName;
}

void TagGroup::setName(const std::string& name)
{
  Session& session = mNode.getSession();

  session.push();
  verse_send_tag_group_create(mNode.getID(), mID, name.c_str());
  session.pop();
}

Tag* TagGroup::getTagByID(uint16 ID)
{
  for (TagList::iterator i = mTags.begin();  i != mTags.end();  i++)
    if ((*i)->getID() == ID)
      return *i;

  return NULL;
}

const Tag* TagGroup::getTagByID(uint16 ID) const
{
  for (TagList::const_iterator i = mTags.begin();  i != mTags.end();  i++)
    if ((*i)->getID() == ID)
      return *i;

  return NULL;
}

Tag* TagGroup::getTagByIndex(unsigned int index)
{
  return mTags[index];
}

const Tag* TagGroup::getTagByIndex(unsigned int index) const
{
  return mTags[index];
}

Tag* TagGroup::getTagByName(const std::string& name)
{
  for (TagList::iterator i = mTags.begin();  i != mTags.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

const Tag* TagGroup::getTagByName(const std::string& name) const
{
  for (TagList::const_iterator i = mTags.begin();  i != mTags.end();  i++)
    if ((*i)->getName() == name)
      return *i;

  return NULL;
}

unsigned int TagGroup::getTagCount(void) const
{
  return mTags.size();
}

Node& TagGroup::getNode(void) const
{
  return mNode;
}

TagGroup::TagGroup(uint16 ID, const std::string& name, Node& node):
  mID(ID),
  mName(name),
  mNode(node)
{
}

TagGroup::~TagGroup(void)
{
  while (mTags.size())
  {
    delete mTags.back();
    mTags.pop_back();
  }
}

void TagGroup::initialize(void)
{
  verse_callback_set((void*) verse_send_tag_create,
                     (void*) receiveTagCreate,
                     NULL);
  verse_callback_set((void*) verse_send_tag_destroy,
                     (void*) receiveTagDestroy,
                     NULL);
}

void TagGroup::receiveTagCreate(void* user, VNodeID ID, uint16 groupID, uint16 tagID, const char* name, VNTagType type, const VNTag* value)
{
  Session* session = Session::getCurrent();

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
      const Tag::ObserverList& observers = tag->getObservers();
      for (Tag::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
	(*i)->onSetName(*tag, name);

      tag->mName = name;
      tag->updateDataVersion();
    }

    if (tag->mType == type)
    {
      // TODO: Compare values.

      const Tag::ObserverList& observers = tag->getObservers();
      for (Tag::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
	(*i)->onSetValue(*tag, *value);

      tag->mValue = *value;
      tag->updateDataVersion();
    }
    else
    {
      const Tag::ObserverList& observers = tag->getObservers();
      for (Tag::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
	(*i)->onSetType(*tag, type, *value);

      tag->mType = type;
      tag->mValue = *value;
      tag->updateDataVersion();
    }
  }
  else
  {
    tag = new Tag(tagID, name, type, *value, *group);
    group->mTags.push_back(tag);
    group->updateStructureVersion();

    const TagGroup::ObserverList& observers = group->getObservers();
    for (TagGroup::ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
      (*i)->onCreateTag(*group, *tag);
  }
}

void TagGroup::receiveTagDestroy(void* user, VNodeID ID, uint16 groupID, uint16 tagID)
{
  Session* session = Session::getCurrent();

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
      // Notify tag observers.
      {
        const Tag::ObserverList& observers = (*tag)->getObservers();
        for (Tag::ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
          (*observer)->onDestroy(*(*tag));
      }

      // Notify tag group observers.
      {
        const TagGroup::ObserverList& observers = group->getObservers();
        for (TagGroup::ObserverList::const_iterator observer = observers.begin();  observer != observers.end();  observer++)
          (*observer)->onDestroyTag(*group, *(*tag));
      }

      delete *tag;
      tags.erase(tag);

      group->updateStructureVersion();
      break;
    }
  }
}

//---------------------------------------------------------------------

void TagGroupObserver::onCreateTag(TagGroup& group, Tag& tag)
{
}

void TagGroupObserver::onDestroyTag(TagGroup& group, Tag& tag)
{
}

void TagGroupObserver::onSetName(TagGroup& group, const std::string& name)
{
}

void TagGroupObserver::onDestroy(TagGroup& group)
{
}

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/

