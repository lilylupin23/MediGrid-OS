import { test } from 'node:test';
import assert from 'node:assert/strict';
import { mkdtempSync, cpSync, readFileSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import path from 'node:path';

test('dataset API, routes, risk boundaries, validation and persistent registration', async () => {
  const dir = mkdtempSync(path.join(tmpdir(), 'medigrid-test-'));
  cpSync(new URL('../data/processed/', import.meta.url), dir, { recursive: true });
  process.env.MEDIGRID_DATA_DIR = dir;
  const { server, risk, route } = await import('./server.mjs');
  await new Promise(resolve => server.listen(0, '127.0.0.1', resolve));
  const base = `http://127.0.0.1:${server.address().port}`;
  const post = (body, extra = {}) => fetch(base + '/api/patients', { method: 'POST', headers: { 'Content-Type': 'application/json', ...extra }, body: JSON.stringify(body) });
  try {
    const data = await (await fetch(base + '/api/data')).json();
    assert.ok(data.patients.length > 0); assert.equal(Object.keys(data).length, 6);
    assert.equal(risk({ age: 65, systolic_bp: 140, heart_rate: 100, oxygen_saturation: 93, temperature: 38 }), 12);
    assert.equal(risk({ age: 64, systolic_bp: 139, heart_rate: 99, oxygen_saturation: 94, temperature: 37.9 }), 0);
    const locations = ['a', 'b', 'c', 'd'].map(location_id => ({ location_id }));
    const edges = [['a','b',10], ['b','c',2], ['a','c',20]].map(([from_location_id,to_location_id,distance]) => ({ from_location_id,to_location_id,distance }));
    assert.deepEqual(route(locations, edges, 'c', 'a'), { path: ['c', 'b', 'a'], distance: 12 });
    assert.deepEqual(route(locations, edges, 'a', 'd'), { path: [], distance: null });
    assert.deepEqual(route(locations, edges, 'a', 'a'), { path: ['a'], distance: 0 });
    assert.throws(() => route(locations, edges, 'x', 'a'));
    const patient = { ...data.patients[0], patient_name: 'Frontend Test Patient' };
    const response = await post(patient); assert.equal(response.status, 201);
    const saved = await response.json(); assert.notEqual(saved.patient_id, patient.patient_id);
    assert.match(readFileSync(path.join(dir, 'patients.txt'), 'utf8'), /Frontend Test Patient/);
    const after = await (await fetch(base + '/api/data')).json(); assert.equal(after.patients.length, data.patients.length + 1);
    assert.equal((await post({ ...patient, patient_name: 'Injected|row' })).status, 400);
    assert.equal((await post({ ...patient, urgency: 3.5 })).status, 400);
    assert.equal((await post({ ...patient, oxygen_saturation: 101 })).status, 400);
    assert.equal((await post(patient, { Origin: 'https://untrusted.example' })).status, 403);
    assert.equal((await fetch(base + '/backend/data.cpp')).status, 404);
    assert.equal((await fetch(base + '/')).status, 200);
  } finally {
    await new Promise(resolve => server.close(resolve));
    rmSync(dir, { recursive: true, force: true });
  }
});
