# Design and Debug Notes

## What was changed

The first dashboard was too visually heavy and admin-only. The updated version applies a calmer product-dashboard approach:

- soft off-white background
- muted green primary color
- no harsh dark gradients
- fewer dashboard charts
- larger readable cards
- gentle fade/hover animation
- reduced-motion support in CSS
- simpler admin dashboard hierarchy
- separate donor portal and admin area

## UX principles applied

- Accessibility first: readable contrast, visible labels, no tiny body text.
- Simplicity: dashboard shows the few numbers that matter most.
- Progressive disclosure: detailed CRUD pages are separated from the dashboard.
- Read-only donor portal: donors should not see admin controls.
- Feedback: buttons and cards have subtle transitions, but motion is not excessive.
- Role separation: donor portal and admin area are separated in sidebar.

## Debug checks performed

Commands run successfully:

```bash
python3 -m py_compile streamlit_app.py
python3 build_core.py
./build/charity_app seed
./build/charity_app verify-donor DNR001 ahmed.khan@example.com
./build/charity_app donor-statement DNR001 ahmed.khan@example.com
python3 tests/run_core_tests.py
```

Result:

```text
All C++ core tests passed.
```

## New donor portal C++ commands

```bash
./build/charity_app verify-donor DNR001 ahmed.khan@example.com
./build/charity_app donor-statement DNR001 ahmed.khan@example.com
./build/charity_app donor-donations DNR001 ahmed.khan@example.com
./build/charity_app donor-campaigns DNR001 ahmed.khan@example.com
./build/charity_app donor-allocations DNR001 ahmed.khan@example.com
```

These commands keep donor verification and donor-specific data access connected to the C++ executable.

## Admin demo login

Default credentials:

```text
username: admin
password: admin123
```

This is a demo-level Streamlit guard, not production-grade authentication. It is included because the hosted app is public and admin CRUD pages should not be mixed with the donor view.
