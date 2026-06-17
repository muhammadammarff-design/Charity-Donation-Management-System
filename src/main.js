import { createClient } from '@supabase/supabase-js';
import './styles.css';

const SUPABASE_URL = import.meta.env.VITE_SUPABASE_URL;
const SUPABASE_ANON_KEY = import.meta.env.VITE_SUPABASE_ANON_KEY;
const app = document.querySelector('#app');

let supabase = null;
let state = {
  role: null,
  tab: 'donor',
  adminUser: null,
  donor: null,
  page: 'dashboard',
  modal: null,
  filterActive: false,
  data: {
    donors: [],
    campaigns: [],
    beneficiaries: [],
    donations: [],
    allocations: [],
    reports: [],
  },
};

function money(value) {
  const n = Number(value || 0);
  if (Math.abs(n) >= 1000) return `Rs. ${Math.round(n / 1000)}k`;
  return `Rs. ${Math.round(n)}`;
}

function moneyFull(value) {
  return `Rs. ${Number(value || 0).toLocaleString()}`;
}

function escapeHtml(value) {
  return String(value ?? '').replace(/[&<>'"]/g, (ch) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', "'": '&#39;', '"': '&quot;' }[ch]));
}

function toast(message, type = 'success') {
  const old = document.querySelector('.floating-toast');
  if (old) old.remove();
  const el = document.createElement('div');
  el.className = `toast ${type} floating-toast`;
  el.style.position = 'fixed';
  el.style.right = '24px';
  el.style.bottom = '24px';
  el.style.zIndex = '200';
  el.textContent = message;
  document.body.appendChild(el);
  setTimeout(() => el.remove(), 3500);
}

function setupScreen() {
  app.innerHTML = `
    <div class="shell">
      ${brandStrip()}
      <div class="login-card setup-card">
        <h2>Supabase connection required</h2>
        <p class="subtitle">Add these environment variables in Vercel project settings, then redeploy:</p>
        <div class="hint-box">
          <code>VITE_SUPABASE_URL</code><br />
          <code>VITE_SUPABASE_ANON_KEY</code>
        </div>
        <p class="subtitle">Run <code>supabase/schema.sql</code> in Supabase SQL Editor first.</p>
      </div>
    </div>`;
}

function brandStrip() {
  return `
    <header class="top-strip">
      <div class="brand">
        <div class="brand-logo">♡</div>
        <div><h1>Amanah Charity System</h1><p>Vercel + Supabase dashboard · C++ OOP source included</p></div>
      </div>
      <div class="live-pill"><span class="dot"></span> Live Supabase project</div>
    </header>`;
}

async function init() {
  if (!SUPABASE_URL || !SUPABASE_ANON_KEY) {
    setupScreen();
    return;
  }
  supabase = createClient(SUPABASE_URL, SUPABASE_ANON_KEY);
  const { data } = await supabase.auth.getSession();
  if (data.session?.user) {
    const { data: profile } = await supabase.from('profiles').select('*').eq('id', data.session.user.id).maybeSingle();
    if (profile?.role === 'admin') {
      state.role = 'admin';
      state.adminUser = data.session.user;
      state.page = 'dashboard';
      await loadAdminData();
      render();
      return;
    }
    await supabase.auth.signOut();
  }
  renderLogin();
}

function renderLogin() {
  app.innerHTML = `
    <div class="shell">
      ${brandStrip()}
      <section class="login-wrap">
        <div class="login-card">
          <h2>Login</h2>
          <p class="subtitle">Choose donor, register donor, or admin access.</p>
          <div class="role-tabs">
            <button class="role-tab ${state.tab === 'donor' ? 'active' : ''}" data-tab="donor">Donor</button>
            <button class="role-tab ${state.tab === 'register' ? 'active' : ''}" data-tab="register">Register</button>
            <button class="role-tab ${state.tab === 'admin' ? 'active' : ''}" data-tab="admin">Admin</button>
          </div>
          ${state.tab === 'donor' ? donorLoginHtml() : ''}
          ${state.tab === 'register' ? donorRegisterHtml() : ''}
          ${state.tab === 'admin' ? adminLoginHtml() : ''}
          <div class="security-note">Data is saved in Supabase PostgreSQL. C++ OOP source remains in the GitHub repository for viva/report.</div>
        </div>
      </section>
    </div>`;

  document.querySelectorAll('[data-tab]').forEach((btn) => btn.addEventListener('click', () => { state.tab = btn.dataset.tab; renderLogin(); }));
  const donorForm = document.querySelector('#donor-login-form');
  const regForm = document.querySelector('#donor-register-form');
  const adminForm = document.querySelector('#admin-login-form');
  if (donorForm) donorForm.addEventListener('submit', handleDonorLogin);
  if (regForm) regForm.addEventListener('submit', handleDonorRegister);
  if (adminForm) adminForm.addEventListener('submit', handleAdminLogin);
}

