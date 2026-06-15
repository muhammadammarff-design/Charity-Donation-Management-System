from __future__ import annotations

import base64
import csv
import json
import os
import platform
import subprocess
import urllib.error
import urllib.request
from datetime import date
from io import BytesIO, StringIO
from pathlib import Path
from textwrap import dedent
from typing import Iterable, List, Tuple

import pandas as pd
import streamlit as st

ROOT = Path(__file__).resolve().parent
SRC_DIR = ROOT / "cpp_core" / "src"
INCLUDE_DIR = ROOT / "cpp_core" / "include"
BUILD_DIR = ROOT / "build"
EXE = BUILD_DIR / ("charity_app.exe" if platform.system() == "Windows" else "charity_app")
DATA_DIR = ROOT / "data"
DATA_FILES = ["donors.txt", "beneficiaries.txt", "campaigns.txt", "donations.txt", "allocations.txt", "reports.txt"]



def render_html(markup: str, *args, **kwargs) -> None:
    """Render HTML without Markdown treating indented blocks as code.

    Streamlit markdown parses indented HTML as code blocks, which broke the UI.
    st.html avoids Markdown parsing on newer Streamlit versions; fallback keeps older versions working.
    """
    cleaned = dedent(str(markup)).strip()
    if hasattr(st, "html"):
        st.html(cleaned)
    else:
        st.markdown(cleaned, unsafe_allow_html=True)

st.set_page_config(
    page_title="Amanah Charity System",
    page_icon="♡",
    layout="wide",
    initial_sidebar_state="expanded",
)

