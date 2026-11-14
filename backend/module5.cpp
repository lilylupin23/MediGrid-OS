
#include "modules.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <climits>
#include <memory>
#include <sstream>
using namespace std;

// COMMON HELPER FUNCTIONS

string normalize(string value) {
for (int i = 0; i < value.length(); i++)
value[i] = tolower(static_cast<unsigned char>(value[i]));
return value;
}

vector<string> splitSynonyms(string value) {
vector<string> result;
string item;
stringstream ss(value);

while (getline(ss, item, ',')) {
size_t start = item.find_first_not_of(" \t");
size_t end = item.find_last_not_of(" \t");

if (start != string::npos)
result.push_back(item.substr(start, end - start + 1));
}

return result;
}

void displayDiagnosis(const Diagnosis& d) {
cout << "\nDiagnosis ID: " << d.id;
cout << "\nName: " << d.name;
cout << "\nCategory: " << d.category;
cout << "\nDescription: " << d.description;
cout << "\nSynonyms: " << d.synonyms << "\n";
}

// 1. TRIE - PREFIX SEARCH FOR DIAGNOSES

class Trie {
struct TrieNode {
TrieNode* children[26];
vector<int> diagnosisIndexes;
bool isEnd;

TrieNode() {
for (int i = 0; i < 26; i++)
children[i] = nullptr;
isEnd = false;
}
};

TrieNode* root;

void collect(TrieNode* node, vector<int>& results) {
if (node == nullptr)
return;

if (node->isEnd) {
for (int index : node->diagnosisIndexes) {
if (find(results.begin(), results.end(), index) == results.end())
results.push_back(index);
}
}

for (int i = 0; i < 26; i++)
collect(node->children[i], results);
}

void destroy(TrieNode* node) {
if (node == nullptr)
return;

for (int i = 0; i < 26; i++)
destroy(node->children[i]);

delete node;
}

public:
Trie() {
root = new TrieNode();
}

~Trie() {
destroy(root);
}

void insert(string word, int diagnosisIndex) {
word = normalize(word);
TrieNode* current = root;

for (char ch : word) {
if (ch < 'a' || ch > 'z')
continue;

int position = ch - 'a';

if (current->children[position] == nullptr)
current->children[position] = new TrieNode();

current = current->children[position];
}

current->isEnd = true;

if (find(current->diagnosisIndexes.begin(), current->diagnosisIndexes.end(), diagnosisIndex) == current->diagnosisIndexes.end())
current->diagnosisIndexes.push_back(diagnosisIndex);
}

vector<int> searchPrefix(string prefix) {
prefix = normalize(prefix);
TrieNode* current = root;

for (char ch : prefix) {
if (ch < 'a' || ch > 'z')
continue;

int position = ch - 'a';

if (current->children[position] == nullptr)
return {};

current = current->children[position];
}

vector<int> results;
collect(current, results);
return results;
}
};

// 2. HASH TABLE - EXACT DIAGNOSIS LOOKUP

class DiagnosisHashTable {
struct Entry {
string key;
vector<int> diagnosisIndexes;
Entry* next;

Entry(string k, int index) {
key = k;
diagnosisIndexes.push_back(index);
next = nullptr;
}
};

static const int TABLE_SIZE = 101;
Entry* table[TABLE_SIZE];

int hashFunction(string key) {
int hashValue = 0;

for (char ch : key)
hashValue = (hashValue * 31 + ch) % TABLE_SIZE;

return hashValue;
}

public:
DiagnosisHashTable() {
for (int i = 0; i < TABLE_SIZE; i++)
table[i] = nullptr;
}

~DiagnosisHashTable() {
for (int i = 0; i < TABLE_SIZE; i++) {
Entry* current = table[i];

while (current != nullptr) {
Entry* temp = current;
current = current->next;
delete temp;
}
}
}

void insert(string key, int diagnosisIndex) {
key = normalize(key);

if (key.empty())
return;

int position = hashFunction(key);
Entry* current = table[position];

while (current != nullptr) {
if (current->key == key) {
if (find(current->diagnosisIndexes.begin(), current->diagnosisIndexes.end(), diagnosisIndex) == current->diagnosisIndexes.end())
current->diagnosisIndexes.push_back(diagnosisIndex);
return;
}

current = current->next;
}

Entry* newEntry = new Entry(key, diagnosisIndex);
newEntry->next = table[position];
table[position] = newEntry;
}

vector<int> search(string key) {
key = normalize(key);
int position = hashFunction(key);
Entry* current = table[position];

while (current != nullptr) {
if (current->key == key)
return current->diagnosisIndexes;

current = current->next;
}

return {};
}
};

// 3. QUAD-TREE - SPATIAL LOCATION SEARCH