function donorLoginHtml() {
  return `
    <form class="form-block" id="donor-login-form">
      <h3>Donor verification</h3>
      <p>View your donation statement using Donor ID and email.</p>
      <div class="field"><label>Donor ID <span>Format: DNR001</span></label><input name="donor_code" value="DNR001" required /></div>
      <div class="field"><label>Email <span>Required</span></label><input name="email" type="email" value="ahmed.khan@example.com" required /></div>
      <div class="hint-box">Demo donor after running schema: <strong>DNR001</strong> + <strong>ahmed.khan@example.com</strong></div>
      <div class="button-row"><button class="btn primary" type="submit">Login as donor</button></div>
    </form>`;
}

function donorRegisterHtml() {
  return `
    <form class="form-block" id="donor-register-form">
      <h3>Register as donor</h3>
      <p>Your Donor ID will be generated by Supabase/PostgreSQL.</p>
      <div class="field"><label>Full name <span>Required</span></label><input name="name" value="New Donor" required /></div>
      <div class="field"><label>Email <span>Required</span></label><input name="email" type="email" value="new.donor@example.com" required /></div>
      <div class="field"><label>Contact <span>7-15 digits</span></label><input name="contact" value="+92 300 1234567" required /></div>
      <div class="field"><label>Address <span>Required</span></label><input name="address" value="Lahore" required /></div>
      <div class="field"><label>Age <span>1-120</span></label><input name="age" type="number" min="1" max="120" value="22" required /></div>
      <div class="field"><label>Donor type <span>Required</span></label><select name="donor_type"><option>Individual</option><option>Organization</option><option>Family</option><option>Anonymous</option></select></div>
      <div class="button-row"><button class="btn green" type="submit">Register donor</button></div>
    </form>`;
}

function adminLoginHtml() {
  return `
    <form class="form-block" id="admin-login-form">
      <h3>Admin login</h3>
      <p>Open dashboard, campaigns, reports, donations, and allocations.</p>
      <div class="field"><label>Email <span>Supabase Auth</span></label><input name="email" type="email" value="admin@example.com" required /></div>
      <div class="field"><label>Password <span>Required</span></label><input name="password" type="password" value="admin123456" required /></div>
      <div class="hint-box">Create this user in Supabase Auth, then insert admin profile as explained in <code>SUPABASE_SETUP.md</code>.</div>
      <div class="button-row"><button class="btn primary" type="submit">Login as admin</button></div>
    </form>`;
}

async function handleDonorLogin(event) {
  event.preventDefault();
  const form = new FormData(event.currentTarget);
  const donor_code = String(form.get('donor_code')).trim().toUpperCase();
  const email = String(form.get('email')).trim().toLowerCase();
  const { data, error } = await supabase.rpc('verify_donor', { p_donor_code: donor_code, p_email: email });
  if (error || !data?.length) {
    toast(error?.message || 'Donor verification failed.', 'error');
    return;
  }
  state.role = 'donor';
  state.donor = { donor_code, email, name: data[0].donor_name };
  state.page = 'donor-statement';
  render();
}

async function handleDonorRegister(event) {
  event.preventDefault();
  const form = new FormData(event.currentTarget);
  const payload = {
    p_name: String(form.get('name')).trim(),
    p_email: String(form.get('email')).trim().toLowerCase(),
    p_contact: String(form.get('contact')).trim(),
    p_address: String(form.get('address')).trim(),
    p_donor_type: String(form.get('donor_type')).trim(),
    p_age: Number(form.get('age')),
  };
  const { data, error } = await supabase.rpc('register_donor', payload);
  if (error) {
    toast(error.message, 'error');
    return;
  }
  const donor_code = data?.[0]?.donor_code;
  toast(`Donor registered. Generated ID: ${donor_code}`);
  state.role = 'donor';
  state.donor = { donor_code, email: payload.p_email, name: payload.p_name };
  state.page = 'donor-statement';
  render();
}