render_html(
    """
    <style>
      :root {
        --page: #f6f3ec;
        --surface: #ffffff;
        --surface-soft: #fbfaf6;
        --blue-50: #eaf2ff;
        --green-50: #e9f8f1;
        --amber-50: #fff4d8;
        --red-50: #fff1f0;
        --text: #1d2939;
        --text-2: #475467;
        --muted: #667085;
        --border: #e4e7ec;
        --border-2: #d0d5dd;
        --blue: #2457a6;
        --blue-dark: #1e4380;
        --green: #0f7a55;
        --green-dark: #0b6446;
        --amber: #b7791f;
        --red: #b42318;
        --shadow: 0 18px 42px rgba(29, 41, 57, .08);
        --shadow-sm: 0 8px 20px rgba(29, 41, 57, .06);
        --radius-xl: 30px;
        --radius-lg: 22px;
        --radius-md: 16px;
      }

      .stApp {
        background:
          radial-gradient(circle at 8% 0%, rgba(36, 87, 166, .10), transparent 32%),
          radial-gradient(circle at 92% 8%, rgba(15, 122, 85, .09), transparent 30%),
          var(--page);
        color: var(--text);
      }

      [data-testid="block-container"] {
        max-width: 1340px;
        padding-top: 1.4rem;
        padding-bottom: 3rem;
      }

      [data-testid="stSidebar"] {
        background: rgba(255,255,255,.92);
        border-right: 1px solid var(--border);
      }

      h1, h2, h3 {
        color: var(--text);
        letter-spacing: -.04em;
      }

      .brand-strip {
        display: flex;
        align-items: center;
        justify-content: space-between;
        gap: 18px;
        margin-bottom: 26px;
      }

      .brand-left {
        display: flex;
        align-items: center;
        gap: 14px;
      }

      .brand-logo {
        width: 52px;
        height: 52px;
        border-radius: 18px;
        display: grid;
        place-items: center;
        background: linear-gradient(135deg, var(--blue), var(--green));
        color: white;
        box-shadow: var(--shadow-sm);
        font-size: 26px;
        font-weight: 950;
      }

      .brand-title {
        margin: 0;
        font-size: 20px;
        line-height: 1.1;
        font-weight: 900;
        letter-spacing: -.03em;
      }

      .brand-subtitle {
        margin: 4px 0 0;
        color: var(--muted);
        font-size: 13px;
      }

      .prototype-pill {
        display: inline-flex;
        align-items: center;
        gap: 8px;
        min-height: 40px;
        padding: 0 14px;
        border-radius: 999px;
        border: 1px solid var(--border);
        background: rgba(255,255,255,.72);
        color: var(--text-2);
        font-size: 13px;
        font-weight: 800;
      }

      .dot {
        width: 9px;
        height: 9px;
        border-radius: 50%;
        background: var(--green);
        box-shadow: 0 0 0 4px rgba(15,122,85,.12);
      }

      .login-card, .page-card, .metric-card, .panel-card, .activity-card, .campaign-row, .priority-card {
        background: rgba(255,255,255,.94);
        border: 1px solid rgba(255,255,255,.90);
        border-radius: var(--radius-xl);
        box-shadow: var(--shadow);
      }

      .login-card {
        max-width: 560px;
        margin: 2.5rem auto;
        padding: 38px;
      }

      .login-title {
        font-size: 31px;
        font-weight: 950;
        margin: 0 0 7px;
      }

      .login-subtitle {
        margin: 0 0 24px;
        color: var(--muted);
        font-size: 14px;
        line-height: 1.55;
      }

      .page-card {
        padding: 40px 34px;
        margin-top: 12px;
        margin-bottom: 24px;
      }

      .page-head-row {
        display: flex;
        justify-content: space-between;
        align-items: flex-start;
        gap: 28px;
      }

      .eyebrow {
        display: inline-flex;
        align-items: center;
        width: fit-content;
        min-height: 30px;
        padding: 0 12px;
        border-radius: 999px;
        background: var(--blue-50);
        color: var(--blue-dark);
        font-size: 12px;
        font-weight: 950;
        text-transform: uppercase;
        letter-spacing: .04em;
      }

      .page-title {
        margin: 9px 0 9px;
        font-size: clamp(30px, 3.2vw, 44px);
        line-height: 1.04;
        font-weight: 950;
        letter-spacing: -.055em;
      }

      .page-description {
        margin: 0;
        color: var(--muted);
        max-width: 730px;
        line-height: 1.6;
      }

      .head-actions {
        display: flex;
        gap: 12px;
        flex-wrap: wrap;
        justify-content: flex-end;
      }

      .metric-grid, .two-grid, .priority-grid, .statement-grid {
        display: grid;
        gap: 34px;
        margin-top: 18px;
        margin-bottom: 26px;
      }

      .metric-grid {
        grid-template-columns: repeat(3, minmax(0, 1fr));
      }

      .two-grid {
        grid-template-columns: repeat(2, minmax(0, 1fr));
      }

      .priority-grid {
        grid-template-columns: repeat(2, minmax(0, 1fr));
      }

      .statement-grid {
        grid-template-columns: 1fr;
      }

      .metric-card {
        padding: 46px 34px;
        min-height: 210px;
        display: flex;
        flex-direction: column;
        justify-content: center;
        transition: .35s cubic-bezier(.2,.8,.2,1);
      }

      .metric-card:hover, .campaign-row:hover, .activity-card:hover {
        transform: translateY(-2px);
        box-shadow: var(--shadow);
      }

      .metric-label {
        color: var(--muted);
        font-size: 13px;
        font-weight: 900;
        margin-bottom: 18px;
      }

      .metric-value {
        font-size: clamp(30px, 4vw, 44px);
        line-height: 1;
        font-weight: 950;
        letter-spacing: -.055em;
        margin-bottom: 18px;
      }

      .metric-help {
        color: var(--text-2);
        font-size: 14px;
        line-height: 1.45;
      }

      .panel-card {
        padding: 42px 34px;
        margin-top: 12px;
        margin-bottom: 24px;
      }

      .panel-title {
        display: flex;
        justify-content: space-between;
        align-items: flex-start;
        gap: 18px;
        margin-bottom: 30px;
      }

      .panel-title h3 {
        margin: 0 0 7px;
        font-size: 21px;
        letter-spacing: -.03em;
      }

      .panel-title p {
        margin: 0;
        color: var(--muted);
        font-size: 14px;
        line-height: 1.45;
      }

      .chip {
        display: inline-flex;
        align-items: center;
        min-height: 28px;
        padding: 0 10px;
        border-radius: 999px;
        background: var(--green-50);
        color: var(--green-dark);
        font-size: 12px;
        font-weight: 950;
        white-space: nowrap;
      }

      .chip.blue {
        background: var(--blue-50);
        color: var(--blue-dark);
      }

      .chip.amber {
        background: var(--amber-50);
        color: #875a00;
      }

      .campaign-list, .activity-list {
        display: grid;
        gap: 28px;
      }

      .campaign-row {
        padding: 26px 24px;
        display: grid;
        grid-template-columns: minmax(0, 1fr) 180px;
        gap: 22px;
        align-items: center;
        transition: .35s cubic-bezier(.2,.8,.2,1);
      }

      .campaign-row h4 {
        margin: 0 0 7px;
        font-size: 17px;
      }

      .campaign-row p {
        margin: 0;
        color: var(--muted);
        font-size: 14px;
        line-height: 1.45;
      }

      .progress-line {
        height: 12px;
        border-radius: 999px;
        background: #eef2f6;
        overflow: hidden;
        margin-top: 16px;
      }

      .progress-line span {
        display: block;
        height: 100%;
        width: var(--w);
        background: linear-gradient(90deg, var(--blue), var(--green));
        border-radius: 999px;
        transform-origin: left;
        animation: scale-x 1.4s cubic-bezier(.2,.8,.2,1) both;
      }

      .campaign-money {
        text-align: right;
      }

      .campaign-money strong {
        display: block;
        font-size: 22px;
        letter-spacing: -.04em;
      }

      .campaign-money span {
        color: var(--muted);
        font-size: 12px;
        font-weight: 850;
      }

      .activity-card {
        display: grid;
        grid-template-columns: 56px minmax(0, 1fr) auto;
        align-items: center;
        gap: 18px;
        padding: 26px 24px;
        transition: .35s cubic-bezier(.2,.8,.2,1);
      }

      .activity-icon {
        width: 56px;
        height: 56px;
        border-radius: 18px;
        display: grid;
        place-items: center;
        font-weight: 950;
        color: var(--blue-dark);
        background: var(--blue-50);
      }

      .activity-icon.green {
        color: var(--green-dark);
        background: var(--green-50);
      }

      .activity-card strong {
        display: block;
        font-size: 15px;
        margin-bottom: 4px;
      }

      .activity-card span {
        color: var(--muted);
        font-size: 13px;
        line-height: 1.35;
      }

      .activity-amount {
        font-size: 16px;
        font-weight: 950;
        white-space: nowrap;
        text-align: right;
      }

      .priority-card {
        padding: 26px 24px;
      }

      .priority-card strong {
        display: block;
        margin-bottom: 8px;
      }

      .priority-card p {
        margin: 0;
        color: var(--muted);
        line-height: 1.45;
        font-size: 14px;
      }

      .table-wrap {
        overflow-x: auto;
        border: 1px solid var(--border);
        border-radius: 22px;
        background: var(--surface);
        margin-top: 8px;
        margin-bottom: 8px;
      }

      table {
        width: 100%;
        min-width: 660px;
        border-collapse: collapse;
        font-size: 14px;
      }

      th, td {
        padding: 16px 18px;
        border-bottom: 1px solid var(--border);
        text-align: left;
        vertical-align: middle;
      }

      th {
        background: #f9fafb;
        color: var(--muted);
        text-transform: uppercase;
        letter-spacing: .04em;
        font-size: 12px;
        font-weight: 950;
      }

      tr:last-child td { border-bottom: 0; }

      .status {
        display: inline-flex;
        align-items: center;
        min-height: 28px;
        padding: 0 10px;
        border-radius: 999px;
        font-size: 12px;
        font-weight: 950;
        background: var(--green-50);
        color: var(--green-dark);
      }

      .status.warn {
        background: var(--amber-50);
        color: #875a00;
      }

      .statement-card {
        padding: 42px 34px;
        border-radius: var(--radius-xl);
        background: linear-gradient(135deg, var(--blue), #3167bd);
        color: white;
        box-shadow: var(--shadow);
      }

      .statement-card h3 {
        margin: 0 0 8px;
        font-size: 30px;
        letter-spacing: -.04em;
      }

      .statement-card p {
        color: rgba(255,255,255,.82);
        line-height: 1.55;
        margin: 0;
      }

      .statement-metrics {
        margin-top: 30px;
        display: grid;
        grid-template-columns: repeat(3, minmax(0, 1fr));
        gap: 20px;
      }

      .statement-metric {
        border-radius: 20px;
        padding: 26px 18px;
        background: rgba(255,255,255,.12);
        border: 1px solid rgba(255,255,255,.22);
      }

      .statement-metric span {
        display: block;
        color: rgba(255,255,255,.76);
        font-size: 12px;
        font-weight: 850;
        margin-bottom: 8px;
      }

      .statement-metric strong {
        font-size: 20px;
        letter-spacing: -.03em;
      }

      .modal-backdrop {
        position: fixed;
        inset: 0;
        z-index: 100;
        display: grid;
        place-items: center;
        padding: 24px;
        background: rgba(29, 41, 57, .34);
        backdrop-filter: blur(8px);
      }

      .modal {
        width: min(640px, 100%);
        max-height: min(780px, calc(100vh - 48px));
        overflow: auto;
        background: var(--surface);
        border: 1px solid rgba(255,255,255,.9);
        border-radius: var(--radius-xl);
        box-shadow: 0 28px 90px rgba(29,41,57,.24);
        padding: 30px;
        animation: fade-up .55s cubic-bezier(.2,.8,.2,1) both;
      }

      .modal-head {
        display: flex;
        justify-content: space-between;
        gap: 18px;
        align-items: flex-start;
        margin-bottom: 22px;
      }

      .modal-head h3 {
        margin: 0 0 7px;
        font-size: 25px;
        letter-spacing: -.04em;
      }

      .modal-head p {
        margin: 0;
        color: var(--muted);
        font-size: 14px;
        line-height: 1.45;
      }

      .modal-close {
        border: 1px solid var(--border);
        background: var(--surface-soft);
        border-radius: 12px;
        width: 42px;
        height: 42px;
        font-weight: 950;
      }

      .form-grid {
        display: grid;
        grid-template-columns: repeat(2, minmax(0, 1fr));
        gap: 16px;
      }

      .field.full-span { grid-column: 1 / -1; }

      .github-note {
        margin-top: 1rem;
        padding: 1rem;
        border-radius: 16px;
        border: 1px solid var(--border);
        background: var(--surface-soft);
        color: var(--muted);
        font-size: 13px;
        line-height: 1.55;
      }

      .stButton > button, .stDownloadButton > button {
        border-radius: 14px !important;
        min-height: 48px;
        font-weight: 900 !important;
        transition: .35s cubic-bezier(.2,.8,.2,1) !important;
      }

      .stButton > button:hover, .stDownloadButton > button:hover {
        transform: translateY(-1px);
        box-shadow: var(--shadow-sm);
      }

      div[data-testid="stForm"] {
        border: 1px solid var(--border);
        border-radius: 22px;
        padding: 1.2rem;
        background: rgba(255,255,255,.75);
      }

      @keyframes fade-up { from { opacity: 0; transform: translateY(22px); } to { opacity: 1; transform: translateY(0); } }
      @keyframes scale-x { from { transform: scaleX(0); } to { transform: scaleX(1); } }

      @media (max-width: 1080px) {
        .page-head-row, .campaign-row, .activity-card { display: flex; flex-direction: column; align-items: stretch; }
        .metric-grid, .two-grid, .priority-grid, .statement-metrics { grid-template-columns: 1fr; }
        .campaign-money, .activity-amount { text-align: left; }
      }

      @media (prefers-reduced-motion: reduce) {
        *, *::before, *::after { animation: none !important; transition: none !important; scroll-behavior: auto !important; }
      }
    </style>
    """,
    unsafe_allow_html=True,
)


