-- Charity & Donation Management System - Supabase PostgreSQL schema
-- IMPORTANT: This script resets the project tables. Run it in a fresh Supabase project
-- or only if you are okay with dropping existing project data.

create extension if not exists pgcrypto;

-- Drop old objects for a clean student-project setup.
drop table if exists reports cascade;
drop table if exists allocations cascade;
drop table if exists donations cascade;
drop table if exists beneficiaries cascade;
drop table if exists campaigns cascade;
drop table if exists donors cascade;
drop table if exists profiles cascade;
drop sequence if exists donor_code_seq cascade;
drop sequence if exists campaign_code_seq cascade;
drop sequence if exists beneficiary_code_seq cascade;
drop sequence if exists donation_code_seq cascade;
drop sequence if exists allocation_code_seq cascade;
drop sequence if exists report_code_seq cascade;

create sequence donor_code_seq start 1;
create sequence campaign_code_seq start 1;
create sequence beneficiary_code_seq start 1;
create sequence donation_code_seq start 1;
create sequence allocation_code_seq start 1;
create sequence report_code_seq start 1;

create table profiles (
  id uuid primary key references auth.users(id) on delete cascade,
  email text not null,
  role text not null check (role in ('admin', 'donor')) default 'donor',
  created_at timestamptz not null default now()
);

create table donors (
  id uuid primary key default gen_random_uuid(),
  donor_code text unique,
  name text not null,
  age int check (age between 1 and 120),
  contact text not null,
  email text not null,
  address text not null,
  donor_type text not null default 'Individual',
  total_donated numeric(12,2) not null default 0 check (total_donated >= 0),
  created_at timestamptz not null default now()
);

create table campaigns (
  id uuid primary key default gen_random_uuid(),
  campaign_code text unique,
  title text not null,
  description text not null,
  target_amount numeric(12,2) not null check (target_amount > 0),
  collected_amount numeric(12,2) not null default 0 check (collected_amount >= 0),
  allocated_amount numeric(12,2) not null default 0 check (allocated_amount >= 0),
  start_date date not null default current_date,
  end_date date not null default current_date,
  status text not null default 'Active' check (status in ('Active', 'Paused', 'Completed', 'Closed')),
  created_at timestamptz not null default now(),
  constraint campaign_date_order check (end_date >= start_date)
);

create table beneficiaries (
  id uuid primary key default gen_random_uuid(),
  beneficiary_code text unique,
  name text not null,
  age int check (age between 1 and 120),
  contact text not null,
  address text not null,
  family_size int not null default 1 check (family_size between 1 and 50),
  need_type text not null,
  campaign_id uuid not null references campaigns(id) on delete restrict,
  total_received numeric(12,2) not null default 0 check (total_received >= 0),
  created_at timestamptz not null default now()
);

create table donations (
  id uuid primary key default gen_random_uuid(),
  donation_code text unique,
  donor_id uuid not null references donors(id) on delete restrict,
  campaign_id uuid not null references campaigns(id) on delete restrict,
  amount numeric(12,2) not null check (amount > 0),
  date date not null default current_date,
  payment_method text not null,
  note text default '',
  created_at timestamptz not null default now()
);

create table allocations (
  id uuid primary key default gen_random_uuid(),
  allocation_code text unique,
  beneficiary_id uuid not null references beneficiaries(id) on delete restrict,
  campaign_id uuid not null references campaigns(id) on delete restrict,
  amount numeric(12,2) not null check (amount > 0),
  date date not null default current_date,
  purpose text not null,
  approved_by text not null default 'Admin',
  created_at timestamptz not null default now()
);

create table reports (
  id uuid primary key default gen_random_uuid(),
  report_code text unique,
  report_type text not null check (report_type in ('Monthly', 'Campaign', 'Combined')),
  period text not null,
  total_donations numeric(12,2) not null default 0,
  total_allocations numeric(12,2) not null default 0,
  remaining_balance numeric(12,2) not null default 0,
  generated_date date not null default current_date,
  remarks text default '',
  created_at timestamptz not null default now()
);

-- Code assignment triggers
create or replace function set_donor_code() returns trigger language plpgsql as $$
begin
  if new.donor_code is null then
    new.donor_code := 'DNR' || lpad(nextval('donor_code_seq')::text, 3, '0');
  end if;
  return new;
