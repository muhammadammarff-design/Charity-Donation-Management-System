# Project Tasks and Execution Plan

## Final Decision

Use **C++ OOP core + Streamlit GUI wrapper**.

Reason: viva is from C++, but GUI gives extra presentation value in the rubric. Streamlit does not contain business logic; it only calls the C++ executable.

Authentication decision: **No full login system**. The rubric does not require login, and adding it would increase complexity without improving the main OOP marks. The project focuses on OOP, CRUD, validation, file handling, GUI, UML, reports, and viva readiness.

---

## C++ Level

The project uses a normal student-friendly C++ level:

- classes and objects
- inheritance
- virtual function polymorphism
- vectors
- file handling with `fstream`
- string processing
- basic validation
- constructors
- operator overloading
- no advanced frameworks or external C++ libraries

---

## Task List

| # | Task | Status |
|---|---|---|
| 1 | Read proposal and rubric | Completed |
| 2 | Decide GUI strategy | Completed |
| 3 | Create project folder structure | Completed |
| 4 | Implement C++ classes | Completed |
| 5 | Implement file handling | Completed |
| 6 | Implement validation | Completed |
| 7 | Implement CRUD operations | Completed |
| 8 | Implement donation recording | Completed |
| 9 | Implement fund allocation with balance checks | Completed |
| 10 | Implement monthly/campaign reports | Completed |
| 11 | Implement CLI command interface | Completed |
| 12 | Implement interactive console menu | Completed |
| 13 | Implement OOP evidence command | Completed |
| 14 | Implement validation evidence command | Completed |
| 15 | Implement Streamlit GUI wrapper | Completed |
| 16 | Add dashboard, forms, tables, charts | Completed |
| 17 | Add seed/reset tools | Completed |
| 18 | Add automated tests | Completed |
| 19 | Generate UML diagrams | Completed |
| 20 | Generate evidence files | Completed |
| 21 | Generate report in Markdown and HTML | Completed |
| 22 | Add README and run instructions | Completed |
| 23 | Final compile/test verification | Completed |

---

## Execution Order Used

1. Built the C++ core first so the actual OOP project is strong.
2. Added CLI commands so Streamlit can call the C++ executable safely.
3. Added validation in C++ before saving anything.
4. Added text-file persistence.
5. Added demo seed data for screenshots and viva.
6. Built Streamlit as a GUI wrapper.
7. Added automated tests and evidence output.
8. Generated UML and report files.

---

## Final Verification Commands

```bash
python3 build_core.py
./build/charity_app seed
./build/charity_app summary
./build/charity_app validation-demo
./build/charity_app oop-demo
python3 tests/run_core_tests.py
```

All core tests passed.
