
#include "modules.h"
#include <algorithm>

struct RiskRecord {
Patient patient;
int score;
string category;
};

// ==========================================
// RISK CALCULATION
// ==========================================

int calculateRisk(Patient p) {
int score = 0;

if (p.age >= 65)
score += 2;

if (p.systolicBP >= 140)
score += 2;

if (p.heartRate >= 100)
score += 2;

if (p.oxygenSaturation < 94)
score += 4;

if (p.temperature >= 38)
score += 2;

return score;
}

string getRiskCategory(int score) {
if (score >= 6)
return "High";

if (score >= 3)
return "Moderate";

return "Low";
}

// ==========================================
// CREATE RISK RECORDS
// ==========================================

vector<RiskRecord> createRiskRecords(HospitalData& data) {
vector<RiskRecord> records;

for (Patient p : data.patients) {
RiskRecord r;
r.patient = p;
r.score = calculateRisk(p);
r.category = getRiskCategory(r.score);
records.push_back(r);
}

return records;
}

// ==========================================
// DISPLAY RECORDS
// ==========================================

void displayRecords(vector<RiskRecord> records) {
if (records.empty()) {
cout << "No patient records available.\n";
return;
}

cout << "\nPATIENT RISK ANALYSIS\n";
cout << "---------------------------------------------\n";

for (RiskRecord r : records) {
cout << r.patient.id << " | "
<< r.patient.name
<< " | Score: " << r.score
<< " | Category: " << r.category << endl;
}
}

// ==========================================
// QUICKSELECT
// ==========================================

int partition(vector<RiskRecord>& records, int low, int high) {
int pivot = records[high].score;
int i = low;

for (int j = low; j < high; j++) {
if (records[j].score >= pivot) {
swap(records[i], records[j]);
i++;
}
}

swap(records[i], records[high]);
return i;
}

RiskRecord quickSelect(vector<RiskRecord>& records, int low, int high, int k) {
while (low <= high) {
int pivotIndex = partition(records, low, high);

if (pivotIndex == k)
return records[pivotIndex];

if (pivotIndex < k)
low = pivotIndex + 1;
else
high = pivotIndex - 1;
}

return records[0];
}

// ==========================================
// MERGE SORT
// ==========================================

void merge(vector<RiskRecord>& records, int left, int mid, int right) {
vector<RiskRecord> temp;

int i = left;
int j = mid + 1;

while (i <= mid && j <= right) {
if (records[i].score >= records[j].score)
temp.push_back(records[i++]);
else
temp.push_back(records[j++]);
}

while (i <= mid)
temp.push_back(records[i++]);

while (j <= right)
temp.push_back(records[j++]);

for (int k = 0; k < temp.size(); k++)
records[left + k] = temp[k];
}

void mergeSort(vector<RiskRecord>& records, int left, int right) {
if (left >= right)
return;

int mid = left + (right - left) / 2;

mergeSort(records, left, mid);
mergeSort(records, mid + 1, right);
merge(records, left, mid, right);
}

// ==========================================
// BUCKET SORT - GROUP BY RISK SCORE
// ==========================================

void bucketSort(vector<RiskRecord>& records) {
vector<vector<RiskRecord>> buckets(13);

for (const RiskRecord& record : records) {
if (record.score >= 0 && record.score <= 12)
buckets[record.score].push_back(record);
}

cout << "\n========== BUCKET SORT: RISK SCORE GROUPS ==========\n";

for (int score = 12; score >= 0; score--) {
if (buckets[score].empty())
continue;

cout << "\n---------------------------------------------\n";
cout << "RISK SCORE: " << score;
cout << " | CATEGORY: " << getRiskCategory(score) << "\n";
cout << "---------------------------------------------\n";

for (const RiskRecord& record : buckets[score]) {
cout << "Patient ID: " << record.patient.id
<< " | Name: " << record.patient.name
<< " | Category: " << record.category << "\n";
}
}

cout << "\n========== END OF BUCKET GROUPS ==========\n";
}

// ==========================================
// MODULE 2
// ==========================================

void runModule2(HospitalData& data) {
int choice;

vector<RiskRecord> records = createRiskRecords(data);

if (records.empty()) {
cout << "\nNo patient records available.\n";
return;
}

do {
cout << "\n========== CLINICAL ANALYTICS ==========\n";
cout << "1. Calculate patient risk scores\n";
cout << "2. Find Kth highest-risk patient (Quickselect)\n";
cout << "3. Sort patients by risk (Merge Sort)\n";
cout << "4. Group patients by risk score (Bucket Sort)\n";
cout << "5. Return to main menu\n";

cout << "\nEnter choice: ";
cin >> choice;

switch (choice) {

case 1:
displayRecords(records);
break;

case 2: {
int k;

cout << "Enter K: ";
cin >> k;

if (k < 1 || k > records.size()) {
cout << "Invalid K value.\n";
break;
}

vector<RiskRecord> temp = records;

RiskRecord result = quickSelect(temp, 0, temp.size() - 1, k - 1);

cout << "\n" << k << "th HIGHEST-RISK PATIENT\n";
cout << result.patient.id << " | "
<< result.patient.name
<< " | Risk Score: " << result.score
<< " | Category: " << result.category << endl;

break;
}

case 3: {
vector<RiskRecord> temp = records;

mergeSort(temp, 0, temp.size() - 1);

cout << "\nPATIENTS SORTED BY RISK (MERGE SORT)\n";
displayRecords(temp);

break;
}

case 4: {
vector<RiskRecord> temp = records;

bucketSort(temp);

break;
}

case 5:
cout << "Returning to main menu.\n";
break;

default:
cout << "Invalid choice.\n";
}

} while (choice != 5);
}