# ----------------------------- C++ core bridge -----------------------------
@st.cache_resource(show_spinner=False)
def compile_core() -> Tuple[bool, str]:
    BUILD_DIR.mkdir(exist_ok=True)
    sources = sorted(str(p) for p in SRC_DIR.glob("*.cpp"))
    if not sources:
        return False, "No C++ source files found. Make sure cpp_core/src exists."

    command = ["g++", "-std=c++17", "-Wall", "-Wextra", "-O2", f"-I{INCLUDE_DIR}", *sources, "-o", str(EXE)]
    try:
        completed = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, check=False)
    except FileNotFoundError:
        return False, "g++ compiler not found. On Streamlit Cloud, packages.txt must contain build-essential."

    if completed.returncode != 0:
        return False, completed.stderr or completed.stdout
    return True, f"C++ core compiled successfully: {EXE.name}"


def run_core(args: Iterable[str]) -> Tuple[bool, str]:
    ok, compile_message = compile_core()
    if not ok:
        return False, compile_message

    env = os.environ.copy()
    env["CHARITY_DATA_DIR"] = str(DATA_DIR)
    DATA_DIR.mkdir(exist_ok=True)

    completed = subprocess.run([str(EXE), *[str(a) for a in args]], cwd=ROOT, env=env, capture_output=True, text=True, check=False)
    output = (completed.stdout or "") + (completed.stderr or "")
    return completed.returncode == 0, output.strip()


def parse_pipe_table(output: str) -> pd.DataFrame:
    if not output or output.startswith("ERROR|") or output.startswith("SUCCESS|"):
        return pd.DataFrame()
    rows = list(csv.reader(StringIO(output), delimiter="|"))
    if not rows:
        return pd.DataFrame()
    return pd.DataFrame(rows[1:], columns=rows[0])


def command_table(args: List[str]) -> Tuple[bool, pd.DataFrame, str]:
    ok, output = run_core(args)
    if not ok or output.startswith("ERROR|"):
        return False, pd.DataFrame(), output
    return True, parse_pipe_table(output), output


def get_table(name: str) -> pd.DataFrame:
    ok, df, output = command_table(["list", name])
    if not ok:
        st.error(output)
        return pd.DataFrame()
    return df


def numeric_columns(df: pd.DataFrame, cols: List[str]) -> pd.DataFrame:
    copy = df.copy()
    for col in cols:
        if col in copy.columns:
            copy[col] = pd.to_numeric(copy[col], errors="coerce").fillna(0)
    return copy


def parse_result(output: str) -> Tuple[bool, str, str]:
    if output.startswith("SUCCESS|"):
        parts = output.split("|", 2)
        return True, parts[1] if len(parts) > 1 else "Operation successful.", parts[2] if len(parts) > 2 else ""
    if output.startswith("ERROR|"):
        return False, output.split("|", 1)[1] if "|" in output else output, ""
    return True, output, ""


