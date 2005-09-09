
#include <verse.h>

#include <Ample.h>

using namespace verse::ample;

class NodeListener : public TagObserver, public TagGroupObserver, public NodeObserver
{
  void onChange(Tag& tag)
  {
    printf("Changed something in tag %s(%u) in group %s(%u) of node %u (%s)\n",
           tag.getName().c_str(),
           tag.getID(),
	   tag.getGroup().getName().c_str(),
           tag.getGroup().getID(),
	   tag.getGroup().getNode().getID(),
	   tag.getGroup().getNode().getName().c_str());
  }

  void onSetName(Tag& tag, const std::string name)
  {
    printf("Changed name of tag %s(%u) of group %s(%u) node %u (%s) to %s\n",
           tag.getName().c_str(),
           tag.getID(),
	   tag.getGroup().getName().c_str(),
	   tag.getGroup().getID(),
	   tag.getGroup().getNode().getID(),
	   tag.getGroup().getNode().getName().c_str(),
	   name.c_str());
  }

  void onDestroy(Tag& tag)
  {
    printf("Destroyed tag %s(%u) of group %s(%u) node %u (%s)\n",
           tag.getName().c_str(),
           tag.getID(),
	   tag.getGroup().getName().c_str(),
	   tag.getGroup().getID(),
	   tag.getGroup().getNode().getID(),
	   tag.getGroup().getNode().getName().c_str());
  }

  void onCreateTag(TagGroup& group, Tag& tag)
  {
    printf("Created tag %s(%u) in group %s(%u) of node %u (%s)\n",
           tag.getName().c_str(),
           tag.getID(),
	   group.getName().c_str(),
	   group.getID(),
	   group.getNode().getID(),
	   group.getNode().getName().c_str());

    tag.addObserver(*this);
  }

  void onDestroyTag(TagGroup& group, Tag& tag)
  {
    printf("Destroyed tag %s(%u) in group %s(%u) of node %u (%s)\n",
           tag.getName().c_str(),
           tag.getID(),
	   group.getName().c_str(),
	   group.getID(),
	   group.getNode().getID(),
	   group.getNode().getName().c_str());
  }

  void onSetName(TagGroup& group, const std::string& name)
  {
    printf("Changed name of group %s(%u) of node %u (%s) to %s\n",
	   group.getName().c_str(),
	   group.getID(),
	   group.getNode().getID(),
	   group.getNode().getName().c_str(),
	   name.c_str());
  }

  void onDestroy(TagGroup& group)
  {
    printf("Destroyed tag group %s(%u) node %u (%s)\n",
	   group.getName().c_str(),
	   group.getID(),
	   group.getNode().getID(),
	   group.getNode().getName().c_str());
  }

  void onCreateTagGroup(Node& node, TagGroup& group)
  {
    printf("Created tag group %s(%u) in node %u (%s)\n",
           group.getName().c_str(),
	   group.getID(),
	   node.getID(),
	   node.getName().c_str());

    group.addObserver(*this);
  }

  void onDestroyTagGroup(Node& node, TagGroup& group)
  {
    printf("Destroyed tag group %s(%u) in node %u (%s)\n",
           group.getName().c_str(),
	   group.getID(),
	   node.getID(),
	   node.getName().c_str());
  }

  void onSetName(Node& node, const std::string& name)
  {
    printf("Set name %s for node %u (%s)\n",
           name.c_str(),
           node.getID(),
           node.getName().c_str());
  }

  void onDestroy(Node& node)
  {
    printf("Destroyed node %u (%s)\n",
	   node.getID(),
	   node.getName().c_str());
  }
};

NodeListener nodeListener;

class TextListener : public TextBufferObserver, public TextNodeObserver
{
  void onReplaceRange(TextBuffer& buffer, uint32 position, uint32 length, const std::string& text)
  {
    printf("Replaced range [%u, %u) in text buffer %s(%u) in text node %u (%s) with %s\n",
           position,
           position + length,
           buffer.getName().c_str(),
           buffer.getID(),
           buffer.getNode().getID(),
           buffer.getNode().getName().c_str(),
           text.c_str());
  }

  void onSetName(TextBuffer& buffer, const std::string& name)
  {
    printf("Changed name of text buffer %s(%u) of text node %u (%s) to %s\n",
	   buffer.getName().c_str(),
	   buffer.getID(),
	   buffer.getNode().getID(),
	   buffer.getNode().getName().c_str(),
	   name.c_str());
  }

  void onDestroy(TextBuffer& buffer)
  {
    printf("Destroyed text buffer %s(%u) in text node %u (%s)\n",
	   buffer.getName().c_str(),
	   buffer.getID(),
	   buffer.getNode().getID(),
	   buffer.getNode().getName().c_str());
  }

  void onCreateBuffer(TextNode& node, TextBuffer& buffer)
  {
    printf("Created text buffer %s(%u) in text node %u (%s)\n",
           buffer.getName().c_str(),
	   buffer.getID(),
	   node.getID(),
	   node.getName().c_str());

    buffer.addObserver(*this);
  }