async function handleAdminLogin(event) {
  event.preventDefault();
  const form = new FormData(event.currentTarget);
  const email = String(form.get('email')).trim().toLowerCase();
  const password = String(form.get('password'));
  const { data, error } = await supabase.auth.signInWithPassword({ email, password });
  if (error) {
    toast(error.message, 'error');
    return;
  }
  const { data: profile, error: profileError } = await supabase.from('profiles').select('*').eq('id', data.user.id).maybeSingle();
  if (profileError || profile?.role !== 'admin') {
    await supabase.auth.signOut();
    toast('This user is not marked as admin in profiles table.', 'error');
    return;
  }
  state.role = 'admin';
  state.adminUser = data.user;
  state.page = 'dashboard';
  await loadAdminData();
  render();
}

async function loadAdminData() {
  const tables = ['donors', 'campaigns', 'beneficiaries', 'donations', 'allocations', 'reports'];
  const results = await Promise.all(tables.map((t) => supabase.from(t).select('*').order('created_at', { ascending: true })));
  results.forEach((result, index) => {
    if (result.error) throw result.error;
    state.data[tables[index]] = result.data || [];
  });
}

function summary() {
  const totalDonations = state.data.donations.reduce((sum, d) => sum + Number(d.amount || 0), 0);
  const totalAllocations = state.data.allocations.reduce((sum, a) => sum + Number(a.amount || 0), 0);
  return { totalDonations, totalAllocations, balance: totalDonations - totalAllocations };
}

function render() {
  if (!state.role) return renderLogin();
  if (state.role === 'donor') {
    renderDonorAsync();
    return;
  }
  app.innerHTML = `
    <div class="shell">
      ${brandStrip()}
      <section class="app-layout">
        ${sideNav()}
        <main class="page-stack">${mainPage()}</main>
      </section>
      ${modalHtml()}
    </div>`;
  bindEvents();
}

function sideNav() {
  if (state.role === 'admin') {
    return `
      <aside class="side-nav">
        <div class="role-card"><strong>Admin Area</strong><span>Management pages unlocked.</span></div>
        <div class="nav-title">Navigation</div>
        ${navButton('dashboard', 'Dashboard')}
        ${navButton('campaigns', 'Campaigns')}
        ${navButton('reports', 'Reports')}
        <button class="btn danger logout" data-action="logout">Logout</button>
      </aside>`;
  }
  return `
    <aside class="side-nav">
      <div class="role-card"><strong>Donor Portal</strong><span>Read-only verified donor pages.</span></div>
      <div class="nav-title">Navigation</div>
      ${navButton('donor-statement', 'My Statement')}
      ${navButton('donor-impact', 'Campaign Impact')}
      <button class="btn danger logout" data-action="logout">Logout</button>
    </aside>`;
}

function navButton(page, label) {
  return `<button class="nav-button ${state.page === page ? 'active' : ''}" data-page="${page}">${label}</button>`;
}

function mainPage() {
  if (state.role === 'admin') {
    if (state.page === 'campaigns') return campaignsPage();
    if (state.page === 'reports') return reportsPage();
    return dashboardPage();
  }
  if (state.page === 'donor-impact') return donorImpactPage();
  return donorStatementPage();
}

function pageHead(eyebrow, title, description, actions = '') {
  return `
    <div class="page-card page-head">
      <div><span class="eyebrow">${eyebrow}</span><h2>${title}</h2><p>${description}</p></div>
      <div class="head-actions">${actions}</div>
    </div>`;
}

function metricGrid(items) {
  return `<div class="metrics">${items.map(([label, value, help]) => `
    <article class="metric-card"><div class="metric-label">${label}</div><div class="metric-value">${value}</div><div class="metric-help">${help}</div></article>`).join('')}</div>`;
}

