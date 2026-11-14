
#include "modules.h"
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <limits>

// ==========================================
// AUTOMATIC DOCTOR ID GENERATION
// ==========================================

string generateDoctorID(const vector<Doctor>& doctors) {
int maxID = 0;

for (const Doctor& doctor : doctors) {
if (doctor.id.length() > 1 && doctor.id[0] == 'D') {
try {
int number = stoi(doctor.id.substr(1));

if (number > maxID)
maxID = number;
}
catch (...) {
}
}
}

stringstream ss;
ss << "D" << setfill('0') << setw(3) << maxID + 1;

return ss.str();
}

// ==========================================
// AVL TREE - DOCTOR REGISTRY
// ==========================================

class AVLTree {
private:
struct Node {
Doctor doctor;
Node* left;
Node* right;
int height;

Node(Doctor d) {
doctor = d;
left = nullptr;
right = nullptr;
height = 1;
}
};

Node* root = nullptr;

int getHeight(Node* node) {
if (node == nullptr)
return 0;
return node->height;
}

int getBalance(Node* node) {
if (node == nullptr)
return 0;
return getHeight(node->left) - getHeight(node->right);
}

void updateHeight(Node* node) {
node->height = 1 + max(getHeight(node->left), getHeight(node->right));
}

Node* rotateRight(Node* y) {
Node* x = y->left;
Node* temp = x->right;

x->right = y;
y->left = temp;

updateHeight(y);
updateHeight(x);

return x;
}

Node* rotateLeft(Node* x) {
Node* y = x->right;
Node* temp = y->left;

y->left = x;
x->right = temp;

updateHeight(x);
updateHeight(y);

return y;
}

Node* insertNode(Node* node, Doctor doctor) {
if (node == nullptr)
return new Node(doctor);

if (doctor.id < node->doctor.id)
node->left = insertNode(node->left, doctor);
else if (doctor.id > node->doctor.id)
node->right = insertNode(node->right, doctor);
else
return node;

updateHeight(node);

int balance = getBalance(node);

if (balance > 1 && doctor.id < node->left->doctor.id)
return rotateRight(node);

if (balance < -1 && doctor.id > node->right->doctor.id)
return rotateLeft(node);

if (balance > 1 && doctor.id > node->left->doctor.id) {
node->left = rotateLeft(node->left);
return rotateRight(node);
}

if (balance < -1 && doctor.id < node->right->doctor.id) {
node->right = rotateRight(node->right);
return rotateLeft(node);
}

return node;
}

Node* minimumNode(Node* node) {
Node* current = node;

while (current->left != nullptr)
current = current->left;

return current;
}

Node* deleteNode(Node* node, string id) {
if (node == nullptr)
return node;

if (id < node->doctor.id)
node->left = deleteNode(node->left, id);
else if (id > node->doctor.id)
node->right = deleteNode(node->right, id);
else {
if (node->left == nullptr || node->right == nullptr) {
Node* temp = node->left ? node->left : node->right;

if (temp == nullptr) {
delete node;
return nullptr;
}

*node = *temp;
delete temp;
}
else {
Node* temp = minimumNode(node->right);
node->doctor = temp->doctor;
node->right = deleteNode(node->right, temp->doctor.id);
}
}

updateHeight(node);

int balance = getBalance(node);

if (balance > 1 && getBalance(node->left) >= 0)
return rotateRight(node);

if (balance > 1 && getBalance(node->left) < 0) {
node->left = rotateLeft(node->left);
return rotateRight(node);
}

if (balance < -1 && getBalance(node->right) <= 0)
return rotateLeft(node);

if (balance < -1 && getBalance(node->right) > 0) {
node->right = rotateRight(node->right);
return rotateLeft(node);
}

return node;
}

Node* searchNode(Node* node, string id) {
if (node == nullptr || node->doctor.id == id)
return node;

if (id < node->doctor.id)
return searchNode(node->left, id);

return searchNode(node->right, id);
}

void inorder(Node* node) {
if (node == nullptr)
return;

inorder(node->left);

cout << node->doctor.id << " | "
<< node->doctor.name << " | "
<< node->doctor.specialization << " | "
<< node->doctor.role << endl;

inorder(node->right);
}

void destroy(Node* node) {
if (node == nullptr)
return;

destroy(node->left);
destroy(node->right);
delete node;
}

public:
AVLTree() {
root = nullptr;
}

~AVLTree() {
destroy(root);
}

void insert(Doctor doctor) {
root = insertNode(root, doctor);
}

void remove(string id) {
root = deleteNode(root, id);
}

bool search(string id, Doctor& result) {
Node* node = searchNode(root, id);

if (node == nullptr)
return false;

result = node->doctor;
return true;
}

void display() {
if (root == nullptr) {
cout << "No doctors registered.\n";
return;
}

cout << "\nDOCTOR REGISTRY (SORTED BY ID)\n";
inorder(root);
}
};

