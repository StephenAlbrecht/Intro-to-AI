#include <stack>
#include <queue>	//priority queue
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
      visited = false;
    }
    double dist(Node *neighbor) {
      //distance formula: square_root((x2-x1)^2 + (y2-y1)^2)
      return sqrt(pow(neighbor->get_x() - this->get_x(), 2) 
                + pow(neighbor->get_y() - this->get_y(), 2));
    }
    // gets neighbor with smallest alphanumeric name, returns nullptr if no options
    Node * get_available_neighbor() {
      for(int i = 0; i < neighbors.size(); i++) {
        if(!neighbors.at(i)->is_visited())
          return neighbors.at(i);
      }
      return nullptr;
    }
    string get_name() { return name; }
    float get_x() { return x; }
    float get_y() { return y; }
    bool is_visited() { return visited; }
    void set_visited() { visited = true; }
    vector<Node *>* get_neighbors() { return &neighbors; }
    void set_neighbors(vector<Node *> neighbors) { this->neighbors = neighbors; }
  private:
    string name;
    float y;
    float x;
    bool visited; // do we still need to mark visited? need a different mechanism?
    double straight_line_dist; // distance to end_node calc'd upon visiting for first time
    vector<Node *> neighbors;
};

struct open_path
{
	stack<Node *> path;
	string top_name;
	double dist_traveled;
	double straight_line_dist;
	double est_dist;
};

//this class is used to compare all elements of pq; sets order in pq from smallest to largest, according to est_dist
class compareFunct {
public:
	double operator() (const open_path &a, const open_path &b) {
		return a.est_dist > b.est_dist;
	}
};

class Network
{
  public:
    Network() {
      fewest_cities = false;
    };
    void add_node(Node *node) {
      nodes.insert(pair<string, Node *>(node->get_name(), node));
    }
    Node * get_node(string name) {
      if(nodes.find(name) != nodes.end())
        return nodes.at(name);
      else
        return nullptr;
    }
    void find_path() { // update for use with heuristics
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
    // steps once through system
    void step() {}
    void exclude_nodes(vector<string> excluded_nodes) {
      for(string name : excluded_nodes) {
        // check if node exists
        Node* excluded_node = get_node(name);
        if(get_node(name) == nullptr)
          continue;      
        // remove from each node's list of neigbors if they are connected
        //map<string, Node *>::iterator map_it = nodes.begin();
        for(pair<string, Node *> element : nodes) {
          Node *node = element.second;
          vector<Node *> *neighbors = node->get_neighbors();
          vector<Node *>::iterator neighbor_location =
              find(neighbors->begin(), neighbors->end(), excluded_node);
          if(neighbor_location != neighbors->end()) {
            *neighbors->erase(neighbor_location);
          }
        }
        // remove from map and delete the Node object
        nodes.erase(nodes.find(name));
        delete excluded_node;
      }
    }
    void set_start_node(Node *node) { start_node = node; }
    void set_end_node(Node *node) { end_node = node; }
    Node * get_start_node() { return start_node; }
    Node * get_end_node() { return end_node; }
    void set_fewest_cities() { fewest_cities = true; }
    bool get_fewest_cities() { return fewest_cities; }
    map<string, Node *> get_nodes() { return nodes; }
    stack<Node*> get_path() { return path; }
    float get_total_distance() { return total_distance; }
    void set_total_distance(float distance) { total_distance = distance; }
	
  private:
    map<string, Node *> nodes;
    stack<Node *> path; // marked for deletion
    Node *start_node;
    Node *end_node;
    float total_distance; // marked for deletion
    bool fewest_cities; // heuristic. any clearer way to signal alternative is straight_line?
	priority_queue<open_path, vector<open_path>, compareFunct> pq;
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
  // should we delete predicate since nodes are no longer chosen alphanumerically?
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
  // prompts user, then sets network.fewest_cities T/F
  static void get_heuristic(Network *network) {
    cout << "Choose heuristic: (1) Straight line distance or (2) Fewest cities" << endl;
    int selection = 0;
    string line;
    while(selection < 1 || selection > 2) {
      getline(cin, line);
      istringstream inSS;
      inSS.str(line);
      while(inSS.good()) {
        inSS >> selection;
      }
      if(selection < 1 || selection > 2) {
        cout << "Please enter 1 or 2." << endl;
      }
    }
    if(selection == 2)
      network->set_fewest_cities();
  }
  // prompts user to enter cities on one line, stores in vector and returns 
  static vector<string> get_excluded_nodes() {
    cout << "Enter any cities you wish to exclude, separated by spaces, or press enter:" << endl;
    string line, excluded_node_name;
    vector<string> excluded_nodes;
    getline(cin, line);
    istringstream inSS;
    inSS.str(line);					
    while(inSS.good()) {			// while stream has no error
      inSS >> excluded_node_name;
      excluded_nodes.push_back(excluded_node_name);
    }
    return excluded_nodes;
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
  // prints current state of the network according to sample output in document
  static void print_step(Network *network) {}
};

int main() {
  Network network;
  NetworkIO::load_locations(&network);
  NetworkIO::load_connections(&network);
  NetworkIO::get_heuristic(&network);
  network.exclude_nodes(NetworkIO::get_excluded_nodes());
  NetworkIO::set_start_node(&network);
  NetworkIO::set_end_node(&network);
  network.find_path();
  NetworkIO::print_path(&network);
  cin.sync();
  cin.get();
}