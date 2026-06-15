"""Small cross-platform helper to compile the C++ OOP core."""
from __future__ import annotations

import os
import platform
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SRC_DIR = ROOT / "cpp_core" / "src"
INCLUDE_DIR = ROOT / "cpp_core" / "include"
BUILD_DIR = ROOT / "build"
EXE = BUILD_DIR / ("charity_app.exe" if platform.system() == "Windows" else "charity_app")


def build() -> Path:
    BUILD_DIR.mkdir(exist_ok=True)
    sources = sorted(str(path) for path in SRC_DIR.glob("*.cpp"))
    command = [
        "g++",
        "-std=c++17",
        "-Wall",
        "-Wextra",
        "-O2",
        f"-I{INCLUDE_DIR}",
        *sources,
        "-o",
        str(EXE),
    ]
    subprocess.run(command, cwd=ROOT, check=True)
    return EXE


if __name__ == "__main__":
    exe = build()
    print(f"Compiled successfully: {exe}")
