#include <stack>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

using namespace std;

class Node
{
  public:
    Node(string name, float x, float y) {
      this->name = name;
      this->x = x;
      this->y = y;
    }
    double dist(Node *neighbor) {
      //distance formula: square_root((x2-x1)^2 + (y2-y1)^2)
      return sqrt(pow(neighbor->get_x() - this->get_x(), 2) 
                + pow(neighbor->get_y() - this->get_y(), 2));
    }
    // gets neighbor with smallest alphanumeric name, returns null if no options
    Node * get_available_neighbor() {
      for(Node *neighbor : neighbors)
        if(!neighbor->is_visited())
          return neighbor;
      return nullptr;
    }
    string get_name() { return name; }
    float get_x() { return x; }
    float get_y() { return y; }
    bool is_visited() { return visited; }
    void set_visited() { visited = true; }
    vector<Node *> get_neighbors() { return neighbors; }
    void set_neighbors(vector<Node *> neighbors) { this->neighbors = neighbors; }
  private:
    string name;
    float y;
    float x;
    bool visited = false;
    vector<Node *> neighbors;
};

class Network
{
  public:
    Network() {};
    void add_node(Node *node) {
      nodes.insert(pair<string, Node *>(node->get_name(), node));
    }
    Node * get_node(string name) {
      if(nodes.find(name) != nodes.end())
        return nodes.at(name);
      else
        return nullptr;
    }
    void find_path() { // dfs
      Node *current, *neighbor;
      path.push(start_node);
      while(!path.empty()) { // empty path = no solution
        current = path.top();
        current->set_visited();
        if(current == end_node) { // found!
          break;
        }
        neighbor = current->get_available_neighbor();
        if (neighbor != nullptr) { // add next destination
          path.push(neighbor);
        } else { // no available neighbors, go back
          path.pop();
        }
      }
    }
    void set_start_node(Node *node) { start_node = node; }
    void set_end_node(Node *node) { end_node = node; }
    Node * get_start_node() { return start_node; }
    Node * get_end_node() { return end_node; }
    map<string, Node *> get_nodes() { return nodes; }
    stack<Node*> get_path() { return path; }
    float get_total_distance() { return total_distance; }
    void set_total_distance(float distance) { total_distance = distance; }
  private:
    map<string, Node *> nodes;
    stack<Node *> path;
    Node *start_node;
    Node *end_node;
    float total_distance;
};

struct NetworkIO
{
  static void load_locations(Network *network) {
    ifstream file;
    string line;
    file.open("locations.txt");
    if (!file.is_open()) {
      cout << "Failure to open locations.txt" << endl;
      return;
    }
    while(getline(file, line) && line != "END") {
      istringstream iss(line);
      string name;
      int x, y;
      iss >> name;
      iss >> x;
      iss >> y;
      network->add_node(new Node(name, x, y));
    }
    file.close();
  }
  static bool predicate(Node* a, Node* b) { return a->get_name() < b->get_name(); }
  static void load_connections(Network *network) {
    ifstream file;
		string line;
		file.open("connections.txt");
		if (!file.is_open()) {
			cout << "Failure to open connections.txt" << endl;
			return;
		}
		while (getline(file, line) && line != "END") {
			istringstream iss(line);
			string name;
			int num_neighbors;
			iss >> name;
			iss >> num_neighbors;

      map<string, Node *> nodes = network->get_nodes();
			Node* n = nodes.at(name);
			vector<Node*> node_neighbors;

			for (int i = 0; i < num_neighbors; i++) {
				string neighbor_name;
				iss >> neighbor_name;

				//find node that matches "neighbor_name" and add to node_neighbors vector
				Node* neighbor = nodes.at(neighbor_name);
				node_neighbors.push_back(neighbor);
			}

			// Sorts node_neighbors according to predicate function: by node name;
			sort(node_neighbors.begin(), node_neighbors.end(), NetworkIO::predicate);

			//add the node_neighbors vector to the current node on input file
			nodes.at(name)->set_neighbors(node_neighbors);
		}
		file.close();
	}
  static void set_start_node(Network *network) {
    int valid_input = 0;
    string name;
    while(!valid_input) {
      cout << "Enter the name of the starting node: ";
      cin >> name;
      Node * start_node = network->get_node(name);
      if(start_node != nullptr) {
        network->set_start_node(start_node);
        valid_input = 1;
      } else {
        cout << "Node " << name << " not found. Try again." << endl;
      }
    }
  }
  static void set_end_node(Network *network) {
    int valid_input = 0;
    string name;
    while(!valid_input) {
      cout << "Enter the name of the destination node: ";
      cin >> name;
      Node * end_node = network->get_node(name);
      if(end_node != nullptr) {
        Node * start_node = network->get_start_node();
        if(end_node == start_node) {
          cout << name << " is the starting node. Try again." << endl;
        } else {
          network->set_end_node(end_node);
          valid_input = 1;
        }
      } else {
        cout << "Node " << name << " not found. Try again." << endl;
      }
    }
  }
  static void print_path(Network *network) {
    stack<Node*> path_copy = network->get_path();
    stack<Node*> path_result;
    if(path_copy.empty())
      cout << "No path found!" << endl;
    while (!path_copy.empty()) {
      path_result.push(path_copy.top());
      path_copy.pop();
    }
    Node* current = path_result.top();
    Node* neighbor;
    float distance = 0.0;
    while (path_result.size() > 1 ) {
      path_result.pop();
      neighbor = path_result.top();
      cout << "From " << current->get_name() << " to " << neighbor->get_name() 
          << ". Length: " << floor(current->dist(neighbor)) << endl;
      distance += current->dist(neighbor);
      current = path_result.top();
    }
    network->set_total_distance(distance);
    cout << "Total Length: " << floor(network->get_total_distance()) << endl;
  }
};

int main() {
  Network network;
  NetworkIO::load_locations(&network);
  NetworkIO::load_connections(&network);
  NetworkIO::set_start_node(&network);
  NetworkIO::set_end_node(&network);
  network.find_path();
  NetworkIO::print_path(&network);
  cin.sync();
  cin.get();
}