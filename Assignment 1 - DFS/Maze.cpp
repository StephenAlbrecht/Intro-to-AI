#include <stack>
#include <map>

using namespace std;

/**I've been using Node* pointers here to reference nodes without copying them
 * but I'm not 100% certain the usage is correct. */
class Node
{
  public:
    Node();
    Node(string name, float x, float y);
    float dist(Node *neighbor);
    // gets neighbor with smallest alphanumeric name, returns null if no options
    Node * get_available_neighbor();
    // need setters, getters
  private:
    const string name;
    const float x;
    const float y;
    const Node *neighbors[];
    bool visited = false;
}

class Network
{
  public:
    Network();
    void add_node(Node node); // just add node to map
    void find_path(); // dfs
  private:
    map<string, Node> nodes;
    stack<Node*> path;
    Node *start_node;
    Node *end_node;
    // distance should be computed at the very end during printing instead of 
    // every time we travel to a new node. Should be more efficient.
    float total_distance;
}

static class networkIO
{
  public:
    void load_locations();
    void load_connections();
    string get_start_node();
    string get_end_node();
    void print_path(Maze maze);
}