def show_result(output: str) -> bool:
    success, message, item_id = parse_result(output)
    if success:
        st.success(message + (f" New ID: {item_id}" if item_id else ""))
    else:
        st.error(message)
    return success


def format_money(value: float | int | str) -> str:
    try:
        number = float(value)
    except Exception:
        number = 0.0
    if abs(number) >= 1000:
        return f"Rs. {number/1000:.0f}k"
    return f"Rs. {number:.0f}"


def format_full_money(value: float | int | str) -> str:
    try:
        return f"Rs. {float(value):,.0f}"
    except Exception:
        return "Rs. 0"


def mask_email(email: str) -> str:
    if "@" not in email:
        return "hidden"
    name, domain = email.split("@", 1)
    if len(name) <= 2:
        return name[:1] + "***@" + domain
    return name[:1] + "***" + name[-1:] + "@" + domain


def refresh() -> None:
    st.rerun()


# ----------------------------- Optional GitHub sync -----------------------------
def secret_value(name: str, default: str = "") -> str:
    if os.environ.get(name):
        return os.environ[name]
    try:
        return str(st.secrets.get(name, default))
    except Exception:
        return default


def github_configured() -> bool:
    return bool(secret_value("GITHUB_TOKEN") and secret_value("GITHUB_REPO"))


def github_api_request(method: str, url: str, token: str, payload: dict | None = None) -> tuple[int, dict]:
    data = None if payload is None else json.dumps(payload).encode("utf-8")
    request = urllib.request.Request(url, data=data, method=method)
    request.add_header("Authorization", f"Bearer {token}")
    request.add_header("Accept", "application/vnd.github+json")
    request.add_header("X-GitHub-Api-Version", "2022-11-28")
    if payload is not None:
        request.add_header("Content-Type", "application/json")
    try:
        with urllib.request.urlopen(request, timeout=20) as response:
            raw = response.read().decode("utf-8")
            return response.status, json.loads(raw) if raw else {}
    except urllib.error.HTTPError as error:
        raw = error.read().decode("utf-8")
        try:
            return error.code, json.loads(raw)
        except Exception:
            return error.code, {"message": raw}


def sync_data_to_github(reason: str = "Update data files") -> tuple[bool, str]:
    token = secret_value("GITHUB_TOKEN")
    repo = secret_value("GITHUB_REPO")
    branch = secret_value("GITHUB_BRANCH", "main")
    prefix = secret_value("GITHUB_DATA_PREFIX", "data").strip("/")

    if not token or not repo:
        return False, "GitHub sync is not configured. Add GITHUB_TOKEN and GITHUB_REPO in Streamlit secrets."

    committed = 0
    for file_name in DATA_FILES:
        local_path = DATA_DIR / file_name
        local_path.parent.mkdir(exist_ok=True)
        content = local_path.read_text(encoding="utf-8") if local_path.exists() else ""
        repo_path = f"{prefix}/{file_name}" if prefix else file_name
        url = f"https://api.github.com/repos/{repo}/contents/{repo_path}"
        status, existing = github_api_request("GET", f"{url}?ref={branch}", token)
        sha = existing.get("sha") if status == 200 else None
        payload = {
            "message": f"Data sync: {reason} ({file_name})",
            "content": base64.b64encode(content.encode("utf-8")).decode("ascii"),
            "branch": branch,
        }
        if sha:
            payload["sha"] = sha
        put_status, put_response = github_api_request("PUT", url, token, payload)
        if put_status not in (200, 201):
            return False, f"GitHub sync failed for {file_name}: {put_response.get('message', put_response)}"
        committed += 1
    return True, f"Synced {committed} data files to GitHub."


def after_mutation(output: str, reason: str) -> None:
    if show_result(output) and github_configured():
        ok, message = sync_data_to_github(reason)
        if ok:
            st.info(message)
        else:
            st.warning(message)


# ----------------------------- Data helpers -----------------------------
def load_data() -> dict[str, pd.DataFrame]:
    donors = numeric_columns(get_table("donors"), ["age", "totalDonated"])
    beneficiaries = numeric_columns(get_table("beneficiaries"), ["age", "familySize", "totalReceived"])
    campaigns = numeric_columns(get_table("campaigns"), ["targetAmount", "collectedAmount", "allocatedAmount", "availableBalance", "progressPercent"])
    donations = numeric_columns(get_table("donations"), ["amount"])
    allocations = numeric_columns(get_table("allocations"), ["amount"])
    reports = numeric_columns(get_table("reports"), ["totalDonations", "totalAllocations", "remainingBalance"])
    return {"donors": donors, "beneficiaries": beneficiaries, "campaigns": campaigns, "donations": donations, "allocations": allocations, "reports": reports}


def summary_from_data(data: dict[str, pd.DataFrame]) -> dict[str, float | int]:
    total_donations = float(data["donations"]["amount"].sum()) if not data["donations"].empty else 0.0
    total_allocations = float(data["allocations"]["amount"].sum()) if not data["allocations"].empty else 0.0
    return {
        "donors": len(data["donors"]),
        "beneficiaries": len(data["beneficiaries"]),
        "campaigns": len(data["campaigns"]),
        "donations": len(data["donations"]),
        "allocations": len(data["allocations"]),
        "totalDonations": total_donations,
        "totalAllocations": total_allocations,
        "remainingBalance": total_donations - total_allocations,
    }


def option_list(df: pd.DataFrame, id_col: str = "id", label_col: str = "title") -> list[str]:
    if df.empty or id_col not in df.columns:
        return []
    if label_col in df.columns:
        return [f"{row[id_col]} - {row[label_col]}" for _, row in df.iterrows()]
    return df[id_col].astype(str).tolist()


def selected_id(option: str) -> str:
    return option.split(" - ", 1)[0] if option else ""


def campaign_title(data: dict[str, pd.DataFrame], campaign_id: str) -> str:
    campaigns = data["campaigns"]
    if campaigns.empty:
        return campaign_id
    found = campaigns[campaigns["id"] == campaign_id]
    return str(found.iloc[0]["title"]) if not found.empty else campaign_id


