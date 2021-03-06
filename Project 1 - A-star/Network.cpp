#include <stack>
#include <queue>	//priority queue
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>

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
		return sqrt(pow(neighbor->get_x() - this->get_x(), 2)
			+ pow(neighbor->get_y() - this->get_y(), 2));
	}
	string get_name() { return name; }
	float get_x() { return x; }
	float get_y() { return y; }
	vector<Node *> *get_neighbors() { return &neighbors; }
	void set_neighbors(vector<Node *> neighbors) { this->neighbors = neighbors; }
private:
	string name;
	float y;
	float x;
	double straight_line_dist; // distance to end_node calc'd upon visiting for first time
	int neighbor_count;
	vector<Node *> neighbors;
};

typedef struct open_path
{
	open_path() {};
	void update(Node * new_top, Node * end_node) {
		dist_traveled += path.top()->dist(new_top);
		path.push(new_top);
		top_name = path.top()->get_name();
		est_dist = dist_traveled + path.top()->dist(end_node);	// f(x) value
		est_cities = path.size() + 1;
	}
	stack<Node *> path;
	string top_name;
	int est_cities;
	double dist_traveled=0;
	double est_dist;
} open_path;

//used to compare all elements of pq; auto orders pq from smallest to largest est_dist, i.e. f(x)
struct CompareFunct {
  bool *fewest_cities;
  CompareFunct(bool *fewest_cities): fewest_cities(fewest_cities) {};
	bool operator() (const open_path *a, const open_path *b) const {
    if(*fewest_cities)
      return a->est_cities > b->est_cities;
    else
      return a->est_dist > b->est_dist;
  }
};

class Network
{
  public:
    Network() {
      fewest_cities = false;
      step_by_step = false;
    };
    void add_node(Node *node) {
      nodes.insert(pair<string, Node *>(node->get_name(), node));
    }
    Node * get_node(string name) {
      if (nodes.find(name) != nodes.end())
        return nodes.at(name);
      else
        return nullptr;
    }
    //add all the paths of the starting node
    void create_starting_path() {
      current = new open_path();
      current->path.push(start_node);
      current->top_name = start_node->get_name();
      for (Node* neighbor : *start_node->get_neighbors()) {
        open_path *new_path = new open_path();
        new_path->path.push(start_node);
        new_path->update(neighbor, end_node);
        pq.push(new_path);
      }
    }

