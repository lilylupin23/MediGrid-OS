import http from 'node:http';
import { readFileSync, writeFileSync, renameSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import path from 'node:path';

const root = fileURLToPath(new URL('../', import.meta.url));
const dataDir = process.env.MEDIGRID_DATA_DIR || path.join(root, 'data/processed');
const tables = ['patients', 'doctors', 'ehr', 'diagnostics', 'locations', 'connections'];
export function readTable(name) {
  const [header, ...lines] = readFileSync(path.join(dataDir, `${name}.txt`), 'utf8').trim().split(/\r?\n/);
  return lines.filter(Boolean).map(line => Object.fromEntries(header.split('|').map((key, i) => [key, line.split('|')[i] || ''])));
}
export function risk(p) {
  return (Number(p.age) >= 65 ? 2 : 0) + (Number(p.systolic_bp) >= 140 ? 2 : 0) + (Number(p.heart_rate) >= 100 ? 2 : 0) + (Number(p.oxygen_saturation) < 94 ? 4 : 0) + (Number(p.temperature) >= 38 ? 2 : 0);
}
export function route(locations, connections, from, to) {
  if (![from, to].every(id => locations.some(l => l.location_id === id))) throw new Error('Choose valid locations.');
  const dist = new Map(locations.map(l => [l.location_id, Infinity])), previous = new Map(), pending = new Set(dist.keys());
  dist.set(from, 0);
  while (pending.size) {
    const u = [...pending].reduce((a, b) => dist.get(a) <= dist.get(b) ? a : b);
    if (dist.get(u) === Infinity || u === to) break;
    pending.delete(u);
    for (const c of connections) {
      const v = c.from_location_id === u ? c.to_location_id : c.to_location_id === u ? c.from_location_id : null;
      if (pending.has(v) && dist.get(u) + Number(c.distance) < dist.get(v)) {
        dist.set(v, dist.get(u) + Number(c.distance)); previous.set(v, u);
      }
    }
  }
  if (dist.get(to) === Infinity) return { path: [], distance: null };
  const result = []; for (let id = to; id; id = previous.get(id)) result.unshift(id);
  return { path: result, distance: dist.get(to) };
}
function register(input) {
  const filename = path.join(dataDir, 'patients.txt');
  const original = readFileSync(filename, 'utf8');
  const keys = original.split(/\r?\n/)[0].split('|');
  const p = {};
  for (const key of keys.slice(1, -1)) {
    const value = String(input[key] ?? '').trim();
    if (!value || value.length > 500 || /[|\r\n\x00-\x1f]/.test(value)) throw new Error(`Invalid ${key.replaceAll('_', ' ')}.`);
    p[key] = value;
  }
  const ranges = { age: [0, 130], systolic_bp: [30, 300], diastolic_bp: [10, 200], heart_rate: [20, 300], temperature: [25, 45], oxygen_saturation: [0, 100], urgency: [1, 5] };
  for (const [key, [min, max]] of Object.entries(ranges)) {
    const n = Number(p[key]);
    if (!Number.isFinite(n) || n < min || n > max || (!['temperature', 'oxygen_saturation'].includes(key) && !Number.isInteger(n))) throw new Error(`Invalid ${key.replaceAll('_', ' ')}.`);
  }
  p.patient_id = 'H' + String(Math.max(0, ...readTable('patients').map(p => Number(p.patient_id.slice(1)) || 0)) + 1).padStart(3, '0');
  p.data_source = 'USER_REGISTERED';
  writeFileSync(filename + '.tmp', original.trimEnd() + '\n' + keys.map(k => p[k]).join('|') + '\n');
  renameSync(filename + '.tmp', filename);
  return p;
}
export const server = http.createServer(async (req, res) => {
  const json = (status, body) => { res.writeHead(status, { 'Content-Type': 'application/json', 'Cache-Control': 'no-store' }); res.end(JSON.stringify(body)); };
  try {
    const url = new URL(req.url, 'http://localhost');
    if (req.method === 'GET' && url.pathname === '/api/data') return json(200, Object.fromEntries(tables.map(t => [t, readTable(t).map(p => t === 'patients' ? { ...p, risk_score: risk(p) } : p)])));
    if (req.method === 'GET' && url.pathname === '/api/route') return json(200, route(readTable('locations'), readTable('connections'), url.searchParams.get('from'), url.searchParams.get('to')));
    if (req.method === 'POST' && url.pathname === '/api/patients') {
      // Same-origin local development API. Prevent cross-site form submissions.
      if (req.headers.origin && req.headers.origin !== `http://${req.headers.host}`) return json(403, { error: 'Origin rejected.' });
      if (!req.headers['content-type']?.startsWith('application/json')) return json(415, { error: 'JSON required.' });
      let body = ''; for await (const chunk of req) { body += chunk; if (body.length > 16384) return json(413, { error: 'Request too large.' }); }
      return json(201, register(JSON.parse(body)));
    }
    if (req.method !== 'GET') return json(405, { error: 'Method not allowed.' });
    const assets = { '/': ['index.html', 'text/html'], '/app.js': ['app.js', 'text/javascript'], '/style.css': ['style.css', 'text/css'] };
    const asset = assets[url.pathname];
    if (!asset) return json(404, { error: 'Not found.' });
    res.writeHead(200, { 'Content-Type': asset[1] + '; charset=utf-8', 'X-Content-Type-Options': 'nosniff' });
    res.end(readFileSync(path.join(root, 'web', asset[0])));
  } catch (error) { json(error.code ? 500 : 400, { error: error.code ? 'Unable to access dataset. Check server files.' : error.message }); }
});
if (process.argv[1] && path.resolve(process.argv[1]) === fileURLToPath(import.meta.url)) server.listen(Number(process.env.PORT || 3000), '127.0.0.1', () => console.log('MediGrid OS: http://127.0.0.1:' + (process.env.PORT || 3000)));