def donor_name(data: dict[str, pd.DataFrame], donor_id: str) -> str:
    donors = data["donors"]
    if donors.empty:
        return donor_id
    found = donors[donors["id"] == donor_id]
    return str(found.iloc[0]["name"]) if not found.empty else donor_id


def beneficiary_name(data: dict[str, pd.DataFrame], beneficiary_id: str) -> str:
    beneficiaries = data["beneficiaries"]
    if beneficiaries.empty:
        return beneficiary_id
    found = beneficiaries[beneficiaries["id"] == beneficiary_id]
    return str(found.iloc[0]["name"]) if not found.empty else beneficiary_id


# ----------------------------- Design components -----------------------------
def brand_strip() -> None:
    render_html(
        """
        <div class="brand-strip">
          <div class="brand-left">
            <div class="brand-logo">♡</div>
            <div>
              <div class="brand-title">Amanah Charity System</div>
              <div class="brand-subtitle">C++ OOP core · Streamlit interface</div>
            </div>
          </div>
          <div class="prototype-pill"><span class="dot"></span> Live project interface</div>
        </div>
        """,
        unsafe_allow_html=True,
    )


def page_header(eyebrow: str, title: str, description: str, actions_html: str = "") -> None:
    render_html(
        f"""
        <div class="page-card">
          <div class="page-head-row">
            <div>
              <span class="eyebrow">{eyebrow}</span>
              <div class="page-title">{title}</div>
              <p class="page-description">{description}</p>
            </div>
            <div class="head-actions">{actions_html}</div>
          </div>
        </div>
        """,
        unsafe_allow_html=True,
    )


def metric_grid(metrics: list[tuple[str, str, str]]) -> None:
    cards = "".join(
        f"""
        <article class="metric-card">
          <div class="metric-label">{label}</div>
          <div class="metric-value">{value}</div>
          <div class="metric-help">{help_text}</div>
        </article>
        """
        for label, value, help_text in metrics
    )
    render_html(f"<div class='metric-grid'>{cards}</div>", unsafe_allow_html=True)


def campaign_rows(campaigns: pd.DataFrame, limit: int | None = None) -> str:
    if campaigns.empty:
        return "<p class='page-description'>No campaigns found. Create a campaign or seed demo data.</p>"
    df = campaigns.head(limit) if limit else campaigns
    rows = []
    for _, row in df.iterrows():
        progress = min(float(row.get("progressPercent", 0)), 100.0)
        status = str(row.get("status", "Active"))
        status_class = "status" if status.lower() == "active" else "status warn"
        rows.append(
            f"""
            <div class="campaign-row">
              <div>
                <h4>{row.get('title', '')} <span class="{status_class}">{status}</span></h4>
                <p>{row.get('description', '')} · Target {format_full_money(row.get('targetAmount', 0))} · Collected {format_full_money(row.get('collectedAmount', 0))} · Allocated {format_full_money(row.get('allocatedAmount', 0))}</p>
                <div class="progress-line"><span style="--w:{progress:.0f}%"></span></div>
              </div>
              <div class="campaign-money"><strong>{format_money(row.get('availableBalance', 0))}</strong><span>{progress:.0f}% funded · available</span></div>
            </div>
            """
        )
    return "".join(rows)


def recent_activity(data: dict[str, pd.DataFrame]) -> str:
    items = []
    donations = data["donations"].copy()
    allocations = data["allocations"].copy()

    if not donations.empty:
        donations["kind"] = "DN"
        donations["sortDate"] = donations["date"]
        for _, row in donations.tail(5).iterrows():
            items.append({
                "kind": "DN",
                "title": f"{donor_name(data, row['donorId'])} donated to {campaign_title(data, row['campaignId'])}",
                "detail": f"{row['paymentMethod']} · {row['date']}",
                "amount": float(row["amount"]),
            })
    if not allocations.empty:
        for _, row in allocations.tail(5).iterrows():
            items.append({
                "kind": "AL",
                "title": f"Funds allocated to {beneficiary_name(data, row['beneficiaryId'])}",
                "detail": f"{campaign_title(data, row['campaignId'])} · {row['purpose']}",
                "amount": float(row["amount"]),
            })

    items = items[-6:][::-1]
    if not items:
        return "<p class='page-description'>No activity yet.</p>"
    return "".join(
        f"""
        <div class="activity-card">
          <div class="{'activity-icon green' if item['kind'] == 'DN' else 'activity-icon'}">{item['kind']}</div>
          <div><strong>{item['title']}</strong><span>{item['detail']}</span></div>
          <div class="activity-amount">{format_full_money(item['amount'])}</div>
        </div>
        """
        for item in items
    )


def priority_actions(data: dict[str, pd.DataFrame]) -> str:
    campaigns = data["campaigns"]
    reports = data["reports"]
    cards = []
    if not campaigns.empty:
        active = campaigns[campaigns["status"].str.lower() == "active"] if "status" in campaigns.columns else campaigns
        if not active.empty:
            lowest = active.sort_values("progressPercent").iloc[0]
            cards.append((
                f"{lowest['title']} is under-funded",
                f"Only {float(lowest['progressPercent']):.0f}% funded. Highlight this campaign to donors before other active drives.",
            ))
    if reports.empty:
        cards.append(("Monthly report not generated", "Generate a report before taking submission screenshots so totals match the evidence."))
    else:
        cards.append(("Latest report is ready", "Reports page contains generated transparency summaries for viva and documentation."))
    if not cards:
        cards.append(("No urgent action", "All core demo records are available."))
    return "".join(f"<div class='priority-card'><strong>{title}</strong><p>{desc}</p></div>" for title, desc in cards[:2])