    // steps once through system
    int step() {
      if (pq.empty())	//no solution
        return -1;
      //test to see if we've found a valid, shortest path
      
      if(current->top_name.compare(end_node->get_name()) == 0) {
        return 0;
      }
      
      open_path * best_path = pq.top();
      pq.pop();

      //current is the best path
      current = best_path;
      Node * current_top = current->path.top();
      vector<Node *> neighbors = *current_top->get_neighbors();

      Node * prev_neighbor = nullptr;
      if(current->path.size() > 1) {
        current->path.pop();
        prev_neighbor = current->path.top();
        current->path.push(current_top);
      }
      for (Node * neighbor : neighbors) {
        if(current->path.size() > 1 && neighbor == prev_neighbor) continue;
        else {
          open_path * nbr_path = new open_path();
          nbr_path->path = current->path;
          nbr_path->dist_traveled = current->dist_traveled;
          nbr_path->est_cities = current->est_cities;
          nbr_path->est_dist = current->est_dist;
          nbr_path->update(neighbor, end_node);
          if(remove_inferior_paths(nbr_path))
            pq.push(nbr_path);
        }
      }
      return 1;
    }
    void exclude_nodes(vector<string> excluded_nodes) {
      for (string name : excluded_nodes) {
        // check if node exists
        Node* excluded_node = get_node(name);
        if (get_node(name) == nullptr)
          continue;
        // remove from each node's list of neigbors if they are connected
        for (pair<string, Node *> element : nodes) {
          Node *node = element.second;
          vector<Node *> *neighbors = node->get_neighbors();
          vector<Node *>::iterator neighbor_location =
            find(neighbors->begin(), neighbors->end(), excluded_node);
          if (neighbor_location != neighbors->end()) {
            *neighbors->erase(neighbor_location);
          }
        }
        // remove from map and delete the Node object
        nodes.erase(nodes.find(name));
        cout << excluded_node->get_name() << " excluded" << endl;
        delete excluded_node;
      }
    }
    bool remove_inferior_paths(open_path* target) {
      open_path * op;
      vector<open_path *> temp_vector;
      bool delete_target = false;

      //copy all the pq elements to temp_vector, then make pq empty
      while (!pq.empty()) {
        op = pq.top();
        pq.pop();
        temp_vector.push_back(op);
      }

      //erase any path that matches top_name and has a larger est_dist than target
      vector<open_path *>::iterator iter = temp_vector.begin();
      while (iter != temp_vector.end()) {
        open_path * temp_path = *iter;
        if (temp_path->top_name.compare(target->top_name) == 0) {
          if(temp_path->est_dist > target->est_dist) {
            iter = temp_vector.erase(iter);
          } else {
            delete_target = true;
            break;
          }
        } 
        else
          ++iter;
      }

      //copy the remaining temp_vector back into priority_queue (pq)
      while (!temp_vector.empty()) {
        op = temp_vector.back();
        pq.push(op);
        temp_vector.pop_back();
      }
      return !delete_target;
    }
    void set_start_node(Node *node) { start_node = node; }
    void set_end_node(Node *node) { end_node = node; }
    Node * get_start_node() { return start_node; }
    Node * get_end_node() { return end_node; }
    open_path* get_current() {return current; }
    void set_fewest_cities() { fewest_cities = true; }
    bool get_fewest_cities() { return fewest_cities; }
    void set_step_by_step() { step_by_step = true; }
    bool is_step_by_step() { return step_by_step; }
    priority_queue<open_path*, vector<open_path *>, CompareFunct> get_pq() { return pq; }
    map<string, Node *> get_nodes() { return nodes; }
  private:
    map<string, Node *> nodes;
    Node *start_node;
    Node *end_node;
    open_path *current;
    bool fewest_cities;
    bool step_by_step;
    CompareFunct cmp{&fewest_cities};
  	priority_queue<open_path *, vector<open_path *>, CompareFunct> pq{cmp};
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
		while (getline(file, line) && line != "END") {
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
			vector<Node*> node_neighbors;

			for (int i = 0; i < num_neighbors; i++) {
				string neighbor_name;
				iss >> neighbor_name;

				//find node that matches "neighbor_name" and add to node_neighbors vector
				Node* neighbor = nodes.at(neighbor_name);
				node_neighbors.push_back(neighbor);
			}

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
		while (selection < 1 || selection > 2) {
			getline(cin, line);
			istringstream inSS;
			inSS.str(line);
			while (inSS.good()) {
				inSS >> selection;
			}
			if (selection < 1 || selection > 2) {
				cout << "Please enter 1 or 2." << endl;
			}
		}
		if (selection == 2)
			network->set_fewest_cities();
	}
  // prompts user, then sets network.step T/F
  static void get_output_type(Network *network) {
    cout << "Choose output type: (1) Normal or (2) Step-by-step" << endl;
    int selection = 0;
    string line;
    while (selection < 1 || selection > 2) {
      getline(cin, line);
      istringstream inSS;
      inSS.str(line);
      while(inSS.good()) {
        inSS >> selection;
      }
      if (selection < 1 || selection > 2 ) {
        cout << "Please enter 1 or 2." << endl;
      }
    }
    if(selection == 2)
      network->set_step_by_step();
  }
	// prompts user to enter cities on one line, stores in vector and returns 
	static vector<string> get_excluded_nodes() {
		cout << "Enter any cities you wish to exclude, separated by spaces, or press enter:" << endl;
		string line, excluded_node_name;
		vector<string> excluded_nodes;
		getline(cin, line);
		istringstream inSS;
		inSS.str(line);
		while (inSS.good()) {			// while stream has no error
			inSS >> excluded_node_name;
			excluded_nodes.push_back(excluded_node_name);
		}
		return excluded_nodes;
	}
	static void set_start_node(Network *network) {
		int valid_input = 0;
		string name;
		while (!valid_input) {
			cout << "Enter the name of the starting node: ";
			cin >> name;
			Node * start_node = network->get_node(name);
			if (start_node != nullptr) {
				network->set_start_node(start_node);
				valid_input = 1;
			}
			else {
				cout << "Node " << name << " not found. Try again." << endl;
			}
		}
	}
	static void set_end_node(Network *network) {
		int valid_input = 0;
		string name;
		while (!valid_input) {
			cout << "Enter the name of the destination node: ";
			cin >> name;
			Node * end_node = network->get_node(name);
			if (end_node != nullptr) {
				Node * start_node = network->get_start_node();
				if (end_node == start_node) {
					cout << name << " is the starting node. Try again." << endl;
				}
				else {
					network->set_end_node(end_node);
					valid_input = 1;
				}
			}
			else {
				cout << "Node " << name << " not found. Try again." << endl;
			}
		}
	}

	
	static void print_path(Network *network) {

		stack<Node*> path_copy = network->get_current()->path;
		stack<Node*> path_result;
		bool first_visit_done = false;

		if (path_copy.empty()) 
			cout << "No path found." << endl; 

		//copy the stack from path_copy to path_result
		while (!path_copy.empty()) {
			path_result.push(path_copy.top());
			path_copy.pop();
		}

		Node* current = path_result.top();
		path_result.pop();

		while (!path_result.empty()) {
			cout << current->get_name() << " to " << path_result.top()->get_name() << ": length = ";

			if (network->get_fewest_cities()) {
				if (!first_visit_done) {
					cout << "2" << endl;
					first_visit_done = true;
				}
				else cout << "1" << endl;
			}
			else {
        cout << fixed << setprecision(2);
				double distance = current->dist(path_result.top());
				cout << distance << endl;
			}
			current = path_result.top();
			path_result.pop();
		}
    cout << "\nTotal path length: ";
    if (network->get_fewest_cities())
      cout << network->get_pq().top()->est_cities - 1 << endl; 
    else
      cout << network->get_pq().top()->dist_traveled << endl;
	}

