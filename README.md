# Charity & Donation Management System

**OOP Project | C++ Core + Streamlit GUI**

Team members:

- Ali Raza — 2540010
- Muhammad Ammar — 2540004
- Taha Ali — 2540008

---

## 1. Project Overview

This project is a **Charity and Donation Management System** designed for small NGOs and charity organizations. It tracks:

- donors
- campaigns
- beneficiaries
- donations
- fund allocations
- reports

The main goal is transparency: every donation received and every rupee allocated to beneficiaries is recorded.

---

## 2. Important Architecture Decision

The project follows this rule:

> **C++ is the real project. Streamlit is only the GUI/interface wrapper.**

The complete OOP logic, input validation, file handling, CRUD operations, reports, inheritance, polymorphism, and operator overloading are implemented in C++.

Streamlit only calls the compiled C++ executable through Python `subprocess` and displays forms/tables/charts.

This keeps the viva focused on C++ OOP while still giving the project a polished GUI.

---

## 3. Features

### Donor Management

- Add donor
- View donors
- Update donor details
- Delete donor if no donation history exists
- Track total donated amount

### Campaign Management

- Add campaign
- View campaign progress
- Update campaign
- Delete campaign if not linked with records
- Track target, collected, allocated, available balance, and progress percentage

### Beneficiary Management

- Add beneficiary
- Link beneficiary to campaign
- View aid received
- Update beneficiary
- Delete beneficiary if no allocation history exists

### Donation Recording

- Record donation by donor and campaign
- Validate amount/date/payment method
- Update donor and campaign totals automatically

### Fund Allocation

- Allocate collected funds to beneficiaries
- Prevent allocation greater than available campaign balance
- Validate campaign-beneficiary relationship

### Reports

- Monthly reports
- Campaign-level reports
- Stored in `reports.txt`
- Operator overloading demonstration with report merging

### Validation Evidence

The C++ core rejects invalid data such as:

- empty donor name
- invalid age
- invalid email
- negative campaign target
- invalid date range
- negative donation amount
- invalid donation date
- fund allocation greater than balance

---

## 4. OOP Concepts Implemented

| Concept | Implementation |
|---|---|
| Classes and Objects | `Person`, `Donor`, `Beneficiary`, `Campaign`, `Donation`, `FundAllocation`, `Report`, `FileManager`, `CharitySystem` |
| Encapsulation | Private/protected data members with public getters/setters |
| Inheritance | `Donor` and `Beneficiary` inherit from `Person` |
| Polymorphism | `displayInfo()` is virtual in `Person` and overridden in child classes |
| Constructors | Default, parameterized, and overloaded constructors used across classes |
| Operator Overloading | `+`, `>`, and `<` overloaded in meaningful classes |
| File Handling | Data stored in `.txt` files under `data/` |
| Object Relationships | Donations link donors/campaigns; allocations link beneficiaries/campaigns |

---

## 5. Project Structure

```text
Charity-Donation-Management-System/
│
├── cpp_core/
│   ├── include/              # C++ header files
│   └── src/                  # C++ source files
│
├── data/                     # Text-file database
│   ├── donors.txt
│   ├── beneficiaries.txt
│   ├── campaigns.txt
│   ├── donations.txt
│   ├── allocations.txt
│   └── reports.txt
│
├── docs/
│   ├── report.md
│   ├── report.html
│   ├── uml/
│   ├── evidence/
│   └── screenshots/
│
├── tests/
│   └── run_core_tests.py
│
├── streamlit_app.py          # GUI wrapper
├── build_core.py             # Cross-platform build helper
├── Makefile                  # Linux/macOS build commands
├── requirements.txt          # Python dependencies
├── packages.txt              # Linux packages for Streamlit Cloud
└── README.md
```

---

## 6. How to Run Locally

### Step 1: Compile the C++ Core

```bash
python3 build_core.py
```

or on Linux/macOS:

```bash
make
```

### Step 2: Seed Demo Data

```bash
./build/charity_app seed
```

On Windows:

```powershell
.\build\charity_app.exe seed
```

### Step 3: Run Console Version

```bash
./build/charity_app
```

### Step 4: Run Streamlit GUI

Install Python dependencies:

```bash
pip install -r requirements.txt
```

Run GUI:

```bash
streamlit run streamlit_app.py
```

---

## 7. CLI Commands

```bash
./build/charity_app help
./build/charity_app seed
./build/charity_app clear
./build/charity_app summary
./build/charity_app list donors
./build/charity_app list campaigns
./build/charity_app list beneficiaries
./build/charity_app list donations
./build/charity_app list allocations
./build/charity_app list reports
```

Example add donor:

```bash
./build/charity_app add-donor "Ahmed Khan" 35 "+92 300 1111111" "ahmed@example.com" "Lahore" "Individual"
```

Example record donation:

```bash
./build/charity_app record-donation DNR001 CAM001 5000 2026-06-14 Cash "Food support"
```

Example validation demo:

```bash
./build/charity_app validation-demo
```

Example OOP demo:

```bash
./build/charity_app oop-demo
```

---

## 8. Run Tests

```bash
python3 tests/run_core_tests.py
```

or:

```bash
make test
```

---

## 9. Deployment Recommendation

Recommended deployment is **Streamlit Community Cloud**:

1. Push this folder to GitHub.
2. Go to Streamlit Community Cloud.
3. Create a new app from the GitHub repository.
4. Set main file path to:

```text
streamlit_app.py
```

5. Streamlit will install Python requirements from `requirements.txt`.
6. `packages.txt` includes `build-essential` so the C++ core can compile on Linux.

### Important Persistence Note

The local text-file database works perfectly for local demo and viva. On Streamlit Cloud, local files may reset after app restart/redeployment. That is why the GUI includes a **Seed Demo Data** button.

For final marking, submit:

- GitHub repository link
- Streamlit app link if deployed
- report from `docs/report.html` or `docs/report.md`
- screenshots/evidence from `docs/screenshots/` and `docs/evidence/`

---

## 10. Why No Full Login System?

A full login/authentication module is not required by the rubric. Adding it would increase complexity without improving core OOP marks much.

Instead, this project focuses on:

- strong C++ OOP
- CRUD features
- validation
- file handling
- GUI
- reports
- UML
- viva-friendly explanation

---

## 11. Viva Talking Points

1. **Why C++ core + Streamlit GUI?**  
   To keep OOP logic in C++ while providing an advanced GUI.

2. **Where is inheritance used?**  
   `Donor` and `Beneficiary` inherit from `Person`.

3. **Where is polymorphism used?**  
   `Person::displayInfo()` is virtual and overridden.

4. **Where is encapsulation used?**  
   Data members are private/protected and accessed through methods.

5. **Where is operator overloading used?**  
   `Donor`, `Donation`, `Campaign`, and `Report` overload operators.

6. **Where is file handling used?**  
   `FileManager` reads/writes text files in the `data/` folder.

7. **How is validation handled?**  
   Validation is implemented in C++ before saving any record.

8. **How is transparency achieved?**  
   Every donation and allocation is saved and reports show collected, allocated, and remaining funds.