function dashboardPage() {
  const s = summary();
  const active = state.data.campaigns.filter((c) => c.status === 'Active');
  const underFunded = [...active].sort((a, b) => progress(a) - progress(b))[0];
  return `
    ${pageHead('Admin overview', 'Clear financial snapshot.', 'Only the most important metrics are shown first. Campaigns and reports have their own pages.', '<button class="btn primary" data-modal="donation">Record donation</button><button class="btn" data-modal="allocation">Allocate funds</button>')}
    ${metricGrid([
      ['Total received', money(s.totalDonations), `${state.data.donations.length} donation records across campaigns.`],
      ['Funds allocated', money(s.totalAllocations), `${state.data.allocations.length} beneficiary allocation records.`],
      ['Available balance', money(s.balance), 'Remaining amount for upcoming distributions.'],
    ])}
    <section class="panel-card"><div class="panel-title"><div><h3>Priority actions</h3><p>Only important alerts are shown here so the dashboard stays useful.</p></div><span class="chip amber">Needs attention</span></div>
      <div class="priority-grid">
        <div class="priority-card"><strong>${underFunded ? `${escapeHtml(underFunded.title)} is under-funded` : 'No campaign found'}</strong><p>${underFunded ? `Only ${progress(underFunded)}% funded. Highlight this campaign to donors before other active drives.` : 'Create a campaign to begin tracking progress.'}</p></div>
        <div class="priority-card"><strong>Monthly report is ready</strong><p>Generate the latest report before submission screenshots so totals match the evidence.</p></div>
      </div>
    </section>
    <section class="panel-card"><div class="panel-title"><div><h3>Campaign progress</h3><p>Readable progress list instead of many competing charts.</p></div><span class="chip blue">${active.length} active</span></div><div class="campaign-list">${campaignRows(active.slice(0, 3))}</div></section>
    <section class="panel-card"><div class="panel-title"><div><h3>Recent activity</h3><p>New donations and allocations appear here after using admin actions.</p></div><span class="chip">Live data</span></div><div class="activity-list">${activityRows()}</div></section>`;
}

function campaignsPage() {
  const list = state.filterActive ? state.data.campaigns.filter((c) => c.status === 'Active') : state.data.campaigns;
  return `
    ${pageHead('Campaign management', 'Campaigns with status and progress.', 'Create campaigns and filter only active campaigns when preparing the demo.', '<button class="btn primary" data-modal="campaign">Create campaign</button><button class="btn" data-action="toggle-active">' + (state.filterActive ? 'Show all' : 'Filter active') + '</button>')}
    <section class="panel-card"><div class="panel-title"><div><h3>Campaign list</h3><p>Target, collected, allocated, available amount, and status.</p></div><span class="chip blue">${state.filterActive ? 'Active only' : 'Showing all'}</span></div><div class="campaign-list">${campaignRows(list)}</div></section>`;
}

function reportsPage() {
  const s = summary();
  return `
    ${pageHead('Reports', 'Transparency reports for submission and viva.', 'Generate monthly reports and use browser print/save as PDF.', '<button class="btn primary" data-action="generate-report">Generate monthly report</button><button class="btn" data-action="print-report">Export PDF</button>')}
    ${metricGrid([
      ['Current donations', money(s.totalDonations), 'Report period uses current saved data.'],
      ['Current allocations', money(s.totalAllocations), 'Aid distributed to beneficiaries.'],
      ['Current balance', money(s.balance), 'Remaining balance after allocation.'],
    ])}
    <section class="panel-card"><div class="panel-title"><div><h3>Generated reports</h3><p>Clear table for screenshots and report evidence.</p></div><span class="chip">${state.data.reports.length} reports</span></div><div class="table-wrap"><table><thead><tr><th>Report ID</th><th>Type</th><th>Period</th><th>Donations</th><th>Allocations</th><th>Balance</th><th>Status</th></tr></thead><tbody>${state.data.reports.map((r) => `<tr><td>${r.report_code}</td><td>${r.report_type}</td><td>${escapeHtml(r.period)}</td><td>${money(r.total_donations)}</td><td>${money(r.total_allocations)}</td><td>${money(r.remaining_balance)}</td><td><span class="status">Ready</span></td></tr>`).join('')}</tbody></table></div></section>`;
}

function campaignRows(campaigns) {
  if (!campaigns.length) return '<p class="security-note">No campaign records found.</p>';
  return campaigns.map((c) => `
    <div class="campaign-row"><div><h4>${escapeHtml(c.title)} <span class="${c.status === 'Active' ? 'status' : 'status warn'}">${c.status}</span></h4><p>${escapeHtml(c.description)} · Target ${moneyFull(c.target_amount)} · Collected ${moneyFull(c.collected_amount)} · Allocated ${moneyFull(c.allocated_amount)}</p><div class="progress-line"><span style="--w:${Math.min(progress(c), 100)}%"></span></div></div><div class="campaign-money"><strong>${money(available(c))}</strong><span>${progress(c)}% funded · available</span></div></div>`).join('');
}

