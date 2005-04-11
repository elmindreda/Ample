
#include <string>
#include <vector>
#include <stack>
#include <list>

#include <AmpleUtil.h>

namespace verse
{
  namespace ample
  {

//---------------------------------------------------------------------

const uint32 INVALID_VERTEX_ID = ~0;
const uint32 INVALID_POLYGON_ID = ~0;

//---------------------------------------------------------------------

template <typename T, typename S>
class Observer;
template <typename T, typename S>
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
class GeometryNode;
class GeometryNodeObserver;
class ObjectMethod;
class ObjectMethodObserver;
class ObjectNode;
class ObjectNodeObserver;
class Session;
class SessionObserver;

//---------------------------------------------------------------------

class Polygon
{
public:
  uint32 mIndices[4];
};

//---------------------------------------------------------------------

class Mesh
{
public:
  typedef std::vector<Vector3> VertexList;
  typedef std::vector<Polygon> PolygonList;
  VertexList mVertices;
  PolygonList mPolygons;
};

//---------------------------------------------------------------------

/*! Base class for observer interfaces.
 */
template <typename T, typename S>
class Observer
{
public:
  typedef Observable<S,T> ObservableType;
  friend class ObservableType;
  /*! Destructor. Detaches the observer from all observed objects.
   */
  inline virtual ~Observer(void);
private:
  typedef std::vector<ObservableType*> ObservableList;
  ObservableList mObservables;
};

//---------------------------------------------------------------------

template <typename T, typename S>
inline Observer<T,S>::~Observer(void)
{
  while (mObservables.size())
    mObservables.front()->removeObserver(dynamic_cast<S&>(*this));
}

//---------------------------------------------------------------------

/*! Base class for all observable objects.
 */
template <typename T, typename S>
class Observable
{
  friend class Session;
public:
  typedef Observer<S,T> ObserverType;
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
private:
  typedef std::vector<T*> ObserverList;
  ObserverList mObservers;
};

//---------------------------------------------------------------------

template <typename T, typename S>
inline Observable<T,S>::~Observable(void)
{
  while (mObservers.size())
    removeObserver(*mObservers.front());
}

template <typename T, typename S>
inline void Observable<T,S>::addObserver(T& observer)
{
  if (std::find(mObservers.begin(), mObservers.end(), &observer) != mObservers.end())
    return;

  mObservers.push_back(&observer);
  static_cast<ObserverType&>(observer).mObservables.push_back(this);
}

template <typename T, typename S>
inline void Observable<T,S>::removeObserver(T& observer)
{
  typename ObserverList::iterator i = std::find(mObservers.begin(), mObservers.end(), &observer);
  if (i != mObservers.end())
  {
    typename ObserverType::ObservableList& observables = static_cast<ObserverType&>(observer).mObservables;
    observables.erase(std::find(observables.begin(), observables.end(), this));
    mObservers.erase(i);
  }
}

//---------------------------------------------------------------------

class Versioned
{
  friend class Session;
public:
  Versioned(void);
  unsigned int getDataVersion(void) const;
  unsigned int getStructureVersion(void) const;
private:
  void updateData(void);
  void updateStructure(void);
  unsigned int mDataVersion;
  unsigned int mStructVersion;
};

//---------------------------------------------------------------------

/*! Node tag class. Represents a single tag in a node's tag group.
 */
class Tag : public Versioned, public Observable<TagObserver, Tag>
{
  friend class Session;
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
class TagObserver : public Observer<Tag, TagObserver>
{
public:
  //! @todo Change.
  /*! Called when the observed tag changes.
   *  @param tag The tag that changed.
   */
  virtual void onChange(Tag& tag);
};

//---------------------------------------------------------------------

/*! Node tag group class. Represents a single tag group in a node.
 */
class TagGroup : public Versioned, public Observable<TagGroupObserver, TagGroup>
{
  friend class Session;
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
  typedef std::vector<Tag*> TagList;
  TagList mTags;
  std::string mName;
  unsigned short mID;
  Node& mNode;
};
  
//---------------------------------------------------------------------

/*! Observer interface for tag groups.
 */
class TagGroupObserver : public Observer<TagGroup, TagGroupObserver>
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
};

//---------------------------------------------------------------------

/*! Node base class. Represents a single node of unknown type.
 */
class Node : public Versioned, public Observable<NodeObserver, Node>
{
  friend class Session;
public:
  enum Type { OBJECT, GEOMETRY, MATERIAL, BITMAP, TEXT, CURVE, AUDIO };
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
class NodeObserver : public Observer<Node, NodeObserver>
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
};

//---------------------------------------------------------------------

/*! Text node buffer class. Represents a single buffer in a text node.
 */
class TextBuffer : public Versioned, public Observable<TextBufferObserver, TextBuffer>
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
  uint16 getID(void) const;
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
  TextBuffer(uint16 ID, const std::string& name, TextNode& node);
  uint16 mID;
  std::string mName;
  std::string mText;
  TextNode& mNode;
};

//---------------------------------------------------------------------

/*! Observer interface for text node buffers.
 */
class TextBufferObserver : public Observer<TextBuffer, TextBufferObserver>
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
  /*! @param ID The ID of the desired text buffer
   *  @return The text buffer with the specified ID, or @c NULL if no such text buffer exists.
   */
  TextBuffer* getBufferByID(uint16 ID);
  /*! @param ID The ID of the desired text buffer
   *  @return The text buffer with the specified ID, or @c NULL if no such text buffer exists.
   */
  const TextBuffer* getBufferByID(uint16 ID) const;
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
class GeometryLayer : public Versioned, public Observable<GeometryLayerObserver, GeometryLayer>
{
  friend class Session;
  friend class GeometryNode;
public:
  /*! Destroys this geometry layer.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void destroy(void);
  /*! Deletes the specified slot.
   *  @param slotID The ID of the slot to delete.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void deleteSlot(uint32 slotID);
  /*! @return @c true if this layer contains vertex data, otherwise @c false.
   */
  bool isVertex(void) const;
  /*! @return @c true if this layer contains polygon data, otherwise @c false.
   */
  bool isPolygon(void) const;
  /*! @return The ID of this geometry layer.
   */
  VLayerID getID(void) const;
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
   *  @param index The index of the desired slot.
   *  @param data The location to store the slot data.
   *  @return @c true if the slot exists, otherwise @c false.
   */
  bool getSlot(uint32 index, void* data) const;
  /*! Changes the specified slot in this geometry layer.
   *  @param index The index of the desired slot.
   *  @param data The location to retrieve the slot data from.
   *  @remarks This call is asynchronous. It will not take effect
   *  until, at the earliest, after the first subsequent call to
   *  Session::update.
   */
  void setSlot(uint32 index, const void* data);
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
  void reserve(unsigned int count);
  static unsigned int getTypeSize(VNGLayerType type);
  static unsigned int getTypeElementCount(VNGLayerType type);
  ByteBlock mData;
  VLayerID mID;
  std::string mName;
  VNGLayerType mType;
  VNRealFormat mFormat;
  GeometryNode& mNode;
  uint32 mDefaultInt;
  real64 mDefaultReal;
};

//---------------------------------------------------------------------

/*! Observer interface for geometry node layers.
 */
class GeometryLayerObserver : public Observer<GeometryLayer, GeometryLayerObserver>
{
public:
  /*! Called before a slot is changed in an observed geometry layer.
   *  @param layer The observed geometry layer.
   *  @param slotID The ID of the slot to be changed.
   *  @param data The data to be written to the slot.
   */
  virtual void onSetSlot(GeometryLayer& layer, uint16 slotID, const void* data);
  /*! Called before an observed geometry layer has its name changed.
   *  @param layer The observed geometry layer.
   *  @param name The new name of the geometry layer.
   */
  virtual void onSetName(GeometryLayer& layer, const std::string& name);
};

//---------------------------------------------------------------------

/*! Geometry node class. Represents a single geometry node.
 */
class GeometryNode : public Node
{
  friend class Session;
public:
  void createLayer(const std::string& name,
                   VNGLayerType type,
		   uint32 defaultInt = 0,
		   real64 defaultReal = 0.0);
  GeometryLayer* getLayerByID(VLayerID ID);
  const GeometryLayer* getLayerByID(VLayerID ID) const;
  GeometryLayer* getLayerByIndex(unsigned int index);
  const GeometryLayer* getLayerByIndex(unsigned int index) const;
  GeometryLayer* getLayerByName(const std::string& name);
  const GeometryLayer* getLayerByName(const std::string& name) const;
  bool isVertex(uint32 index) const;
  bool isPolygon(uint32 index) const;
  bool getVertex(uint32 index, void* data) const;
  bool getPolygon(uint32 index, void* data) const;
  unsigned int getVertexSize(void) const;
  unsigned int getPolygonSize(void) const;
private:
  GeometryNode(VNodeID ID, VNodeOwner owner, Session& session);
  ~GeometryNode(void);
  typedef std::vector<GeometryLayer*> LayerList;
  typedef std::vector<bool> ValidityMap;
  LayerList mLayers;
  ValidityMap mValidVertices;
  ValidityMap mValidPolygons;
};

//---------------------------------------------------------------------

/*! Observer interface for geometry nodes.
 */
class GeometryNodeObserver : public NodeObserver
{
public:
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

class ObjectMethod : public Observable<ObjectMethodObserver, ObjectMethod>
{
  friend class Session;
public:
  const std::string& getName(void) const;
private:
  std::string mName;
};

//---------------------------------------------------------------------

class ObjectMethodObserver : public Observer<ObjectMethodObserver, ObjectMethod>
{
  friend class Session;
public:
};

//---------------------------------------------------------------------

class ObjectNode : public Node, public Observable<ObjectNodeObserver, ObjectNode>
{
  friend class Session;
public:
private:
  ObjectNode(VNodeID ID, VNodeOwner owner, Session& session); 
};

//---------------------------------------------------------------------

class ObjectNodeObserver : public NodeObserver
{
  friend class Session;
public:
};

//---------------------------------------------------------------------

class Session : public Versioned, public Observable<SessionObserver, Session>
{
public:
  enum State { CONNECTING, CONNECTED, TERMINATED };
  void push(void);
  void pop(void);
  void terminate(const std::string& byebye);
  void createNode(const std::string& name, VNodeType type);
  State getState(void) const;
  Node* getNodeByID(VNodeID ID);
  const Node* getNodeByID(VNodeID ID) const;
  Node* getNodeByIndex(unsigned int index);
  const Node* getNodeByIndex(unsigned int index) const;
  Node* getNodeByName(const std::string& name);
  const Node* getNodeByName(const std::string& name) const;
  unsigned int getNodeCount(void) const;
  Node* getAvatarNode(void);
  const Node* getAvatarNode(void) const;
  const std::string& getAddress(void) const;
  static Session* create(const std::string& address,
			 const std::string& username,
			 const std::string& password,
			 unsigned int typeMask = 0);
  static Session* find(const std::string& address);
  static void update(uint32 microseconds);
  static void terminateAll(const std::string& byebye);
  static Session* getCurrent(void);
private:
  Session(const std::string& address,
          const std::string& username,
	  VSession internal);
  ~Session(void);
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
  static void receiveAccept(void* user, VNodeID avatarID, const char* address, uint8* hostID);
  static void receiveTerminate(void* user, const char* address, const char* byebye);
  static void receiveNodeCreate(void* user, VNodeID ID, VNodeType type, VNodeOwner owner);
  static void receiveNodeDestroy(void* user, VNodeID ID);
  static void receiveNodeNameSet(void* user, VNodeID ID, const char *name);
  static void receiveTagGroupCreate(void* user, VNodeID ID, uint16 groupID, const char* name);
  static void receiveTagGroupDestroy(void* user, VNodeID ID, uint16 groupID);
  static void receiveTagCreate(void* user, VNodeID ID, uint16 groupID, uint16 tagID, const char* name, VNTagType type, const VNTag* value);
  static void receiveTagDestroy(void* user, VNodeID ID, uint16 groupID, uint16 tagID);
  static void receiveNodeLanguageSet(void* user, VNodeID ID, const char* language);
  static void receiveTextBufferCreate(void* user, VNodeID ID, uint16 bufferID, const char* name);
  static void receiveTextBufferDestroy(void* user, VNodeID ID, VNMBufferID bufferID);
  static void receiveTextBufferSet(void* user, VNodeID ID, VNMBufferID bufferID, uint32 position, uint32 length, const char* text);
  static void receiveGeometryLayerCreate(void* data, VNodeID ID, VLayerID layerID, const char* name, VNGLayerType type, uint32 defaultInt, real64 defaultReal);
  static void receiveGeometryLayerDestroy(void* data, VNodeID ID, VLayerID layerID);
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
  static void receiveCreaseSetVertex(void* user, VNodeID nodeID, const char *layer, uint32 def_crease);
  static void receiveCreaseSetEdge(void* user, VNodeID nodeID, const char *layer, uint32 def_crease);
  static void receiveBoneCreate(void* user, VNodeID nodeID, uint16 bone_id, const char *weight, const char *reference, uint32 parent, real64 pos_x, real64 pos_y, real64 pos_z, real64 rot_x, real64 rot_y, real64 rot_z, real64 rot_w);
  static void receiveBoneDestroy(void* user, VNodeID nodeID, uint16 bone_id);
};

//---------------------------------------------------------------------

class SessionObserver : public Observer<Session, SessionObserver>
{
public:
  virtual void onAccept(Session& session);
  virtual void onTerminate(Session& session, const std::string& byebye);
  virtual void onCreateNode(Session& session, Node& node);
  virtual void onDestroyNode(Session& session, Node& node);
};

//---------------------------------------------------------------------

  } /*namespace ample*/
} /*namespace verse*/


