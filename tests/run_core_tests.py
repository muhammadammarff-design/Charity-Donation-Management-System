from __future__ import annotations

import os
import platform
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / "build" / ("charity_app.exe" if platform.system() == "Windows" else "charity_app")


def run(args, data_dir):
    env = os.environ.copy()
    env["CHARITY_DATA_DIR"] = str(data_dir)
    result = subprocess.run([str(EXE), *map(str, args)], cwd=ROOT, env=env, text=True, capture_output=True)
    return result.returncode, (result.stdout + result.stderr).strip()


def assert_contains(text, expected):
    assert expected in text, f"Expected {expected!r} in output:\n{text}"


def main():
    if not EXE.exists():
        subprocess.run(["python3", "build_core.py"], cwd=ROOT, check=True)

    with tempfile.TemporaryDirectory() as tmp:
        data_dir = Path(tmp)

        code, out = run(["clear"], data_dir)
        assert code == 0
        assert_contains(out, "SUCCESS")

        code, out = run(["add-donor", "", 25, "+92 300 1234567", "valid@example.com", "Lahore", "Individual"], data_dir)
        assert_contains(out, "ERROR|Donor name cannot be empty")

        code, out = run(["add-campaign", "Bad", "Bad target", -1, "2026-06-01", "2026-06-30", "Active"], data_dir)
        assert_contains(out, "ERROR|Campaign target amount")

        code, out = run(["seed"], data_dir)
        assert_contains(out, "SUCCESS|Demo data")

        code, out = run(["summary"], data_dir)
        assert_contains(out, "donors|3")
        assert_contains(out, "campaigns|3")
        assert_contains(out, "totalDonations|345000.00")

        code, out = run(["record-donation", "DNR001", "CAM001", -100, "2026-06-14", "Cash", "negative test"], data_dir)
        assert_contains(out, "ERROR|Donation amount must be greater than zero")

        code, out = run(["allocate-funds", "BEN001", "CAM001", 999999999, "2026-06-14", "too much", "Admin"], data_dir)
        assert_contains(out, "ERROR|Allocation exceeds available")

        code, out = run(["generate-report", "monthly", "2026-06"], data_dir)
        assert_contains(out, "SUCCESS|Monthly report")

        code, out = run(["verify-donor", "DNR001", "ahmed.khan@example.com"], data_dir)
        assert_contains(out, "SUCCESS|Donor verified")

        code, out = run(["donor-statement", "DNR001", "ahmed.khan@example.com"], data_dir)
        assert_contains(out, "donorName|Ahmed Khan")
        assert_contains(out, "totalDonated|75000.00")

        code, out = run(["donor-statement", "DNR001", "wrong@example.com"], data_dir)
        assert_contains(out, "ERROR|Donor verification failed")

        code, out = run(["oop-demo"], data_dir)
        assert_contains(out, "Inheritance")
        assert_contains(out, "operator+ on Donation")

    print("All C++ core tests passed.")


if __name__ == "__main__":
    main()