# ----------------------------- PDF export -----------------------------
def build_report_pdf(data: dict[str, pd.DataFrame]) -> bytes:
    summary = summary_from_data(data)
    try:
        from reportlab.lib.pagesizes import A4
        from reportlab.pdfgen import canvas
    except Exception:
        return (
            "Amanah Charity Transparency Report\n\n"
            f"Total donations: {format_full_money(summary['totalDonations'])}\n"
            f"Total allocations: {format_full_money(summary['totalAllocations'])}\n"
            f"Remaining balance: {format_full_money(summary['remainingBalance'])}\n"
        ).encode("utf-8")

    buffer = BytesIO()
    c = canvas.Canvas(buffer, pagesize=A4)
    width, height = A4
    y = height - 70
    c.setFont("Helvetica-Bold", 18)
    c.drawString(50, y, "Amanah Charity Transparency Report")
    y -= 34
    c.setFont("Helvetica", 12)
    c.drawString(50, y, f"Total donations: {format_full_money(summary['totalDonations'])}")
    y -= 22
    c.drawString(50, y, f"Total allocations: {format_full_money(summary['totalAllocations'])}")
    y -= 22
    c.drawString(50, y, f"Remaining balance: {format_full_money(summary['remainingBalance'])}")
    y -= 34
    c.setFont("Helvetica-Bold", 13)
    c.drawString(50, y, "Campaigns")
    y -= 24
    c.setFont("Helvetica", 10)
    for _, row in data["campaigns"].head(12).iterrows():
        line = f"{row['id']} - {row['title']} | Collected {format_full_money(row['collectedAmount'])} | Allocated {format_full_money(row['allocatedAmount'])} | Available {format_full_money(row['availableBalance'])}"
        c.drawString(50, y, line[:115])
        y -= 17
        if y < 70:
            c.showPage()
            y = height - 70
    c.save()
    return buffer.getvalue()


# ----------------------------- Login -----------------------------
def login_page() -> None:
    brand_strip()
    render_html("<div class='login-card'><div class='login-title'>Login</div><p class='login-subtitle'>Choose donor, register donor, or admin access.</p>", unsafe_allow_html=True)
    tab = st.radio("Choose access", ["Donor", "Register", "Admin"], horizontal=True, label_visibility="collapsed")

    if tab == "Donor":
        with st.form("donor_login_form"):
            st.markdown("### Donor verification")
            st.caption("View your donation statement using Donor ID and email.")
            donor_id = st.text_input("Donor ID", value="DNR001")
            email = st.text_input("Email", value="ahmed.khan@example.com")
            submitted = st.form_submit_button("Login as donor", type="primary")
        if submitted:
            ok, output = run_core(["verify-donor", donor_id.strip().upper(), email.strip()])
            if ok and output.startswith("SUCCESS|"):
                st.session_state.role = "donor"
                st.session_state.donor_id = donor_id.strip().upper()
                st.session_state.donor_email = email.strip()
                refresh()
            else:
                show_result(output)

    elif tab == "Register":
        with st.form("donor_register_form"):
            st.markdown("### Register as donor")
            st.caption("Your Donor ID will be generated by the C++ system.")
            name = st.text_input("Full name", value="New Donor")
            age = st.number_input("Age", min_value=1, max_value=120, value=22)
            contact = st.text_input("Contact", value="+92 300 1234567")
            email = st.text_input("Email", value="new.donor@example.com")
            address = st.text_input("Address", value="Lahore")
            donor_type = st.selectbox("Donor type", ["Individual", "Organization", "Family", "Anonymous"])
            submitted = st.form_submit_button("Register donor", type="primary")
        if submitted:
            ok, output = run_core(["add-donor", name, int(age), contact, email, address, donor_type])
            success, message, new_id = parse_result(output)
            if ok and success and new_id:
                st.success(f"Donor registered successfully. Generated Donor ID: {new_id}")
                after_mutation(output, "donor registration")
                st.session_state.role = "donor"
                st.session_state.donor_id = new_id
                st.session_state.donor_email = email
                refresh()
            else:
                show_result(output)

    else:
        with st.form("admin_login_form"):
            st.markdown("### Admin login")
            st.caption("Open management dashboard, campaigns, reports, and admin actions.")
            username = st.text_input("Username", value="admin")
            password = st.text_input("Password", type="password", value="admin123")
            submitted = st.form_submit_button("Login as admin", type="primary")
        if submitted:
            expected_user = secret_value("ADMIN_USERNAME", "admin")
            expected_password = secret_value("ADMIN_PASSWORD", "admin123")
            if username == expected_user and password == expected_password:
                st.session_state.role = "admin"
                st.session_state.admin_logged_in = True
                refresh()
            else:
                st.error("Wrong admin credentials.")

    render_html(
        "<div class='security-note'>Prototype note: final data changes are stored through the C++ file system. GitHub sync needs a GitHub token configured in Streamlit secrets.</div></div>",
        unsafe_allow_html=True,
    )


# ----------------------------- Admin pages -----------------------------
def admin_dashboard(data: dict[str, pd.DataFrame]) -> None:
    summary = summary_from_data(data)
    page_header(
        "Admin overview",
        "Clear financial snapshot.",
        "Only the most important metrics are shown first. Campaigns and reports have their own pages.",
    )
    col1, col2 = st.columns([1, 1])
    with col1:
        if st.button("Record donation", type="primary", use_container_width=True):
            st.session_state.action_panel = "donation"
            refresh()
    with col2:
        if st.button("Allocate funds", use_container_width=True):
            st.session_state.action_panel = "allocation"
            refresh()

    if st.session_state.get("action_panel") == "donation":
        record_donation_panel(data)
    elif st.session_state.get("action_panel") == "allocation":
        allocate_funds_panel(data)

    metric_grid([
        ("Total received", format_money(summary["totalDonations"]), f"{summary['donations']} donation records across campaigns."),
        ("Funds allocated", format_money(summary["totalAllocations"]), f"{summary['allocations']} beneficiary allocation records."),
        ("Available balance", format_money(summary["remainingBalance"]), "Remaining amount for upcoming distributions."),
    ])

    render_html(
        f"""
        <section class="panel-card">
          <div class="panel-title"><div><h3>Priority actions</h3><p>Only important alerts are shown here so the dashboard stays useful.</p></div><span class="chip amber">Needs attention</span></div>
          <div class="priority-grid">{priority_actions(data)}</div>
        </section>
        """,
        unsafe_allow_html=True,
    )

    active = data["campaigns"]
    if not active.empty and "status" in active.columns:
        active = active[active["status"].str.lower() == "active"]
    render_html(
        f"""
        <section class="panel-card">
          <div class="panel-title"><div><h3>Campaign progress</h3><p>Readable progress list instead of many competing charts.</p></div><span class="chip blue">{len(active)} active</span></div>
          <div class="campaign-list">{campaign_rows(active, limit=3)}</div>
        </section>
        """,
        unsafe_allow_html=True,
    )

    render_html(
        f"""
        <section class="panel-card">
          <div class="panel-title"><div><h3>Recent activity</h3><p>New donations and allocations appear here after using the action buttons above.</p></div><span class="chip">Live data</span></div>
          <div class="activity-list">{recent_activity(data)}</div>
        </section>
        """,
        unsafe_allow_html=True,
    )


