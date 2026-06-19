import fs from 'node:fs';

const main = fs.readFileSync(new URL('../src/main.js', import.meta.url), 'utf8');
const css = fs.readFileSync(new URL('../src/styles.css', import.meta.url), 'utf8');
const schema = fs.readFileSync(new URL('../supabase/schema.sql', import.meta.url), 'utf8');
const index = fs.readFileSync(new URL('../index.html', import.meta.url), 'utf8');

function assert(condition, message) {
  if (!condition) throw new Error(message);
}

const bannedUiText = [
  'Create this user in Supabase Auth, then insert admin profile',
  'Data is saved in Supabase PostgreSQL. C++ OOP source remains',
  'Vercel + Supabase dashboard · C++ OOP source included',
];
for (const text of bannedUiText) {
  assert(!main.includes(text), `Banned UI text still present: ${text}`);
  assert(!index.includes(text), `Banned index text still present: ${text}`);
}

for (const required of [
  'checkSupabaseConnection',
  'Supabase not connected',
  'Connected to Supabase',
  'donor_make_donation',
  'public_campaigns',
  'Make Donation',
  'donor-donation-form',
  'handleDonorRegister',
  'handleAdminLogin',
]) {
  assert(main.includes(required), `Missing frontend feature: ${required}`);
}

for (const required of [
  'create or replace function public_campaigns()',
  'create or replace function donor_make_donation',
  'grant execute on function donor_make_donation',
  'grant execute on function public_campaigns',
  'validate_and_apply_allocation',
  'generate_monthly_report',
]) {
  assert(schema.includes(required), `Missing schema feature: ${required}`);
}

for (const required of [
  '@media (max-width: 1080px)',
  '@media (max-width: 720px)',
  'grid-template-columns: 1fr',
  'overflow-x: auto',
  'prefers-reduced-motion',
]) {
  assert(css.includes(required), `Missing responsive/accessibility CSS: ${required}`);
}

console.log('Web static crash tests passed.');