  // prints current state of the network according to sample output in document
  static void print_step(Network *network) {
    // print network->current->getname
    open_path *current = network->get_current();
    cout << "----------------------------------------------------" << endl;
    cout << "Current node: " << current->top_name << endl;
    // print current's neighbors
    cout << "Current node's neighbors: ";
    vector<Node *> neighbors = *current->path.top()->get_neighbors();
    if(neighbors.empty()) {
      cout << " - ";
    } else {
      if(current->path.size() > 1) {
        Node * current_top = current->path.top();
        current->path.pop();
      
        for(Node * neighbor : neighbors) { // omit previously visited neighbor
          if(neighbor != current->path.top()) 
            cout << neighbor->get_name() << " ";
        }
        current->path.push(current_top);
      } else {
        for(Node * neighbor : neighbors) { // omit previously visited neighbor
          cout << neighbor->get_name() << " ";
        }
      }
    }
    // print top_name of each open path and est_dist / est_cities depending on heuristic
		open_path * temp_op;
		vector<open_path *> temp_vector;
    priority_queue<open_path *, vector<open_path *>, CompareFunct> pq = network->get_pq();
    cout << endl << "Open paths: ";
    while(!pq.empty()) {
      temp_op = pq.top();
      pq.pop();
      temp_vector.push_back(temp_op);
    }
    cout << fixed << setprecision(2);
    if(network->get_fewest_cities()) {
      for(open_path * op : temp_vector)
        cout << op->top_name << "(" << op->est_cities << ") ";
    } else {
      for(open_path * op : temp_vector)
        cout << op->top_name << "(" << op->est_dist << ") ";
    }
    cout << endl;
    cin.sync();
    cin.get();
  }
};

int main() {
	Network network;
	NetworkIO::load_locations(&network);
	NetworkIO::load_connections(&network);
	NetworkIO::get_heuristic(&network);
	NetworkIO::get_output_type(&network);
	network.exclude_nodes(NetworkIO::get_excluded_nodes());
	NetworkIO::set_start_node(&network);
	NetworkIO::set_end_node(&network);
	network.create_starting_path();
	cout << endl;
	int status;
	if (network.is_step_by_step()) {
		do {
			NetworkIO::print_step(&network);
			status = network.step();
		} while (status != 0);
	} else {
		do {
		  status = network.step();
		} while (status != 0);
	}
	if(status == -1) {
		cout << "//=============== NO SOLUTION ===============//" << endl;
	} else if (status == 0) {
		cout << "//=========== FINAL SOLUTION PATH ===========//" << endl;
		NetworkIO::print_path(&network);
	}

	cin.sync();
	cin.get();
}