def record_donation_panel(data: dict[str, pd.DataFrame]) -> None:
    render_html("<section class='panel-card'><div class='panel-title'><div><h3>Record donation</h3><p>Validate donor, campaign, amount, and method before saving.</p></div></div>", unsafe_allow_html=True)
    donors = option_list(data["donors"], "id", "name")
    campaigns = option_list(data["campaigns"], "id", "title")
    with st.form("record_donation_form"):
        c1, c2 = st.columns(2)
        donor = c1.selectbox("Donor", donors) if donors else ""
        campaign = c2.selectbox("Campaign", campaigns) if campaigns else ""
        c3, c4 = st.columns(2)
        amount = c3.number_input("Amount", min_value=1.0, value=5000.0, step=500.0)
        method = c4.text_input("Payment method", value="Cash")
        note = st.text_input("Note", value="General donation")
        submitted = st.form_submit_button("Save donation", type="primary")
        cancel = st.form_submit_button("Cancel")
    if submitted:
        if not donors or not campaigns:
            st.error("Add donor and campaign data first.")
        else:
            ok, output = run_core(["record-donation", selected_id(donor), selected_id(campaign), float(amount), str(date.today()), method, note])
            after_mutation(output, "record donation")
            if ok and output.startswith("SUCCESS|"):
                st.session_state.action_panel = None
                refresh()
    if cancel:
        st.session_state.action_panel = None
        refresh()
    render_html("</section>", unsafe_allow_html=True)


def allocate_funds_panel(data: dict[str, pd.DataFrame]) -> None:
    render_html("<section class='panel-card'><div class='panel-title'><div><h3>Allocate funds</h3><p>Validate beneficiary, campaign, amount, and available balance before saving.</p></div></div>", unsafe_allow_html=True)
    beneficiaries = option_list(data["beneficiaries"], "id", "name")
    campaigns = option_list(data["campaigns"], "id", "title")
    with st.form("allocation_form"):
        c1, c2 = st.columns(2)
        beneficiary = c1.selectbox("Beneficiary", beneficiaries) if beneficiaries else ""
        campaign = c2.selectbox("Campaign", campaigns) if campaigns else ""
        c3, c4 = st.columns(2)
        amount = c3.number_input("Amount", min_value=1.0, value=10000.0, step=500.0)
        purpose = c4.text_input("Purpose", value="Food package")
        approved_by = st.text_input("Approved by", value="Admin")
        submitted = st.form_submit_button("Save allocation", type="primary")
        cancel = st.form_submit_button("Cancel")
    if submitted:
        if not beneficiaries or not campaigns:
            st.error("Add beneficiary and campaign data first.")
        else:
            ok, output = run_core(["allocate-funds", selected_id(beneficiary), selected_id(campaign), float(amount), str(date.today()), purpose, approved_by])
            after_mutation(output, "allocate funds")
            if ok and output.startswith("SUCCESS|"):
                st.session_state.action_panel = None
                refresh()
    if cancel:
        st.session_state.action_panel = None
        refresh()
    render_html("</section>", unsafe_allow_html=True)


def campaigns_page(data: dict[str, pd.DataFrame]) -> None:
    page_header("Campaign management", "Campaigns with status and progress.", "Create campaigns and filter only active campaigns when preparing the demo.")
    c1, c2 = st.columns([1, 1])
    with c1:
        if st.button("Create campaign", type="primary", use_container_width=True):
            st.session_state.show_campaign_form = not st.session_state.get("show_campaign_form", False)
    with c2:
        filter_active = st.toggle("Filter active", value=st.session_state.get("filter_active", False))
        st.session_state.filter_active = filter_active

    if st.session_state.get("show_campaign_form"):
        render_html("<section class='panel-card'><div class='panel-title'><div><h3>Create campaign</h3><p>Campaign is saved through the C++ core.</p></div></div>", unsafe_allow_html=True)
        with st.form("create_campaign_form"):
            title = st.text_input("Campaign title", value="Winter Clothing Drive")
            description = st.text_input("Description", value="Warm clothing support for low-income families")
            target = st.number_input("Target amount", min_value=1.0, value=150000.0, step=5000.0)
            col1, col2 = st.columns(2)
            start = col1.date_input("Start date", date.today())
            end = col2.date_input("End date", date.today())
            status = st.selectbox("Status", ["Active", "Paused", "Completed", "Closed"])
            submitted = st.form_submit_button("Create campaign", type="primary")
            cancel = st.form_submit_button("Cancel")
        if submitted:
            ok, output = run_core(["add-campaign", title, description, float(target), str(start), str(end), status])
            after_mutation(output, "create campaign")
            if ok and output.startswith("SUCCESS|"):
                st.session_state.show_campaign_form = False
                refresh()
        if cancel:
            st.session_state.show_campaign_form = False
            refresh()
        render_html("</section>", unsafe_allow_html=True)

    campaigns = data["campaigns"]
    if filter_active and not campaigns.empty and "status" in campaigns.columns:
        campaigns = campaigns[campaigns["status"].str.lower() == "active"]
    render_html(
        f"""
        <section class="panel-card">
          <div class="panel-title"><div><h3>Campaign list</h3><p>Target, collected, allocated, available amount, and status.</p></div><span class="chip blue">{'Active only' if filter_active else 'Showing all'}</span></div>
          <div class="campaign-list">{campaign_rows(campaigns)}</div>
        </section>
        """,
        unsafe_allow_html=True,
    )


