
#include "modules.h"
#include <queue>
#include <limits>
#include <algorithm>
#include <iomanip>

// ==========================================
// GRAPH - HOSPITAL CAMPUS
// ==========================================

class HospitalGraph {
private:
struct Edge {
int destination;
double distance;
};

vector<Location> locations;
vector<vector<Edge>> adj;

int findLocation(string id) {
for (int i = 0; i < locations.size(); i++) {
if (locations[i].id == id)
return i;
}
return -1;
}

public:
HospitalGraph(HospitalData& data) {
locations = data.locations;
adj.resize(locations.size());

for (const Connection& c : data.connections) {
int u = findLocation(c.fromID);
int v = findLocation(c.toID);

if (u == -1 || v == -1)
continue;

adj[u].push_back({v, c.distance});
adj[v].push_back({u, c.distance});
}
}

// DISPLAY LOCATIONS
void displayLocations() {
cout << "\nHOSPITAL LOCATIONS\n";

if (locations.empty()) {
cout << "No locations available.\n";
return;
}

for (const Location& l : locations) {
cout << l.id << " | "
<< l.name << " | "
<< l.department << " | "
<< "Coordinates: (" << l.x << ", " << l.y << ")"
<< endl;
}
}

// DISPLAY CONNECTIONS
void displayConnections() {
cout << "\nHOSPITAL CONNECTIONS\n";

bool found = false;

for (int i = 0; i < locations.size(); i++) {
for (const Edge& e : adj[i]) {
if (i < e.destination) {
cout << locations[i].name << " <-> "
<< locations[e.destination].name
<< " | Distance: " << e.distance
<< endl;

found = true;
}
}
}

if (!found)
cout << "No connections available.\n";
}

// DIJKSTRA'S SHORTEST PATH
void shortestPath(string sourceID, string destinationID) {
int source = findLocation(sourceID);
int destination = findLocation(destinationID);

if (source == -1 || destination == -1) {
cout << "Invalid location ID.\n";
return;
}

int n = locations.size();

vector<double> dist(n, numeric_limits<double>::max());
vector<int> parent(n, -1);

priority_queue<
pair<double, int>,
vector<pair<double, int>>,
greater<pair<double, int>>
> pq;

dist[source] = 0;
pq.push({0, source});

while (!pq.empty()) {
double currentDistance = pq.top().first;
int u = pq.top().second;
pq.pop();

if (currentDistance > dist[u])
continue;

if (u == destination)
break;

for (const Edge& e : adj[u]) {
int v = e.destination;
double newDistance = dist[u] + e.distance;

if (newDistance < dist[v]) {
dist[v] = newDistance;
parent[v] = u;
pq.push({newDistance, v});
}
}
}

if (dist[destination] == numeric_limits<double>::max()) {
cout << "No route exists between these locations.\n";
return;
}

vector<int> path;
int current = destination;

while (current != -1) {
path.push_back(current);
current = parent[current];
}

reverse(path.begin(), path.end());

cout << "\nSHORTEST ROUTE\n";

for (int i = 0; i < path.size(); i++) {
cout << locations[path[i]].name;

if (i != path.size() - 1)
cout << " -> ";
}

cout << "\nTotal Distance: "
<< dist[destination] << endl;
}

// PRIM'S MINIMUM SPANNING TREE
void minimumSpanningTree() {
int n = locations.size();

if (n == 0) {
cout << "No locations available.\n";
return;
}

vector<bool> visited(n, false);

priority_queue<
pair<double, pair<int, int>>,
vector<pair<double, pair<int, int>>>,
greater<pair<double, pair<int, int>>>
> pq;

double totalDistance = 0;
int edgesSelected = 0;

visited[0] = true;

for (const Edge& e : adj[0])
pq.push({e.distance, {0, e.destination}});

cout << "\nMINIMUM SPANNING TREE\n";
cout << "Starting Location: " << locations[0].name << endl;

while (!pq.empty() && edgesSelected < n - 1) {
double weight = pq.top().first;
int u = pq.top().second.first;
int v = pq.top().second.second;
pq.pop();

if (visited[v])
continue;

visited[v] = true;
edgesSelected++;
totalDistance += weight;

cout << locations[u].name << " -- "
<< locations[v].name
<< " | Distance: " << weight << endl;

for (const Edge& e : adj[v]) {
if (!visited[e.destination]) {
pq.push({e.distance, {v, e.destination}});
}
}
}

if (edgesSelected != n - 1) {
cout << "\nThe graph is disconnected. "
<< "A complete MST cannot be formed.\n";
return;
}

cout << "\nTotal MST Distance: "
<< totalDistance << endl;
cout << "Connections Selected: "
<< edgesSelected << endl;
}
};

// ==========================================
// MODULE 4
// ==========================================

void runModule4(HospitalData& data) {
HospitalGraph graph(data);

int choice;

do {
cout << "\n========== HOSPITAL CAMPUS INFRASTRUCTURE ==========\n";
cout << "1. Display Hospital Locations\n";
cout << "2. Display Hospital Connections\n";
cout << "3. Find Shortest Route (Dijkstra)\n";
cout << "4. Generate Minimum Spanning Tree (Prim)\n";
cout << "5. Return to Main Menu\n";
cout << "Enter choice: ";

cin >> choice;
cin.ignore(numeric_limits<streamsize>::max(), '\n');

switch (choice) {

// DISPLAY LOCATIONS
case 1:
graph.displayLocations();
break;

// DISPLAY CONNECTIONS
case 2:
graph.displayConnections();
break;

// SHORTEST PATH
case 3: {
string sourceID, destinationID;

cout << "\nEnter Starting Location ID: ";
getline(cin, sourceID);

cout << "Enter Destination Location ID: ";
getline(cin, destinationID);

graph.shortestPath(sourceID, destinationID);
break;
}

// MINIMUM SPANNING TREE
case 4:
graph.minimumSpanningTree();
break;

// RETURN
case 5:
cout << "Returning to main menu.\n";
break;

default:
cout << "Invalid choice.\n";
}

} while (choice != 5);
}