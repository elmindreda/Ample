
// NOTE: YES, THIS IS A GREAT BIG HACK. There.

#include <verse.h>
#include <Ample.h>

#include <stdio.h>

#include <exception>

#if __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

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

Renderer renderer;

static void update(int value)
{
  Session::update(1000);
  glutPostRedisplay();
  glutTimerFunc(35, update, 0);
}

static void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  renderer.render();
  glutSwapBuffers();
}

int main(int argc, char** argv)
{
  std::string address;
  if (argc > 1)
    address = argv[1];
  else
    address = "localhost";

  Session* session = Session::create(address, "render", "secret");
  session->addObserver(renderer);

  glutInit(&argc, argv);

  glutInitWindowSize(640, 480);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Ample renderer");
  glutDisplayFunc(display);
  glutTimerFunc(35, update, 0);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glFrontFace(GL_CW);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  glutMainLoop();
  return 0;
}

