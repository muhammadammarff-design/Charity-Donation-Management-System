# Supabase + Vercel Setup Guide

This version uses **Vercel for the frontend** and **Supabase PostgreSQL for live data**.

The C++ OOP source code is still included in `cpp_core/` for viva, report, and OOP evaluation. The hosted dashboard uses Supabase because it gives reliable cloud persistence and avoids Streamlit HTML rendering issues.

---

## 1. Create Supabase project

1. Go to <https://supabase.com>
2. Create a new project.
3. Open the project dashboard.
4. Go to **SQL Editor**.
5. Open this file from the GitHub repo:

```text
supabase/schema.sql
```

6. Paste the full SQL into Supabase SQL Editor.
7. Click **Run**.

This creates:

- donors
- campaigns
- beneficiaries
- donations
- allocations
- reports
- profiles
- donor/admin RPC functions
- RLS policies
- demo data

---

## 2. Create admin user

Go to:

```text
Supabase Dashboard → Authentication → Users → Add user
```

Create this demo admin:

```text
Email: admin@example.com
Password: admin123456
Auto Confirm User: ON
```

Then go back to SQL Editor and run:

```sql
insert into profiles(id, email, role)
select id, email, 'admin'
from auth.users
where email = 'admin@example.com'
on conflict (id) do update set role = 'admin';
```

This marks that Auth user as admin.

---

## 3. Get Supabase keys

Go to:

```text
Project Settings → API
```

Copy:

```text
Project URL
anon public key
```

Do **not** use the service role key in frontend code.

---

## 4. Deploy on Vercel

1. Upload/push this project to GitHub.
2. Go to <https://vercel.com>
3. Click **Add New Project**.
4. Import your GitHub repo.
5. Vercel should detect Vite automatically.
6. Add Environment Variables:

```text
VITE_SUPABASE_URL = your Supabase Project URL
VITE_SUPABASE_ANON_KEY = your Supabase anon public key
```

7. Click **Deploy**.

---

## 5. Test logins

### Donor login

Use seeded donor:

```text
Donor ID: DNR001
Email: ahmed.khan@example.com
```

### Admin login

Use the admin user you created:

```text
Email: admin@example.com
Password: admin123456
```

---

## 6. What is dynamic?

All live web data is stored in Supabase:

- donor registration creates a donor row and generated donor code
- admin creates campaigns
- admin records donations
- admin allocates funds
- reports are generated in database
- donor portal reads only matching donor data

---

## 7. Important security note

The frontend uses the Supabase anon key, which is normal and safe only because Row Level Security and RPC functions control access.

Do not expose the service role key.

---

## 8. If admin sees empty/error data

Check that you created the admin user and inserted their profile row:

```sql
select * from profiles;
```

You should see:

```text
admin@example.com | admin
```

---

## 9. If donor login fails

Check seeded donors:

```sql
select donor_code, name, email from donors;
```

Expected demo row:

```text
DNR001 | Ahmed Khan | ahmed.khan@example.com
```