function available(c) { return Number(c.collected_amount || 0) - Number(c.allocated_amount || 0); }
function progress(c) { return c.target_amount > 0 ? Math.round((Number(c.collected_amount || 0) / Number(c.target_amount)) * 100) : 0; }

function activityRows() {
  const items = [];
  state.data.donations.slice(-5).forEach((d) => items.push({ kind: 'DN', title: `${nameById('donors', d.donor_id)} donated to ${nameById('campaigns', d.campaign_id, 'title')}`, detail: `${d.payment_method} · ${d.date}`, amount: d.amount }));
  state.data.allocations.slice(-5).forEach((a) => items.push({ kind: 'AL', title: `Funds allocated to ${nameById('beneficiaries', a.beneficiary_id)}`, detail: `${nameById('campaigns', a.campaign_id, 'title')} · ${a.purpose}`, amount: a.amount }));
  return items.slice(-6).reverse().map((a) => `<div class="activity-card"><div class="${a.kind === 'DN' ? 'activity-icon green' : 'activity-icon'}">${a.kind}</div><div><strong>${escapeHtml(a.title)}</strong><span>${escapeHtml(a.detail)}</span></div><div class="activity-amount">${moneyFull(a.amount)}</div></div>`).join('') || '<p class="security-note">No activity yet.</p>';
}

function nameById(table, id, field = 'name') {
  return state.data[table].find((x) => x.id === id)?.[field] || id;
}

async function donorStatementPage() {
  return '<section class="panel-card"><div class="panel-title"><div><h3>Loading statement...</h3></div></div></section>';
}

function donorStatementPlaceholder(statement, donations) {
  return `
    ${pageHead('Donor portal', 'My donation statement.', 'Read-only view. Donors see their own records, not admin CRUD tools.')}
    <div class="statement-grid"><div class="statement-card"><h3>${escapeHtml(statement.donor_name)}</h3><p>Verified donor · Email protected: <strong>${maskEmail(statement.email)}</strong></p><div class="statement-metrics"><div class="statement-metric"><span>Total donated</span><strong>${money(statement.total_donated)}</strong></div><div class="statement-metric"><span>Donations</span><strong>${statement.donation_count}</strong></div><div class="statement-metric"><span>Campaigns</span><strong>${statement.campaigns_supported}</strong></div></div></div></div>
    <section class="panel-card"><div class="panel-title"><div><h3>Donation history</h3><p>Only this donor's records are shown.</p></div></div><div class="table-wrap"><table><thead><tr><th>Date</th><th>Campaign</th><th>Method</th><th>Amount</th></tr></thead><tbody>${donations.length ? donations.map((d) => `<tr><td>${d.date}</td><td>${escapeHtml(d.campaign_title)}</td><td>${escapeHtml(d.payment_method)}</td><td>${moneyFull(d.amount)}</td></tr>`).join('') : '<tr><td colspan="4">No donations recorded yet.</td></tr>'}</tbody></table></div></section>`;
}

function donorImpactPlaceholder(campaigns, allocations) {
  return `
    ${pageHead('Campaign impact', 'Where supported campaign funds went.', 'Donors see campaign-level allocation transparency without private contact information.')}
    <section class="panel-card"><div class="panel-title"><div><h3>Supported campaigns</h3><p>Campaigns connected to this donor donation history.</p></div><span class="chip blue">${campaigns.length} supported</span></div><div class="campaign-list">${campaignRows(campaigns.map((c) => ({ ...c, target_amount: c.target_amount, collected_amount: c.collected_amount, allocated_amount: c.allocated_amount, status: c.status })))}</div></section>
    <section class="panel-card"><div class="panel-title"><div><h3>Allocation evidence</h3><p>Beneficiary contact information is hidden from donor view.</p></div></div><div class="table-wrap"><table><thead><tr><th>Campaign</th><th>Beneficiary</th><th>Date</th><th>Purpose</th><th>Amount</th></tr></thead><tbody>${allocations.length ? allocations.map((a) => `<tr><td>${escapeHtml(a.campaign_title)}</td><td>${escapeHtml(a.beneficiary_name)}</td><td>${a.date}</td><td>${escapeHtml(a.purpose)}</td><td>${moneyFull(a.amount)}</td></tr>`).join('') : '<tr><td colspan="5">No allocation evidence available yet.</td></tr>'}</tbody></table></div></section>`;
}

function donorShell(content) {
  return content;
}

