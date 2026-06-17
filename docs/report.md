# Charity & Donation Management System — OOP Project Report

**Course:** Object-Oriented Programming  
**Core Language:** C++17  
**Hosted Interface:** Vercel frontend + Supabase PostgreSQL backend  
**Team Members:** Ali Raza (2540010), Muhammad Ammar (2540004), Taha Ali (2540008)

---

## 1. Problem Definition

Small charities often track donors, campaigns, beneficiaries, donations, and fund allocations using paper records or spreadsheets. This creates weak transparency because donors cannot easily see how their money was used.

---

## 2. Final Solution Design

The final system has two parts:

1. **C++ OOP Core**  
   Implements the main OOP project concepts: classes, inheritance, polymorphism, constructors, operator overloading, validation, file handling, and reports.

2. **Hosted Web Dashboard**  
   Uses Vercel for frontend hosting and Supabase PostgreSQL for live cloud data. This avoids Streamlit HTML rendering problems and provides a professional dashboard interface.

GitHub stores source code. Supabase stores live application data.

---

## 3. Tools and Technologies

| Area | Technology |
|---|---|
| OOP Core | C++17 |
| Hosted Frontend | HTML, CSS, JavaScript, Vite, Vercel |
| Cloud Database | Supabase PostgreSQL |
| Authentication | Supabase Auth for admin, Donor ID + email for donor portal |
| Local C++ Storage | Text files in `data/` |
| Documentation | Markdown, HTML, SVG UML |

---

## 4. UML Diagrams

### Class Diagram

![UML Class Diagram](uml/class_diagram.svg)

### Use Case Diagram

![UML Use Case Diagram](uml/use_case_diagram.svg)

---

## 5. C++ Class Structure

| Class | Responsibility |
|---|---|
| `Person` | Base class for shared identity fields |
| `Donor` | Donor details and total donation tracking |
| `Beneficiary` | Beneficiary details and aid received tracking |
| `Campaign` | Target, collected, allocated, and available balance |
| `Donation` | Donation transaction record |
| `FundAllocation` | Fund distribution record |
| `Report` | Monthly/campaign summary |
| `FileManager` | Text-file persistence |
| `CharitySystem` | Controller for CRUD, validation, reports |

---

## 6. OOP Concepts

| Concept | Implementation |
|---|---|
| Encapsulation | Private/protected members with methods |
| Inheritance | `Donor` and `Beneficiary` inherit from `Person` |
| Polymorphism | Virtual `displayInfo()` overridden in derived classes |
| Constructors | Default, parameterized, and overloaded constructors |
| Operator Overloading | `+`, `>`, `<` in `Donor`, `Donation`, `Campaign`, `Report` |
| File Handling | C++ reads/writes `.txt` files |

---

## 7. Hosted Web Features

### Donor Portal

- Register donor
- Generate donor ID automatically
- Login using Donor ID + email
- View donation history
- View supported campaign impact
- Hide beneficiary contact details

### Admin Area

- Admin login with Supabase Auth
- Dashboard metrics
- Priority actions
- Create campaigns
- Filter active campaigns
- Record donations
- Allocate funds
- Generate monthly reports
- Export/print report

---

## 8. Database Design

Supabase tables:

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

The schema includes triggers for generated codes such as `DNR001`, `CAM001`, and `RPT001`, plus validation for donation and allocation amounts.

---

## 9. Input Validation

Validation is implemented in both C++ and Supabase:

| Validation | Example |
|---|---|
| Positive amount | Donation/allocation amount must be greater than zero |
| Over-allocation prevention | Allocation cannot exceed available campaign balance |
| Campaign relationship | Beneficiary must belong to selected campaign |
| Donor verification | Donor ID + email must match |
| Date validation | C++ validates date formats |

---

## 10. Testing

C++ tests:

```bash
python3 tests/run_core_tests.py
```

Evidence commands:

```bash
./build/charity_app validation-demo
./build/charity_app oop-demo
```

---

## 11. Conclusion

The project preserves the required C++ OOP implementation while using a modern hosted web dashboard for a better user interface and persistent cloud data. This gives both strong viva-ready C++ code and a professional live application.