end;
$$;

create or replace function set_campaign_code() returns trigger language plpgsql as $$
begin
  if new.campaign_code is null then
    new.campaign_code := 'CAM' || lpad(nextval('campaign_code_seq')::text, 3, '0');
  end if;
  return new;
end;
$$;

create or replace function set_beneficiary_code() returns trigger language plpgsql as $$
begin
  if new.beneficiary_code is null then
    new.beneficiary_code := 'BEN' || lpad(nextval('beneficiary_code_seq')::text, 3, '0');
  end if;
  return new;
end;
$$;

create or replace function set_donation_code() returns trigger language plpgsql as $$
begin
  if new.donation_code is null then
    new.donation_code := 'DNT' || lpad(nextval('donation_code_seq')::text, 3, '0');
  end if;
  return new;
end;
$$;

create or replace function set_allocation_code() returns trigger language plpgsql as $$
begin
  if new.allocation_code is null then
    new.allocation_code := 'ALC' || lpad(nextval('allocation_code_seq')::text, 3, '0');
  end if;
  return new;
end;
$$;

create or replace function set_report_code() returns trigger language plpgsql as $$
begin
  if new.report_code is null then
    new.report_code := 'RPT' || lpad(nextval('report_code_seq')::text, 3, '0');
  end if;
  return new;
end;
$$;

create trigger trg_donor_code before insert on donors for each row execute function set_donor_code();
create trigger trg_campaign_code before insert on campaigns for each row execute function set_campaign_code();
create trigger trg_beneficiary_code before insert on beneficiaries for each row execute function set_beneficiary_code();
create trigger trg_donation_code before insert on donations for each row execute function set_donation_code();
create trigger trg_allocation_code before insert on allocations for each row execute function set_allocation_code();
create trigger trg_report_code before insert on reports for each row execute function set_report_code();

-- Totals and validation triggers
create or replace function apply_donation_totals() returns trigger language plpgsql as $$
begin
  update donors set total_donated = total_donated + new.amount where id = new.donor_id;
  update campaigns set collected_amount = collected_amount + new.amount where id = new.campaign_id;
  return new;
end;
$$;

create or replace function validate_and_apply_allocation() returns trigger language plpgsql as $$
declare
  available numeric(12,2);
  beneficiary_campaign uuid;
begin
  select campaign_id into beneficiary_campaign from beneficiaries where id = new.beneficiary_id;
  if beneficiary_campaign is null then
    raise exception 'Beneficiary does not exist.';
  end if;
  if beneficiary_campaign <> new.campaign_id then
    raise exception 'Beneficiary is not linked to this campaign.';
  end if;

  select collected_amount - allocated_amount into available from campaigns where id = new.campaign_id;
  if available is null then
    raise exception 'Campaign does not exist.';
  end if;
  if new.amount > available then
    raise exception 'Allocation exceeds available campaign balance. Available: %', available;
  end if;

  update beneficiaries set total_received = total_received + new.amount where id = new.beneficiary_id;
  update campaigns set allocated_amount = allocated_amount + new.amount where id = new.campaign_id;
  return new;
end;
$$;

create trigger trg_apply_donation after insert on donations for each row execute function apply_donation_totals();
create trigger trg_apply_allocation before insert on allocations for each row execute function validate_and_apply_allocation();

-- Admin role helper
create or replace function is_admin() returns boolean
language sql stable security definer set search_path = public as $$
  select exists(select 1 from profiles where id = auth.uid() and role = 'admin');
$$;

-- Donor-facing RPC functions. These are SECURITY DEFINER so donors can view only sanitized data by donor_code + email.
create or replace function register_donor(
  p_name text,
  p_email text,
  p_contact text,
  p_address text,
  p_donor_type text default 'Individual',
  p_age int default 18
) returns table(donor_code text)
language plpgsql security definer set search_path = public as $$
begin
  if length(trim(p_name)) < 2 then raise exception 'Name is required.'; end if;
  if position('@' in p_email) = 0 then raise exception 'Valid email is required.'; end if;
  if length(regexp_replace(coalesce(p_contact,''), '\D', '', 'g')) < 7 then raise exception 'Valid contact is required.'; end if;

  return query
  insert into donors(name, age, contact, email, address, donor_type)
  values(trim(p_name), p_age, trim(p_contact), lower(trim(p_email)), trim(p_address), coalesce(nullif(trim(p_donor_type), ''), 'Individual'))
  returning donors.donor_code;
