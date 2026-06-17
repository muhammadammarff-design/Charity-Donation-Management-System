# Viva Guide — Charity & Donation Management System

## One-minute explanation

This is a Charity and Donation Management System. The core OOP implementation is written in C++ and includes donors, beneficiaries, campaigns, donations, fund allocations, reports, file handling, validation, inheritance, polymorphism, constructors, and operator overloading.

The hosted dashboard uses Vercel and Supabase so the live app has a clean frontend and persistent cloud database. GitHub stores the code; Supabase stores live data.

---

## Why Vercel + Supabase?

Streamlit caused custom HTML/CSS rendering issues. Vercel is better for a polished web interface and Supabase provides PostgreSQL database, auth, and APIs.

The C++ source remains the OOP project for viva.

---

## Main C++ Classes

| Class | Explanation |
|---|---|
| `Person` | Base class for ID, name, age, contact, address |
| `Donor` | Inherits from Person and tracks donor data |
| `Beneficiary` | Inherits from Person and tracks aid receiver data |
| `Campaign` | Tracks campaign target, collection, allocation, balance |
| `Donation` | Stores donation event |
| `FundAllocation` | Stores aid distribution |
| `Report` | Stores monthly/campaign summaries |
| `FileManager` | Handles text-file storage |
| `CharitySystem` | Main controller class |

---

## OOP Concepts

- Encapsulation: private/protected members
- Inheritance: Donor and Beneficiary inherit from Person
- Polymorphism: virtual `displayInfo()`
- Constructors: default, parameterized, overloaded
- Operator overloading: `+`, `>`, `<`
- File handling: `.txt` data files

---

## Web Dashboard Features

Admin can:

- create campaigns
- record donations
- allocate funds
- generate reports
- export/print reports

Donor can:

- register
- get generated donor ID
- log in with Donor ID + email
- view donation history
- view campaign impact

---

## Common Questions

### Where is inheritance used?

`Donor` and `Beneficiary` inherit from `Person`.

### Where is polymorphism used?

`displayInfo()` is virtual in `Person` and overridden in child classes.

### Why Supabase?

It gives PostgreSQL, authentication, APIs, and persistent cloud storage, which is better for a hosted dashboard than local text files.

### Is C++ still important?

Yes. The C++ core is the actual OOP implementation for course evaluation and viva.

### How is over-allocation prevented?

The C++ core checks available balance, and the Supabase schema also has a database trigger that blocks allocation greater than campaign available balance.
