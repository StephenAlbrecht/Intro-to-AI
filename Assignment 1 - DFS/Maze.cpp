#include <stack>
#include <map>

using namespace std;

/** we could make this a struct and have a separate utility function for dist.
 * struct makes everything public, but since most members are const and should
 * be assigned upon construction, I don't think it matters. Thoughts? */
class Node
{
  public:
    Node();
    Node(string name, float x, float y);
    float dist(Node *neighbor);
    // need setters, getters
  private:
    const string name;
    const float x;
    const float y;
    const Node *neighbors[];
    bool visited = false;
}

class Maze
{
  public:
    Maze();
    void add_node(Node node); // just add node to map
    void find_exit(); // dfs
  private:
    map<string, Node> nodes;
    stack<Node*> path;
    Node *start_node;
    Node *end_node;
    // distance should be computed at the very end during printing instead of 
    // every time we travel to a new node. Should be more efficient.
    float total_distance;
}

static class mazeIO
{
  public:
    void load_locations();
    void load_connections();
    string get_start_node();
    string get_end_node();
    void print_path(Maze maze);
}