end;
$$;

create or replace function verify_donor(p_donor_code text, p_email text)
returns table(donor_code text, donor_name text, masked_email text)
language sql stable security definer set search_path = public as $$
  select d.donor_code, d.name,
         left(d.email, 1) || '***' || right(split_part(d.email,'@',1), 1) || '@' || split_part(d.email,'@',2) as masked_email
  from donors d
  where d.donor_code = upper(trim(p_donor_code)) and lower(d.email) = lower(trim(p_email));
$$;

create or replace function donor_statement(p_donor_code text, p_email text)
returns table(donor_code text, donor_name text, email text, donor_type text, total_donated numeric, donation_count bigint, campaigns_supported bigint)
language sql stable security definer set search_path = public as $$
  select d.donor_code,
         d.name,
         d.email,
         d.donor_type,
         d.total_donated,
         count(distinct dn.id) as donation_count,
         count(distinct dn.campaign_id) as campaigns_supported
  from donors d
  left join donations dn on dn.donor_id = d.id
  where d.donor_code = upper(trim(p_donor_code)) and lower(d.email) = lower(trim(p_email))
  group by d.id;
$$;

create or replace function donor_donations(p_donor_code text, p_email text)
returns table(date date, campaign_title text, payment_method text, amount numeric, note text)
language sql stable security definer set search_path = public as $$
  select dn.date, c.title, dn.payment_method, dn.amount, dn.note
  from donors d
  join donations dn on dn.donor_id = d.id
  join campaigns c on c.id = dn.campaign_id
  where d.donor_code = upper(trim(p_donor_code)) and lower(d.email) = lower(trim(p_email))
  order by dn.date desc, dn.created_at desc;
$$;

create or replace function donor_campaigns(p_donor_code text, p_email text)
returns table(campaign_code text, title text, description text, target_amount numeric, collected_amount numeric, allocated_amount numeric, available_balance numeric, progress_percent numeric, status text)
language sql stable security definer set search_path = public as $$
  select distinct c.campaign_code,
         c.title,
         c.description,
         c.target_amount,
         c.collected_amount,
         c.allocated_amount,
         (c.collected_amount - c.allocated_amount) as available_balance,
         case when c.target_amount > 0 then round((c.collected_amount / c.target_amount) * 100, 2) else 0 end as progress_percent,
         c.status
  from donors d
  join donations dn on dn.donor_id = d.id
  join campaigns c on c.id = dn.campaign_id
  where d.donor_code = upper(trim(p_donor_code)) and lower(d.email) = lower(trim(p_email))
  order by c.title;
$$;

create or replace function donor_allocations(p_donor_code text, p_email text)
returns table(campaign_title text, beneficiary_name text, date date, purpose text, amount numeric)
language sql stable security definer set search_path = public as $$
  select x.campaign_title, x.beneficiary_name, x.date, x.purpose, x.amount
  from (
    select distinct c.title as campaign_title, b.name as beneficiary_name, a.date, a.purpose, a.amount, a.created_at
    from donors d
    join donations dn on dn.donor_id = d.id
    join campaigns c on c.id = dn.campaign_id
    join allocations a on a.campaign_id = c.id
    join beneficiaries b on b.id = a.beneficiary_id
    where d.donor_code = upper(trim(p_donor_code)) and lower(d.email) = lower(trim(p_email))
  ) x
  order by x.date desc, x.created_at desc;
$$;


create or replace function public_campaigns()
returns table(campaign_code text, title text, description text, target_amount numeric, collected_amount numeric, allocated_amount numeric, available_balance numeric, progress_percent numeric, status text)
language sql stable security definer set search_path = public as $$
  select c.campaign_code,
         c.title,
         c.description,
         c.target_amount,
         c.collected_amount,
         c.allocated_amount,
         (c.collected_amount - c.allocated_amount) as available_balance,
         case when c.target_amount > 0 then round((c.collected_amount / c.target_amount) * 100, 2) else 0 end as progress_percent,
         c.status
  from campaigns c
  where c.status = 'Active'
  order by c.created_at desc;
