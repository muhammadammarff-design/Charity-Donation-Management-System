# Charity & Donation Management System

**OOP Project | C++ Core + Vercel/Supabase Web Dashboard**

Team members:

- Ali Raza — 2540010
- Muhammad Ammar — 2540004
- Taha Ali — 2540008

---

## Project Overview

This project solves a real charity transparency problem: donors, campaigns, beneficiaries, donations, allocations, and reports are tracked in one system.

The repository contains two important parts:

1. **C++ OOP Core** in `cpp_core/`  
   Used for OOP evaluation: classes, inheritance, polymorphism, constructors, operator overloading, file handling, validation, reports.

2. **Web Dashboard** using **Vercel + Supabase**  
   Used as the polished hosted interface with real cloud data persistence.

---

## Why Vercel + Supabase?

Streamlit had HTML/CSS rendering issues for a custom dashboard. The final hosted version uses:

- **Vercel** for frontend hosting
- **Supabase PostgreSQL** for database, auth, and APIs
- **C++ source code** preserved for the OOP report and project assessment

Supabase stores live dynamic data. GitHub stores source code.

---

## Features

### Donor Portal

- Donor registration
- System-generated Donor ID, e.g. `DNR001`
- Donor login using Donor ID + email
- Donor can submit a donation record to an active campaign
- Donation statement
- Donation history
- Campaign impact view
- Allocation evidence without beneficiary private contact details

### Admin Dashboard

- Admin login through Supabase Auth
- Financial snapshot
- Priority actions
- Campaign progress
- Recent activity
- Record donation
- Allocate funds
- Create campaign
- Filter active campaigns
- Generate monthly reports
- Export/print report PDF

### Database

Supabase PostgreSQL tables:

- `donors`
- `campaigns`
- `beneficiaries`
- `donations`
- `allocations`
- `reports`
- `profiles`

Schema file:

```text
supabase/schema.sql
```

---

## OOP Concepts in C++ Core

| Concept | Implementation |
|---|---|
| Classes and Objects | `Person`, `Donor`, `Beneficiary`, `Campaign`, `Donation`, `FundAllocation`, `Report`, `FileManager`, `CharitySystem` |
| Encapsulation | Private/protected data members with getters/setters |
| Inheritance | `Donor` and `Beneficiary` inherit from `Person` |
| Polymorphism | `displayInfo()` is virtual in `Person` and overridden |
| Constructors | Default, parameterized, and overloaded constructors |
| Operator Overloading | `+`, `>`, `<` in meaningful classes |
| File Handling | C++ text-file storage under `data/` |

---

## Local C++ Testing

Compile C++ core:

```bash
python3 build_core.py
```

Seed demo data:

```bash
./build/charity_app seed
```

Run tests:

```bash
python3 tests/run_core_tests.py
```

Show OOP evidence:

```bash
./build/charity_app oop-demo
```

Show validation evidence:

```bash
./build/charity_app validation-demo
```

---

## Web App Local Setup

Install Node dependencies:

```bash
npm install
```

Create `.env` locally:

```text
VITE_SUPABASE_URL=your_supabase_project_url
VITE_SUPABASE_ANON_KEY=your_supabase_anon_key
```

Run locally:

```bash
npm run dev
```

Build:

```bash
npm run build
```

---

## Supabase Setup

Follow:

```text
SUPABASE_SETUP.md
```

Quick summary:

1. Create Supabase project.
2. Run `supabase/schema.sql` in SQL Editor.
3. Create admin user in Supabase Auth.
4. Insert admin profile row.
5. Add Supabase URL and anon key to Vercel environment variables.

---

## Demo Credentials

After running schema:

### Donor

```text
Donor ID: DNR001
Email: ahmed.khan@example.com
```

### Admin

Create this in Supabase Auth:

```text
Email: admin@example.com
Password: admin123456
```

Then insert admin profile as explained in `SUPABASE_SETUP.md`.

---

## Project Structure

```text
Charity-Donation-Management-System/
├── cpp_core/                 # C++ OOP source code
├── data/                     # C++ local text-file sample data
├── docs/                     # Report, UML, evidence
├── src/                      # Vercel frontend JavaScript/CSS
├── supabase/schema.sql       # Supabase PostgreSQL backend
├── index.html
├── package.json
├── vercel.json
├── build_core.py
├── tests/run_core_tests.py
├── SUPABASE_SETUP.md
└── README.md
```

---

## Submission

Submit:

- GitHub repository link
- Vercel live app link
- project report from `docs/report.html` or `docs/report.md`
- UML diagrams from `docs/uml/`
- evidence files from `docs/evidence/`
