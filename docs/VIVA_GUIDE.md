# Viva Guide — Charity & Donation Management System

## 1. One-Minute Project Explanation

This is a Charity and Donation Management System built in C++ using OOP. It helps a small charity track donors, campaigns, beneficiaries, donations, fund allocations, and reports. The main purpose is transparency: every donation received and every amount distributed is recorded. The GUI is made in Streamlit, but it only calls the C++ executable. All main logic is inside C++. The updated interface has an Admin Area and a read-only Donor Portal where donors can verify using donor ID and email to view their donation history.

---

## 2. Why Streamlit + C++?

We wanted an advanced interface for better presentation, but our viva is from C++. Therefore, C++ remains the core project. Streamlit is only a wrapper that sends commands to the C++ program and displays the result.

---

## 3. Main Classes

| Class | Explanation |
|---|---|
| `Person` | Base class for common fields like ID, name, age, contact, and address. |
| `Donor` | Child class of `Person`; stores email, donor type, and total donated. |
| `Beneficiary` | Child class of `Person`; stores need type, family size, linked campaign, and total received. |
| `Campaign` | Stores campaign target, collected amount, allocated amount, balance, and status. |
| `Donation` | Stores each donation transaction. |
| `FundAllocation` | Stores each fund distribution record. |
| `Report` | Stores monthly/campaign reports. |
| `FileManager` | Handles text file reading/writing. |
| `CharitySystem` | Controller class that manages all operations. |

---

## 4. OOP Concepts

### Encapsulation

All class data members are private/protected. We access or update them using getters, setters, and class methods.

### Inheritance

`Donor` and `Beneficiary` inherit common fields from `Person`.

```cpp
class Donor : public Person
class Beneficiary : public Person
```

### Polymorphism

`displayInfo()` is virtual in `Person` and overridden in `Donor` and `Beneficiary`.

### Constructors

Classes have default and parameterized constructors. Some classes also have overloaded constructors for simpler object creation.

### Operator Overloading

| Operator | Class | Use |
|---|---|---|
| `+` | `Donor` | Combine total donations of two donors. |
| `>` | `Donor` | Compare which donor donated more. |
| `+` | `Donation` | Add two donation amounts. |
| `>` | `Donation` | Compare donation amounts. |
| `+` | `Campaign` | Combine collected amounts. |
| `<` | `Campaign` | Compare campaign collections. |
| `+` | `Report` | Merge reports. |
| `>` | `Report` | Compare report donation totals. |

---

## 5. File Handling

Data is saved in text files:

- `donors.txt`
- `beneficiaries.txt`
- `campaigns.txt`
- `donations.txt`
- `allocations.txt`
- `reports.txt`

The class `FileManager` reads and writes these files. The format is pipe-separated.

---

## 6. Input Validation Examples

The C++ core validates:

- empty names
- invalid age
- invalid email
- invalid contact number
- negative amount
- invalid date
- campaign end date before start date
- allocation amount greater than available campaign balance
- beneficiary not linked to selected campaign

Command to show validation evidence:

```bash
./build/charity_app validation-demo
```

---

## 7. Important Design Decisions

### Why not pure Streamlit?

Pure Streamlit would mean Python is the main project, but our OOP viva is from C++.

### Why not Vercel?

Vercel is better for frontend/serverless apps, not for a C++ file-handling project.

### Why simple admin login and donor verification?

The hosted app is public, so admin features should not be mixed with donor viewing. A simple Streamlit-level admin login separates CRUD pages from the read-only donor portal. Donor verification uses Donor ID + email and the verification command is handled by the C++ executable.

### Why text files instead of database?

The proposal promised C++ file handling. Text files also make the storage easy to explain in viva.

---

## 8. Demo Flow for Viva

1. Run:

```bash
python3 build_core.py
./build/charity_app seed
```

2. Show console summary:

```bash
./build/charity_app summary
```

3. Show donors/campaigns:

```bash
./build/charity_app list donors
./build/charity_app list campaigns
```

4. Show invalid donation rejected:

```bash
./build/charity_app record-donation DNR001 CAM001 -50 2026-06-14 Cash Invalid
```

5. Show validation evidence:

```bash
./build/charity_app validation-demo
```

6. Show OOP evidence:

```bash
./build/charity_app oop-demo
```

7. Run GUI:

```bash
streamlit run streamlit_app.py
```

---

## 9. Common Questions and Answers

### Q: Where is inheritance used?

`Donor` and `Beneficiary` inherit from `Person`.

### Q: Where is polymorphism used?

`displayInfo()` is virtual in `Person` and overridden in child classes.

### Q: How do you prevent over-allocation?

Before saving an allocation, C++ checks campaign available balance:

```text
available = collectedAmount - allocatedAmount
```

If allocation amount is greater than available, it is rejected.

### Q: What happens when a donation is recorded?

A `Donation` object is created, saved in `donations.txt`, and campaign/donor totals are recalculated.

### Q: What is the role of `CharitySystem`?

It is the controller class. It manages all vectors, validation calls, CRUD operations, reports, and saving/loading.

### Q: What is the role of `FileManager`?

It hides the file reading/writing details from the rest of the system.