  void onDestroyBuffer(TextNode& node, TextBuffer& buffer)
  {
    printf("Destroyed text buffer %s(%u) in text node %u (%s)\n",
           buffer.getName().c_str(),
	   buffer.getID(),
	   node.getID(),
	   node.getName().c_str());
  }

  void onSetLanguage(TextNode& node, const std::string& language)
  {
    printf("Set language of text node %u (%s) to %s\n",
           node.getID(),
           node.getName().c_str(),
           language.c_str());
  }
};

TextListener textListener;

class GeometryListener : public GeometryLayerObserver, public GeometryNodeObserver
{
  void onSetSlot(GeometryLayer& layer, uint32 slotID, const void* data)
  {
    printf("Changed slot %u in layer %s(%u) of geometry node %u (%s)\n",
           slotID,
	   layer.getName().c_str(),
	   layer.getID(),
	   layer.getNode().getID(),
	   layer.getNode().getName().c_str());
  }
  
  void onSetName(GeometryLayer& layer, const std::string& name)
  {
    printf("Changed name of layer %s(%u) to %s in geometry node %u (%s)\n",
           layer.getName().c_str(),
	   layer.getID(),
	   name.c_str(),
	   layer.getNode().getID(),
	   layer.getNode().getName().c_str());
  }

  void onDestroy(GeometryLayer& layer)
  {
    printf("Destroyed geometry layer %s(%u) of in geometry node %u (%s)\n",
           layer.getName().c_str(),
	   layer.getID(),
	   layer.getNode().getID(),
	   layer.getNode().getName().c_str());
  }

  void onCreateVertex(GeometryNode& node, uint32 vertexID, const BaseVertex& vertex)
  {
    printf("Created vertex %u in geometry node %u(%s)\n",
           vertexID,
	   node.getID(),
	   node.getName().c_str());
  }
  
  void onChangeBaseVertex(GeometryNode& node, uint32 vertexID, const BaseVertex& vertex)
  {
    printf("Changed vertex %u in geometry node %u(%s)\n",
           vertexID,
	   node.getID(),
	   node.getName().c_str());
  }

  void onDeleteVertex(GeometryNode& node, uint32 vertexID)
  {
    printf("Deleted vertex %u in geometry node %u(%s)\n",
           vertexID,
	   node.getID(),
	   node.getName().c_str());
  }
  
  void onCreatePolygon(GeometryNode& node, uint32 polygonID, const BasePolygon& polygon)
  {
    printf("Created polygon %u in geometry node %u(%s)\n",
           polygonID,
	   node.getID(),
	   node.getName().c_str());
  }
  
  void onChangeBasePolygon(GeometryNode& node, uint32 polygonID, const BasePolygon& polygon)
  {
    printf("Changed polygon %u in geometry node %u(%s)\n",
           polygonID,
	   node.getID(),
	   node.getName().c_str());
  }

  void onDeletePolygon(GeometryNode& node, uint32 polygonID)
  {
    printf("Deleted polygon %u in geometry node %u(%s)\n",
           polygonID,
	   node.getID(),
	   node.getName().c_str());
  }

  void onCreateLayer(GeometryNode& node, GeometryLayer& layer)
  {
    printf("Created geometry layer %s(%u) of type %u in geometry node %u (%s)\n",
           layer.getName().c_str(),
	   layer.getID(),
	   layer.getType(),
	   node.getID(),
	   node.getName().c_str());

    layer.addObserver(*this);
  }

  void onDestroyLayer(GeometryNode& node, GeometryLayer& layer)
  {
    printf("Destroyed geometry layer %s(%u) of in geometry node %u (%s)\n",
           layer.getName().c_str(),
	   layer.getID(),
	   node.getID(),
	   node.getName().c_str());
  }
};

GeometryListener geometryListener;

class SessionListener : public SessionObserver
{
public:
  void onAccept(Session& session)
  {
    printf("Accepted session %s\n", session.getAddress().c_str());
  }

  void onTerminate(Session& session, const std::string& byebye)
  {
    printf("Terminated session %s with %s\n",
           session.getAddress().c_str(),
	   byebye.c_str());
  }

  void onCreateNode(Session& session, Node& node)
  {
    printf("Created node %u of type %u\n", node.getID(), node.getType());

    node.addObserver(nodeListener);

    switch (node.getType())
    {
      case V_NT_GEOMETRY:
      {
        node.addObserver(geometryListener);
        break;
      }

      case V_NT_TEXT:
      {
        node.addObserver(textListener);
        break;
      }
    }
  }

  void onDestroyNode(Session& session, Node& node)
  {
    printf("Destroyed node %u (%s)\n", node.getID(), node.getName().c_str());
  }
};

SessionListener sessionListener;

int main(int argc, char** argv)
{
  std::string host;

  if (argc == 1)
    host = "localhost";
  else
    host = argv[1];

  Session* session = Session::create(host, "ample-linter", "secret");
  if (!session)
    return 1;

  session->addObserver(sessionListener);

  while (session->getState() != Session::TERMINATED)
    Session::update(10000);
}