$$;

create or replace function donor_make_donation(
  p_donor_code text,
  p_email text,
  p_campaign_code text,
  p_amount numeric,
  p_payment_method text,
  p_note text default ''
) returns table(donation_code text)
language plpgsql security definer set search_path = public as $$
declare
  v_donor_id uuid;
  v_campaign_id uuid;
begin
  if p_amount is null or p_amount <= 0 then
    raise exception 'Donation amount must be greater than zero.';
  end if;
  if length(trim(coalesce(p_payment_method, ''))) < 2 then
    raise exception 'Payment method is required.';
  end if;

  select id into v_donor_id
  from donors
  where donor_code = upper(trim(p_donor_code)) and lower(email) = lower(trim(p_email));

  if v_donor_id is null then
    raise exception 'Donor verification failed.';
  end if;

  select id into v_campaign_id
  from campaigns
  where campaign_code = upper(trim(p_campaign_code)) and status = 'Active';

  if v_campaign_id is null then
    raise exception 'Active campaign not found.';
  end if;

  return query
  insert into donations(donor_id, campaign_id, amount, date, payment_method, note)
  values(v_donor_id, v_campaign_id, p_amount, current_date, trim(p_payment_method), coalesce(p_note, ''))
  returning donations.donation_code;
end;
$$;

create or replace function generate_monthly_report(p_month text)
returns table(report_code text)
language plpgsql security definer set search_path = public as $$
declare
  total_d numeric(12,2);
  total_a numeric(12,2);
begin
  -- Allow Supabase SQL Editor / setup scripts where auth.uid() is null.
  -- Block normal authenticated non-admin users in the live app.
  if auth.uid() is not null and not is_admin() then
    raise exception 'Admin access required.';
  end if;

  if p_month !~ '^\d{4}-\d{2}$' then
    raise exception 'Month must be in YYYY-MM format.';
  end if;

  select coalesce(sum(amount),0) into total_d from donations where to_char(date, 'YYYY-MM') = p_month;
  select coalesce(sum(amount),0) into total_a from allocations where to_char(date, 'YYYY-MM') = p_month;

  return query
  insert into reports(report_type, period, total_donations, total_allocations, remaining_balance, generated_date, remarks)
  values('Monthly', p_month, total_d, total_a, total_d - total_a, current_date, 'Generated monthly transparency report.')
  returning reports.report_code;
end;
$$;

-- Row Level Security
alter table profiles enable row level security;
alter table donors enable row level security;
alter table campaigns enable row level security;
alter table beneficiaries enable row level security;
alter table donations enable row level security;
alter table allocations enable row level security;
alter table reports enable row level security;

create policy "profiles self or admin read" on profiles for select to authenticated using (id = auth.uid() or is_admin());
create policy "admins manage profiles" on profiles for all to authenticated using (is_admin()) with check (is_admin());

create policy "admins manage donors" on donors for all to authenticated using (is_admin()) with check (is_admin());
create policy "admins manage campaigns" on campaigns for all to authenticated using (is_admin()) with check (is_admin());
create policy "admins manage beneficiaries" on beneficiaries for all to authenticated using (is_admin()) with check (is_admin());
create policy "admins manage donations" on donations for all to authenticated using (is_admin()) with check (is_admin());
create policy "admins manage allocations" on allocations for all to authenticated using (is_admin()) with check (is_admin());
create policy "admins manage reports" on reports for all to authenticated using (is_admin()) with check (is_admin());

-- Explicit API grants for PostgREST/Supabase client access. RLS still controls what rows can be used.
grant usage on schema public to anon, authenticated;
grant select, insert, update, delete on profiles, donors, campaigns, beneficiaries, donations, allocations, reports to authenticated;
grant usage, select on all sequences in schema public to authenticated;

