const $ = s => document.querySelector(s);
const esc = v => String(v ?? '').replace(/[&<>"']/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[c]));
const icons = { overview: '▦', patients: '♡', analytics: '▥', doctors: '♧', campus: '⌘', diagnostics: '⌕' };
const pages = { overview: 'Overview', patients: 'ER intake & triage', analytics: 'Clinical analytics', doctors: 'Doctors & EHR', campus: 'Campus infrastructure', diagnostics: 'Diagnosis search' };
let data, page = 'overview', query = '', filter = '', offset = 0;
const size = 12;
const riskName = p => p.risk_score >= 6 ? 'High' : p.risk_score >= 3 ? 'Moderate' : 'Low';
const badge = (text, kind = '') => `<span class="badge ${kind}"><i></i>${esc(text)}</span>`;
const riskBadge = p => badge(riskName(p), riskName(p).toLowerCase());
const initials = name => name.split(' ').map(n => n[0]).slice(0, 2).join('');
const count = n => n.toLocaleString();
async function api(url, options) { const res = await fetch(url, options); const result = await res.json(); if (!res.ok) throw new Error(result.error || 'Request failed.'); return result; }
async function refresh() { data = await api('/api/data'); }
function toast(text) { $('#toast').textContent = text; $('#toast').classList.add('visible'); setTimeout(() => $('#toast').classList.remove('visible'), 3500); }
function title(kicker, heading, subtitle, action = '') { return `<div class="page-heading"><div><p class="eyebrow">${kicker}</p><h1>${heading}</h1><p class="muted">${subtitle}</p></div>${action}</div>`; }
const addButton = '<button class="button" data-action="register"><span>＋</span> Register patient</button>';
function stats() {
  return `<div class="stats">${[[data.patients.length, 'Patient records', 'Across the hospital dataset', '♡', 'teal'], [data.patients.filter(p => p.urgency >= 4).length, 'High-urgency records', 'Urgency level 4 or 5', '↗', 'red'], [data.doctors.length, 'Registered doctors', 'Across all specialties', '♧', 'blue'], [data.ehr.length, 'Health records', 'Documented patient encounters', '▤', 'purple']].map(([n, label, note, icon, color]) => `<article class="stat"><div>${label}<span class="stat-icon ${color}">${icon}</span></div><strong>${count(n)}</strong><small>${note}</small></article>`).join('')}</div>`;
}
function table(headers, rows) { return `<div class="table-scroll"><table><thead><tr>${headers.map(h => `<th>${h}</th>`).join('')}</tr></thead><tbody>${rows.length ? rows.join('') : `<tr><td colspan="${headers.length}" class="empty">No records match your search.</td></tr>`}</tbody></table></div>`; }
function person(p) { return `<div class="person"><span class="avatar">${esc(initials(p.patient_name))}</span><div><b>${esc(p.patient_name)}</b><small>${esc(p.patient_id)} · ${esc(p.age)} years · ${esc(p.gender)}</small></div></div>`; }
function patientRows(records, analytics = false) { return records.map(p => `<tr><td>${person(p)}</td><td>${esc(p.symptoms)}</td><td>${analytics ? `${esc(p.systolic_bp)}/${esc(p.diastolic_bp)} <small>mmHg</small>` : badge(`Level ${p.urgency}`, p.urgency >= 4 ? 'high' : p.urgency >= 3 ? 'moderate' : 'low')}</td><td>${riskBadge(p)}${analytics ? `<small class="score">${p.risk_score}/12</small>` : ''}</td><td><button class="text-button" data-patient="${esc(p.patient_id)}">View record ↗</button></td></tr>`); }
function overview() {
  const high = data.patients.filter(p => riskName(p) === 'High').length, moderate = data.patients.filter(p => riskName(p) === 'Moderate').length, low = data.patients.length - high - moderate;
  const total = data.patients.length || 1;
  const priority = [...data.patients].sort((a, b) => b.urgency - a.urgency || b.risk_score - a.risk_score).slice(0, 5);
  return title('CARE OPERATIONS', 'A clearer view of care.', 'Your hospital workspace, connected in one place.', addButton) + stats() + `<div class="overview-grid"><section class="card"><div class="section-head"><div><h2>Patient risk overview</h2><p class="muted">Risk distribution across all patient records</p></div><span class="soft-label">All records</span></div><div class="risk-content"><div class="donut" style="--high:${high / total * 100}%;--moderate:${(high + moderate) / total * 100}%"><div><strong>${count(total)}</strong><small>patient records</small></div></div><div class="legend">${[[high, 'High', 'Requires closer review'], [moderate, 'Moderate', 'Elevated risk factors'], [low, 'Low', 'Fewer risk factors']].map(([n, label, sub]) => `<div><i class="legend-dot ${label.toLowerCase()}"></i><span><b>${label} risk</b><small>${sub}</small></span><strong>${n}</strong><small>${Math.round(n / total * 100)}%</small></div>`).join('')}</div></div><div class="card-note">ⓘ Scores follow the repository’s educational risk model.</div></section><section class="card quick"><p class="eyebrow">YOUR WORKSPACE</p><h2>Care starts with<br>a connected team.</h2><p>Find the people, records, and places<br>you need to keep care moving.</p><a href="#doctors">Explore doctor registry <span>↗</span></a><div class="grid-art" aria-hidden="true"><span>✚</span></div><div class="quick-bottom"><span class="dot"></span> ${data.doctors.length} doctors · ${data.locations.length} campus locations</div></section></div><section class="card priority"><div class="section-head"><div><h2>Priority patient records <span class="number-pill">${priority.length}</span></h2><p class="muted">Highest urgency first, then risk score</p></div><a class="text-button" href="#patients">View all patients →</a></div>${table(['PATIENT', 'PRESENTING SYMPTOMS', 'URGENCY', 'RISK LEVEL', ''], patientRows(priority))}</section><div class="module-links"><a href="#campus"><span>⌘</span><div><h3>Find your way</h3><p>Explore campus locations and routes</p></div>↗</a><a href="#diagnostics"><span>⌕</span><div><h3>Explore diagnoses</h3><p>Search conditions, categories, and synonyms</p></div>↗</a></div>`;
}
function toolbar(placeholder, options = []) { return `<div class="toolbar"><label class="search"><span>⌕</span><input id="search" aria-label="Search records" placeholder="${placeholder}" value="${esc(query)}"></label>${options.length ? `<select id="filter" aria-label="Filter records"><option value="">All ${page === 'patients' ? 'urgency levels' : page === 'analytics' ? 'risk levels' : 'specialties'}</option>${options.map(o => `<option ${filter === o ? 'selected' : ''}>${esc(o)}</option>`).join('')}</select>` : ''}</div>`; }
function pagination(total) { return `<div class="pagination"><span>${total ? offset + 1 : 0}–${Math.min(offset + size, total)} of ${count(total)} records</span><div><button class="button secondary" data-step="-1" ${offset === 0 ? 'disabled' : ''}>← Previous</button><button class="button secondary" data-step="1" ${offset + size >= total ? 'disabled' : ''}>Next →</button></div></div>`; }
function patients(analytics = false) {
  const records = [...data.patients].filter(p => `${p.patient_name} ${p.patient_id} ${p.symptoms} ${p.city}`.toLowerCase().includes(query.toLowerCase()) && (!filter || (analytics ? riskName(p) === filter : String(p.urgency) === filter))).sort((a, b) => analytics ? b.risk_score - a.risk_score : b.urgency - a.urgency);
  return title(analytics ? 'CLINICAL INSIGHTS' : 'PATIENT MANAGEMENT', analytics ? 'Clinical analytics' : 'ER intake & triage', analytics ? 'Review risk factors using the repository’s scoring model.' : 'Search patient records and register new arrivals.', analytics ? '' : addButton) + (analytics ? stats() : '') + `<section class="card">${toolbar('Search by name, ID, city, or symptoms…', analytics ? ['High', 'Moderate', 'Low'] : ['5', '4', '3', '2', '1'])}${table(['PATIENT', 'PRESENTING SYMPTOMS', analytics ? 'BLOOD PRESSURE' : 'URGENCY', 'RISK LEVEL', ''], patientRows(records.slice(offset, offset + size), analytics))}${pagination(records.length)}</section>${analytics ? '<p class="muted model-note">Model: age ≥65 (+2), systolic BP ≥140 (+2), heart rate ≥100 (+2), oxygen &lt;94% (+4), temperature ≥38°C (+2). High ≥6; moderate ≥3; low &lt;3.</p>' : '<p class="muted model-note">Records are sorted by recorded urgency. The console app’s in-memory waiting queue and assessment history are separate.</p>'}`;
}
function doctors() {
  const records = data.doctors.filter(d => `${d.doctor_name} ${d.doctor_id} ${d.specialization}`.toLowerCase().includes(query.toLowerCase()));
  return title('PEOPLE & RECORDS', 'Doctor registry', 'Meet the specialists behind every patient encounter.') + `<section class="card">${toolbar('Search doctors or specialties…')}<div class="doctor-grid">${records.slice(offset, offset + size).map(d => `<article class="doctor"><span class="avatar">${esc(initials(d.doctor_name))}</span><small>${esc(d.doctor_id)}</small><h3>Dr. ${esc(d.doctor_name)}</h3><p>${esc(d.specialization)}</p><small>${esc(d.role)}</small><button class="text-button" data-doctor="${esc(d.doctor_id)}">View health records →</button></article>`).join('') || '<p class="empty">No doctors found.</p>'}</div>${pagination(records.length)}</section>`;
}
function diagnostics() {
  const records = data.diagnostics.filter(d => Object.values(d).join(' ').toLowerCase().includes(query.toLowerCase()));
  return title('KNOWLEDGE DIRECTORY', 'Diagnosis search', 'Find conditions by name, ID, category, or synonym.') + `<section class="card">${toolbar('Search diagnoses, e.g. hypertension…')}${table(['DIAGNOSIS', 'CATEGORY', 'DESCRIPTION', 'SYNONYMS'], records.slice(offset, offset + size).map(d => `<tr><td><b>${esc(d.diagnosis_name)}</b><small class="block">${esc(d.diagnosis_id)}</small></td><td>${badge(d.category, 'neutral')}</td><td>${esc(d.description)}</td><td>${esc(d.synonyms)}</td></tr>`))}${pagination(records.length)}</section>`;
}
function campus() {
  const options = data.locations.map(l => `<option value="${esc(l.location_id)}">${esc(l.location_name)}</option>`).join('');
  const xs = data.locations.map(l => +l.x_coordinate), ys = data.locations.map(l => +l.y_coordinate);
  const pos = l => [50 + (+l.x_coordinate - Math.min(...xs)) / (Math.max(...xs) - Math.min(...xs) || 1) * 700, 40 + (+l.y_coordinate - Math.min(...ys)) / (Math.max(...ys) - Math.min(...ys) || 1) * 380];
  const find = id => data.locations.find(l => l.location_id === id);
  return title('CONNECTED CAMPUS', 'Campus infrastructure', 'Explore the hospital network and find the shortest route.') + `<section class="card"><div class="section-head"><div><h2>Hospital campus</h2><p class="muted">${data.locations.length} locations · ${data.connections.length} connections · Dataset coordinates</p></div></div><form id="route-form" class="toolbar"><label>From<select id="from">${options}</select></label><label>To<select id="to">${options}</select></label><button class="button">Find shortest route ↗</button></form><p id="route-result" role="status" class="route-result">Choose two locations to plan a route.</p><div class="map"><svg viewBox="0 0 900 480" role="img" aria-label="Hospital campus locations and connections">${data.connections.map(c => { const a = find(c.from_location_id), b = find(c.to_location_id); if (!a || !b) return ''; const [x1, y1] = pos(a), [x2, y2] = pos(b); return `<line data-edge="${esc(c.from_location_id)}:${esc(c.to_location_id)}" x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}"/>`; }).join('')}${data.locations.map(l => { const [x,y] = pos(l); return `<g><circle cx="${x}" cy="${y}" r="7"/><text x="${x + 12}" y="${y + 4}">${esc(l.location_name)}</text><title>${esc(l.department)}</title></g>`; }).join('')}</svg></div></section>`;
}
function render() {
  $('#crumb').textContent = pages[page];
  document.querySelectorAll('nav a').forEach(a => { a.classList.toggle('active', a.hash === '#' + page); if (a.hash === '#' + page) a.setAttribute('aria-current', 'page'); else a.removeAttribute('aria-current'); });
  $('#content').innerHTML = ({ overview, patients, analytics: () => patients(true), doctors, diagnostics, campus })[page]();
  if (page === 'campus' && $('#to').options.length > 1) $('#to').selectedIndex = 1;
}
function ehrTable(records) { return table(['DATE / RECORD', 'PATIENT / DOCTOR', 'DIAGNOSIS', 'TREATMENT / OUTCOME'], records.map(e => `<tr><td>${esc(e.visit_date)}<small class="block">${esc(e.ehr_id)}</small></td><td>${esc(data.patients.find(p => p.patient_id === e.patient_id)?.patient_name || e.patient_id)}<small class="block">Dr. ${esc(data.doctors.find(d => d.doctor_id === e.doctor_id)?.doctor_name || e.doctor_id)}</small></td><td>${esc(data.diagnostics.find(d => d.diagnosis_id === e.diagnosis_id)?.diagnosis_name || e.diagnosis_id)}<small class="block">${esc(e.diagnosis_notes)}</small></td><td>${esc(e.treatment)}<small class="block">${esc(e.outcome)}</small></td></tr>`)); }
function detail(heading, html) {
  $('#modal').innerHTML = `<div class="section-head"><h2>${esc(heading)}</h2><button type="button" class="icon-button" id="close-modal" aria-label="Close dialog">×</button></div>${html}`;
  $('#modal').showModal();
}
const originalForm = $('#modal').innerHTML;
function register() {
  $('#modal').innerHTML = originalForm;
  const fields = [['patient_name', 'Full name'], ['age', 'Age', 'number', 0, 130], ['gender', 'Gender'], ['blood_group', 'Blood group'], ['city', 'City'], ['symptoms', 'Presenting symptoms'], ['systolic_bp', 'Systolic BP (mmHg)', 'number', 30, 300], ['diastolic_bp', 'Diastolic BP (mmHg)', 'number', 10, 200], ['heart_rate', 'Heart rate (bpm)', 'number', 20, 300], ['temperature', 'Temperature (°C)', 'number', 25, 45], ['oxygen_saturation', 'Oxygen saturation (%)', 'number', 0, 100], ['urgency', 'Urgency (1–5, clinician assigned)', 'number', 1, 5]];
  $('#form-fields').innerHTML = fields.map(([key, label, type = 'text', min, max]) => `<label>${label}<input name="${key}" type="${type}" required maxlength="500" ${type === 'number' ? `min="${min}" max="${max}" step="${['temperature', 'oxygen_saturation'].includes(key) ? '0.1' : '1'}"` : ''}></label>`).join('');
  $('#modal').showModal();
}
document.addEventListener('click', event => {
  const el = event.target.closest('button'); if (!el) return;
  if (el.id === 'close-modal' || el.id === 'cancel-modal') $('#modal').close();
  if (el.dataset.action === 'register') register();
  if (el.dataset.step) { offset += Number(el.dataset.step) * size; render(); }
  if (el.dataset.patient) { const p = data.patients.find(p => p.patient_id === el.dataset.patient); detail(p.patient_name, `<p class="muted">${esc(p.patient_id)} · ${esc(p.age)} years · ${esc(p.gender)} · ${esc(p.city)} · Blood group ${esc(p.blood_group)}</p><div class="vitals">${[['Blood pressure', `${p.systolic_bp}/${p.diastolic_bp}`, 'mmHg'], ['Heart rate', p.heart_rate, 'bpm'], ['Temperature', p.temperature, '°C'], ['Oxygen', p.oxygen_saturation, '%']].map(([l,v,u]) => `<div><small>${l}</small><strong>${esc(v)} <small>${u}</small></strong></div>`).join('')}</div><p>${esc(p.symptoms)}</p><p>${riskBadge(p)} · Urgency ${esc(p.urgency)}/5</p><h3>Health records</h3>${ehrTable(data.ehr.filter(e => e.patient_id === p.patient_id))}`); }
  if (el.dataset.doctor) { const d = data.doctors.find(d => d.doctor_id === el.dataset.doctor); detail(`Dr. ${d.doctor_name}`, `<p class="muted">${esc(d.specialization)}</p>${ehrTable(data.ehr.filter(e => e.doctor_id === d.doctor_id))}`); }
});
document.addEventListener('input', event => {
  if (event.target.id !== 'search') return;
  query = event.target.value; const position = event.target.selectionStart; offset = 0; render(); $('#search').focus(); $('#search').setSelectionRange(position, position);
});
document.addEventListener('change', e => { if (e.target.id === 'filter') { filter = e.target.value; offset = 0; render(); } });
document.addEventListener('submit', async e => {
  if (!['patient-form', 'route-form'].includes(e.target.id)) return; e.preventDefault();
  if (e.target.id === 'patient-form') {
    const button = $('#save-patient'); button.disabled = true; $('#form-error').textContent = '';
    try { const p = await api('/api/patients', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(Object.fromEntries(new FormData(e.target))) }); await refresh(); $('#modal').close(); render(); toast(`${p.patient_name} registered as ${p.patient_id}`); }
    catch (err) { $('#form-error').textContent = err.message; } finally { button.disabled = false; }
  } else {
    try { const result = await api(`/api/route?from=${encodeURIComponent($('#from').value)}&to=${encodeURIComponent($('#to').value)}`); $('#route-result').textContent = result.path.length ? `${result.path.map(id => data.locations.find(l => l.location_id === id).location_name).join(' → ')} · Distance: ${result.distance} dataset units` : 'No route connects these locations.'; document.querySelectorAll('[data-edge]').forEach(el => { const [a,b] = el.dataset.edge.split(':'); el.classList.toggle('selected', result.path.some((id,i) => i && ((result.path[i-1] === a && id === b) || (result.path[i-1] === b && id === a)))); }); }
    catch (err) { $('#route-result').textContent = err.message; }
  }
});
function navigate() { page = Object.hasOwn(pages, location.hash.slice(1)) ? location.hash.slice(1) : 'overview'; query = ''; filter = ''; offset = 0; if (data) render(); }
$('nav').innerHTML = Object.entries(pages).map(([key, label]) => `<a href="#${key}"><span>${icons[key]}</span>${label}${key === 'overview' ? '<i></i>' : ''}</a>`).join('');
window.addEventListener('hashchange', navigate);
try { await refresh(); navigate(); } catch (err) { $('#content').innerHTML = `<div class="card empty"><h2>Unable to load workspace</h2><p>${esc(err.message)}</p><button class="button" onclick="location.reload()">Try again</button></div>`; }