def reports_page(data: dict[str, pd.DataFrame]) -> None:
    summary = summary_from_data(data)
    page_header("Reports", "Transparency reports for submission and viva.", "Generate reports and download a PDF summary.")
    c1, c2 = st.columns([1, 1])
    with c1:
        month = st.text_input("Report month", value="2026-06")
        if st.button("Generate monthly report", type="primary", use_container_width=True):
            ok, output = run_core(["generate-report", "monthly", month])
            after_mutation(output, "generate report")
            if ok and output.startswith("SUCCESS|"):
                refresh()
    with c2:
        pdf_bytes = build_report_pdf(data)
        st.download_button(
            "Export PDF",
            data=pdf_bytes,
            file_name="charity_transparency_report.pdf",
            mime="application/pdf" if pdf_bytes.startswith(b"%PDF") else "text/plain",
            use_container_width=True,
        )

    metric_grid([
        ("Current donations", format_money(summary["totalDonations"]), "Report period uses current saved data."),
        ("Current allocations", format_money(summary["totalAllocations"]), "Aid distributed to beneficiaries."),
        ("Current balance", format_money(summary["remainingBalance"]), "Remaining balance after allocation."),
    ])

    reports = data["reports"]
    render_html("<section class='panel-card'><div class='panel-title'><div><h3>Generated reports</h3><p>Clear table for screenshots and report evidence.</p></div></div>", unsafe_allow_html=True)
    st.dataframe(reports, use_container_width=True, hide_index=True)
    render_html("</section>", unsafe_allow_html=True)


# ----------------------------- Donor pages -----------------------------
def donor_statement_page(data: dict[str, pd.DataFrame]) -> None:
    donor_id = st.session_state.get("donor_id", "")
    email = st.session_state.get("donor_email", "")
    ok, statement_df, output = command_table(["donor-statement", donor_id, email])
    if not ok or statement_df.empty:
        st.error(output)
        return
    statement = {row["metric"]: row["value"] for _, row in statement_df.iterrows()}
    total = float(statement.get("totalDonated", 0))

    page_header("Donor portal", "My donation statement.", "Read-only view. Donors see their own records, not admin CRUD tools.")
    render_html(
        f"""
        <div class="statement-grid">
          <div class="statement-card">
            <h3>{statement.get('donorName', donor_id)}</h3>
            <p>Verified donor · Email protected: <strong>{mask_email(statement.get('email', email))}</strong></p>
            <div class="statement-metrics">
              <div class="statement-metric"><span>Total donated</span><strong>{format_money(total)}</strong></div>
              <div class="statement-metric"><span>Donations</span><strong>{statement.get('donationCount', '0')}</strong></div>
              <div class="statement-metric"><span>Campaigns</span><strong>{statement.get('campaignsSupported', '0')}</strong></div>
            </div>
          </div>
        </div>
        """,
        unsafe_allow_html=True,
    )

    ok, donations, output = command_table(["donor-donations", donor_id, email])
    donations = numeric_columns(donations, ["amount"])
    render_html("<section class='panel-card'><div class='panel-title'><div><h3>Donation history</h3><p>Only this donor's records are shown.</p></div></div>", unsafe_allow_html=True)
    if ok and not donations.empty:
        st.dataframe(donations, use_container_width=True, hide_index=True)
    else:
        st.info("No donations recorded yet.")
    render_html("</section>", unsafe_allow_html=True)


def donor_impact_page(data: dict[str, pd.DataFrame]) -> None:
    donor_id = st.session_state.get("donor_id", "")
    email = st.session_state.get("donor_email", "")
    page_header("Campaign impact", "Where supported campaign funds went.", "Donors see campaign-level allocation transparency without private contact information.")

    ok, campaigns, output = command_table(["donor-campaigns", donor_id, email])
    campaigns = numeric_columns(campaigns, ["targetAmount", "collectedAmount", "allocatedAmount", "availableBalance", "progressPercent"])
    render_html(
        f"""
        <section class="panel-card">
          <div class="panel-title"><div><h3>Supported campaigns</h3><p>Campaigns connected to this donor's donation history.</p></div><span class="chip blue">{len(campaigns)} supported</span></div>
          <div class="campaign-list">{campaign_rows(campaigns)}</div>
        </section>
        """,
        unsafe_allow_html=True,
    )

    ok, allocations, output = command_table(["donor-allocations", donor_id, email])
    allocations = numeric_columns(allocations, ["amount"])
    render_html("<section class='panel-card'><div class='panel-title'><div><h3>Allocation evidence</h3><p>Beneficiary contact information is hidden from donor view.</p></div></div>", unsafe_allow_html=True)
    if ok and not allocations.empty:
        st.dataframe(allocations, use_container_width=True, hide_index=True)
    else:
        st.info("No campaign allocation evidence available yet.")
    render_html("</section>", unsafe_allow_html=True)


# ----------------------------- Main app -----------------------------
def sidebar(role: str) -> str:
    with st.sidebar:
        st.markdown("### Amanah Charity")
        st.caption("Dynamic data from C++ files")
        st.success("C++ core ready")
        if github_configured():
            st.info("GitHub sync configured")
        else:
            st.warning("GitHub sync not configured")
        st.divider()
        if role == "admin":
            page = st.radio("Navigation", ["Dashboard", "Campaigns", "Reports"], label_visibility="collapsed")
            st.divider()
            if st.button("Seed demo data", use_container_width=True):
                ok, output = run_core(["seed"])
                after_mutation(output, "seed demo data")
                refresh()
            if st.button("Sync data to GitHub", use_container_width=True):
                ok, message = sync_data_to_github("manual sync")
                st.success(message) if ok else st.error(message)
        else:
            page = st.radio("Navigation", ["My Statement", "Campaign Impact"], label_visibility="collapsed")
        st.divider()
        if st.button("Logout", use_container_width=True):
            for key in ["role", "admin_logged_in", "donor_id", "donor_email", "action_panel"]:
                st.session_state.pop(key, None)
            refresh()
    return page


def main() -> None:
    ok, status = compile_core()
    if not ok:
        st.error(status)
        st.stop()

    if "role" not in st.session_state:
        login_page()
        return

    brand_strip()
    data = load_data()
    role = st.session_state.get("role")
    page = sidebar(role)

    if role == "admin":
        if page == "Dashboard":
            admin_dashboard(data)
        elif page == "Campaigns":
            campaigns_page(data)
        elif page == "Reports":
            reports_page(data)
    else:
        if page == "My Statement":
            donor_statement_page(data)
        elif page == "Campaign Impact":
            donor_impact_page(data)


if __name__ == "__main__":
    main()
