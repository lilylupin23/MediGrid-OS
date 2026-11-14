
#include "modules.h"

int main() {
HospitalData data;

if (!loadData(data)) {
cout << "Data loading failed!" << endl;
return 1;
}

int choice;

do {
cout << "\n========== MEDIGRID OS ==========\n";
cout << "1. ER Intake & Triage\n";
cout << "2. Clinical Analytics\n";
cout << "3. Doctor Registry & EHR\n";
cout << "4. Campus Infrastructure\n";
cout << "5. Spatial Search & Diagnostics\n";
cout << "6. Exit\n";
cout << "Enter choice: ";

cin >> choice;

switch (choice) {
case 1:
runModule1(data);
break;

case 2:
runModule2(data);
break;

case 3:
runModule3(data);
break;

case 4:
runModule4(data);
break;

case 5:
runModule5(data);
break;

case 6:
cout << "Exiting MediGrid OS.\n";
break;

default:
cout << "Invalid choice.\n";
}
} while (choice != 6);

return 0;
}