// ==========================================
// THREADED BINARY TREE - EHR CHRONOLOGY
// ==========================================

class ThreadedTree {
private:
struct Node {
EHR record;
Node* left;
Node* right;
bool leftThread;
bool rightThread;

Node(EHR e) {
record = e;
left = nullptr;
right = nullptr;
leftThread = true;
rightThread = true;
}
};

Node* root = nullptr;

string getKey(EHR e) {
return e.visitDate + "|" + e.id;
}

void destroy(Node* node) {
if (node == nullptr)
return;

if (!node->leftThread)
destroy(node->left);

if (!node->rightThread)
destroy(node->right);

delete node;
}

public:
ThreadedTree() {
root = nullptr;
}

void insert(EHR record) {
Node* newNode = new Node(record);

if (root == nullptr) {
root = newNode;
return;
}

Node* current = root;

while (true) {
if (getKey(record) < getKey(current->record)) {
if (!current->leftThread) {
current = current->left;
}
else {
newNode->left = current->left;
newNode->right = current;
current->left = newNode;
current->leftThread = false;
return;
}
}
else {
if (!current->rightThread) {
current = current->right;
}
else {
newNode->right = current->right;
newNode->left = current;
current->right = newNode;
current->rightThread = false;
return;
}
}
}
}

void display() {
if (root == nullptr) {
cout << "No EHR records available.\n";
return;
}

cout << "\nEHR RECORDS IN CHRONOLOGICAL ORDER\n";

Node* current = root;

while (!current->leftThread)
current = current->left;

while (current != nullptr) {
cout << current->record.id << " | "
<< current->record.patientID << " | "
<< current->record.doctorID << " | "
<< current->record.visitDate << " | "
<< current->record.diagnosisNotes << endl;

if (current->rightThread)
current = current->right;
else {
current = current->right;

while (current != nullptr && !current->leftThread)
current = current->left;
}
}
}

~ThreadedTree() {
destroy(root);
}
};

// ==========================================
// B+ TREE - PATIENT EHR INDEX
// ==========================================

class BPlusTree {
private:
static const int MAX_KEYS = 3;

struct Node {
bool leaf;
vector<string> keys;
vector<vector<string>> values;
vector<Node*> children;
Node* next;

Node(bool isLeaf) {
leaf = isLeaf;
next = nullptr;
}
};

struct Split {
string key;
Node* right;
};

Node* root = nullptr;

Node* findLeaf(string key) {
Node* current = root;

while (current != nullptr && !current->leaf) {
int i = 0;

while (i < current->keys.size() && key >= current->keys[i])
i++;

current = current->children[i];
}

return current;
}

Split insertRecursive(Node* node, string key, string ehrID) {
if (node->leaf) {
int i = 0;

while (i < node->keys.size() && node->keys[i] < key)
i++;

if (i < node->keys.size() && node->keys[i] == key) {
if (find(node->values[i].begin(), node->values[i].end(), ehrID) == node->values[i].end())
node->values[i].push_back(ehrID);

return {"", nullptr};
}

node->keys.insert(node->keys.begin() + i, key);
node->values.insert(node->values.begin() + i, vector<string>{ehrID});

if (node->keys.size() <= MAX_KEYS)
return {"", nullptr};

Node* right = new Node(true);
int mid = node->keys.size() / 2;

right->keys.assign(node->keys.begin() + mid, node->keys.end());
right->values.assign(node->values.begin() + mid, node->values.end());

node->keys.resize(mid);
node->values.resize(mid);

right->next = node->next;
node->next = right;

return {right->keys[0], right};
}

int i = 0;

while (i < node->keys.size() && key >= node->keys[i])
i++;

Split result = insertRecursive(node->children[i], key, ehrID);

if (result.right == nullptr)
return {"", nullptr};

node->keys.insert(node->keys.begin() + i, result.key);
node->children.insert(node->children.begin() + i + 1, result.right);

if (node->keys.size() <= MAX_KEYS)
return {"", nullptr};

Node* right = new Node(false);
int mid = node->keys.size() / 2;

string promoted = node->keys[mid];

right->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
right->children.assign(node->children.begin() + mid + 1, node->children.end());

node->keys.resize(mid);
node->children.resize(mid + 1);

return {promoted, right};
}

void destroy(Node* node) {
if (node == nullptr)
return;

for (Node* child : node->children)
destroy(child);

delete node;
}

public:
BPlusTree() {
root = new Node(true);
}

~BPlusTree() {
destroy(root);
}

void insert(string patientID, string ehrID) {
Split result = insertRecursive(root, patientID, ehrID);

if (result.right != nullptr) {
Node* newRoot = new Node(false);

newRoot->keys.push_back(result.key);
newRoot->children.push_back(root);
newRoot->children.push_back(result.right);

root = newRoot;
}
}

vector<string> search(string patientID) {
Node* leaf = findLeaf(patientID);

if (leaf == nullptr)
return {};

for (int i = 0; i < leaf->keys.size(); i++) {
if (leaf->keys[i] == patientID)
return leaf->values[i];
}

return {};
}

void display() {
Node* current = root;

while (!current->leaf)
current = current->children[0];

cout << "\nB+ TREE PATIENT INDEX\n";

while (current != nullptr) {
for (int i = 0; i < current->keys.size(); i++) {
cout << current->keys[i] << " : ";

for (string id : current->values[i])
cout << id << " ";

cout << endl;
}

current = current->next;
}
}
};

