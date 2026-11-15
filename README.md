# MediGrid-OS
A hospital management and clinical analytics system implementing advanced data structures and algorithms in C++.

## Web frontend

Requires Node.js 22 or later. No dependency installation or build step is needed.

```sh
node web/server.mjs
```

Open http://127.0.0.1:3000 in a browser. Alternatively run `npm start` if npm is installed. Set `PORT` to use another port.

### Run on another laptop

1. Install Node.js 22 or newer from https://nodejs.org (the LTS release is suitable).
2. Install Git from https://git-scm.com/downloads, then open a terminal and run:

   ```sh
   git clone https://github.com/lilylupin23/MediGrid-OS.git
   cd MediGrid-OS
   node web/server.mjs
   ```

3. Open http://127.0.0.1:3000. Keep the terminal open while using the app; press Ctrl+C to stop it.

On Windows, you can also double-click `start-medigrid.cmd` after installing Node.js.

Without Git, select **Code → Download ZIP** on GitHub, extract the entire archive, open a terminal in the extracted project folder, and run `node web/server.mjs`. Keep the `web` and `data` folders together. No `npm install`, C++ compiler, or database installation is required for the web app.

To get future code updates, stop the server and run `git pull --ff-only` in the project folder, then start it again. Patient registrations are saved locally to `data/processed/patients.txt`; they do not automatically sync between laptops. Back up that file before updating if you have added records. If Git reports local dataset changes, preserve them before resolving the update rather than discarding them.

The responsive interface includes:

- Overview with dataset totals, risk distribution, and priority records.
- ER records with search, urgency filters, pagination, patient details, and registration.
- Clinical analytics sorted by the risk score from `backend/module2.cpp`.
- Doctor registry and linked electronic health records.
- Campus map and shortest routes using the undirected connections dataset.
- Diagnosis search by name, identifier, category, description, and synonym.

### Integration

The original backend is an interactive C++ console program, not an HTTP service. `web/server.mjs` adds a local Node HTTP adapter over the same six `data/processed/*.txt` files. It does **not** execute the supplied `medigrid.exe` or invoke the C++ algorithms. Risk scoring and Dijkstra routing are implemented in the adapter to match the repository logic; changes to those C++ rules should be reflected in the adapter.

`GET /api/data` reads the six datasets; `GET /api/route?from=L001&to=L002` returns a shortest route; `POST /api/patients` validates and saves a new record with an automatically generated ID. Registration persists to the patient file used by the C++ loader. Restart the console application to load web registrations. Run only one writer at a time: the file format does not support concurrent console and web writes. The console waiting queue and assessment history are session-only and are not represented as live queue state in the web interface.

This is a local educational prototype with simulated records, without authentication or production clinical controls. The server binds to loopback only. Google Fonts are optional; system fonts are used when offline. Campus distances retain the dataset's unspecified units.

### Verification

```sh
node --test web/server.test.mjs
```

Tests use a temporary copy of the datasets and cover loading, risk boundaries, shortest paths, invalid input, persistence, origin checks, and static file restrictions. Repository data is left unchanged.
