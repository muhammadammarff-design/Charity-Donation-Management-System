# Project Tasks and Final Architecture

## Final Decision

The project now uses:

```text
C++ OOP Core + Vercel Frontend + Supabase PostgreSQL Backend
```

Reason:

- Streamlit caused custom HTML/CSS rendering issues.
- Vercel handles the polished frontend correctly.
- Supabase provides free PostgreSQL persistence, auth, APIs, and Row Level Security.
- C++ source code remains the OOP core for viva and project report.

---

## Completed Tasks

| # | Task | Status |
|---|---|---|
| 1 | Preserve C++ OOP core | Completed |
| 2 | Preserve C++ tests and evidence commands | Completed |
| 3 | Remove Streamlit-specific app files | Completed |
| 4 | Create Vercel/Vite frontend | Completed |
| 5 | Create Supabase PostgreSQL schema | Completed |
| 6 | Add donor registration with generated donor ID | Completed |
| 7 | Add donor login and read-only donor portal | Completed |
| 8 | Add admin login using Supabase Auth | Completed |
| 9 | Add dashboard, campaign, and report pages | Completed |
| 10 | Add donation recording | Completed |
| 11 | Add fund allocation with database validation | Completed |
| 12 | Add campaign creation and active filter | Completed |
| 13 | Add report generation | Completed |
| 14 | Add PDF/print export | Completed |
| 15 | Add setup guide for Supabase + Vercel | Completed |
| 16 | Clean irrelevant old files | Completed |

---

## Important Notes

- GitHub stores source code.
- Supabase stores live app data.
- Vercel hosts the frontend.
- C++ source remains for OOP viva and report.

---

## Verification

C++ verification commands:

```bash
python3 build_core.py
./build/charity_app seed
python3 tests/run_core_tests.py
./build/charity_app validation-demo
./build/charity_app oop-demo
```

Frontend verification:

```bash
npm install
npm run build
```