// ==========================================
// HELPER FUNCTIONS
// ==========================================

bool doctorExists(HospitalData& data, string id) {
for (const Doctor& d : data.doctors) {
if (d.id == id)
return true;
}

return false;
}

bool patientExists(HospitalData& data, string id) {
for (const Patient& p : data.patients) {
if (p.id == id)
return true;
}

return false;
}

bool diagnosisExists(HospitalData& data, string id) {
for (const Diagnosis& d : data.diagnoses) {
if (d.id == id)
return true;
}

return false;
}

void showDoctor(Doctor d) {
cout << "\nDoctor ID: " << d.id << endl;
cout << "Name: " << d.name << endl;
cout << "Specialization: " << d.specialization << endl;
cout << "Role: " << d.role << endl;
}

void showEHR(EHR e) {
cout << "\nEHR ID: " << e.id << endl;
cout << "Patient ID: " << e.patientID << endl;
cout << "Doctor ID: " << e.doctorID << endl;
cout << "Diagnosis ID: " << e.diagnosisID << endl;
cout << "Visit Date: " << e.visitDate << endl;
cout << "Symptoms: " << e.symptoms << endl;
cout << "Diagnosis Notes: " << e.diagnosisNotes << endl;
cout << "Treatment: " << e.treatment << endl;
cout << "Outcome: " << e.outcome << endl;
cout << "Episode Reference: " << e.episodeReference << endl;
}

// ==========================================
// MODULE 3
// ==========================================

