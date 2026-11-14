
import csv
from pathlib import Path
from datetime import datetime

# Get the project directory automatically
PROJECT_ROOT = Path(__file__).resolve().parent.parent
RAW = PROJECT_ROOT / "data" / "raw"
PROCESSED = PROJECT_ROOT / "data" / "processed"

FILES = {
    "patients.txt": "patient_id",
    "doctors.txt": "doctor_id",
    "ehr.txt": "ehr_id",
    "diagnostics.txt": "diagnosis_id",
    "locations.txt": "location_id",
    "connections.txt": "connection_id"
}

EXPECTED_HEADERS = {
    "patients.txt": [
        "patient_id", "patient_name", "age", "gender",
        "blood_group", "city", "symptoms", "systolic_bp",
        "diastolic_bp", "heart_rate", "temperature",
        "oxygen_saturation", "urgency", "data_source"
    ],
    "doctors.txt": [
        "doctor_id", "doctor_name", "specialization",
        "role", "data_source"
    ],
    "ehr.txt": [
        "ehr_id", "patient_id", "doctor_id", "diagnosis_id",
        "visit_date", "symptoms", "diagnosis_notes",
        "treatment", "outcome", "episode_reference", "data_source"
    ],
    "diagnostics.txt": [
        "diagnosis_id", "diagnosis_name", "category",
        "description", "synonyms"
    ],
    "locations.txt": [
        "location_id", "location_name", "department",
        "x_coordinate", "y_coordinate"
    ],
    "connections.txt": [
        "connection_id", "from_location_id",
        "to_location_id", "distance"
    ]
}

data = {}
headers = {}
issues = []
corrections = []


# LOAD DATASETS

def load_file(filename):
    path = RAW / filename

    if not path.exists():
        issues.append(f"{filename}: File not found")
        return

    try:
        with open(path, "r", encoding="utf-8-sig", newline="") as file:
            reader = csv.DictReader(file, delimiter="|")

            if not reader.fieldnames:
                issues.append(f"{filename}: Missing header")
                return

            actual_headers = [h.strip() for h in reader.fieldnames]
            expected = EXPECTED_HEADERS[filename]

            if actual_headers != expected:
                issues.append(
                    f"{filename}: Header mismatch. Expected {expected}, got {actual_headers}"
                )

            headers[filename] = actual_headers
            rows = []

            for row in reader:
                values = list(row.values())

                # Remove repeated header rows
                if values == actual_headers:
                    corrections.append(
                        f"{filename}: Duplicate header row removed"
                    )
                    continue

                cleaned_row = {}

                for key, value in row.items():
                    if key is not None:
                        cleaned_row[key.strip()] = (
                            value.strip() if value else ""
                        )

                rows.append(cleaned_row)

            data[filename] = rows

            print(f"Loaded {filename}: {len(rows)} records")

    except Exception as e:
        issues.append(f"{filename}: Could not read file: {e}")


# CHECK MISSING VALUES

def check_missing_values(filename):
    for row in data.get(filename, []):
        for column, value in row.items():
            if value == "":
                issues.append(
                    f"{filename}: Missing {column} in record "
                    f"{row.get(FILES[filename], 'Unknown')}"
                )


# CHECK DUPLICATE IDS

def check_ids(filename, id_column):
    seen = set()

    for row in data.get(filename, []):
        value = row.get(id_column, "")

        if not value:
            continue

        if value in seen:
            issues.append(
                f"{filename}: Duplicate ID {value}"
            )
        else:
            seen.add(value)


# CHECK NUMERIC VALUES

def check_numeric_ranges():
    ranges = {
        "age": (0, 120),
        "systolic_bp": (40, 300),
        "diastolic_bp": (20, 200),
        "heart_rate": (20, 250),
        "temperature": (25, 45),
        "oxygen_saturation": (50, 100),
        "urgency": (1, 5),
        "x_coordinate": (0, 10000),
        "y_coordinate": (0, 10000),
        "distance": (0.01, 10000)
    }

    numeric_columns = {
        "patients.txt": [
            "age", "systolic_bp", "diastolic_bp",
            "heart_rate", "temperature",
            "oxygen_saturation", "urgency"
        ],
        "locations.txt": ["x_coordinate", "y_coordinate"],
        "connections.txt": ["distance"]
    }

    for filename, columns in numeric_columns.items():
        for row in data.get(filename, []):
            for column in columns:
                try:
                    value = float(row[column])

                    minimum, maximum = ranges[column]

                    if not minimum <= value <= maximum:
                        issues.append(
                            f"{filename}: Unusual {column} value "
                            f"{value} in record "
                            f"{row.get(FILES[filename], 'Unknown')}"
                        )

                except (ValueError, KeyError):
                    issues.append(
                        f"{filename}: Invalid numeric value for "
                        f"{column} in record "
                        f"{row.get(FILES[filename], 'Unknown')}"
                    )


