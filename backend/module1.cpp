
#include "modules.h"
#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
using namespace std;

// ==========================================
// PATIENT QUEUE
// ==========================================

class PatientQueue {
struct Node {
int patientIndex;
Node* next;

Node(int index) {
patientIndex = index;
next = nullptr;
}
};

Node* front;
Node* rear;

public:
PatientQueue() {
front = rear = nullptr;
}

~PatientQueue() {
while (front != nullptr) {
Node* temp = front;
front = front->next;
delete temp;
}
}

bool empty() {
return front == nullptr;
}

void enqueue(int index) {
Node* node = new Node(index);

if (rear == nullptr) {
front = rear = node;
return;
}

rear->next = node;
rear = node;
}

void remove(int index) {
if (front == nullptr)
return;

if (front->patientIndex == index) {
Node* temp = front;
front = front->next;

if (front == nullptr)
rear = nullptr;

delete temp;
return;
}

Node* current = front;

while (current->next != nullptr && current->next->patientIndex != index)
current = current->next;

if (current->next != nullptr) {
Node* temp = current->next;
current->next = temp->next;

if (rear == temp)
rear = current;

delete temp;
}
}

void display(const vector<Patient>& patients) {
if (empty()) {
cout << "\nNo patients are waiting.\n";
return;
}

cout << "\n========== WAITING PATIENTS ==========\n";

Node* current = front;

while (current != nullptr) {
const Patient& p = patients[current->patientIndex];

cout << p.id << " | "
<< p.name << " | "
<< "Urgency: " << p.urgency << "/5\n";

current = current->next;
}
}
};

// ==========================================
// MAX HEAP - EMERGENCY PRIORITY
// ==========================================

class MaxHeap {
vector<int> heap;

void heapifyUp(int index, const vector<Patient>& patients) {
while (index > 0) {
int parent = (index - 1) / 2;

if (patients[heap[parent]].urgency >= patients[heap[index]].urgency)
break;

swap(heap[parent], heap[index]);
index = parent;
}
}

void heapifyDown(int index, const vector<Patient>& patients) {
int size = heap.size();

while (true) {
int largest = index;
int left = 2 * index + 1;
int right = 2 * index + 2;

if (left < size && patients[heap[left]].urgency > patients[heap[largest]].urgency)
largest = left;

if (right < size && patients[heap[right]].urgency > patients[heap[largest]].urgency)
largest = right;

if (largest == index)
break;

swap(heap[index], heap[largest]);
index = largest;
}
}

public:
void insert(int index, const vector<Patient>& patients) {
heap.push_back(index);
heapifyUp(heap.size() - 1, patients);
}

int extractMax(const vector<Patient>& patients) {
if (heap.empty())
return -1;

int result = heap[0];

heap[0] = heap.back();
heap.pop_back();

if (!heap.empty())
heapifyDown(0, patients);

return result;
}

bool empty() {
return heap.empty();
}

void display(const vector<Patient>& patients) {
if (heap.empty()) {
cout << "\nNo patients in the priority heap.\n";
return;
}

cout << "\n========== EMERGENCY PRIORITY HEAP ==========\n";

for (int index : heap) {
const Patient& p = patients[index];

cout << p.id << " | "
<< p.name << " | "
<< "Urgency: " << p.urgency << "/5\n";
}
}
};

// ==========================================
// PATIENT STACK - ASSESSMENT HISTORY
// ==========================================

class PatientStack {
vector<int> history;

public:
void push(int index) {
history.push_back(index);
}

void display(const vector<Patient>& patients) {
if (history.empty()) {
cout << "\nNo patients have been assessed yet.\n";
return;
}

cout << "\n========== ASSESSMENT HISTORY ==========\n";

for (int i = history.size() - 1; i >= 0; i--) {
const Patient& p = patients[history[i]];

cout << p.id << " | "
<< p.name << " | "
<< "Urgency: " << p.urgency << "/5\n";
}
}
};

// ==========================================
// AUTOMATIC PATIENT ID GENERATION
// ==========================================

string generatePatientID(const vector<Patient>& patients) {
int maxID = 0;

for (const Patient& p : patients) {
if (p.id.length() > 1 && p.id[0] == 'H') {
try {
int number = stoi(p.id.substr(1));
maxID = max(maxID, number);
}
catch (...) {
}
}
}

int nextID = maxID + 1;

return "H" + string(nextID < 10 ? "00" : nextID < 100 ? "0" : "") + to_string(nextID);
}

// ==========================================
// URGENCY CALCULATION
// ==========================================