void runModule3(HospitalData& data) {
AVLTree doctorTree;
ThreadedTree ehrTree;
BPlusTree ehrIndex;

// BUILD TREES FROM DATASETS

for (const Doctor& d : data.doctors)
doctorTree.insert(d);

for (const EHR& e : data.ehr) {
ehrTree.insert(e);
ehrIndex.insert(e.patientID, e.id);
}

int choice;

do {
cout << "\n========== DOCTOR REGISTRY & EHR ==========\n";
cout << "1. Register Doctor\n";
cout << "2. Search Doctor\n";
cout << "3. Update Doctor\n";
cout << "4. Delete Doctor\n";
cout << "5. Display Doctor Registry\n";
cout << "6. Add EHR Record\n";
cout << "7. Search Patient EHR History\n";
cout << "8. Display EHR Chronologically\n";
cout << "9. Display B+ Tree Patient Index\n";
cout << "10. Return to Main Menu\n";
cout << "Enter choice: ";

cin >> choice;
cin.ignore(numeric_limits<streamsize>::max(), '\n');

// REGISTER DOCTOR

if (choice == 1) {
Doctor d;

cout << "\nDOCTOR REGISTRATION\n";

d.id = generateDoctorID(data.doctors);
cout << "Automatically generated Doctor ID: " << d.id << endl;

cout << "Doctor Name: ";
getline(cin, d.name);

cout << "Specialization: ";
getline(cin, d.specialization);

cout << "Role: ";
getline(cin, d.role);

d.source = "FRONTEND";

data.doctors.push_back(d);

if (saveDoctors(data)) {
doctorTree.insert(d);
cout << "Doctor registered successfully.\n";
cout << "Doctor ID: " << d.id << endl;
}
else {
data.doctors.pop_back();
cout << "ERROR: Doctor could not be saved. Registration cancelled.\n";
}
}

// SEARCH DOCTOR

else if (choice == 2) {
string id;
Doctor d;

cout << "Enter Doctor ID: ";
getline(cin, id);

if (doctorTree.search(id, d))
showDoctor(d);
else
cout << "Doctor not found.\n";
}

// UPDATE DOCTOR

else if (choice == 3) {
string id;
Doctor d;

cout << "Enter Doctor ID to update: ";
getline(cin, id);

if (!doctorTree.search(id, d)) {
cout << "Doctor not found.\n";
continue;
}

showDoctor(d);

cout << "\nEnter updated name: ";
getline(cin, d.name);

cout << "Enter updated specialization: ";
getline(cin, d.specialization);

cout << "Enter updated role: ";
getline(cin, d.role);

doctorTree.remove(id);
doctorTree.insert(d);

for (Doctor& existing : data.doctors) {
if (existing.id == id) {
existing = d;
break;
}
}

if (saveDoctors(data))
cout << "Doctor details updated successfully.\n";
else
cout << "Warning: Could not save doctor dataset.\n";
}

// DELETE DOCTOR

else if (choice == 4) {
string id;

cout << "Enter Doctor ID to delete: ";
getline(cin, id);

if (!doctorExists(data, id)) {
cout << "Doctor not found.\n";
continue;
}

bool assigned = false;

for (const EHR& e : data.ehr) {
if (e.doctorID == id) {
assigned = true;
break;
}
}

if (assigned) {
cout << "Cannot delete doctor: EHR records are linked to this doctor.\n";
continue;
}

doctorTree.remove(id);

data.doctors.erase(
remove_if(data.doctors.begin(), data.doctors.end(),
[id](const Doctor& d) {
return d.id == id;
}),
data.doctors.end()
);

if (saveDoctors(data))
cout << "Doctor deleted successfully.\n";
else
cout << "Warning: Could not save doctor dataset.\n";
}

// DISPLAY DOCTORS

else if (choice == 5) {
doctorTree.display();
}

// ADD EHR

else if (choice == 6) {
EHR e;

cout << "\nADD ELECTRONIC HEALTH RECORD\n";

int nextID = 1;

for (const EHR& existing : data.ehr) {
if (existing.id.rfind("HEHR", 0) == 0) {
try {
int number = stoi(existing.id.substr(4));
nextID = max(nextID, number + 1);
}
catch (...) {
}
}
}

stringstream ehrIDStream;
ehrIDStream << "HEHR" << setfill('0') << setw(3) << nextID;
e.id = ehrIDStream.str();

cout << "Patient ID: ";
getline(cin, e.patientID);

if (!patientExists(data, e.patientID)) {
cout << "Patient does not exist.\n";
continue;
}

cout << "Doctor ID: ";
getline(cin, e.doctorID);

if (!doctorExists(data, e.doctorID)) {
cout << "Doctor does not exist.\n";
continue;
}

cout << "Diagnosis ID: ";
getline(cin, e.diagnosisID);

if (!e.diagnosisID.empty() && !diagnosisExists(data, e.diagnosisID)) {
cout << "Diagnosis ID does not exist.\n";
continue;
}

cout << "Visit Date (YYYY-MM-DD): ";
getline(cin, e.visitDate);

cout << "Symptoms: ";
getline(cin, e.symptoms);

cout << "Diagnosis Notes: ";
getline(cin, e.diagnosisNotes);

cout << "Treatment: ";
getline(cin, e.treatment);

cout << "Outcome: ";
getline(cin, e.outcome);

cout << "Episode Reference: ";
getline(cin, e.episodeReference);

e.source = "FRONTEND";

data.ehr.push_back(e);
ehrTree.insert(e);
ehrIndex.insert(e.patientID, e.id);

if (saveEHR(data))
cout << "\nEHR record added and dataset updated.\n";
else
cout << "\nWarning: EHR added in memory, but dataset could not be saved.\n";

cout << "Generated EHR ID: " << e.id << endl;
}

// SEARCH PATIENT EHR HISTORY

else if (choice == 7) {
string patientID;

cout << "Enter Patient ID: ";
getline(cin, patientID);

vector<string> ids = ehrIndex.search(patientID);

if (ids.empty()) {
cout << "No EHR records found for this patient.\n";
continue;
}

cout << "\nPATIENT MEDICAL HISTORY\n";

for (string id : ids) {
for (const EHR& e : data.ehr) {
if (e.id == id) {
showEHR(e);
break;
}
}
}
}

// CHRONOLOGICAL EHR DISPLAY

else if (choice == 8) {
ehrTree.display();
}

// DISPLAY B+ TREE INDEX

else if (choice == 9) {
ehrIndex.display();
}

// RETURN

else if (choice == 10) {
cout << "Returning to main menu.\n";
}

// INVALID

else {
cout << "Invalid choice.\n";
}

} while (choice != 10);
}