function modalHtml() {
  if (!state.modal) return '';
  const titleMap = { donation: 'Record donation', allocation: 'Allocate funds', campaign: 'Create campaign' };
  return `<div class="modal-backdrop" data-action="close-modal"><div class="modal" onclick="event.stopPropagation()"><div class="modal-head"><div><h3>${titleMap[state.modal]}</h3><p>Data will be saved in Supabase.</p></div><button class="modal-close" data-action="close-modal">×</button></div>${state.modal === 'donation' ? donationForm() : state.modal === 'allocation' ? allocationForm() : campaignForm()}</div></div>`;
}

function donationForm() {
  return `<form id="donation-form"><div class="form-grid"><div class="field"><label>Donor</label><select name="donor_id">${state.data.donors.map((d) => `<option value="${d.id}">${d.donor_code} - ${escapeHtml(d.name)}</option>`).join('')}</select></div><div class="field"><label>Campaign</label><select name="campaign_id">${state.data.campaigns.filter((c) => c.status === 'Active').map((c) => `<option value="${c.id}">${c.campaign_code} - ${escapeHtml(c.title)}</option>`).join('')}</select></div><div class="field"><label>Amount</label><input name="amount" type="number" min="1" value="5000" /></div><div class="field"><label>Method</label><input name="payment_method" value="Cash" /></div><div class="field full-span"><label>Note</label><input name="note" value="General donation" /></div></div><div class="button-row"><button class="btn primary" type="submit">Save donation</button><button class="btn" type="button" data-action="close-modal">Cancel</button></div></form>`;
}

function allocationForm() {
  return `<form id="allocation-form"><div class="form-grid"><div class="field"><label>Beneficiary</label><select name="beneficiary_id">${state.data.beneficiaries.map((b) => `<option value="${b.id}">${b.beneficiary_code} - ${escapeHtml(b.name)}</option>`).join('')}</select></div><div class="field"><label>Campaign</label><select name="campaign_id">${state.data.campaigns.filter((c) => c.status === 'Active').map((c) => `<option value="${c.id}">${c.campaign_code} - ${escapeHtml(c.title)}</option>`).join('')}</select></div><div class="field"><label>Amount</label><input name="amount" type="number" min="1" value="10000" /></div><div class="field"><label>Purpose</label><input name="purpose" value="Food package" /></div></div><div class="button-row"><button class="btn primary" type="submit">Save allocation</button><button class="btn" type="button" data-action="close-modal">Cancel</button></div></form>`;
}

function campaignForm() {
  return `<form id="campaign-form"><div class="form-grid"><div class="field full-span"><label>Campaign title</label><input name="title" value="Winter Clothing Drive" /></div><div class="field"><label>Target amount</label><input name="target_amount" type="number" min="1" value="150000" /></div><div class="field"><label>Status</label><select name="status"><option>Active</option><option>Paused</option></select></div><div class="field"><label>Start date</label><input name="start_date" type="date" value="2026-06-16" /></div><div class="field"><label>End date</label><input name="end_date" type="date" value="2026-07-16" /></div><div class="field full-span"><label>Description</label><input name="description" value="Warm clothing support for low-income families" /></div></div><div class="button-row"><button class="btn primary" type="submit">Create campaign</button><button class="btn" type="button" data-action="close-modal">Cancel</button></div></form>`;
}

function bindEvents() {
  document.querySelectorAll('[data-page]').forEach((btn) => btn.addEventListener('click', async () => {
    state.page = btn.dataset.page;
    if (state.role === 'donor') {
      await renderDonorAsync();
    } else {
      render();
    }
  }));
  document.querySelectorAll('[data-modal]').forEach((btn) => btn.addEventListener('click', () => { state.modal = btn.dataset.modal; render(); }));
  document.querySelectorAll('[data-action="close-modal"]').forEach((btn) => btn.addEventListener('click', () => { state.modal = null; render(); }));
  const logout = document.querySelector('[data-action="logout"]');
  if (logout) logout.addEventListener('click', handleLogout);
  const toggle = document.querySelector('[data-action="toggle-active"]');
  if (toggle) toggle.addEventListener('click', () => { state.filterActive = !state.filterActive; render(); });
  const gen = document.querySelector('[data-action="generate-report"]');
  if (gen) gen.addEventListener('click', generateReport);
  const print = document.querySelector('[data-action="print-report"]');
  if (print) print.addEventListener('click', () => window.print());
  const donation = document.querySelector('#donation-form');
  if (donation) donation.addEventListener('submit', submitDonation);
  const allocation = document.querySelector('#allocation-form');
  if (allocation) allocation.addEventListener('submit', submitAllocation);
  const campaign = document.querySelector('#campaign-form');
  if (campaign) campaign.addEventListener('submit', submitCampaign);
}

