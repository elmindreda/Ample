//---------------------------------------------------------------------
// Simple C++ retained mode library for Verse
// Copyright (c) PDC, KTH
// Written by Camilla Berglund <clb@kth.se>
//---------------------------------------------------------------------

#ifndef __AMPLE_H__
#define __AMPLE_H__

#include <algorithm>
#include <string>
#include <vector>
#include <stack>
#include <list>
#include <map>

#include <AmpleUtil.h>

namespace verse
{
  namespace ample
  {

//---------------------------------------------------------------------

const uint32 INVALID_VERTEX_ID = ((uint32) ~0);
const uint32 INVALID_POLYGON_ID = ((uint32) ~0);

const VLayerID BASE_VERTEX_LAYER_ID = 0;
const VLayerID BASE_POLYGON_LAYER_ID = 1;

//---------------------------------------------------------------------

class BaseVertex;
class BasePolygon;
class BaseMesh;

class Quaternion64;

template <typename T>
class Observer;
template <typename T>
class Observable;

class Versioned;

class Tag;
class TagObserver;
class TagGroup;
class TagGroupObserver;

class Node;
class NodeObserver;

class TextBuffer;
class TextBufferObserver;
class TextNode;
class TextNodeObserver;

class GeometryLayer;
class GeometryLayerObserver;
class Bone;
class BoneObserver;
class GeometryNode;
class GeometryNodeObserver;

class Method;
class MethodObserver;
class MethodGroup;
class MethodGroupObserver;
class Link;
class LinkObserver;
class ObjectNode;
class ObjectNodeObserver;

class BitmapLayer;
class BitmapLayerObserver;
class BitmapNode;
class BitmapNodeObserver;

class Fragment;
class FragmentObserver;
class MaterialNode;
class MaterialNodeObserver;

class Session;
class SessionObserver;

//---------------------------------------------------------------------

class BaseVertex
{
public:
  BaseVertex(void);
  BaseVertex(real64 sx, real64 sy, real64 sz);
  bool isValid(void) const;
  void set(real64 sx, real64 sy, real64 sz);
  void setInvalid(void);
  real64 x;
  real64 y;
  real64 z;
};

//---------------------------------------------------------------------

class BasePolygon
{
public:
  BasePolygon(void);
  BasePolygon(uint32 v0, uint32 v1, uint32 v2, uint32 v3);
  bool isValid(void) const;
  void set(uint32 v0, uint32 v1, uint32 v2, uint32 v3); 
  void setInvalid(void);
  uint32 mIndices[4];
};

//---------------------------------------------------------------------

class BaseMesh
{
public:
  typedef std::vector<BaseVertex> VertexList;
  typedef std::vector<BasePolygon> PolygonList;
  VertexList mVertices;
  PolygonList mPolygons;
};

//---------------------------------------------------------------------

class Quaternion64 : public VNQuat64
{
public:
  Quaternion64(void);
  Quaternion64(real64 sx, real64 sy, real64 sz, real64 sw);
  void invert(void);
  void normalize(void);
  template <typename T>
  void rotateVector(Vector3<T>& vector) const;
  real64 dotProduct(const Quaternion64& other) const;
  Quaternion64 interpolateTo(real64 t, const Quaternion64& other) const;
  Quaternion64 operator * (real64 value) const;
  Quaternion64 operator / (real64 value) const;
  Quaternion64 operator + (const Quaternion64& quat) const;
  Quaternion64 operator * (const Quaternion64& quat) const;
  Quaternion64& operator += (const Quaternion64& quat);
  Quaternion64& operator *= (const Quaternion64& quat);
  Quaternion64& operator = (const VNQuat32& source);
  Quaternion64& operator = (const VNQuat64& source);
  bool operator == (const Quaternion64& quaternion) const;
  bool operator != (const Quaternion64& quaternion) const;
  void set(real64 sx, real64 sy, real64 sz, real64 sw);
  void setDefaults(void);
  template <typename T>
  void setVectorRotation(const Vector3<T>& vector, T angle);
  template <typename T>
  void getAxisRotation(Vector3<T>& axis, T& angle) const;
  template <typename T>
  void setAxisRotation(const Vector3<T>& axis, T angle);
};

//---------------------------------------------------------------------

class Translation
{
public:
  Vector3d mPosition;
  Vector3d mSpeed;
  Vector3d mAccel;
  Vector3d mDragNormal;
  uint32 mSeconds;
  uint32 mFraction;
  real64 mDrag; 
};

//---------------------------------------------------------------------

class Rotation
{
public:
  Quaternion64 mRotation;
  Quaternion64 mSpeed;
  Quaternion64 mAccel;
  Quaternion64 mDragNormal;
  uint32 mSeconds;
  uint32 mFraction;
  real64 mDrag;
};

//---------------------------------------------------------------------

class Block
{
public:
  Block(void);
  Block(const Block& source);
  ~Block(void);
  void resize(size_t count);
  void reserve(size_t count);
  void release(void);
  operator void* (void);
  operator const void* (void) const;
  Block& operator = (const Block& source);
  void* getItem(size_t index);
  const void* getItem(size_t index) const;
  void* getItems(void);
  const void* getitems(void) const;
  void setItem(void* item, size_t index);
  size_t getItemSize(void) const;
  void setItemSize(size_t size);
  size_t getItemCount(void) const;
  size_t getGranularity(void) const;
  void setGranularity(size_t grain);
private:
  size_t mItemCount;
  size_t mItemSize;
  size_t mGrain;
  uint8* mData;
};

//---------------------------------------------------------------------

/*! Base class for observer interfaces.
 */
template <typename T>
class Observer
{
public:
  typedef Observable<T> ObservableType;
  friend class Observable<T>;
  /*! Destructor. Detaches the observer from all observed objects.
   */
  inline virtual ~Observer(void);
  /*! Detaches all observed objects from this observer.
   */
  inline void detachObservables(void);
private:
  typedef std::vector<ObservableType*> ObservableList;
  ObservableList mObservables;
};

//---------------------------------------------------------------------

template <typename T>
inline Observer<T>::~Observer(void)
{
  detachObservables();
}

template <typename T>
inline void Observer<T>::detachObservables(void)
{
  while (mObservables.size())
    mObservables.front()->removeObserver(*this);
}

//---------------------------------------------------------------------

/*! Base class for all observable objects.
 */
template <typename T>
class Observable
{
public:
  typedef Observer<T> ObserverType;
  typedef std::vector<T*> ObserverList;
  /*! Destructor. Detaches all attached observers.
   */
  inline virtual ~Observable(void);
  /*! Adds the specified observer to this object.
   *  @param observer The observer to add.
   *  @remarks An observer can only be added once. Additional additions are ignored.
   */
  inline void addObserver(T& observer);
  /*! Removes the specified observer from this object.
   *  @param observer The observer to remove.
   */
  inline void removeObserver(T& observer); 
  /*! Removes the specified observer from this object.
   *  @param observer The observer to remove.
   */
  inline void removeObserver(Observer<T>& observer); 
  /*! @return The list of observers attached to this object.
   */
  inline const ObserverList& getObservers(void) const;
private:
  ObserverList mObservers;
};

//---------------------------------------------------------------------

template <typename T>
inline Observable<T>::~Observable(void)
{
  while (mObservers.size())
    removeObserver(*mObservers.front());
}

template <typename T>
inline void Observable<T>::addObserver(T& observer)
{
  if (std::find(mObservers.begin(), mObservers.end(), &observer) != mObservers.end())
    return;

  mObservers.push_back(&observer);
  static_cast<ObserverType&>(observer).mObservables.push_back(this);
}

template <typename T>
inline void Observable<T>::removeObserver(T& observer)
{
  typename ObserverList::iterator i = std::find(mObservers.begin(), mObservers.end(), &observer);
  if (i != mObservers.end())
  {
    typename ObserverType::ObservableList& observables = static_cast<ObserverType&>(observer).mObservables;
    observables.erase(std::find(observables.begin(), observables.end(), this));
    mObservers.erase(i);
  }
}

template <typename T>
inline void Observable<T>::removeObserver(Observer<T>& observer)
{
  typename ObserverList::iterator i = std::find(mObservers.begin(), mObservers.end(), &observer);
  if (i != mObservers.end())
  {
    typename ObserverType::ObservableList& observables = observer.mObservables;
    observables.erase(std::find(observables.begin(), observables.end(), this));
    mObservers.erase(i);
  }
}

template <typename T>
inline const typename Observable<T>::ObserverList& Observable<T>::getObservers(void) const
{
  return mObservers;
}

//---------------------------------------------------------------------

/*! Base class for versioned objects.
 */
class Versioned
{
  friend class TagGroup;
  friend class Node;
  friend class TextBuffer;
  friend class TextNode;
  friend class GeometryLayer;
  friend class GeometryNode;
  friend class MethodGroup;
  friend class Link;
  friend class ObjectNode;
  friend class BitmapNode;
  friend class Session;
public:
  /*! Constructor.
   */
  Versioned(void);
  /*! @return The current version of the internal state.
   *  @remarks This value is updated for any kind of change.
   */
  unsigned int getDataVersion(void) const;
  /*! @return The current version of the internal structure.
   *  @remarks This value is only updated when the internal structure changes.
   */
  unsigned int getStructureVersion(void) const;
private:
  void updateDataVersion(void);
  void updateStructureVersion(void);
  unsigned int mDataVersion;
  unsigned int mStructVersion;
};

//---------------------------------------------------------------------

/*! Node tag class. Represents a single tag in a node's tag group.
 */
class Tag : public Versioned, public Observable<TagObserver>
{
  friend class TagGroup;
public:
  /*! Destroys this tag.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void destroy(void);
  /*! @return The ID of this tag.
   */
  uint16 getID(void) const;
  /*! @return The name of this tag.
   */
  const std::string& getName(void) const;
  /*! Changes the name of this tag.
   *  @param name The desired name.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setName(const std::string& name);
  //! @todo Replace VNTag with some OO.
  /*! @return The type of this tag.
   */
  VNTagType getType(void) const;
  /*! Changes the type of this tag and sets an initial value.
   *  @param type The desired type.
   *  @param value The desired initial value.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setType(VNTagType type, const VNTag& value);
  /*! @return The current value of this tag.
   */
  const VNTag& getValue(void) const;
  /*! Changes the value of this tag.
   *  @param value The desired value.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setValue(const VNTag& value);
  /*! @return The group containing this tag.
   */
  TagGroup& getGroup(void) const;
private:
  Tag(uint16 ID, const std::string& name, VNTagType type, const VNTag& value, TagGroup& group);
  uint16 mID;
  std::string mName;
  VNTagType mType;
  VNTag mValue;
  TagGroup& mGroup;
};

//---------------------------------------------------------------------

/*! Observer interface for tags.
 */
class TagObserver : public Observer<TagObserver>
{
public:
  virtual void onSetType(Tag& tag, VNTagType type, const VNTag& value);
  virtual void onSetValue(Tag& tag, const VNTag& value);
  /*! Called before an observed tag has its name changed.
   *  @param tag The observed tag.
   *  @param name The new name of the tag.
   */
  virtual void onSetName(Tag& tag, const std::string name);
  /*! Called before an observed tag is destroyed.
   *  @param tag The tag to be destroyed.
   */
  virtual void onDestroy(Tag& tag);
};

//---------------------------------------------------------------------

/*! Node tag group class. Represents a single tag group in a node.
 */
class TagGroup : public Versioned, public Observable<TagGroupObserver>
{
  friend class Node;
public:
  /*! Destroys this tag group.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void destroy(void);
  /*! Creates a new tag in this tag group.
   *  @param name The desired name of the tag.
   *  @param type The desired type of the tag.
   *  @param value The initial value of the tag.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void createTag(const std::string& name, VNTagType type, const VNTag& value);
  /*! @return The ID of this tag group.
   */
  uint16 getID(void) const;
  /*! @return The name of this tag group.
   */
  const std::string& getName(void) const;
  /*! Changes the name of this tag group.
   *  @param name The desired name.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setName(const std::string& name);
  /*! @param ID The ID of the desired tag.
   *  @return The tag with the specified ID, or @c NULL if no such tag exists.
   */
  Tag* getTagByID(uint16 ID);
  /*! @param ID The ID of the desired tag.
   *  @return The tag with the specified ID, or @c NULL if no such tag exists.
   */
  const Tag* getTagByID(uint16 ID) const;
  /*! @param index The index of the desired tag.
   *  @return The tag at the specified index, or @c NULL if no such tag exists.
   */
  Tag* getTagByIndex(unsigned int index);
  /*! @param index The index of the desired tag.
   *  @return The tag at the specified index, or @c NULL if no such tag exists.
   */
  const Tag* getTagByIndex(unsigned int index) const;
  /*! @param name The name of the desired tag.
   *  @return The tag with the specified name, or @c NULL if no such tag exists.
   */
  Tag* getTagByName(const std::string& name);
  /*! @param name The name of the desired tag.
   *  @return The tag with the specified name, or @c NULL if no such tag exists.
   */
  const Tag* getTagByName(const std::string& name) const;
  /*! @return The number of tags in this tag group.
   */
  unsigned int getTagCount(void) const;
  /*! @return The node containing this tag group.
   */
  Node& getNode(void) const;
private:
  TagGroup(uint16 ID, const std::string& name, Node& node);
  ~TagGroup(void);
  static void initialize(void);
  static void receiveTagCreate(void* user, VNodeID ID, uint16 groupID, uint16 tagID, const char* name, VNTagType type, const VNTag* value);
  static void receiveTagDestroy(void* user, VNodeID ID, uint16 groupID, uint16 tagID);
  typedef std::vector<Tag*> TagList;
  TagList mTags;
  std::string mName;
  uint16 mID;
  Node& mNode;
};
  
//---------------------------------------------------------------------

/*! Observer interface for tag groups.
 */
class TagGroupObserver : public Observer<TagGroupObserver>
{
public:
  /*! Called after a new tag is created in an observed tag group.
   *  @param group The tag group in which the tag was created.
   *  @param tag The newly created tag.
   */
  virtual void onCreateTag(TagGroup& group, Tag& tag);
  /*! Called before a tag is destroyed in an observed tag group.
   *  @param group The tag group containing the tag to be destroyed.
   *  @param tag The tag to be destroyed.
   */
  virtual void onDestroyTag(TagGroup& group, Tag& tag);
  /*! Called before an observed tag group has its name changed.
   *  @param group The observed tag group.
   *  @param name The new name of the tag group.
   */
  virtual void onSetName(TagGroup& group, const std::string& name);
  /*! Called before an observed tag group is destroyed.
   *  @param group The tag group to be destroyed.
   */
  virtual void onDestroy(TagGroup& group);
};

//---------------------------------------------------------------------

/*! Node base class. Represents a single node of unknown type.
 */
class Node : public Versioned, public Observable<NodeObserver>
{
  friend class Session;
public:
  /*! Destroys this node.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void destroy(void);
  /*! Creates a new tag group in this node.
   *  @param name The desired name of the tag group.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void createTagGroup(const std::string& name);
  /*! @param ID The ID of the desired tag group.
   *  @return The tag group with the specified ID, or @c NULL if no such tag group exists.
   */
  TagGroup* getTagGroupByID(uint16 ID);
  /*! @param ID The ID of the desired tag group.
   *  @return The tag group with the specified ID, or @c NULL if no such tag group exists.
   */
  const TagGroup* getTagGroupByID(uint16 ID) const;
  /*! @param index The index of the desired tag group.
   *  @return The tag group at the specified index, or @c NULL if no such tag group exists.
   */
  TagGroup* getTagGroupByIndex(unsigned int index);
  /*! @param index The index of the desired tag group.
   *  @return The tag group at the specified index, or @c NULL if no such tag group exists.
   */
  const TagGroup* getTagGroupByIndex(unsigned int index) const;
  /*! @param name The name of the desired tag group.
   *  @return The tag group with the specified name, or @c NULL if no such tag group exists.
   */
  TagGroup* getTagGroupByName(const std::string& name);
  /*! @param name The name of the desired tag group.
   *  @return The tag group with the specified name, or @c NULL if no such tag group exists.
   */
  const TagGroup* getTagGroupByName(const std::string& name) const;
  /*! @return The number of tag groups in this node.
   */
  unsigned int getTagGroupCount(void) const;
  /*! @param groupName The name of the group of the desired tag.
   *  @param tagName The name of the desired tag.
   *  @return The tag with the specified name, in the specified group.
   */
  Tag* getTagByNames(const std::string& groupName, const std::string& tagName);
  /*! @param groupName The name of the group of the desired tag.
   *  @param tagName The name of the desired tag.
   *  @return The tag with the specified name, in the specified group.
   */
  const Tag* getTagByNames(const std::string& groupName, const std::string& tagName) const;
  /*! @return @c true if this node was created from this client, otherwise @c false.
   */
  bool isMine(void) const;
  /*! @return The ID of this node.
   */
  VNodeID getID(void) const;
  /*! @return The type of this node.
   */
  VNodeType getType(void) const;
  /*! @return The name of this node.
   */
  const std::string& getName(void) const;
  /*! Changes the name of this node.
   *  @param name The desired name.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setName(const std::string& name);
  /*! @return The session containing this node.
   */
  Session& getSession(void) const;
protected:
  /*! Constructs a node object.
   *  @param ID The ID of the node.
   *  @param type The type of the node.
   *  @param owner The owner specification of the node.
   *  @param session The session containing the node.
   */
  Node(VNodeID ID, VNodeType type, VNodeOwner owner, Session& session);
  /*! Destructor.
   */
  virtual ~Node(void);
private:
  static void initialize(void);
  static void receiveNodeNameSet(void* user, VNodeID ID, const char *name);
  static void receiveTagGroupCreate(void* user, VNodeID ID, uint16 groupID, const char* name);
  static void receiveTagGroupDestroy(void* user, VNodeID ID, uint16 groupID);
  typedef std::vector<TagGroup*> TagGroupList;
  VNodeID mID;
  VNodeType mType;
  VNodeOwner mOwner;
  std::string mName;
  TagGroupList mGroups;
  Session& mSession;
};

//---------------------------------------------------------------------

/*! Observer interface for nodes.
 */
class NodeObserver : public Observer<NodeObserver>
{
public:
  /*! Called after a new tag group is created in an observed node.
   *  @param node The node in which the tag group was created.
   *  @param group The newly created tag group.
   */
  virtual void onCreateTagGroup(Node& node, TagGroup& group);
  /*! Called before a tag group is destroyed in an observed node.
   *  @param node The node containing the tag group to be destroyed.
   *  @param group The tag group to be destroyed.
   */
  virtual void onDestroyTagGroup(Node& node, TagGroup& group);
  /*! Called before an observed node has its name changed.
   *  @param node The node to be renamed.
   *  @param name The new name of the tag group.
   */
  virtual void onSetName(Node& node, const std::string& name);
  /*! Called before an observed node is destroyed.
   *  @param node The node to be destroyed.
   */
  virtual void onDestroy(Node& node);
};

//---------------------------------------------------------------------

class NodeNameObserver : public Observer<NodeNameObserver>
{
public:
  virtual void onNewNode(Node& node);
  virtual void onDestroyNode(Node& node);
};

//---------------------------------------------------------------------

/*! Text node buffer class. Represents a single buffer in a text node.
 */
class TextBuffer : public Versioned, public Observable<TextBufferObserver>
{
  friend class Session;
  friend class TextNode;
public:
  /*! Replaces the specified portion (range) with the specified string.
   *  @param position The start of the range to replace.
   *  @param length The number of characters to replace.
   *  @param text The string to replace the specified range with.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void replaceRange(uint32 position,
		    uint32 length,
		    const std::string& text);
  /*! Destroys this text buffer.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void destroy(void);
  /*! @return The ID of this text buffer.
   */
  VBufferID getID(void) const;
  /*! @return The name of this text buffer.
   */
  const std::string& getName(void) const;
  /*! Changes the name of this text buffer.
   *  @param name The desired name.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setName(const std::string& name);
  /*! @return The current contents of this text buffer.
   */
  const std::string& getText(void) const;
  /*! Replaces the entire contents of this text buffer with the specified string.
   *  @param text The desired contents.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setText(const std::string& text);
  /*! @return The node containing this text buffer.
   */
  TextNode& getNode(void) const;
private:
  TextBuffer(VBufferID ID, const std::string& name, TextNode& node);
  static void initialize(void);
  static void receiveTextBufferSet(void* user, VNodeID ID, VBufferID bufferID, uint32 position, uint32 length, const char* text);
  VBufferID mID;
  std::string mName;
  std::string mText;
  TextNode& mNode;
};

//---------------------------------------------------------------------

/*! Observer interface for text node buffers.
 */
class TextBufferObserver : public Observer<TextBufferObserver>
{
public:
  /*! Called before a portion (range) of an observed text buffer is modified.
   *  @param buffer The buffer to be modified.
   *  @param position The start of the range to be replaced.
   *  @param length The number of characters to be replaced.
   *  @param text The string to replace the specified range with.
   */
  virtual void onReplaceRange(TextBuffer& buffer, uint32 position, uint32 length, const std::string& text);
  /*! Called before an observed text buffer has its name changed.
   *  @param buffer The text buffer to be renamed.
   *  @param name The new name of the text buffer.
   */
  virtual void onSetName(TextBuffer& buffer, const std::string& name);
  /*! Called before an observed text buffer is destroyed.
   *  @param buffer The text buffer to be destroyed.
   */
  virtual void onDestroy(TextBuffer& buffer);
};

//---------------------------------------------------------------------

/*! Text node class. Represents a single text node.
 */
class TextNode : public Node
{
  friend class Session;
public:
  /*! Creates a new text buffer in this node.
   *  @param name The desired name of the text buffer.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void createBuffer(const std::string& name);
  /*! @param ID The ID of the desired text buffer.
   *  @return The text buffer with the specified ID, or @c NULL if no such text buffer exists.
   */
  TextBuffer* getBufferByID(VBufferID ID);
  /*! @param ID The ID of the desired text buffer.
   *  @return The text buffer with the specified ID, or @c NULL if no such text buffer exists.
   */
  const TextBuffer* getBufferByID(VBufferID ID) const;
  /*! @param index The index of the desired text buffer.
   *  @return The text buffer at the specified index, or @c NULL if no such text buffer exists.
   */
  TextBuffer* getBufferByIndex(unsigned int index);
  /*! @param index The index of the desired text buffer.
   *  @return The text buffer at the specified index, or @c NULL if no such text buffer exists.
   */
  const TextBuffer* getBufferByIndex(unsigned int index) const;
  /*! @param name The name of the desired text buffer
   *  @return The text buffer with the specified name, or @c NULL if no such text buffer exists.
   */
  TextBuffer* getBufferByName(const std::string& name);
  /*! @param name The name of the desired text buffer
   *  @return The text buffer with the specified name, or @c NULL if no such text buffer exists.
   */
  const TextBuffer* getBufferByName(const std::string& name) const;
  /*! @return The number of text buffers in this node.
   */
  unsigned int getBufferCount(void) const;
  /*! @return The language of this node.
   */
  const std::string& getLanguage(void) const;
  /*! Changes the language of this node.
   *  @param language The desired language.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setLanguage(const std::string& language);
private:
  TextNode(VNodeID ID, VNodeOwner owner, Session& session);
  ~TextNode(void);
  static void initialize(void);
  static void receiveNodeLanguageSet(void* user, VNodeID ID, const char* language);
  static void receiveTextBufferCreate(void* user, VNodeID ID, VBufferID bufferID, const char* name);
  static void receiveTextBufferDestroy(void* user, VNodeID ID, VBufferID bufferID);
  typedef std::vector<TextBuffer*> BufferList;
  BufferList mBuffers;
  std::string mLanguage;
};

//---------------------------------------------------------------------

/*! Observer interface for text nodes.
 */
class TextNodeObserver : public NodeObserver
{
public:
  /*! Called after a new text buffer is created in an observed node.
   *  @param node The node in which the text buffer was created.
   *  @param buffer The newly created text buffer.
   */
  virtual void onCreateBuffer(TextNode& node, TextBuffer& buffer);
  /*! Called before a text buffer is destroyed in an observed node.
   *  @param node The node containing the text buffer to be destroyed.
   *  @param buffer The text buffer to be destroyed.
   */
  virtual void onDestroyBuffer(TextNode& node, TextBuffer& buffer);
  /*! Called before an observed node has its language changed.
   *  @param node The observed node.
   *  @param language The new language of the node.
   */
  virtual void onSetLanguage(TextNode& node, const std::string& language);
};

//---------------------------------------------------------------------

/*! Geometry node layer class. Represents a single layer in a geometry node.
 */
class GeometryLayer : public Versioned, public Observable<GeometryLayerObserver>
{
  friend class GeometryNode;
public:
  /*! Geometry stack enumeration.
   */
  enum Stack
  {
    /*! The geometry layer is a member of the vertex stack.
     */
    VERTEX,
    /*! The geometry layer is a member of the polygon stack.
     */
    POLYGON,
  };
  /*! Destroys this geometry layer.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void destroy(void);
  /*! @return The ID of this geometry layer.
   */
  VLayerID getID(void) const;
  /*! @return The stack in which this layer exists.
   */
  Stack getStack(void) const;
  /*! @return The quality of real value geometry layers.
   */
  VNRealFormat getRealFormat(void) const;
  /*! @return The name of this geometry layer.
   */
  const std::string& getName(void) const;
  /*! Changes the name of this geometry layer.
   *  @param name The desired name.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setName(const std::string& name);
  /*! @return The size, in bytes, of a slot in this geometry layer.
   */
  unsigned int getSlotSize(void) const;
  /*! @return The type of the slots in this geometry layer.
   */
  VNGLayerType getType(void) const;
  /*! Retrieves the specified slot from this geometry layer.
   *  @param slotID The index of the desired slot.
   *  @param data The location to store the slot data.
   *  @return @c true if the slot exists, otherwise @c false.
   */
  bool getSlot(uint32 slotID, void* data) const;
  /*! Changes the specified slot in this geometry layer.
   *  @param slotID The index of the desired slot.
   *  @param data The location to retrieve the slot data from.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setSlot(uint32 slotID, const void* data);
  /*! @return the default value for uninitialized integer slots.
   */
  uint32 getDefaultInt(void) const;
  /*! @return the default value for uninitialized real slots.
   */
  real64 getDefaultReal(void) const;
  /*! @return The node containing this geometry layer.
   */
  GeometryNode& getNode(void) const;
private:
  GeometryLayer(VLayerID ID, const std::string& name, VNGLayerType type,
		GeometryNode& node, uint32 defaultInt, real64 defaultReal);
  void reserve(size_t slotCount);
  static void initialize(void);
  static void receiveVertexSetXyzReal32(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, real32 x, real32 y, real32 z);
  static void receiveVertexDeleteReal32(void* user, VNodeID nodeID, uint32 vertexID);
  static void receiveVertexSetXyzReal64(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, real64 x, real64 y, real64 z);
  static void receiveVertexDeleteReal64(void* user, VNodeID nodeID, uint32 vertexID);
  static void receiveVertexSetUint32(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, uint32 value);
  static void receiveVertexSetReal64(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, real64 value);
  static void receiveVertexSetReal32(void* user, VNodeID nodeID, VLayerID layerID, uint32 vertexID, real32 value);
  static void receivePolygonSetCornerUint32(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, uint32 v0, uint32 v1, uint32 v2, uint32 v3);
  static void receivePolygonDelete(void* user, VNodeID nodeID, uint32 polygonID);
  static void receivePolygonSetCornerReal64(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, real64 v0, real64 v1, real64 v2, real64 v3);
  static void receivePolygonSetCornerReal32(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, real32 v0, real32 v1, real32 v2, real32 v3);
  static void receivePolygonSetFaceUint8(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, uint8 value);
  static void receivePolygonSetFaceUint32(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, uint32 value);
  static void receivePolygonSetFaceReal64(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, real64 value);
  static void receivePolygonSetFaceReal32(void* user, VNodeID nodeID, VLayerID layerID, uint32 polygonID, real32 value);
  static unsigned int getTypeSize(VNGLayerType type);
  static unsigned int getTypeElementCount(VNGLayerType type);
  Block mData;
  VLayerID mID;
  std::string mName;
  Stack mStack;
  VNGLayerType mType;
  VNRealFormat mFormat;
  GeometryNode& mNode;
  uint32 mDefaultInt;
  real64 mDefaultReal;
};

//---------------------------------------------------------------------

/*! Observer interface for geometry node layers.
 */
class GeometryLayerObserver : public Observer<GeometryLayerObserver>
{
public:
  /*! Called before a slot is changed in an observed geometry layer.
   *  @param layer The observed geometry layer.
   *  @param slotID The ID of the slot to be changed.
   *  @param data The data to be written to the slot.
   */
  virtual void onSetSlot(GeometryLayer& layer, uint32 slotID, const void* data);
  /*! Called before an observed geometry layer has its name changed.
   *  @param layer The observed geometry layer.
   *  @param name The new name of the geometry layer.
   */
  virtual void onSetName(GeometryLayer& layer, const std::string& name);
  /*! Called before an observed geometry layer is destroyed.
   *  @param layer The geometry layer to be destroyed.
   */
  virtual void onDestroy(GeometryLayer& layer);
};

//---------------------------------------------------------------------

class Bone : public Observable<BoneObserver>
{
public:
  void destroy(void);
  const Vector3d getPosition(void) const;
  void setPosition(const Vector3d& position);
  const Quaternion64 getRotation(void) const;
  void setRotation(const Quaternion64& rotation);
  const std::string& getCurveLabel(void) const;
  void setCurveLabel(const std::string& label);
  uint16 getID(void) const;
private:
  Bone(uint16 ID,
       const std::string& weight,
       const std::string& reference,
       const Vector3d& position,
       const Quaternion64& rotation,
       const std::string& curveLabel);
  uint16 mID;
  std::string mWeight;
  std::string mReference;
  Vector3d mPosition;
  Quaternion64 mRotation;
  std::string mCurveLabel;
};

//---------------------------------------------------------------------

class BoneObserver : public Observer<BoneObserver>
{
public:
  virtual void onDestroy(Bone& bone);
};

//---------------------------------------------------------------------

/*! Geometry node class. Represents a single geometry node.
 */
class GeometryNode : public Node
{
  friend class GeometryLayer;
  friend class Session;
public:
  /*! Creates a geometry layer in this node.
   *  @param name The desired name of the geometry layer.
   *  @param type The desired slot type of the geometry layer.
   *  @param defaultInt The desired default value for integer slots.
   *  @param defaultReal The desired default value for real slots.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void createLayer(const std::string& name,
                   VNGLayerType type,
		   uint32 defaultInt = 0,
		   real64 defaultReal = 0.0);
  /*! Builds a base mesh of all the valid polygons and vertices in this geometry node.
   *  @param mesh The mesh to receive the constructed geometry.
   *  @return @c true if successful, or @c false if no valid geometry was available.
   *  @remarks This method is intended to make geometry extraction easier.
   */
  bool getBaseMesh(BaseMesh& mesh);
  /*! @param ID The ID of the desired geometry layer.
   *  @return The geometry layer with the specified ID, or @c NULL if no such geometry layer exists.
   */
  GeometryLayer* getLayerByID(VLayerID ID);
  /*! @param ID The ID of the desired geometry layer.
   *  @return The geometry layer with the specified ID, or @c NULL if no such geometry layer exists.
   */
  const GeometryLayer* getLayerByID(VLayerID ID) const;
  /*! @param index The index of the desired geometry layer.
   *  @return The geometry layer at the specified index, or @c NULL if no such geometry layer exists.
   */
  GeometryLayer* getLayerByIndex(unsigned int index);
  /*! @param index The index of the desired geometry layer.
   *  @return The geometry layer at the specified index, or @c NULL if no such geometry layer exists.
   */
  const GeometryLayer* getLayerByIndex(unsigned int index) const;
  /*! @param name The name of the desired geometry layer
   *  @return The geometry layer with the specified name, or @c NULL if no such geometry layer exists.
   */
  GeometryLayer* getLayerByName(const std::string& name);
  /*! @param name The name of the desired geometry layer
   *  @return The geometry layer with the specified name, or @c NULL if no such geometry layer exists.
   */
  const GeometryLayer* getLayerByName(const std::string& name) const;
  /*! @return @c true if the vertex with the specified ID is valid.
   */
  bool isVertex(uint32 vertexID) const;
  /*! @return @c true if the polygon with the specified ID is valid.
   */
  bool isPolygon(uint32 polygonID) const;
  /*! Retrieves the base layer data of the specified vertex.
   *  @param vertexID The ID of the desired vertex.
   *  @param vertex The base layer data of the desired vertex.
   *  @return @c true if the vertex was valid, otherwise @c false.
   */
  bool getBaseVertex(uint32 vertexID, BaseVertex& vertex) const;
  /*! Retrieves the base layer data of the specified polygon.
   *  @param polygonID The ID of the desired polygon.
   *  @param polygon The base layer data of the desired vertex.
   *  @return @c true if the polygon was valid, otherwise @c false.
   */
  bool getBasePolygon(uint32 polygonID, BasePolygon& polygon) const;
  /*! Sets the base layer data for the specified vertex.
   *  @param vertexID The ID of the vertex to create or change.
   *  @param vertex The data for the base vertex layer.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setBaseVertex(uint32 vertexID, const BaseVertex& vertex);
  /*! Sets the base layer data for the specified polygon.
   *  @param polygonID The ID of the polygon to create or change.
   *  @param polygon The data for the base polygon layer.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setBasePolygon(uint32 polygonID, const BasePolygon& polygon);
  /*! Deletes the vertex with the specified ID.
   *  @param vertexID The ID of the vertex to delete.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void deleteVertex(uint32 vertexID);
  /*! Deletes the polygon with the specified ID.
   *  @param polygonID The ID of the polygon to delete.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void deletePolygon(uint32 polygonID);
  /*! @return The size, in bytes, of all the geometry layer slots that make up a single vertex.
   */
  size_t getVertexSize(void) const;
  /*! @return The size, in bytes, of all the geometry layer slots that make up a single polygon.
   */
  size_t getPolygonSize(void) const;
  /*! @return The geometry layer containing vertex crease data, or
   *  @c NULL if no such layer exists.
   */
  GeometryLayer* getVertexCreaseLayer(void);
  /*! @return The name of the geometry layer containing vertex crease
   *  data, or the empty string if no name is set.
   */
  const std::string& getVertexCreaseLayerName(void) const;
  /*! @return The geometry layer containing edge crease data, or
   *  @c NULL if no such layer exists.
   */
  GeometryLayer* getEdgeCreaseLayer(void);
  /*! @return The name of the geometry layer containing edge crease
   *  data, or the empty string if no name is set.
   */
  const std::string& getEdgeCreaseLayerName(void) const;
  /*! @return The highest currently valid vertex ID.
   */
  uint32 getHighestVertexID(void) const;
  /*! @return The lowest available vertex ID.
   *  @remarks This isn't updated by local vertex creation requests.
   */
  uint32 getFirstFreeVertexID(void) const;
  /*! @return The highest currently valid polygon ID.
   */
  uint32 getHighestPolygonID(void) const;
  /*! @return The lowest available polygon ID.
   *  @remarks This isn't updated by local polygon creation requests.
   */
  uint32 getFirstFreePolygonID(void) const;
  /*! @return The number of valid vertices.
   */
  uint32 getVertexCount(void) const;
  /*! @return The number of valid polygons.
   */
  uint32 getPolygonCount(void) const;
private:
  GeometryNode(VNodeID ID, VNodeOwner owner, Session& session);
  ~GeometryNode(void);
  static void initialize(void);
  static void receiveGeometryLayerCreate(void* data, VNodeID ID, VLayerID layerID, const char* name, VNGLayerType type, uint32 defaultInt, real64 defaultReal);
  static void receiveGeometryLayerDestroy(void* data, VNodeID ID, VLayerID layerID);
  static void receiveCreaseSetVertex(void* user, VNodeID nodeID, const char *layer, uint32 def_crease);
  static void receiveCreaseSetEdge(void* user, VNodeID nodeID, const char *layer, uint32 def_crease);
  static void receiveBoneCreate(void* user, VNodeID nodeID, uint16 bone_id, const char *weight, const char *reference, uint32 parent, real64 pos_x, real64 pos_y, real64 pos_z, real64 rot_x, real64 rot_y, real64 rot_z, real64 rot_w);
  static void receiveBoneDestroy(void* user, VNodeID nodeID, uint16 bone_id);
  static void receiveVertexDeleteReal32(void* user, VNodeID nodeID, uint32 vertexID);
  static void receiveVertexDeleteReal64(void* user, VNodeID nodeID, uint32 vertexID);
  typedef std::vector<bool> ValidityMap;
  typedef std::vector<GeometryLayer*> LayerList;
  typedef std::map<uint32,uint32> VertexIndexMap;
  LayerList mLayers;
  GeometryLayer* mBaseVertexLayer;
  GeometryLayer* mBasePolygonLayer;
  ValidityMap mValidVertices;
  ValidityMap mValidPolygons;
  std::string mVertexCreases;
  uint32 mVertexDefaultCrease;
  std::string mEdgeCreases;
  uint32 mEdgeDefaultCrease;
  uint32 mHighestVertexID;
  uint32 mHighestPolygonID;
  uint32 mFirstFreeVertexID;
  uint32 mFirstFreePolygonID;
  uint32 mVertexCount;
  uint32 mPolygonCount;
};

//---------------------------------------------------------------------

/*! Observer interface for geometry nodes.
 */
class GeometryNodeObserver : public NodeObserver
{
public:
  /*! Called after a new vertex is created in an observed geometry node.
   *  @param node The geometry node in which the vertex was created.
   *  @param vertexID The ID of the newly created vertex.
   *  @param vertex The base layer data for the newly created vertex.
   */
  virtual void onCreateVertex(GeometryNode& node, uint32 vertexID, const BaseVertex& vertex);
  /*! Called before a change is made to the base layer of a vertex in an observed geometry node.
   *  @param node The node containing the vertex to be changed.
   *  @param vertexID The ID of the vertex to be changed.
   *  @param vertex The new base layer data for the vertex.
   */
  virtual void onChangeBaseVertex(GeometryNode& node, uint32 vertexID, const BaseVertex& vertex);
  /*! Called before a vertex is deleted in an observed geometry node.
   *  @param node The geometry node containing the vertex to be deleted.
   *  @param vertexID The ID of the vertex to be deleted.
   */
  virtual void onDeleteVertex(GeometryNode& node, uint32 vertexID);
  /*! Called after a new polygon is created in an observed geometry node.
   *  @param node The geometry node in which the polygon was created.
   *  @param polygonID The ID of the newly created polygon.
   *  @param polygon The base layer data for the newly created polygon.
   */
  virtual void onCreatePolygon(GeometryNode& node, uint32 polygonID, const BasePolygon& polygon);
  /*! Called before a change is made to the base layer of a polygon in an observed geometry node.
   *  @param node The node containing the polygon to be changed.
   *  @param polygonID The ID of the polygon to be changed.
   *  @param polygon The new base layer data for the polygon.
   */
  virtual void onChangeBasePolygon(GeometryNode& node, uint32 polygonID, const BasePolygon& polygon);
  /*! Called before a polygon is deleted in an observed geometry node.
   *  @param node The geometry node containing the polygon to be deleted.
   *  @param polygonID The ID of the polygon to be deleted.
   */
  virtual void onDeletePolygon(GeometryNode& node, uint32 polygonID);
  /*! Called after a new geometry layer is created in an observed node.
   *  @param node The node in which the geometry layer was created.
   *  @param layer The newly created geometry buffer.
   */
  virtual void onCreateLayer(GeometryNode& node, GeometryLayer& layer);
  /*! Called before a geometry layer is destroyed in an observed node.
   *  @param node The node containing the geometry layer to be destroyed.
   *  @param layer The geometry layer to be destroyed.
   */
  virtual void onDestroyLayer(GeometryNode& node, GeometryLayer& layer);
};

//---------------------------------------------------------------------

struct MethodParam
{
  MethodParam(const std::string& name, VNOParamType type);
  size_t getSize(void) const;
  std::string mName;
  VNOParamType mType;
};

//---------------------------------------------------------------------

typedef std::vector<MethodParam> MethodParamList;
                                                         
//---------------------------------------------------------------------

typedef std::vector<VNOParam> MethodArgumentList;

//---------------------------------------------------------------------

/*! Object method. Represents a single method in an object node.
 */
class Method : public Versioned, public Observable<MethodObserver>
{
  friend class MethodGroup;
public:
  /*! Destroys this method.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void destroy(void);
  /*! Issues a call to this method with the specified arguments.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  bool call(const MethodArgumentList& arguments, VNodeID senderID = (VNodeID) ~0);
  /*! @return The ID of this object method.
   */
  uint16 getID(void) const;
  /*! @return The name of this object method.
   */
  const std::string& getName(void) const;
  void setName(const std::string& name);
  /*! @return The number of parameters for this method.
   */
  uint8 getParamCount(void) const;
  /*! @param index The index of the desired parameter.
   *  @return The parameter with the specified index.
   */
  const MethodParam& getParam(uint8 index);
  /*! @return The method group containing this method.
   */
  MethodGroup& getGroup(void) const;
private:
  Method(uint16 ID, const std::string& name, MethodGroup& group);
  static void initialize(void);
  static void receiveMethodCall(void* user, VNodeID nodeID, uint16 groupID, uint16 methodID, VNodeID senderID, const VNOPackedParams* arguments);
  typedef std::vector<VNOParamType> ParamTypeList;
  MethodParamList mParams;
  ParamTypeList mTypes;
  uint16 mID;
  std::string mName;
  MethodGroup& mGroup;
};

//---------------------------------------------------------------------

/*! Observer interface for object node methods.
 */
class MethodObserver : public Observer<MethodObserver>
{
public:
  /*! Called when a call has been issued to an observed object method.
   *  @param method The method which has been called.
   *  @param arguments The arguments passed to the called method.
   */
  virtual void onCall(Method& method, const MethodArgumentList& arguments);  
  virtual void onSetName(Method& method, const std::string& name);
  /*! Called before an observed method is destroyed.
   *  @param method The method to be destroyed.
   */
  virtual void onDestroy(Method& method);
};

//---------------------------------------------------------------------

/*! Method group class. Represents a single method group in an object node.
 */
class MethodGroup : public Versioned, public Observable<MethodGroupObserver>
{
  friend class ObjectNode;
public:
  /*! Destroys this method group.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void destroy(void);
  /*! Creates a new method in this method group.
   *  @param name The desired name of the method.
   *  @param parameters The parameters accepted by the method.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void createMethod(const std::string& name, const MethodParamList& parameters);
  /*! @param methodID The ID of the desired method.
   *  @return The method with the specified ID, or @c NULL if no such method exists.
   */
  Method* getMethodByID(uint16 methodID);
  /*! @param methodID The ID of the desired method.
   *  @return The method with the specified ID, or @c NULL if no such method exists.
   */
  const Method* getMethodByID(uint16 methodID) const;
  /*! @param index The index of the desired method.
   *  @return The method at the specified index, or @c NULL if no such method exists.
   */
  Method* getMethodByIndex(unsigned int index);
  /*! @param index The index of the desired method.
   *  @return The method at the specified index, or @c NULL if no such method exists.
   */
  const Method* getMethodByIndex(unsigned int index) const;
  /*! @param name The name of the desired method.
   *  @return The method with the specified name, or @c NULL if no such method exists.
   */
  Method* getMethodByName(const std::string& name);
  /*! @param name The name of the desired method.
   *  @return The method with the specified name, or @c NULL if no such method exists.
   */
  const Method* getMethodByName(const std::string& name) const;
  /*! @return The number of methods in this node.
   */
  unsigned int getMethodCount(void) const;
  /*! @return The ID of this method group.
   */
  const uint16 getID(void) const;
  /*! @return The name of this method group.
   */
  const std::string& getName(void) const;
  /*! @return The node containing this method group.
   */
  ObjectNode& getNode(void) const;
private:
  MethodGroup(uint16 ID, const std::string& name, ObjectNode& node);
  ~MethodGroup(void);
  static void initialize(void);
  static void receiveMethodCreate(void* user, VNodeID nodeID, uint16 groupID, uint16 methodID, const char* name, uint8 paramCount, const VNOParamType* paramTypes, const char** paramNames);
  static void receiveMethodDestroy(void* user, VNodeID nodeID, uint16 groupID, uint16 methodID);
  typedef std::vector<Method*> MethodList;
  MethodList mMethods;
  uint16 mID;
  std::string mName;
  ObjectNode& mNode;
};

//---------------------------------------------------------------------

/*! Observer interface for object node methods.
 */
class MethodGroupObserver : public Observer<MethodGroupObserver>
{
public:
  /*! Called after a new method is created in an observed method group.
   *  @param group The observed method group.
   *  @param method The newly created object method.
   */
  virtual void onCreateMethod(MethodGroup& group, Method& method);
  /*! Called beforfe a method is created in an observed method group.
   *  @param group The observed method group.
   *  @param method The object method to be destroyed.
   */
  virtual void onDestroyMethod(MethodGroup& group, Method& method);
  /*! Called before an observed method group has its name changed.
   *  @param group The observed method group.
   *  @param name The new name of the method group.
   */
  virtual void onSetName(MethodGroup& group, const std::string& name);
  /*! Called before an observed method group is destroyed.
   *  @param group The method group to be destroyed.
   */
  virtual void onDestroy(MethodGroup& group);
};

//---------------------------------------------------------------------

/* Node link class. Represents a single link between an object node and another node.
 */
class Link : public Versioned, public Observable<LinkObserver>
{
  friend class ObjectNode;
public:
  /*! Destroys this node link.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void destroy(void);
  /*! @return The ID of this node link.
   */
  uint16 getID(void) const;
  /*! @return The ID of the node that this node link points to.
   */
  VNodeID getLinkedNodeID(void) const;
  /*! @return The node that this link points to, or @c NULL if the node
   *  doesn't exist.
   */
  Node* getLinkedNode(void) const;
  void setLinkedNode(VNodeID nodeID);
  /*! @return The ID of "who knows".
   */
  VNodeID getTargetNodeID(void) const;
  /*! @return The "who knows" that this link points to, or @c NULL if
   *  the node doesn't exist.
   */
  Node* getTargetNode(void) const;
  void setTargetNode(VNodeID nodeID);
  /*! @return The name of this node.
   */
  const std::string& getName(void) const;
  void setName(const std::string& name);
  /*! @return The node containing this node link.
   */
  ObjectNode& getNode(void) const;
private:
  Link(uint16 ID, const std::string& name, VNodeID nodeID, VNodeID targetID, ObjectNode& node);
  void sendData(void);
  void receiveLinkSet(void* user, VNodeID nodeID, uint16 linkID, VNodeID linkedNodeID, const char* name, uint32 targetNodeID);
  class Data
  {
  public:
    VNodeID mNodeID;
    VNodeID mTargetID;
    std::string mName;
  };
  Data mState;
  Data mCache;
  uint16 mID;
  ObjectNode& mNode;
};

//---------------------------------------------------------------------

class LinkObserver : public Observer<LinkObserver>
{
public:
  virtual void onSetLinkedNode(Link& link, VNodeID nodeID);
  virtual void onSetTargetNode(Link& link, VNodeID targetID);
  virtual void onSetName(Link& link, const std::string name);
  virtual void onDestroy(Link& link);
};

//---------------------------------------------------------------------

/*! Object node class. Represents a single object node.
 */
class ObjectNode : public Node
{
  friend class Session;
public:
  /*! Creates a new method group in this node.
   *  @param name The desired name of the method group.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void createMethodGroup(const std::string& name);
  /*! Creates a new link to the specified node.
   *  @param name The desired name of the link.
   *  @param nodeID The ID of the node to link to.
   *  @param targetID Who knows.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void createLink(const std::string& name, VNodeID nodeID, VNodeID targetID = (VNodeID) ~0);
  /*! @return @c true if this object node emits light, otherwise @c false.
   *  @remarks This is a helper method and not strictly neccessary.
   */
  bool isLight(void) const;
  /*! Sets the light intensity of this object node. If set to non-zero,
   * this object node becomes a light source.
   */
  void setLightIntensity(const ColorRGB& intensity);
  /*! @return The light intensity of this object node.
   */
  const ColorRGB& getLightIntensity(void) const;
  /*! @param groupID The ID of the desired method group.
   *  @return The method group with the specified ID, or @c NULL if no such method group exists.
   */
  MethodGroup* getMethodGroupByID(uint16 groupID);
  /*! @param groupID The ID of the desired method group.
   *  @return The method group with the specified ID, or @c NULL if no such method group exists.
   */
  const MethodGroup* getMethodGroupByID(uint16 groupID) const;
  /*! @param index The index of the desired method group.
   *  @return The method group at the specified index, or @c NULL if no such method group exists.
   */
  MethodGroup* getMethodGroupByIndex(unsigned int index);
  /*! @param index The index of the desired method group.
   *  @return The method group at the specified index, or @c NULL if no such method group exists.
   */
  const MethodGroup* getMethodGroupByIndex(unsigned int index) const;
  /*! @param name The name of the desired method group
   *  @return The method group with the specified name, or @c NULL if no such method group exists.
   */
  MethodGroup* getMethodGroupByName(const std::string& name);
  /*! @param name The name of the desired method group
   *  @return The method group with the specified name, or @c NULL if no such method group exists.
   */
  const MethodGroup* getMethodGroupByName(const std::string& name) const;
  /*! @return The number of method groups in this node.
   */
  unsigned int getMethodGroupCount(void) const;
  /*! @param groupID The ID of the desired link.
   *  @return The link with the specified ID, or @c NULL if no such link exists.
   */
  Link* getLinkByID(uint16 groupID);
  /*! @param groupID The ID of the desired link.
   *  @return The link with the specified ID, or @c NULL if no such link exists.
   */
  const Link* getLinkByID(uint16 groupID) const;
  /*! @param index The index of the desired link.
   *  @return The link at the specified index, or @c NULL if no such link exists.
   */
  Link* getLinkByIndex(unsigned int index);
  /*! @param index The index of the desired link.
   *  @return The link at the specified index, or @c NULL if no such link exists.
   */
  const Link* getLinkByIndex(unsigned int index) const;
  /*! @param name The name of the desired link
   *  @return The link with the specified name, or @c NULL if no such link exists.
   */
  Link* getLinkByName(const std::string& name);
  /*! @param name The name of the desired link
   *  @return The link with the specified name, or @c NULL if no such link exists.
   */
  const Link* getLinkByName(const std::string& name) const;
  /*! @return The number of links in this node.
   */
  unsigned int getLinkCount(void) const;
  /*! @param name The name of the link to return the node of.
   *  @return The node pointed to by the link with the specified name.
   */
  Node* getNodeByLinkName(const std::string& name) const;
  void setTranslation(const Translation& translation);
  void setRotation(const Rotation& rotation);
  /*! @return The current position of this object node.
   */
  const Vector3d& getPosition(void) const;
  /*! Sets the position of this object node.
   *  @param position The desired new position.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setPosition(const Vector3d& position);
  /*! @return The current movement speed of this object node.
   */
  const Vector3d& getSpeed(void) const;
  /*! Sets the movement speed of this object node.
   *  @param speed The desired new movement speed.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setSpeed(const Vector3d& speed);
  /*! @return The current movement acceleration of this object node.
   */
  const Vector3d& getAccel(void) const;
  /*! Sets the movement acceleration of this object node.
   *  @param accel The desired new movement acceleration.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setAccel(const Vector3d& accel);
  /*! @return The current rotation of this object node.
   */
  const Quaternion64& getRotation(void) const;
  /*! Sets the rotation of this object node.
   *  @param rotation The desired new rotation.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setRotation(const Quaternion64& rotation);
  const Quaternion64& getRotationSpeed(void) const;
  void setRotationSpeed(const Quaternion64& speed);
  const Quaternion64& getRotationAccel(void) const;
  void setRotationAccel(const Quaternion64& accel);
  /*! @return The scaling of this object node.
   */
  const Vector3d& getScale(void) const;
  /*! Changes the scaling of this object node.
   *  @param scale The desired scaling.
   */
  void setScale(const Vector3d& scale);
private:
  ObjectNode(VNodeID ID, VNodeOwner owner, Session& session); 
  ~ObjectNode(void);
  void sendTranslation(void);
  void sendRotation(void);
  static void initialize(void);
  static void receiveTransformPosReal32(void* user, VNodeID nodeID, uint32 seconds, uint32 fraction, const real32* pos, const real32* speed, const real32* accelerate, const real32* dragNormal, real32 drag);
  static void receiveTransformRotReal32(void* user, VNodeID nodeID, uint32 seconds, uint32 fraction, const VNQuat32* rot, const VNQuat32* speed, const VNQuat32* accelerate, const VNQuat32* dragNormal, real32 drag);
  static void receiveTransformScaleReal32(void* user, VNodeID nodeID, real32 scaleX, real32 scaleY, real32 scaleZ);
  static void receiveTransformPosReal64(void* user, VNodeID nodeID, uint32 seconds, uint32 fraction, const real64* pos, const real64* speed, const real64* accelerate, const real64* dragNormal, real64 drag);
  static void receiveTransformRotReal64(void* user, VNodeID nodeID, uint32 seconds, uint32 fraction, const VNQuat64* rot, const VNQuat64* speed, const VNQuat64* accelerate, const VNQuat64* dragNormal, real64 drag);
  static void receiveTransformScaleReal64(void* user, VNodeID nodeID, real64 scaleX, real64 scaleY, real64 scaleZ);
  static void receiveLightSet(void* user, VNodeID nodeID, real64 lightR, real64 lightG, real64 lightB);
  static void receiveLinkSet(void* user, VNodeID nodeID, uint16 linkID, VNodeID linkedNodeID, const char* name, uint32 targetNodeID);
  static void receiveLinkDestroy(void* user, VNodeID nodeID, uint16 linkID);
  static void receiveMethodGroupCreate(void* user, VNodeID nodeID, uint16 groupID, const char* name);
  static void receiveMethodGroupDestroy(void* user, VNodeID nodeID, uint16 groupID);
  static void receiveAnimRun(void* user, VNodeID nodeID, uint16 linkID, uint32 seconds, uint32 fraction, real64 pos, real64 speed, real64 accel, real64 scale, real64 scaleSpeed);
  typedef std::vector<MethodGroup*> MethodGroupList;
  typedef std::vector<Link*> LinkList;
  MethodGroupList mGroups;
  LinkList mLinks;
  Translation mTranslation;
  Translation mTranslationCache;
  Rotation mRotation;
  Rotation mRotationCache;
  Vector3d mScale;
  ColorRGB mIntensity;
};

//---------------------------------------------------------------------

/*! Observer interface for object nodes.
 */
class ObjectNodeObserver : public NodeObserver
{
public:
  /*! Called after a new method group is created in an observed object node.
   *  @param node The observed object node.
   *  @param group The newly created method group.
   */
  virtual void onCreateMethodGroup(ObjectNode& node, MethodGroup& group);
  /*! Called before a method group is destroyed in an observed object node.
   *  @param node The observed object node.
   *  @param group The method group to be destroyed.
   */
  virtual void onDestroyMethodGroup(ObjectNode& node, MethodGroup& group);
  /*! Called after a new link is created in an observed object node.
   *  @param node The observed object node.
   *  @param link The newly created link.
   */
  virtual void onCreateLink(ObjectNode& node, Link& link);
  /*! Called before a link is destroyed in an observed object node.
   *  @param node The observed object node.
   *  @param link The link to be destroyed.
   */
  virtual void onDestroyLink(ObjectNode& node, Link& link);
  virtual void onSetScale(ObjectNode& node, Vector3d& scale);
  virtual void onSetLightIntensity(ObjectNode& node, const ColorRGB& color);
};

//---------------------------------------------------------------------

/*! @remarks Not finished.
 */
class BitmapLayer : public Versioned, public Observable<BitmapLayerObserver>
{
  friend class BitmapNode;
public:
  void destroy(void);
  void getTile(uint16 tileX, uint16 tileY, uint16 z, VNBTile& tile);
  void setTile(uint16 tileX, uint16 tileY, uint16 z, const VNBTile& tile);
  VLayerID getID(void) const;
  const std::string& getName(void) const;
  VNBLayerType getType(void) const;
  BitmapNode& getNode(void) const;
private:
  BitmapLayer(VLayerID ID, const std::string& name, BitmapNode& node, VNBLayerType type);
  static void initialize(void);
  static void receiveTileSet(VNodeID nodeID, VLayerID layerID, uint16 tileX, uint16 tileY, uint16 z, VNBLayerType type, const VNBTile* data);
  VLayerID mID;
  std::string mName;
  BitmapNode& mNode;
  VNBLayerType mType;
};

//---------------------------------------------------------------------

/*! @remarks Not finished.
 */
class BitmapLayerObserver : public Observer<BitmapLayerObserver>
{
public:
  virtual void setTile(BitmapLayer& layer, uint16 tileX, uint16 tileY, uint16 z, const VNBTile& tile);
  virtual void onSetType(BitmapLayer& layer, VNBLayerType type);
  virtual void onSetName(BitmapLayer& layer, const std::string name);
  virtual void onDestroy(BitmapLayer& layer);
};

//---------------------------------------------------------------------

/*! @remarks Not finished.
 */
class BitmapNode : public Node
{
  friend class Session;
public:
  void createLayer(const std::string& name, VNBLayerType type);
  /*! @param ID The ID of the desired bitmap layer.
   *  @return The bitmap layer with the specified ID, or @c NULL if no such bitmap layer exists.
   */
  BitmapLayer* getLayerByID(VLayerID ID);
  /*! @param ID The ID of the desired bitmap layer.
   *  @return The bitmap layer with the specified ID, or @c NULL if no such bitmap layer exists.
   */
  const BitmapLayer* getLayerByID(VLayerID ID) const;
  /*! @param index The index of the desired bitmap layer.
   *  @return The bitmap layer at the specified index, or @c NULL if no such bitmap layer exists.
   */
  BitmapLayer* getLayerByIndex(unsigned int index);
  /*! @param index The index of the desired bitmap layer.
   *  @return The bitmap layer at the specified index, or @c NULL if no such bitmap layer exists.
   */
  const BitmapLayer* getLayerByIndex(unsigned int index) const;
  /*! @param name The name of the desired bitmap layer
   *  @return The bitmap layer with the specified name, or @c NULL if no such bitmap layer exists.
   */
  BitmapLayer* getLayerByName(const std::string& name);
  /*! @param name The name of the desired bitmap layer
   *  @return The bitmap layer with the specified name, or @c NULL if no such bitmap layer exists.
   */
  const BitmapLayer* getLayerByName(const std::string& name) const;
  uint16 getLayerCount(void) const;
  void setDimensions(uint16 width, uint16 height, uint16 depth = 1);
  uint16 getWidth(void) const;
  uint16 getHeight(void) const;
  uint16 getDepth(void) const;
  unsigned int getDimensionCount(void) const;
private:
  BitmapNode(VNodeID ID, VNodeOwner owner, Session& session); 
  ~BitmapNode(void);
  static void initialize(void);
  static void receiveDimensionsSet(void* user, VNodeID nodeID, uint16 width, uint16 height, uint16 depth);
  static void receiveLayerCreate(void* user, VNodeID nodeID, VLayerID layerID, const char* name, VNBLayerType type);
  static void receiveLayerDestroy(void* user, VNodeID nodeID, VLayerID layerID);
  typedef std::vector<BitmapLayer*> LayerList;
  LayerList mLayers;
  uint16 mWidth;
  uint16 mHeight;
  uint16 mDepth;
};

//---------------------------------------------------------------------

/*! @remarks Not finished.
 */
class BitmapNodeObserver : public NodeObserver
{
public:
  virtual void onSetDimensions(BitmapNode& node, uint16 width, uint16 height, uint16 depth);
  virtual void onCreateLayer(BitmapNode& node, BitmapLayer& layer);
  virtual void onDestroyLayer(BitmapNode& node, BitmapLayer& layer);
};

//---------------------------------------------------------------------

/*! @remarks Not finished.
 */
class Fragment : public Versioned, public Observable<FragmentObserver>
{
  friend class MaterialNode;
public:
  void destroy(void);
  VNMFragmentID getID(void) const;
private:
  Fragment(VNMFragmentID ID);
};

//---------------------------------------------------------------------

/*! @remarks Not finished.
 */
class FragmentObserver : public Observer<FragmentObserver>
{
public:
  virtual void onDestroy(Fragment& fragment);
};

//---------------------------------------------------------------------

/*! @remarks Not finished.
 */
class MaterialNode : public Node
{
  friend class Session;
public:
  /*! @param ID The ID of the desired material fragment.
   *  @return The material fragment with the specified ID, or @c NULL
   *  if no such material fragment exists.
   */
  Fragment* getFragmentByID(VNMFragmentID ID);
  /*! @param ID The ID of the desired material fragment.
   *  @return The material fragment with the specified ID, or @c NULL
   *  if no such material fragment exists.
   */
  const Fragment* getFragmentByID(VNMFragmentID ID) const;
  /*! @param index The index of the desired material fragment.
   *  @return The material fragment at the specified index, or @c NULL
   *  if no such material fragment exists.
   */
  Fragment* getFragmentByIndex(unsigned int index);
  /*! @param index The index of the desired material fragment.
   *  @return The material fragment at the specified index, or @c NULL
   *  if no such material fragment exists.
   */
  const Fragment* getFragmentByIndex(unsigned int index) const;
  /*! @param name The name of the desired material fragment
   *  @return The material fragment with the specified name, or @c NULL
   *  if no such material fragment exists.
   */
  Fragment* getFragmentByName(const std::string& name);
  /*! @param name The name of the desired material fragment
   *  @return The material fragment with the specified name, or @c NULL
   *  if no such material fragment exists.
   */
  const Fragment* getFragmentByName(const std::string& name) const;
  /*! @return The number of fragments in this material node.
   */
  uint16 getFragmentCount(void) const;
private:
  MaterialNode(VNodeID ID, VNodeOwner owner, Session& session); 
  ~MaterialNode(void);
  static void initialize(void);
  typedef std::vector<Fragment*> FragmentList;
  FragmentList mFragments;
};

//---------------------------------------------------------------------

/*! @remarks Not finished.
 */
class MaterialNodeObserver : public NodeObserver
{
public:
  virtual void onCreateFragment(MaterialNode& node, Fragment& fragment);
  virtual void onDestroyFragment(MaterialNode& node, Fragment& fragment);
};

//---------------------------------------------------------------------

/*! Session class. Represents a single session with a verse server.
 */
class Session : public Versioned, public Observable<SessionObserver>
{
public:
  /*! Session state enumeration.
   */
  enum State
  {
    /*! The session is still being created.
     */
    CONNECTING,
    /*! The session is established and exchanging data.
     */
    CONNECTED,
    /*! The session is terminated. The object will remain for inspection
     *  until a new session is created with the same server.
     */
    TERMINATED,
    /*! The session is released, i.e. flagged for deletion. The object will
     *  remain until the first subsequent call to Session::update.
     */
    RELEASED,
  };
  /*! Pushes this session onto the internal stack and makes it the active session.
   */
  void push(void);
  /*! Pops this session from the internal stack and activates the previous one.
   */
  void pop(void);
  /*! Terminates this session with the specified message.
   *  @param byebye The desired termination message.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void terminate(const std::string& byebye);
  /*! Schedules a terminated session for destruction. Actual destruction will take place at
   *  the first subsequent call to Session::update.
   *  Note that this call has no effect unless the session is in the terminated state.
   */
  void release(void);
  /*! Creates a node with the specified name and of the specified type.
   *  @param name The desired name of the node.
   *  @param type The desired type of the node.
   *  @remarks Due to the structure of the verse protocol, the created
   *  node will not immediately receive the specified name.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void createNode(const std::string& name, VNodeType type);
  /*! @return The state of this session.
   */
  State getState(void) const;
  /*! @param ID The ID of the desired node.
   *  @return The node with the specified ID, or @c NULL if no such node exists.
   */
  Node* getNodeByID(VNodeID ID);
  /*! @param ID The ID of the desired node.
   *  @return The node with the specified ID, or @c NULL if no such node exists.
   */
  const Node* getNodeByID(VNodeID ID) const;
  /*! @param index The index of the desired node.
   *  @return The node at the specified index, or @c NULL if no such node exists.
   */
  Node* getNodeByIndex(unsigned int index);
  /*! @param index The index of the desired node.
   *  @return The node at the specified index, or @c NULL if no such node exists.
   */
  const Node* getNodeByIndex(unsigned int index) const;
  /*! @param name The name of the desired node
   *  @return The node with the specified name, or @c NULL if no such node exists.
   */
  Node* getNodeByName(const std::string& name);
  /*! @param name The name of the desired node
   *  @return The node with the specified name, or @c NULL if no such node exists.
   */
  const Node* getNodeByName(const std::string& name) const;
  /*! @return The number of nodes visible in this session.
   */
  unsigned int getNodeCount(void) const;
  /*! @return The avatar node for this session.
   */
  Node* getAvatarNode(void);
  /*! @return The avatar node for this session.
   */
  const Node* getAvatarNode(void) const;
  /*! @return The address of the verse server for this session.
   */
  const std::string& getAddress(void) const;
  /*! Creates a session with the specified verse server.
   *  @param address The address of the desired verse server.
   *  @param username The desired user name for the session.
   *  @param password The password for the specified user.
   *  @param typeMask A bitmask for the desired node types.
   */
  static Session* create(const std::string& address,
			 const std::string& username,
			 const std::string& password,
			 unsigned int typeMask = 0);
  /*! Searches for a session with the specified server address.
   *  @param address The server address to search for.
   *  @return The session with the specified address, or @c NULL.
   */
  static Session* find(const std::string& address);
  /*! Updates all session and associated data.
   *  This call will cause all the commands issued and received since
   *  the last update to take effect, including triggering observers.
   *  @param microseconds The maximum number of microseconds to block,
   *  when waiting for new commands.
   */
  static void update(uint32 microseconds);
  /*! Terminates all connected sessions with the specified message.
   *  @param byebye The desired termination message.
   */
  static void terminateAll(const std::string& byebye);
  /*! @return The active session, or @c NULL if no session is active.
   */
  static Session* getCurrent(void);
  /*! @param index The index of the desired session.
   *  @return The session with the specified index, or @c NULL if no
   *  such session exists.
   */
  static Session* getByIndex(unsigned int index);
  /*! @return The number of sessions.
   */
  static unsigned int getCount(void);
private:
  Session(const std::string& address,
          const std::string& username,
	  VSession internal);
  ~Session(void);
  static void receiveAccept(void* user, VNodeID avatarID, const char* address, uint8* hostID);
  static void receiveTerminate(void* user, const char* address, const char* byebye);
  static void receiveNodeCreate(void* user, VNodeID ID, VNodeType type, VNodeOwner owner);
  static void receiveNodeDestroy(void* user, VNodeID ID);
  class PendingNode
  {
  public:
    PendingNode(const std::string& name, VNodeType type);
    std::string mName;
    VNodeType mType;
  };
  typedef std::list<Session*> SessionList;
  typedef std::stack<Session*> SessionStack;
  typedef std::list<PendingNode> PendingList;
  typedef std::vector<Node*> NodeList;
  NodeList mNodes;
  PendingList mPending;
  std::string mAddress;
  std::string mUserName;
  VSession mInternal;
  VNodeID mAvatarID;
  State mState;
  uint32 mTypeMask;
  static SessionList msSessions;
  static SessionStack msStack;
  static Session* msCurrent;
  static bool msInitialized;
};

//---------------------------------------------------------------------

class SessionObserver : public Observer<SessionObserver>
{
public:
  /*! Called after an observed session has been accepted by the server.
   *  @param session The newly accepted session.
   */
  virtual void onAccept(Session& session);
  /*! Called after an observed session has been terminated.
   *  @param session The newly terminated session.
   *  @param byebye The termination message from the server.
   */
  virtual void onTerminate(Session& session, const std::string& byebye);
  /*! Called befiore an observed session is destroyed.
   *  @param session The session to be destroyed.
   */
  virtual void onDestroy(Session& session);
  /*! Called after a node is created in an observed session.
   *  @param session The session in which the node was created.
   *  @param node The newly created node.
   */
  virtual void onCreateNode(Session& session, Node& node);
  /*! Called before a node is destroyed in an observed session.
   *  @param session The session containing the node to be destroyed.
   *  @param node The node to be destroyed.
   */
  virtual void onDestroyNode(Session& session, Node& node);
};

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/

#endif /* __AMPLE_H__ */