# CHECK DATES

def check_dates():
    for row in data.get("ehr.txt", []):
        try:
            datetime.strptime(row["visit_date"], "%Y-%m-%d")
        except (ValueError, KeyError):
            issues.append(
                f"EHR {row.get('ehr_id', 'Unknown')}: Invalid visit date"
            )


# CHECK REFERENTIAL INTEGRITY

def check_references():
    patients = {
        r.get("patient_id") for r in data.get("patients.txt", [])
    }

    doctors = {
        r.get("doctor_id") for r in data.get("doctors.txt", [])
    }

    diagnoses = {
        r.get("diagnosis_id") for r in data.get("diagnostics.txt", [])
    }

    locations = {
        r.get("location_id") for r in data.get("locations.txt", [])
    }

    for row in data.get("ehr.txt", []):
        if row.get("patient_id") not in patients:
            issues.append(
                f"EHR {row.get('ehr_id')}: Unknown patient"
            )

        if row.get("doctor_id") not in doctors:
            issues.append(
                f"EHR {row.get('ehr_id')}: Unknown doctor"
            )

        if row.get("diagnosis_id") not in diagnoses:
            issues.append(
                f"EHR {row.get('ehr_id')}: Unknown diagnosis"
            )

    for row in data.get("connections.txt", []):
        if row.get("from_location_id") not in locations:
            issues.append(
                f"Connection {row.get('connection_id')}: "
                "Unknown starting location"
            )

        if row.get("to_location_id") not in locations:
            issues.append(
                f"Connection {row.get('connection_id')}: "
                "Unknown destination"
            )


# SAVE PROCESSED DATASETS

def save_file(filename):
    rows = data.get(filename, [])

    if filename not in headers:
        return

    output = PROCESSED / filename

    with open(output, "w", newline="", encoding="utf-8") as file:
        writer = csv.DictWriter(
            file,
            fieldnames=headers[filename],
            delimiter="|",
            extrasaction="ignore"
        )

        writer.writeheader()
        writer.writerows(rows)

    print(f"Saved {filename}")


# GENERATE VALIDATION REPORT

def generate_report():
    report_path = PROCESSED / "validation_report.txt"

    with open(report_path, "w", encoding="utf-8") as file:
        file.write("MEDIGRID OS - DATA VALIDATION REPORT\n")
        file.write("=" * 45 + "\n\n")

        file.write("DATASET RECORD COUNTS\n")
        file.write("-" * 45 + "\n")

        for filename in FILES:
            count = len(data.get(filename, []))
            file.write(f"{filename}: {count} records\n")

        file.write("\nCORRECTIONS\n")
        file.write("-" * 45 + "\n")

        if corrections:
            for correction in corrections:
                file.write(correction + "\n")
        else:
            file.write("No corrections required.\n")

        file.write("\nVALIDATION ISSUES\n")
        file.write("-" * 45 + "\n")

        if issues:
            for issue in issues:
                file.write(issue + "\n")
        else:
            file.write("No issues detected.\n")

        file.write(
            f"\nTotal corrections: {len(corrections)}\n"
        )
        file.write(
            f"Total issues: {len(issues)}\n"
        )


# MAIN FUNCTION

def main():
    print("\nMEDIGRID OS - DATA PREPROCESSING")
    print("=" * 40)

    PROCESSED.mkdir(parents=True, exist_ok=True)

    # Load all six datasets
    for filename in FILES:
        load_file(filename)

    # Run validations
    for filename, id_column in FILES.items():
        if filename in data:
            check_ids(filename, id_column)
            check_missing_values(filename)

    check_numeric_ranges()
    check_dates()
    check_references()

    # Export processed datasets
    for filename in FILES:
        if filename in data:
            save_file(filename)

    # Generate report
    generate_report()

    print("\nPREPROCESSING COMPLETED")
    print("=" * 40)
    print(f"Datasets loaded: {len(data)} / {len(FILES)}")
    print(f"Corrections: {len(corrections)}")
    print(f"Validation issues: {len(issues)}")
    print(f"Output directory: {PROCESSED}")


if __name__ == "__main__":
    main()