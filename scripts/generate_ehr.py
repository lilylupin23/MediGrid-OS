
import csv
import random
from pathlib import Path
from datetime import date, timedelta

random.seed(42)

BASE = Path(__file__).resolve().parent.parent
DATA = BASE / "data" / "raw"

PATIENT_FILE = DATA / "patients.txt"
DOCTOR_FILE = DATA / "doctors.txt"
DIAGNOSIS_FILE = DATA / "diagnostics.txt"
EHR_FILE = DATA / "ehr.txt"

def read_table(path):
    with open(path, "r", encoding="utf-8", newline="") as file:
        reader = csv.DictReader(file, delimiter="|")
        return reader.fieldnames or [], list(reader)

def write_table(path, fields, rows):
    with open(path, "w", encoding="utf-8", newline="") as file:
        writer = csv.DictWriter(file, fieldnames=fields, delimiter="|")
        writer.writeheader()
        writer.writerows(rows)

# Episode-inspired seed cases.
# Demographics, vitals and additional clinical details are simulated.

cases = [
    {
        "name": "Rebecca Adler",
        "episode": "S1E01 - Pilot",
        "age_range": (30, 45),
        "gender": "Female",
        "doctor": "D001",
        "diagnosis": "D061",
        "symptoms": "Speech difficulties and collapse"
    },
    {
        "name": "Dan",
        "episode": "S1E02 - Paternity",
        "age_range": (15, 18),
        "gender": "Male",
        "doctor": "D004",
        "diagnosis": "D062",
        "symptoms": "Night terrors and hallucinations"
    },
    {
        "name": "Brandon Merrell",
        "episode": "S1E03 - Occam's Razor",
        "age_range": (17, 25),
        "gender": "Male",
        "doctor": "D001",
        "diagnosis": "D063",
        "symptoms": "Acute illness with worsening symptoms"
    },
    {
        "name": "Mary Augustine",
        "episode": "S1E05 - Damned If You Do",
        "age_range": (30, 55),
        "gender": "Female",
        "doctor": "D005",
        "diagnosis": "D064",
        "symptoms": "Severe skin rash and allergic reactions"
    },
    {
        "name": "Elise Snow",
        "episode": "S1E07 - Fidelity",
        "age_range": (30, 50),
        "gender": "Female",
        "doctor": "D001",
        "diagnosis": "D065",
        "symptoms": "Progressive neurological symptoms"
    },
    {
        "name": "Matt Davis",
        "episode": "S1E08 - Poison",
        "age_range": (15, 18),
        "gender": "Male",
        "doctor": "D004",
        "diagnosis": "D066",
        "symptoms": "Hallucinations and collapse"
    },
    {
        "name": "John Henry Giles",
        "episode": "S1E09 - DNR",
        "age_range": (50, 70),
        "gender": "Male",
        "doctor": "D001",
        "diagnosis": "D067",
        "symptoms": "Progressive paralysis"
    },
    {
        "name": "Victoria Madsen",
        "episode": "S1E10 - Histories",
        "age_range": (35, 60),
        "gender": "Female",
        "doctor": "D001",
        "diagnosis": "D068",
        "symptoms": "Seizures and neurological deterioration"
    },
    {
        "name": "Keith Foster",
        "episode": "S1E11 - Detox",
        "age_range": (30, 60),
        "gender": "Male",
        "doctor": "D006",
        "diagnosis": "D069",
        "symptoms": "Progressive systemic illness"
    },
    {
        "name": "Hank Wiggen",
        "episode": "S1E12 - Sports Medicine",
        "age_range": (25, 45),
        "gender": "Male",
        "doctor": "D006",
        "diagnosis": "D070",
        "symptoms": "Weakness and unexplained illness"
    },
    {
        "name": "Gabriel Reilich",
        "episode": "S1E13 - Cursed",
        "age_range": (10, 15),
        "gender": "Male",
        "doctor": "D001",
        "diagnosis": "D071",
        "symptoms": "Skin changes and progressive illness"
    },
    {
        "name": "Carly Forlano",
        "episode": "S1E14 - Control",
        "age_range": (20, 35),
        "gender": "Female",
        "doctor": "D001",
        "diagnosis": "D004",
        "symptoms": "Breathlessness and signs of heart failure"
    },
    {
        "name": "Joey Arnello",
        "episode": "S1E15 - Mob Rules",
        "age_range": (25, 45),
        "gender": "Male",
        "doctor": "D001",
        "diagnosis": "D072",
        "symptoms": "Acute neurological and metabolic symptoms"
    },
    {
        "name": "Jessica Simms",
        "episode": "S1E16 - Heavy",
        "age_range": (25, 45),
        "gender": "Female",
        "doctor": "D001",
        "diagnosis": "D024",
        "symptoms": "Weight changes and related systemic symptoms"
    },
    {
        "name": "Gary H. Wright",
        "episode": "S1E17 - Role Model",
        "age_range": (35, 60),
        "gender": "Male",
        "doctor": "D001",
        "diagnosis": "D073",
        "symptoms": "Recurrent infections and persistent illness"
    }
]

# Additional diagnoses required by these episode cases.