class QuadTree {
struct Point {
double x, y;
int locationIndex;
};

struct Node {
double minX, maxX, minY, maxY;
vector<Point> points;
Node* children[4];
int depth;

Node(double x1, double x2, double y1, double y2, int d) {
minX = x1;
maxX = x2;
minY = y1;
maxY = y2;
depth = d;

for (int i = 0; i < 4; i++)
children[i] = nullptr;
}

bool isLeaf() {
return children[0] == nullptr;
}
};

Node* root;
const int CAPACITY = 4;
const int MAX_DEPTH = 12;

bool intersects(Node* node, double x1, double x2, double y1, double y2) {
return !(node->maxX < x1 || node->minX > x2 ||
node->maxY < y1 || node->minY > y2);
}

int getQuadrant(Node* node, double x, double y) {
double midX = (node->minX + node->maxX) / 2;
double midY = (node->minY + node->maxY) / 2;

if (x >= midX && y >= midY)
return 0;

if (x < midX && y >= midY)
return 1;

if (x < midX && y < midY)
return 2;

return 3;
}

void split(Node* node) {
double midX = (node->minX + node->maxX) / 2;
double midY = (node->minY + node->maxY) / 2;
int depth = node->depth + 1;

node->children[0] = new Node(midX, node->maxX, midY, node->maxY, depth);
node->children[1] = new Node(node->minX, midX, midY, node->maxY, depth);
node->children[2] = new Node(node->minX, midX, node->minY, midY, depth);
node->children[3] = new Node(midX, node->maxX, node->minY, midY, depth);

vector<Point> oldPoints = node->points;
node->points.clear();

for (Point p : oldPoints) {
int q = getQuadrant(node, p.x, p.y);
insertPoint(node->children[q], p);
}
}

void insertPoint(Node* node, Point point) {
if (node->isLeaf()) {
if (node->points.size() < CAPACITY || node->depth >= MAX_DEPTH) {
node->points.push_back(point);
return;
}

split(node);
}

int q = getQuadrant(node, point.x, point.y);
insertPoint(node->children[q], point);
}

void searchRegion(Node* node, double x1, double x2, double y1, double y2, const vector<Location>& locations, vector<int>& results) {
if (node == nullptr || !intersects(node, x1, x2, y1, y2))
return;

for (Point p : node->points) {
if (p.x >= x1 && p.x <= x2 && p.y >= y1 && p.y <= y2)
results.push_back(p.locationIndex);
}

for (int i = 0; i < 4; i++)
searchRegion(node->children[i], x1, x2, y1, y2, locations, results);
}

void destroy(Node* node) {
if (node == nullptr)
return;

for (int i = 0; i < 4; i++)
destroy(node->children[i]);

delete node;
}

public:
QuadTree(const vector<Location>& locations) {
root = nullptr;

if (locations.empty())
return;

double minX = locations[0].x;
double maxX = locations[0].x;
double minY = locations[0].y;
double maxY = locations[0].y;

for (const Location& location : locations) {
minX = min(minX, location.x);
maxX = max(maxX, location.x);
minY = min(minY, location.y);
maxY = max(maxY, location.y);
}

// Give the root a non-zero area, even if all points share coordinates.
double padding = 0.001;
if (minX == maxX) {
minX -= padding;
maxX += padding;
}
if (minY == maxY) {
minY -= padding;
maxY += padding;
}

root = new Node(minX, maxX, minY, maxY, 0);

for (int i = 0; i < locations.size(); i++) {
Point p = {locations[i].x, locations[i].y, i};
insertPoint(root, p);
}
}

~QuadTree() {
destroy(root);
}

vector<int> search(double x1, double x2, double y1, double y2, const vector<Location>& locations) {
vector<int> results;

if (root == nullptr)
return results;

if (x1 > x2)
swap(x1, x2);

if (y1 > y2)
swap(y1, y2);

searchRegion(root, x1, x2, y1, y2, locations, results);
return results;
}
};

// 4. SEGMENT TREE - RANGE QUERIES ON SYSTOLIC BP