-- Allow RPC execution for anon and authenticated users.
grant execute on function register_donor(text, text, text, text, text, int) to anon, authenticated;
grant execute on function verify_donor(text, text) to anon, authenticated;
grant execute on function donor_statement(text, text) to anon, authenticated;
grant execute on function donor_donations(text, text) to anon, authenticated;
grant execute on function donor_campaigns(text, text) to anon, authenticated;
grant execute on function donor_allocations(text, text) to anon, authenticated;
grant execute on function public_campaigns() to anon, authenticated;
grant execute on function donor_make_donation(text, text, text, numeric, text, text) to anon, authenticated;
grant execute on function generate_monthly_report(text) to authenticated;

-- Seed demo data
insert into campaigns(title, description, target_amount, start_date, end_date, status) values
('Flood Relief 2026', 'Emergency food, shelter, and medicines for flood affected families.', 500000, '2026-06-01', '2026-07-31', 'Active'),
('Education Support', 'School fees, books, bags, and uniforms for deserving students.', 300000, '2026-06-05', '2026-08-30', 'Active'),
('Medical Aid Drive', 'Treatment and medicines for low-income patients.', 400000, '2026-06-10', '2026-09-15', 'Active');

insert into donors(name, age, contact, email, address, donor_type) values
('Ahmed Khan', 35, '+92 300 1111111', 'ahmed.khan@example.com', 'Lahore', 'Individual'),
('Fatima Foundation', 12, '+92 321 2222222', 'contact@fatimafoundation.org', 'Karachi', 'Organization'),
('Sara Ali', 28, '+92 333 3333333', 'sara.ali@example.com', 'Islamabad', 'Individual');

insert into beneficiaries(name, age, contact, address, family_size, need_type, campaign_id) values
('Bilal Family', 42, '+92 311 4444444', 'Sukkur', 6, 'Food and Shelter', (select id from campaigns where campaign_code='CAM001')),
('Hina Student', 16, '+92 312 5555555', 'Lahore', 1, 'Education Fee', (select id from campaigns where campaign_code='CAM002')),
('Zain Patient', 31, '+92 313 6666666', 'Karachi', 4, 'Medical Treatment', (select id from campaigns where campaign_code='CAM003'));

insert into donations(donor_id, campaign_id, amount, date, payment_method, note) values
((select id from donors where donor_code='DNR001'), (select id from campaigns where campaign_code='CAM001'), 75000, '2026-06-12', 'Bank Transfer', 'Initial flood relief support'),
((select id from donors where donor_code='DNR002'), (select id from campaigns where campaign_code='CAM001'), 125000, '2026-06-14', 'Cheque', 'Corporate charity donation'),
((select id from donors where donor_code='DNR002'), (select id from campaigns where campaign_code='CAM002'), 90000, '2026-06-15', 'Bank Transfer', 'Education sponsorship'),
((select id from donors where donor_code='DNR003'), (select id from campaigns where campaign_code='CAM003'), 55000, '2026-06-16', 'Cash', 'Medical aid donation');

insert into allocations(beneficiary_id, campaign_id, amount, date, purpose, approved_by) values
((select id from beneficiaries where beneficiary_code='BEN001'), (select id from campaigns where campaign_code='CAM001'), 60000, '2026-06-18', 'Ration bags and temporary shelter', 'Admin'),
((select id from beneficiaries where beneficiary_code='BEN002'), (select id from campaigns where campaign_code='CAM002'), 35000, '2026-06-20', 'School fee and books', 'Admin'),
((select id from beneficiaries where beneficiary_code='BEN003'), (select id from campaigns where campaign_code='CAM003'), 30000, '2026-06-21', 'Medicine purchase', 'Admin');

select * from generate_monthly_report('2026-06');
insert into reports(report_type, period, total_donations, total_allocations, remaining_balance, generated_date, remarks)
values('Campaign', 'CAM001 - Flood Relief 2026', 200000, 60000, 140000, current_date, 'Generated campaign-level transparency report.');

-- ADMIN SETUP AFTER RUNNING THIS SCHEMA:
-- 1. Go to Supabase Dashboard > Authentication > Users > Add user.
-- 2. Create admin user, for example: admin@example.com / admin123456
-- 3. Then run this SQL, replacing the email if needed:
-- insert into profiles(id, email, role)
-- select id, email, 'admin' from auth.users where email = 'admin@example.com'
-- on conflict (id) do update set role = 'admin';