int suggestUrgency(string symptoms) {
int urgency = 1;

if (symptoms.find("unconscious") != string::npos ||
symptoms.find("severe bleeding") != string::npos ||
symptoms.find("breathing difficulty") != string::npos)
urgency = 5;

else if (symptoms.find("chest pain") != string::npos ||
symptoms.find("serious open wound") != string::npos)
urgency = 4;

else if (symptoms.find("pain") != string::npos ||
symptoms.find("fever") != string::npos)
urgency = 2;

return urgency;
}

int confirmUrgency(int suggested) {
int choice;

cout << "\nSuggested urgency: " << suggested << "/5";
cout << "\nEnter 0 to accept or enter urgency from 1 to 5: ";
cin >> choice;

if (choice == 0)
return suggested;

if (choice >= 1 && choice <= 5)
return choice;

cout << "Invalid urgency. Using suggested urgency.\n";
return suggested;
}

// ==========================================
// REGISTER NEW PATIENT
// ==========================================

void registerPatient(HospitalData& data, PatientQueue& waiting, MaxHeap& heap) {
Patient p;

p.id = generatePatientID(data.patients);

cout << "\n========== NEW PATIENT REGISTRATION ==========\n";
cout << "Generated Patient ID: " << p.id << "\n";

cout << "Patient name: ";
cin >> ws;
getline(cin, p.name);

cout << "Age: ";
cin >> p.age;

cout << "Gender: ";
cin >> ws;
getline(cin, p.gender);

cout << "Blood group: ";
cin >> ws;
getline(cin, p.bloodGroup);

cout << "City: ";
cin >> ws;
getline(cin, p.city);

cout << "Symptoms: ";
cin >> ws;
getline(cin, p.symptoms);

cout << "Systolic BP: ";
cin >> p.systolicBP;

cout << "Diastolic BP: ";
cin >> p.diastolicBP;

cout << "Heart rate: ";
cin >> p.heartRate;

cout << "Temperature (Celsius): ";
cin >> p.temperature;

cout << "Oxygen saturation: ";
cin >> p.oxygenSaturation;

p.urgency = suggestUrgency(p.symptoms);
p.urgency = confirmUrgency(p.urgency);
p.source = "USER_REGISTERED";

// SAVE PATIENT BEFORE ADDING TO QUEUE

data.patients.push_back(p);

if (!savePatients(data)) {
cout << "\nERROR: Patient could not be saved.\n";
data.patients.pop_back();
return;
}

int index = data.patients.size() - 1;

waiting.enqueue(index);
heap.insert(index, data.patients);

cout << "\nPatient registered and saved successfully!\n";
cout << "Patient ID: " << p.id << "\n";
cout << "Urgency: " << p.urgency << "/5\n";
}

// ==========================================
// ASSESS NEXT PATIENT
// ==========================================

void assessNextPatient(HospitalData& data, PatientQueue& waiting, MaxHeap& heap, PatientStack& history) {
if (heap.empty()) {
cout << "\nNo patients are waiting for assessment.\n";
return;
}

int index = heap.extractMax(data.patients);

if (index < 0 || index >= data.patients.size()) {
cout << "\nInvalid patient record.\n";
return;
}

waiting.remove(index);

Patient& p = data.patients[index];

cout << "\n========== PATIENT ASSESSMENT ==========\n";
cout << "Patient ID: " << p.id;
cout << "\nName: " << p.name;
cout << "\nAge: " << p.age;
cout << "\nSymptoms: " << p.symptoms;
cout << "\nUrgency: " << p.urgency << "/5\n";

history.push(index);

cout << "\nPatient assessment completed.\n";
cout << "Patient record is already saved in the dataset.\n";
}

// ==========================================
// MODULE 1
// ==========================================

void runModule1(HospitalData& data) {
PatientQueue waiting;
MaxHeap heap;
PatientStack history;

int choice;

do {
cout << "\n========== ER INTAKE & TRIAGE ==========\n";
cout << "1. Register New Patient\n";
cout << "2. View Waiting Queue\n";
cout << "3. Assess Next Patient\n";
cout << "4. View Emergency Priority Heap\n";
cout << "5. Display Assessment History\n";
cout << "6. Return to Main Menu\n";
cout << "Enter choice: ";

cin >> choice;

if (cin.fail()) {
cin.clear();
cin.ignore(numeric_limits<streamsize>::max(), '\n');
cout << "\nInvalid input. Enter a number.\n";
continue;
}

cin.ignore(numeric_limits<streamsize>::max(), '\n');

if (choice == 1) {
registerPatient(data, waiting, heap);
}

else if (choice == 2) {
waiting.display(data.patients);
}

else if (choice == 3) {
assessNextPatient(data, waiting, heap, history);
}

else if (choice == 4) {
heap.display(data.patients);
}

else if (choice == 5) {
history.display(data.patients);
}

else if (choice == 6) {
cout << "\nReturning to main menu.\n";
}

else {
cout << "\nInvalid choice.\n";
}

} while (choice != 6);
}