
#include "data.h"
#include <stdexcept>
#include <filesystem>

// SPLIT A LINE USING |
vector<string> splitLine(string line) {
vector<string> fields;
stringstream ss(line);
string field;

while (getline(ss, field, '|')) {
fields.push_back(field);
}

return fields;
}

// READ DATASET ROWS
bool readRows(string filename, vector<vector<string>>& rows) {
ifstream file(filename);

if (!file.is_open())
return false;

string line;
getline(file, line);

while (getline(file, line)) {
if (line.empty())
continue;

rows.push_back(splitLine(line));
}

file.close();
return true;
}

// LOAD ALL DATASETS
bool loadData(HospitalData& data) {
vector<vector<string>> rows;

data.patients.clear();
data.doctors.clear();
data.ehr.clear();
data.diagnoses.clear();
data.locations.clear();
data.connections.clear();

// PATIENTS
if (!readRows("data/processed/patients.txt", rows))
return false;

try {
for (vector<string> r : rows) {
if (r.size() < 14)
continue;

Patient p;
p.id = r[0];
p.name = r[1];
p.age = stoi(r[2]);
p.gender = r[3];
p.bloodGroup = r[4];
p.city = r[5];
p.symptoms = r[6];
p.systolicBP = stoi(r[7]);
p.diastolicBP = stoi(r[8]);
p.heartRate = stoi(r[9]);
p.temperature = stod(r[10]);
p.oxygenSaturation = stod(r[11]);
p.urgency = stoi(r[12]);
p.source = r[13];

data.patients.push_back(p);
}
}
catch (...) {
cout << "Error reading patients dataset.\n";
return false;
}

// DOCTORS
rows.clear();

if (!readRows("data/processed/doctors.txt", rows))
return false;

for (vector<string> r : rows) {
if (r.size() < 5)
continue;

Doctor d;
d.id = r[0];
d.name = r[1];
d.specialization = r[2];
d.role = r[3];
d.source = r[4];

data.doctors.push_back(d);
}

// EHR
rows.clear();

if (!readRows("data/processed/ehr.txt", rows))
return false;

for (vector<string> r : rows) {
if (r.size() < 11)
continue;

EHR e;
e.id = r[0];
e.patientID = r[1];
e.doctorID = r[2];
e.diagnosisID = r[3];
e.visitDate = r[4];
e.symptoms = r[5];
e.diagnosisNotes = r[6];
e.treatment = r[7];
e.outcome = r[8];
e.episodeReference = r[9];
e.source = r[10];

data.ehr.push_back(e);
}

// DIAGNOSTICS
rows.clear();

if (!readRows("data/processed/diagnostics.txt", rows))
return false;

for (vector<string> r : rows) {
if (r.size() < 5)
continue;

Diagnosis d;
d.id = r[0];
d.name = r[1];
d.category = r[2];
d.description = r[3];
d.synonyms = r[4];

data.diagnoses.push_back(d);
}

// LOCATIONS
rows.clear();

if (!readRows("data/processed/locations.txt", rows))
return false;

try {
for (vector<string> r : rows) {
if (r.size() < 5)
continue;

Location l;
l.id = r[0];
l.name = r[1];
l.department = r[2];
l.x = stod(r[3]);
l.y = stod(r[4]);

data.locations.push_back(l);
}
}
catch (...) {
cout << "Error reading locations dataset.\n";
return false;
}

// CONNECTIONS
rows.clear();

if (!readRows("data/processed/connections.txt", rows))
return false;

try {
for (vector<string> r : rows) {
if (r.size() < 4)
continue;

Connection c;
c.id = r[0];
c.fromID = r[1];
c.toID = r[2];
c.distance = stod(r[3]);

data.connections.push_back(c);
}
}
catch (...) {
cout << "Error reading connections dataset.\n";
return false;
}

cout << "All six datasets loaded successfully.\n";
return true;
}

// SAVE PATIENTS
bool savePatients(const HospitalData& data) {
namespace fs = std::filesystem;

fs::path folder = fs::current_path() / "data" / "processed";

if (!fs::exists(folder))
folder = fs::current_path().parent_path() / "data" / "processed";

if (!fs::exists(folder))
return false;

fs::path path = folder / "patients.txt";

ofstream file(path);

if (!file.is_open())
return false;

file << "patient_id|patient_name|age|gender|blood_group|city|symptoms|systolic_bp|diastolic_bp|heart_rate|temperature|oxygen_saturation|urgency|data_source\n";

for (const Patient& p : data.patients) {
file << p.id << "|"
<< p.name << "|"
<< p.age << "|"
<< p.gender << "|"
<< p.bloodGroup << "|"
<< p.city << "|"
<< p.symptoms << "|"
<< p.systolicBP << "|"
<< p.diastolicBP << "|"
<< p.heartRate << "|"
<< p.temperature << "|"
<< p.oxygenSaturation << "|"
<< p.urgency << "|"
<< p.source << "\n";
}

file.flush();

if (!file.good())
return false;

file.close();

cout << "Saved patient records to: " << fs::absolute(path) << endl;

return true;
}

// SAVE DOCTORS
bool saveDoctors(const HospitalData& data) {
ofstream file("data/processed/doctors.txt");

if (!file.is_open())
return false;

file << "doctor_id|doctor_name|specialization|role|data_source\n";

for (const Doctor& d : data.doctors) {
file << d.id << "|"
<< d.name << "|"
<< d.specialization << "|"
<< d.role << "|"
<< d.source << "\n";
}

return true;
}

// SAVE EHR
bool saveEHR(const HospitalData& data) {
ofstream file("data/processed/ehr.txt");

if (!file.is_open())
return false;

file << "ehr_id|patient_id|doctor_id|diagnosis_id|visit_date|symptoms|diagnosis_notes|treatment|outcome|episode_reference|data_source\n";

for (const EHR& e : data.ehr) {
file << e.id << "|"
<< e.patientID << "|"
<< e.doctorID << "|"
<< e.diagnosisID << "|"
<< e.visitDate << "|"
<< e.symptoms << "|"
<< e.diagnosisNotes << "|"
<< e.treatment << "|"
<< e.outcome << "|"
<< e.episodeReference << "|"
<< e.source << "\n";
}

return true;
}

// SAVE DIAGNOSTICS
bool saveDiagnostics(const HospitalData& data) {
ofstream file("data/processed/diagnostics.txt");

if (!file.is_open())
return false;

file << "id|name|category|description|synonyms\n";

for (const Diagnosis& d : data.diagnoses) {
file << d.id << "|"
<< d.name << "|"
<< d.category << "|"
<< d.description << "|"
<< d.synonyms << "\n";
}

return true;
}

// SAVE LOCATIONS
bool saveLocations(const HospitalData& data) {
ofstream file("data/processed/locations.txt");

if (!file.is_open())
return false;

file << "id|name|department|x|y\n";

for (const Location& l : data.locations) {
file << l.id << "|"
<< l.name << "|"
<< l.department << "|"
<< l.x << "|"
<< l.y << "\n";
}

return true;
}

// SAVE CONNECTIONS
bool saveConnections(const HospitalData& data) {
ofstream file("data/processed/connections.txt");

if (!file.is_open())
return false;

file << "id|fromID|toID|distance\n";

for (const Connection& c : data.connections) {
file << c.id << "|"
<< c.fromID << "|"
<< c.toID << "|"
<< c.distance << "\n";
}

return true;
}