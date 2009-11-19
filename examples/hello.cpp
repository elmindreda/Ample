
#include <verse.h>

#include "Ample.h"

#include <iostream>

using namespace verse::ample;

class NodeLister : public SessionObserver, public NodeObserver
{
public:
  void onCreateNode(Session& session, Node& node)
  {
    node.addObserver(*this);
 }
  void onSetName(Node& node, const std::string& name)
  {
    std::cout << "node: " << name << std::endl;
  }
};

int main(int argc, char** argv)
{
  std::string host;

  if (argc == 1)
    host = "localhost";
  else
    host = argv[1];

  Session* session = Session::create(host, "spoo", "fleem");
  if (!session)
    return 1;

  NodeLister lister;
  session->addObserver(lister);

  while (session->getState() != Session::TERMINATED)
    Session::update(10000);
}

