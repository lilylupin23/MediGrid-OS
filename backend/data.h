
#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct Patient {
string id, name, gender, bloodGroup, city, symptoms, source;
int age, systolicBP, diastolicBP, heartRate, urgency;
double temperature, oxygenSaturation;
};

struct Doctor {
string id, name, specialization, role, source;
};

struct EHR {
string id, patientID, doctorID, diagnosisID;
string visitDate, symptoms, diagnosisNotes;
string treatment, outcome, episodeReference, source;
};

struct Diagnosis {
string id, name, category, description, synonyms;
};

struct Location {
string id, name, department;
double x, y;
};

struct Connection {
string id, fromID, toID;
double distance;
};

struct HospitalData {
vector<Patient> patients;
vector<Doctor> doctors;
vector<EHR> ehr;
vector<Diagnosis> diagnoses;
vector<Location> locations;
vector<Connection> connections;
};

bool loadData(HospitalData& data);

bool savePatients(const HospitalData& data);
bool saveDoctors(const HospitalData& data);
bool saveEHR(const HospitalData& data);
bool saveDiagnostics(const HospitalData& data);
bool saveLocations(const HospitalData& data);
bool saveConnections(const HospitalData& data);

#endif