# Submission and GitHub File Guide

This guide explains what should be added to GitHub, what should be kept for the oral/project presentation, and what should not be included in the final GitHub ZIP.

---

## 1. Add These Files/Folders to GitHub

These are the important source-code and documentation files.

```text
.env.example
.gitignore
README.md
SUPABASE_SETUP.md
SUBMISSION_FILE_GUIDE.md
Makefile
build_core.py
index.html
package.json
package-lock.json
vercel.json

cpp_core/
data/
docs/report.md
docs/VIVA_GUIDE.md
docs/evidence/
docs/screenshots/
docs/uml/
src/
supabase/
tests/
```

### Why these should be included

- `cpp_core/` contains the C++ OOP implementation.
- `src/` contains the hosted web dashboard frontend.
- `supabase/schema.sql` contains the database structure, functions, triggers, policies, and seed data.
- `data/` contains sample local text-file data for the C++ core.
- `tests/` contains C++ and web static test files.
- `docs/report.md` is the semester project report draft.
- `docs/uml/` and `docs/evidence/` support the report and project explanation.
- `.env.example` shows required environment variables without exposing real credentials.
- `.gitignore` protects generated files and secrets from being committed.

---

## 2. Keep These for Presentation / Assessment Support

These files are useful when explaining the project to the teacher or evaluator.

```text
docs/report.md
docs/VIVA_GUIDE.md
docs/uml/class_diagram.svg
docs/uml/use_case_diagram.svg
docs/evidence/
cpp_core/
data/
README.md
SUPABASE_SETUP.md
```

Also keep the final PDF report separately once it is generated:

```text
Charity_Donation_Management_System_Second_Semester_Project_Report.pdf
```

The PDF report can be submitted to the teacher or LMS if required, but it does not need to be committed to GitHub unless the teacher specifically asks for it there.

---

## 3. Do Not Add These to GitHub

These are generated, private, outdated, or unnecessary for the final GitHub submission.

```text
node_modules/
dist/
build/
.env
.env.local
*.zip
*.pdf
*.exe
*.o
*.out
__pycache__/
.venv/
.DS_Store
.vscode/
.idea/
```

### Old/optional files that should be left out of the final clean ZIP

```text
streamlit_app.py
requirements.txt
packages.txt
TASKS_AND_PLAN.md
design/
docs/report.html
```

Reason: the final hosted app is the Vercel + Supabase version, so old Streamlit/prototype files are not needed in the clean GitHub ZIP.

---

## 4. Final Clean ZIP Contents

The final clean ZIP should contain only the GitHub-ready project:

```text
Charity-Donation-Management-System-Final/
├── .env.example
├── .gitignore
├── README.md
├── SUPABASE_SETUP.md
├── SUBMISSION_FILE_GUIDE.md
├── Makefile
├── build_core.py
├── index.html
├── package.json
├── package-lock.json
├── vercel.json
├── cpp_core/
├── data/
├── docs/
│   ├── report.md
│   ├── VIVA_GUIDE.md
│   ├── evidence/
│   ├── screenshots/
│   └── uml/
├── src/
├── supabase/
└── tests/
```

---

## 5. Final Commands Before Submission

Run these commands before final submission:

```bash
npm install
npm run build
node tests/web_static_tests.mjs
python3 tests/run_core_tests.py
```

Expected results:

```text
Web static crash tests passed.
All C++ core tests passed.
```

---

## 6. Important Note About Secrets

Never upload real Supabase keys in `.env` or `.env.local`.

Only upload `.env.example`, which contains placeholder values.
