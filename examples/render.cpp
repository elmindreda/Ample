
#include <GL/glfw.h>

#include <verse.h>
#include <Ample.h>

#include <stdio.h>

#include <exception>

using namespace verse::ample;

class Renderer : public SessionObserver
{
public:
  void render(void);
private:
  void render(const BaseMesh& mesh);
  void onCreateNode(Session& session, Node& node);
  void onDestroyNode(Session& session, Node& node);
  typedef std::list<GeometryNode*> NodeList;
  NodeList nodes;
};

void Renderer::render(void)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.f, 1.f, 0.1f, 100.f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.f, 0.f, -10.f);

  for (NodeList::const_iterator i = nodes.begin();  i != nodes.end();  i++)
  {
    BaseMesh mesh;
    if ((*i)->getBaseMesh(mesh))
      render(mesh);
  }
}

void Renderer::render(const BaseMesh& mesh)
{
  for (BaseMesh::PolygonList::const_iterator i = mesh.mPolygons.begin();  i != mesh.mPolygons.end();  i++)
  {
    glBegin(GL_POLYGON);
    glColor3f(1.f, 1.f, 1.f);
    for (unsigned int j = 0;  j < 4;  j++)
    {
      if ((*i).mIndices[j] == INVALID_VERTEX_ID)
        break;
      const BaseVertex& vertex = mesh.mVertices[(*i).mIndices[j]];
      glVertex3d(vertex.x, vertex.y, vertex.z);
    }
    glEnd();
  }
}

void Renderer::onCreateNode(Session& session, Node& node)
{
  if (GeometryNode* geometryNode = dynamic_cast<GeometryNode*>(&node))
    nodes.push_back(geometryNode);
}

void Renderer::onDestroyNode(Session& session, Node& node)
{
  if (GeometryNode* geometryNode = dynamic_cast<GeometryNode*>(&node))
    nodes.remove(geometryNode);
}

int main(int argc, char** argv)
{
  if (!glfwInit())
    return -1;

  if (!glfwOpenWindow(640, 480, 8, 8, 8, 0, 16, 0, GLFW_WINDOW))
  {
    glfwTerminate();
    return -1;
  }

  std::string address;

  if (argc > 1)
    address = argv[1];
  else
    address = "localhost";

  glClearColor(0.f, 0.f, 0.f, 1.f);

  Session* session = Session::create(address, "render", "secret");

  Renderer renderer;
  session->addObserver(renderer);

  while (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE)
  {
    Session::update(10000);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer.render();
    glfwSwapBuffers();
  }

  glfwTerminate();
  return 0;
}