async function renderDonorAsync() {
  app.innerHTML = `<div class="shell">${brandStrip()}<section class="app-layout">${sideNav()}<main class="page-stack"><section class="panel-card">Loading...</section></main></section></div>`;
  const params = { p_donor_code: state.donor.donor_code, p_email: state.donor.email };
  if (state.page === 'donor-impact') {
    const [campaigns, allocations] = await Promise.all([
      supabase.rpc('donor_campaigns', params),
      supabase.rpc('donor_allocations', params),
    ]);
    app.innerHTML = `<div class="shell">${brandStrip()}<section class="app-layout">${sideNav()}<main class="page-stack">${donorImpactPlaceholder(campaigns.data || [], allocations.data || [])}</main></section></div>`;
  } else {
    const [statement, donations] = await Promise.all([
      supabase.rpc('donor_statement', params),
      supabase.rpc('donor_donations', params),
    ]);
    app.innerHTML = `<div class="shell">${brandStrip()}<section class="app-layout">${sideNav()}<main class="page-stack">${donorStatementPlaceholder((statement.data || [])[0] || {}, donations.data || [])}</main></section></div>`;
  }
  bindEvents();
}

async function submitDonation(event) {
  event.preventDefault();
  const form = new FormData(event.currentTarget);
  const payload = {
    donor_id: form.get('donor_id'),
    campaign_id: form.get('campaign_id'),
    amount: Number(form.get('amount')),
    date: new Date().toISOString().slice(0, 10),
    payment_method: String(form.get('payment_method')).trim(),
    note: String(form.get('note')).trim(),
  };
  const { error } = await supabase.from('donations').insert(payload);
  if (error) return toast(error.message, 'error');
  toast('Donation recorded.');
  state.modal = null;
  await loadAdminData();
  render();
}

async function submitAllocation(event) {
  event.preventDefault();
  const form = new FormData(event.currentTarget);
  const payload = {
    beneficiary_id: form.get('beneficiary_id'),
    campaign_id: form.get('campaign_id'),
    amount: Number(form.get('amount')),
    date: new Date().toISOString().slice(0, 10),
    purpose: String(form.get('purpose')).trim(),
    approved_by: state.adminUser?.email || 'Admin',
  };
  const { error } = await supabase.from('allocations').insert(payload);
  if (error) return toast(error.message, 'error');
  toast('Funds allocated.');
  state.modal = null;
  await loadAdminData();
  render();
}

async function submitCampaign(event) {
  event.preventDefault();
  const form = new FormData(event.currentTarget);
  const payload = {
    title: String(form.get('title')).trim(),
    description: String(form.get('description')).trim(),
    target_amount: Number(form.get('target_amount')),
    start_date: form.get('start_date'),
    end_date: form.get('end_date'),
    status: form.get('status'),
  };
  const { error } = await supabase.from('campaigns').insert(payload);
  if (error) return toast(error.message, 'error');
  toast('Campaign created.');
  state.modal = null;
  state.page = 'campaigns';
  await loadAdminData();
  render();
}

async function generateReport() {
  const { error } = await supabase.rpc('generate_monthly_report', { p_month: '2026-06' });
  if (error) return toast(error.message, 'error');
  toast('Monthly report generated.');
  await loadAdminData();
  render();
}

async function handleLogout() {
  await supabase.auth.signOut();
  state = { role: null, tab: 'donor', adminUser: null, donor: null, page: 'dashboard', modal: null, filterActive: false, data: { donors: [], campaigns: [], beneficiaries: [], donations: [], allocations: [], reports: [] } };
  renderLogin();
}

function maskEmail(email = '') {
  if (!email.includes('@')) return 'hidden';
  const [name, domain] = email.split('@');
  return `${name[0] || ''}***${name.length > 2 ? name[name.length - 1] : ''}@${domain}`;
}

init().catch((error) => {
  console.error(error);
  app.innerHTML = `<div class="shell">${brandStrip()}<div class="login-card"><h2>App error</h2><p class="subtitle">${escapeHtml(error.message)}</p></div></div>`;
});
