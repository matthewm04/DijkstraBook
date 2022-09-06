#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>
#include <cstdlib>
#include <queue>
#include <stack>
using namespace std;

#define IS_QUIT(s) (s == "X" || s == "x")
#define FILENAME "fb_weighted.csv"
using namespace std;

// A node in the Facebook graph
struct Person {
    int best_weight;        // The sum of the weights along the best path
    string best_parent;     // The parent node for the best path
    vector<string> friends; // String names of the friend edges
    vector<int> weights;    // Weights for each friend edge
};

struct PersonCandidate {
    string name;
    int weight;
    string parent;
    bool operator< (const PersonCandidate& rhs) const {
        return weight > rhs.weight;
    }
};

void parse_line(const string& str,
    vector<string>& line) {
    istringstream istr(str);
    string tmp;
    while (getline(istr, tmp, ',')) {
        line.push_back(tmp);
    }
}

void add_to_tent(priority_queue<PersonCandidate>& pq, Person p, string start) {
    // take all of p's friends and add them to TENT and make sure start is the parent of each one

    PersonCandidate tmpP;
    for (int i = 0; i < p.friends.size(); i++) {
        tmpP.name = p.friends[i];
        tmpP.parent = start;
        tmpP.weight = p.weights[i];
        pq.push(tmpP);
    }
}

// Output the shortest path
// - everyone:  reference variable to the graph
// - starting:  string name of the starting person
// - ending: string name of the ending person
bool dijkstra(map<string, Person>& everyone,
    string starting, string ending) {

    priority_queue<PersonCandidate> tentPQ;

    // Put root in PATHS
    Person& p = everyone[starting];
    p.best_weight = 0;
    p.best_parent = "";

    add_to_tent(tentPQ, p, starting);

    while (!tentPQ.empty()) {
        // Get the best entry from PATHS
        PersonCandidate pc = tentPQ.top();
        tentPQ.pop();

        // Get this person in the graph
        Person& p = everyone[pc.name];

        if (p.best_weight != -1)
            continue;

        p.best_weight = pc.weight;
        p.best_parent = pc.parent;

        if (pc.name == ending)
            return true;

        add_to_tent(tentPQ, p, pc.name);
    }
    return (false);
}

int main() {
    ifstream inFile(FILENAME);
    vector<string> row;
    vector<string> names;
    map<string, Person> everyone;
    string inputLine;

    // Verify that the file open was OK
    if (!inFile.good()) {
        cerr << "Invalid file." << endl;
        return (-1);
    }

    // Read the header line of the file (first line, contains column labels).
    // We save this line (names) so we can lookup the string names when
    // needed.
    getline(inFile, inputLine);
    parse_line(inputLine, names);

    // Reach each subsequent entry
    while (getline(inFile, inputLine)) {
        if (inFile.eof())
            break;
        vector<string> row;
        Person p;
        parse_line(inputLine, row);
        // Start at 1 (0th field is the string name)
        for (size_t i = 1; i < row.size(); i++) {
            int adj_status = atoi(row[i].c_str());
            // A '1' indicates an adjacency, so skip if we get a '0'
            // If there is an adjacency to this person, push the string name
            // of that person on the adjacency list.
            if (adj_status > 0) {
                p.friends.push_back(names[i]);
                p.weights.push_back(adj_status);
            }

            // Initialize the other fields
            p.best_weight = -1;
            p.best_parent = "";
        }
        // Add this (new) person to the map.
        // In this map, the key is the string name of the person, and
        // the value is their Person structure (adjacency list).
        everyone.insert(make_pair(row[0], p));
    }

    // The main loop of the program
    for (;;) {

        string to, from;
        cout << endl << "Enter the starting name (X to quit): ";
        getline(cin, from);
        if (IS_QUIT(from))
            break;

        cout << endl << "Enter the ending name (X to quit): ";
        getline(cin, to);
        if (IS_QUIT(to))
            break;

        if (everyone.count(from) == 0 || everyone.count(to) == 0) {
            cout << "One or more people is not in the map." << endl;
            continue;
        }

        // Run the calculation
        if (dijkstra(everyone, from, to)) {
            cout << "The best path between these two people is: " << endl;
            // Construct the path from the parents stored
            // Output the path in reverse

            // creates a stack to output in reverse
            string tmpStr = to;
            stack<string> orderStack;
            while (everyone[tmpStr].best_parent != "") {
                orderStack.push(tmpStr);
                tmpStr = everyone[tmpStr].best_parent;
            }

            // outputs stack
            cout << from << endl;
            while (!orderStack.empty()) {
                cout << orderStack.top() << endl;
                orderStack.pop();
            }
        }
        else {
            cout << "There is NOT a path between these two people." << endl;
        }

        // Clean up all the state
        for (auto i = everyone.begin(); i != everyone.end(); i++) {
            Person& p = i->second;
            p.best_weight = -1;
            p.best_parent = "";
        }
    }
    cout << "Exiting..." << endl;
}