class SegmentTree {
struct Node {
int minimum, maximum;
long long sum;
};

vector<Node> tree;
int n;

void build(int node, int start, int end, const vector<Patient>& patients) {
if (start == end) {
int value = patients[start].systolicBP;
tree[node] = {value, value, value};
return;
}

int mid = (start + end) / 2;

build(node * 2, start, mid, patients);
build(node * 2 + 1, mid + 1, end, patients);

tree[node].minimum = min(tree[node * 2].minimum, tree[node * 2 + 1].minimum);
tree[node].maximum = max(tree[node * 2].maximum, tree[node * 2 + 1].maximum);
tree[node].sum = tree[node * 2].sum + tree[node * 2 + 1].sum;
}

Node query(int node, int start, int end, int left, int right) {
if (right < start || end < left)
return {INT_MAX, INT_MIN, 0};

if (left <= start && end <= right)
return tree[node];

int mid = (start + end) / 2;

Node a = query(node * 2, start, mid, left, right);
Node b = query(node * 2 + 1, mid + 1, end, left, right);

return {
min(a.minimum, b.minimum),
max(a.maximum, b.maximum),
a.sum + b.sum
};
}

public:
SegmentTree(const vector<Patient>& patients) {
n = patients.size();

if (n > 0) {
tree.resize(4 * n);
build(1, 0, n - 1, patients);
}
}

void rangeQuery(int left, int right, const vector<Patient>& patients) {
if (n == 0) {
cout << "\nNo patient records available.\n";
return;
}

if (left < 1 || right > n || left > right) {
cout << "\nInvalid patient index range.\n";
return;
}

Node result = query(1, 0, n - 1, left - 1, right - 1);

cout << "\nPatient records from index " << left << " to " << right;
cout << "\nMinimum systolic BP: " << result.minimum;
cout << "\nMaximum systolic BP: " << result.maximum;
cout << "\nTotal systolic BP: " << result.sum;
cout << "\nAverage systolic BP: " << (double)result.sum / (right - left + 1) << "\n";

cout << "\nIncluded patients:\n";

for (int i = left - 1; i < right; i++)
cout << patients[i].id << " - " << patients[i].name << " - " << patients[i].systolicBP << " mmHg\n";
}
};

// 5. MODULE INITIALIZATION

void runModule5(HospitalData& data) {
Trie diagnosisTrie;
DiagnosisHashTable diagnosisHash;
QuadTree locationTree(data.locations);
SegmentTree bpTree(data.patients);

// Insert diagnosis names and synonyms into Trie and Hash Table.

for (int i = 0; i < data.diagnoses.size(); i++) {
const Diagnosis& d = data.diagnoses[i];

diagnosisTrie.insert(d.name, i);
diagnosisHash.insert(d.id, i);
diagnosisHash.insert(d.name, i);

vector<string> synonyms = splitSynonyms(d.synonyms);

for (string synonym : synonyms) {
diagnosisTrie.insert(synonym, i);
diagnosisHash.insert(synonym, i);
}
}

int choice;

do {
cout << "\n========== SPATIAL SEARCH & DIAGNOSTICS ==========\n";
cout << "1. Search Diagnosis by Prefix (Trie)\n";
cout << "2. Exact Diagnosis Lookup (Hash Table)\n";
cout << "3. Search Hospital Locations by Region (Quad-Tree)\n";
cout << "4. Patient Blood Pressure Range Query (Segment Tree)\n";
cout << "5. Return to Main Menu\n";
cout << "Enter choice: ";
cin >> choice;

if (choice == 1) {
string prefix;

cout << "\nEnter diagnosis prefix: ";
cin >> ws;
getline(cin, prefix);

vector<int> results = diagnosisTrie.searchPrefix(prefix);

if (results.empty()) {
cout << "\nNo matching diagnoses found.\n";
}
else {
cout << "\nMatching diagnoses:\n";

for (int index : results)
displayDiagnosis(data.diagnoses[index]);
}
}

else if (choice == 2) {
string key;

cout << "\nEnter diagnosis ID, name, or synonym: ";
cin >> ws;
getline(cin, key);

vector<int> results = diagnosisHash.search(key);

if (results.empty()) {
cout << "\nNo exact diagnosis match found.\n";
}
else {
for (int index : results)
displayDiagnosis(data.diagnoses[index]);
}
}

else if (choice == 3) {
if (data.locations.empty()) {
cout << "\nNo hospital locations available.\n";
continue;
}

double x1, x2, y1, y2;

cout << "\nEnter minimum X coordinate: ";
cin >> x1;

cout << "Enter maximum X coordinate: ";
cin >> x2;

cout << "Enter minimum Y coordinate: ";
cin >> y1;

cout << "Enter maximum Y coordinate: ";
cin >> y2;

vector<int> results = locationTree.search(x1, x2, y1, y2, data.locations);

if (results.empty()) {
cout << "\nNo locations found in this region.\n";
}
else {
cout << "\nLocations in the selected region:\n";

for (int index : results) {
const Location& location = data.locations[index];

cout << "\nID: " << location.id;
cout << "\nName: " << location.name;
cout << "\nDepartment: " << location.department;
cout << "\nCoordinates: (" << location.x << ", " << location.y << ")\n";
}
}
}

else if (choice == 4) {
if (data.patients.empty()) {
cout << "\nNo patient records available.\n";
continue;
}

cout << "\nPatient records are indexed from 1 to " << data.patients.size() << ".\n";

int left, right;

cout << "Enter starting patient index: ";
cin >> left;

cout << "Enter ending patient index: ";
cin >> right;

bpTree.rangeQuery(left, right, data.patients);
}

else if (choice == 5) {
cout << "\nReturning to main menu.\n";
}

else {
cout << "\nInvalid choice.\n";
}

} while (choice != 5);
}