new_diagnoses = [
    ["D061", "Neurocysticercosis", "Neurological", "Parasitic infection affecting the central nervous system", "Cysticercosis"],
    ["D062", "Subacute Sclerosing Panencephalitis", "Neurological", "Rare progressive neurological disorder associated with measles infection", "SSPE"],
    ["D063", "Colchicine Poisoning", "Emergency Medicine", "Toxic reaction resulting from colchicine exposure", "Colchicine toxicity"],
    ["D064", "Copper IUD Related Allergic Reaction", "Immunology", "Allergic or inflammatory reaction associated with an intrauterine device", "IUD reaction"],
    ["D065", "African Trypanosomiasis", "Infectious Disease", "Parasitic infection transmitted by tsetse flies", "African sleeping sickness"],
    ["D066", "Organophosphate Poisoning", "Emergency Medicine", "Toxic exposure to organophosphate compounds", "Organophosphate toxicity"],
    ["D067", "Arteriovenous Malformation", "Neurological", "Abnormal connection between arteries and veins", "AVM"],
    ["D068", "Rabies", "Infectious Disease", "Viral infection affecting the central nervous system", "Rabies infection"],
    ["D069", "Naphthalene Poisoning", "Emergency Medicine", "Toxic exposure to naphthalene", "Naphthalene toxicity"],
    ["D070", "Cadmium Poisoning", "Emergency Medicine", "Toxic exposure to cadmium", "Cadmium toxicity"],
    ["D071", "Leprosy", "Infectious Disease", "Chronic infection caused by Mycobacterium leprae", "Hansen disease"],
    ["D072", "Ornithine Transcarbamylase Deficiency", "Metabolic", "Inherited disorder affecting ammonia metabolism", "OTC deficiency"],
    ["D073", "Common Variable Immunodeficiency", "Immunology", "Primary immune deficiency involving reduced antibody production", "CVID"]
]

# Load doctors and diagnoses.

doctor_fields, doctors = read_table(DOCTOR_FILE)
diagnosis_fields, diagnoses = read_table(DIAGNOSIS_FILE)

doctor_ids = {row["doctor_id"] for row in doctors}
diagnosis_ids = {row["diagnosis_id"] for row in diagnoses}

# Add missing diagnoses without duplicating existing IDs.

for diagnosis in new_diagnoses:
    if diagnosis[0] not in diagnosis_ids:
        diagnoses.append(dict(zip(diagnosis_fields, diagnosis)))
        diagnosis_ids.add(diagnosis[0])

write_table(DIAGNOSIS_FILE, diagnosis_fields, diagnoses)

# Prepare patient file.

patient_fields, patients = read_table(PATIENT_FILE)

patient_required = [
    "patient_id", "patient_name", "age", "gender",
    "blood_group", "city", "symptoms", "systolic_bp",
    "diastolic_bp", "heart_rate", "temperature",
    "oxygen_saturation", "urgency", "data_source"
]

for field in patient_required:
    if field not in patient_fields:
        patient_fields.append(field)

existing_patient_ids = {row["patient_id"] for row in patients}

# Prepare EHR file.

ehr_fields, ehr_records = read_table(EHR_FILE)

ehr_required = [
    "ehr_id", "patient_id", "doctor_id", "diagnosis_id",
    "visit_date", "symptoms", "diagnosis_notes",
    "treatment", "outcome", "episode_reference", "data_source"
]

for field in ehr_required:
    if field not in ehr_fields:
        ehr_fields.append(field)

existing_ehr_ids = {row["ehr_id"] for row in ehr_records}

blood_groups = ["A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"]

start_date = date(2025, 1, 1)

# Generate patient and matching EHR records.

for i, case in enumerate(cases, start=1):

    patient_id = f"H{i:03}"
    ehr_id = f"HEHR{i:03}"

    if patient_id in existing_patient_ids:
        continue

    if case["doctor"] not in doctor_ids:
        raise ValueError("Invalid doctor ID: " + case["doctor"])

    if case["diagnosis"] not in diagnosis_ids:
        raise ValueError("Invalid diagnosis ID: " + case["diagnosis"])

    age = random.randint(*case["age_range"])
    systolic = random.randint(100, 155)
    diastolic = random.randint(65, 100)
    heart_rate = random.randint(65, 115)
    temperature = round(random.uniform(36.5, 39.0), 1)
    oxygen = random.randint(91, 100)

    urgency = 3

    if oxygen < 94 or heart_rate >= 110:
        urgency = 4

    if oxygen < 90 or heart_rate >= 130:
        urgency = 5

    patient = {
        "patient_id": patient_id,
        "patient_name": case["name"],
        "age": age,
        "gender": case["gender"],
        "blood_group": random.choice(blood_groups),
        "city": "Princeton",
        "symptoms": case["symptoms"],
        "systolic_bp": systolic,
        "diastolic_bp": diastolic,
        "heart_rate": heart_rate,
        "temperature": temperature,
        "oxygen_saturation": oxygen,
        "urgency": urgency,
        "data_source": "HOUSE_EPISODE_INSPIRED"
    }

    visit_date = start_date + timedelta(days=random.randint(0, 600))

    ehr = {
        "ehr_id": ehr_id,
        "patient_id": patient_id,
        "doctor_id": case["doctor"],
        "diagnosis_id": case["diagnosis"],
        "visit_date": visit_date.isoformat(),
        "symptoms": case["symptoms"],
        "diagnosis_notes": "Episode-inspired case. Demographics and clinical details simulated.",
        "treatment": "Simulated educational treatment entry",
        "outcome": random.choice(["Follow-up required", "Stable", "Discharged"]),
        "episode_reference": case["episode"],
        "data_source": "HOUSE_EPISODE_INSPIRED"
    }

    patients.append(patient)
    ehr_records.append(ehr)

    existing_patient_ids.add(patient_id)
    existing_ehr_ids.add(ehr_id)

# Save both datasets.

write_table(PATIENT_FILE, patient_fields, patients)
write_table(EHR_FILE, ehr_fields, ehr_records)

print("Seed patient and EHR generation completed.")
print("Total patient records:", len(patients))
print("Total EHR records:", len(ehr_records))
print("New seed cases:", len(cases))
