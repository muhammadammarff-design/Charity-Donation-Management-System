from __future__ import annotations

import csv
import html
import os
import platform
import subprocess
from datetime import date
from io import StringIO
from pathlib import Path
from typing import Iterable, List, Tuple

import pandas as pd
import streamlit as st

ROOT = Path(__file__).resolve().parent
SRC_DIR = ROOT / "cpp_core" / "src"
INCLUDE_DIR = ROOT / "cpp_core" / "include"
BUILD_DIR = ROOT / "build"
EXE = BUILD_DIR / ("charity_app.exe" if platform.system() == "Windows" else "charity_app")
DATA_DIR = ROOT / "data"

st.set_page_config(
    page_title="Charity Donation System",
    layout="wide",
    initial_sidebar_state="expanded",
)

# Calm, accessible theme. No harsh gradients, no dark glare.
st.markdown(
    """
    <style>
        :root {
            --bg: #f7faf8;
            --surface: #ffffff;
            --surface-soft: #f0f7f3;
            --text: #1f2933;
            --muted: #64748b;
            --primary: #2f7d64;
            --primary-dark: #225c4a;
            --border: #dbe7e1;
            --success: #247857;
            --warning: #b7791f;
            --danger: #b42318;
            --shadow: 0 12px 28px rgba(31, 41, 51, 0.07);
            --radius: 18px;
        }

        .stApp {
            background: linear-gradient(180deg, #f7faf8 0%, #ffffff 52%, #f8fafc 100%);
            color: var(--text);
        }

        [data-testid="block-container"] {
            padding-top: 1.4rem;
            padding-bottom: 3rem;
            max-width: 1180px;
        }

        [data-testid="stSidebar"] {
            background: #ffffff;
            border-right: 1px solid var(--border);
        }

        h1, h2, h3 {
            color: var(--text);
            letter-spacing: -0.02em;
        }

        .soft-hero {
            background: rgba(255,255,255,0.92);
            border: 1px solid var(--border);
            border-radius: 24px;
            padding: 1.35rem 1.55rem;
            box-shadow: var(--shadow);
            margin-bottom: 1rem;
            animation: fadeUp .45s ease-out both;
        }

        .soft-hero .eyebrow {
            color: var(--primary);
            font-size: .78rem;
            font-weight: 800;
            text-transform: uppercase;
            letter-spacing: .08em;
            margin-bottom: .25rem;
        }

        .soft-hero h1 {
            margin: 0;
            font-size: clamp(1.55rem, 2.5vw, 2.35rem);
            line-height: 1.15;
        }

        .soft-hero p {
            color: var(--muted);
            margin: .45rem 0 0;
            font-size: 1rem;
            max-width: 850px;
        }

        .card-grid {
            display: grid;
            grid-template-columns: repeat(4, minmax(0, 1fr));
            gap: .85rem;
            margin: .8rem 0 1rem;
        }

        @media (max-width: 920px) {
            .card-grid { grid-template-columns: repeat(2, minmax(0, 1fr)); }
        }
        @media (max-width: 560px) {
            .card-grid { grid-template-columns: 1fr; }
        }

        .stat-card {
            background: var(--surface);
            border: 1px solid var(--border);
            border-radius: var(--radius);
            padding: 1rem 1.05rem;
            box-shadow: 0 8px 22px rgba(31,41,51,.055);
            animation: fadeUp .55s ease-out both;
            transition: transform .22s ease, box-shadow .22s ease, border-color .22s ease;
            min-height: 110px;
        }

        .stat-card:hover {
            transform: translateY(-3px);
            box-shadow: 0 14px 32px rgba(31,41,51,.09);
            border-color: #b8d8c8;
        }

        .stat-label {
            color: var(--muted);
            font-size: .84rem;
            margin-bottom: .35rem;
        }

        .stat-value {
            color: var(--text);
            font-size: 1.65rem;
            font-weight: 800;
            line-height: 1.1;
        }

        .stat-help {
            color: var(--primary-dark);
            font-size: .82rem;
            margin-top: .45rem;
        }

        .section-card {
            background: rgba(255,255,255,.96);
            border: 1px solid var(--border);
            border-radius: 20px;
            padding: 1rem 1.05rem;
            box-shadow: 0 8px 22px rgba(31,41,51,.045);
            margin-bottom: .95rem;
            animation: fadeUp .5s ease-out both;
        }

        .mini-title {
            color: var(--text);
            font-weight: 800;
            font-size: 1.04rem;
            margin-bottom: .35rem;
        }

        .muted { color: var(--muted); }

        .pill {
            display: inline-block;
            padding: .25rem .55rem;
            border-radius: 999px;
            background: var(--surface-soft);
            border: 1px solid var(--border);
            color: var(--primary-dark);
            font-size: .78rem;
            font-weight: 700;
        }

        .stButton>button, .stDownloadButton>button {
            border-radius: 12px !important;
            min-height: 42px;
            transition: transform .18s ease, box-shadow .18s ease, border-color .18s ease;
        }
        .stButton>button:hover, .stDownloadButton>button:hover {
            transform: translateY(-1px);
            box-shadow: 0 8px 18px rgba(47,125,100,.14);
        }

        input, textarea, select {
            border-radius: 10px !important;
        }

        @keyframes fadeUp {
            from { opacity: 0; transform: translateY(10px); }
            to { opacity: 1; transform: translateY(0); }
        }

        @media (prefers-reduced-motion: reduce) {
            * { animation: none !important; transition: none !important; }
        }
    </style>
    """,
    unsafe_allow_html=True,
)


def safe(value: object) -> str:
    return html.escape(str(value))


def soft_header(title: str, subtitle: str, eyebrow: str = "Charity Transparency") -> None:
    st.markdown(
        f"""
        <div class="soft-hero">
            <div class="eyebrow">{safe(eyebrow)}</div>
            <h1>{safe(title)}</h1>
            <p>{safe(subtitle)}</p>
        </div>
        """,
        unsafe_allow_html=True,
    )


def stat_card(label: str, value: object, help_text: str = "") -> None:
    st.markdown(
        f"""
        <div class="stat-card">
            <div class="stat-label">{safe(label)}</div>
            <div class="stat-value">{safe(value)}</div>
            <div class="stat-help">{safe(help_text)}</div>
        </div>
        """,
        unsafe_allow_html=True,
    )


def section_start(title: str, note: str = "") -> None:
    st.markdown('<div class="section-card">', unsafe_allow_html=True)
    st.markdown(f"<div class='mini-title'>{safe(title)}</div>", unsafe_allow_html=True)
    if note:
        st.markdown(f"<div class='muted'>{safe(note)}</div>", unsafe_allow_html=True)


def section_end() -> None:
    st.markdown("</div>", unsafe_allow_html=True)


@st.cache_resource(show_spinner=False)
def compile_core() -> Tuple[bool, str]:
    BUILD_DIR.mkdir(exist_ok=True)
    sources = sorted(str(p) for p in SRC_DIR.glob("*.cpp"))
    if not sources:
        return False, "No C++ source files found. Make sure cpp_core/src exists at repository root."

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

    command = [str(EXE), *[str(arg) for arg in args]]
    completed = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, text=True, check=False)
    output = (completed.stdout or "") + (completed.stderr or "")
    return completed.returncode == 0, output.strip()


def parse_pipe_table(output: str) -> pd.DataFrame:
    if not output or output.startswith("ERROR|") or output.startswith("SUCCESS|"):
        return pd.DataFrame()
    reader = csv.reader(StringIO(output), delimiter="|")
    rows = list(reader)
    if not rows:
        return pd.DataFrame()
    return pd.DataFrame(rows[1:], columns=rows[0])


def parse_result(output: str) -> Tuple[bool, str, str]:
    if output.startswith("SUCCESS|"):
        parts = output.split("|", 2)
        return True, parts[1] if len(parts) > 1 else "Operation successful.", parts[2] if len(parts) > 2 else ""
    if output.startswith("ERROR|"):
        parts = output.split("|", 1)
        return False, parts[1] if len(parts) > 1 else "Operation failed.", ""
    return True, output, ""


def show_result(output: str) -> None:
    success, message, item_id = parse_result(output)
    if success:
        st.success(f"{message}" + (f" New ID: {item_id}" if item_id else ""))
    else:
        st.error(message)


def get_table(name: str) -> pd.DataFrame:
    ok, output = run_core(["list", name])
    if not ok:
        st.error(output)
        return pd.DataFrame()
    return parse_pipe_table(output)


def command_table(args: List[str]) -> Tuple[bool, pd.DataFrame, str]:
    ok, output = run_core(args)
    if not ok or output.startswith("ERROR|"):
        return False, pd.DataFrame(), output
    return True, parse_pipe_table(output), output


def get_summary() -> dict:
    ok, df, output = command_table(["summary"])
    if not ok or df.empty:
        if output:
            st.error(output)
        return {}
    return {row["metric"]: row["value"] for _, row in df.iterrows()}


def numeric_columns(df: pd.DataFrame, cols: List[str]) -> pd.DataFrame:
    copy = df.copy()
    for col in cols:
        if col in copy.columns:
            copy[col] = pd.to_numeric(copy[col], errors="coerce").fillna(0)
    return copy


def id_options(df: pd.DataFrame, label_col: str) -> List[str]:
    if df.empty or "id" not in df.columns:
        return []
    if label_col in df.columns:
        return [f"{row['id']} - {row[label_col]}" for _, row in df.iterrows()]
    return df["id"].tolist()


def selected_id(option: str) -> str:
    return option.split(" - ", 1)[0] if option else ""


def refresh() -> None:
    st.rerun()


def get_secret(name: str, default: str) -> str:
    value = os.environ.get(name)
    if value:
        return value
    try:
        return str(st.secrets.get(name, default))
    except Exception:
        return default


def ensure_demo_data_if_empty() -> None:
    donors = get_table("donors")
    if donors.empty:
        st.info("No demo records are loaded yet. Click below to load sample data for testing.")
        if st.button("Load demo data", type="primary", key="load_demo_empty"):
            ok, output = run_core(["seed"])
            show_result(output)
            if ok:
                refresh()


def login_admin() -> bool:
    if st.session_state.get("admin_logged_in"):
        return True

    soft_header("Admin Login", "Admin area is protected so public visitors cannot accidentally change demo records.", "Access Control")
    st.info("Demo credentials: username `admin`, password `admin123`. You can change them later using Streamlit secrets.")
    with st.form("admin_login_form"):
        username = st.text_input("Username", value="admin")
        password = st.text_input("Password", type="password")
        submitted = st.form_submit_button("Login", type="primary")

    if submitted:
        expected_user = get_secret("ADMIN_USERNAME", "admin")
        expected_password = get_secret("ADMIN_PASSWORD", "admin123")
        if username == expected_user and password == expected_password:
            st.session_state["admin_logged_in"] = True
            st.success("Logged in successfully.")
            refresh()
        else:
            st.error("Invalid admin username or password.")
    return False


def donor_portal_page() -> None:
    soft_header(
        "Donor Transparency Portal",
        "A donor can verify using donor ID and email, then view their donations and how supported campaigns are using funds.",
        "Read-only donor view",
    )
    ensure_demo_data_if_empty()

    with st.expander("Sample donor for hosted demo", expanded=True):
        st.write("Donor ID: `DNR001`  |  Email: `ahmed.khan@example.com`")

    with st.form("donor_verify_form"):
        col1, col2 = st.columns(2)
        donor_id = col1.text_input("Donor ID", value=st.session_state.get("donor_id", "DNR001"))
        email = col2.text_input("Email", value=st.session_state.get("donor_email", "ahmed.khan@example.com"))
        submitted = st.form_submit_button("View my donations", type="primary")

    if submitted:
        ok, output = run_core(["verify-donor", donor_id, email])
        if ok and output.startswith("SUCCESS|"):
            st.session_state["donor_verified"] = True
            st.session_state["donor_id"] = donor_id.strip()
            st.session_state["donor_email"] = email.strip()
            st.success("Donor verified. Your transparency statement is shown below.")
        else:
            st.session_state["donor_verified"] = False
            show_result(output)

    if not st.session_state.get("donor_verified"):
        st.markdown("<span class='pill'>Read-only</span> <span class='pill'>Donation history</span> <span class='pill'>Campaign transparency</span>", unsafe_allow_html=True)
        return

    donor_id = st.session_state["donor_id"]
    email = st.session_state["donor_email"]

    ok, statement_df, statement_output = command_table(["donor-statement", donor_id, email])
    if not ok or statement_df.empty:
        show_result(statement_output)
        return
    statement = {row["metric"]: row["value"] for _, row in statement_df.iterrows()}

    st.markdown('<div class="card-grid">', unsafe_allow_html=True)
    stat_card("Donor", statement.get("donorName", "-"), statement.get("donorId", ""))
    stat_card("Total Donated", f"Rs. {statement.get('totalDonated', '0.00')}", "Recorded in C++ files")
    stat_card("Donations", statement.get("donationCount", "0"), "Number of donation records")
    stat_card("Campaigns Supported", statement.get("campaignsSupported", "0"), "Causes linked to this donor")
    st.markdown("</div>", unsafe_allow_html=True)

    donations_ok, donations, donations_output = command_table(["donor-donations", donor_id, email])
    campaigns_ok, campaigns, campaigns_output = command_table(["donor-campaigns", donor_id, email])
    allocations_ok, allocations, allocations_output = command_table(["donor-allocations", donor_id, email])

    donations = numeric_columns(donations, ["amount"])
    campaigns = numeric_columns(campaigns, ["targetAmount", "collectedAmount", "allocatedAmount", "availableBalance", "progressPercent"])
    allocations = numeric_columns(allocations, ["amount"])

    left, right = st.columns([1.05, 1])
    with left:
        section_start("My Donation History", "Only donations matching your verified donor ID are displayed.")
        if donations_ok and not donations.empty:
            st.dataframe(donations, use_container_width=True, hide_index=True)
            st.bar_chart(donations.set_index("date")["amount"])
        else:
            st.info("No donations found for this donor.")
        section_end()

    with right:
        section_start("Supported Campaigns", "Collected, allocated, and available balances for campaigns you supported.")
        if campaigns_ok and not campaigns.empty:
            for _, row in campaigns.iterrows():
                progress = min(float(row.get("progressPercent", 0)) / 100.0, 1.0)
                st.write(f"**{row['title']}** — Rs. {row['collectedAmount']:.2f} collected of Rs. {row['targetAmount']:.2f}")
                st.progress(progress, text=f"{row['progressPercent']:.2f}% funded")
                st.caption(f"Allocated: Rs. {row['allocatedAmount']:.2f} | Available: Rs. {row['availableBalance']:.2f}")
        else:
            st.info(campaigns_output or "No supported campaign data found.")
        section_end()

    section_start("Where supported campaign funds were allocated", "This is campaign-level transparency, not a private beneficiary login.")
    if allocations_ok and not allocations.empty:
        st.dataframe(allocations, use_container_width=True, hide_index=True)
    else:
        st.info(allocations_output or "No allocation records found for supported campaigns.")
    section_end()


def admin_dashboard_page() -> None:
    soft_header(
        "Admin Dashboard",
        "Simple overview of donations received, funds allocated, and remaining balance. Use the side menu for detailed management.",
        "Admin overview",
    )
    summary = get_summary()
    campaigns = numeric_columns(get_table("campaigns"), ["targetAmount", "collectedAmount", "allocatedAmount", "availableBalance", "progressPercent"])
    donations = numeric_columns(get_table("donations"), ["amount"])
    donors = numeric_columns(get_table("donors"), ["totalDonated"])

    st.markdown('<div class="card-grid">', unsafe_allow_html=True)
    stat_card("Total Donations", f"Rs. {summary.get('totalDonations', '0.00')}", f"{summary.get('donations', '0')} donation records")
    stat_card("Allocated Funds", f"Rs. {summary.get('totalAllocations', '0.00')}", f"{summary.get('allocations', '0')} aid allocations")
    stat_card("Remaining Balance", f"Rs. {summary.get('remainingBalance', '0.00')}", "Available for future aid")
    stat_card("Active Records", f"{summary.get('donors', '0')} donors", f"{summary.get('campaigns', '0')} campaigns")
    st.markdown('</div>', unsafe_allow_html=True)

    left, right = st.columns([1.05, 1])
    with left:
        section_start("Campaign funding status", "Calm progress bars instead of a crowded chart.")
        if campaigns.empty:
            st.info("No campaigns found. Use System Tools to seed demo data or add a campaign.")
        else:
            for _, row in campaigns.sort_values("progressPercent", ascending=False).iterrows():
                progress = min(float(row["progressPercent"]) / 100.0, 1.0)
                st.write(f"**{row['title']}**")
                st.progress(progress, text=f"{row['progressPercent']:.2f}% funded")
                st.caption(f"Collected Rs. {row['collectedAmount']:.2f} | Allocated Rs. {row['allocatedAmount']:.2f} | Available Rs. {row['availableBalance']:.2f}")
        section_end()

    with right:
        section_start("Quick insight", "Fast facts for viva/demo.")
        if not donors.empty:
            top_donor = donors.sort_values("totalDonated", ascending=False).iloc[0]
            st.write(f"**Top donor:** {top_donor['name']} — Rs. {top_donor['totalDonated']:.2f}")
        if not campaigns.empty:
            lowest = campaigns.sort_values("progressPercent", ascending=True).iloc[0]
            st.write(f"**Needs attention:** {lowest['title']} — {lowest['progressPercent']:.2f}% funded")
        if not donations.empty:
            st.write("**Recent donations**")
            st.dataframe(donations.tail(5), use_container_width=True, hide_index=True)
        st.caption("Dashboard intentionally stays simple: key numbers, campaign progress, recent activity.")
        section_end()


def donor_management_page() -> None:
    soft_header("Donor Management", "Add, update, delete, and view donors. Donation history prevents unsafe deletion.", "Admin")
    donors = get_table("donors")

    add_col, edit_col = st.columns([1, 1])
    with add_col:
        section_start("Add donor")
        with st.form("add_donor_form", clear_on_submit=True):
            name = st.text_input("Full Name")
            age = st.number_input("Age / Organization Years", min_value=1, max_value=120, value=25)
            contact = st.text_input("Contact", placeholder="+92 300 1234567")
            email = st.text_input("Email", placeholder="donor@example.com")
            address = st.text_input("Address")
            donor_type = st.selectbox("Donor Type", ["Individual", "Organization", "Family", "Anonymous"])
            submitted = st.form_submit_button("Add Donor", type="primary")
        if submitted:
            ok, output = run_core(["add-donor", name, int(age), contact, email, address, donor_type])
            show_result(output)
            if ok and output.startswith("SUCCESS|"):
                refresh()
        section_end()

    with edit_col:
        section_start("Update or delete donor")
        options = id_options(donors, "name")
        if not options:
            st.info("No donor records available.")
        else:
            option = st.selectbox("Select Donor", options)
            donor_id = selected_id(option)
            row = donors[donors["id"] == donor_id].iloc[0]
            with st.form("update_donor_form"):
                new_name = st.text_input("Full Name", row["name"])
                new_age = st.number_input("Age / Organization Years", min_value=1, max_value=120, value=int(row["age"]))
                new_contact = st.text_input("Contact", row["contact"])
                new_email = st.text_input("Email", row["email"])
                new_address = st.text_input("Address", row["address"])
                new_type = st.text_input("Donor Type", row["donorType"])
                update_clicked = st.form_submit_button("Update Donor")
            if update_clicked:
                ok, output = run_core(["update-donor", donor_id, new_name, int(new_age), new_contact, new_email, new_address, new_type])
                show_result(output)
                if ok and output.startswith("SUCCESS|"):
                    refresh()
            if st.button("Delete Selected Donor", key="delete_donor"):
                ok, output = run_core(["delete-donor", donor_id])
                show_result(output)
                if ok and output.startswith("SUCCESS|"):
                    refresh()
        section_end()

    section_start("Donor records")
    st.dataframe(donors, use_container_width=True, hide_index=True)
    section_end()


def campaign_management_page() -> None:
    soft_header("Campaign Management", "Create and manage campaigns with target, collection, allocation, and progress tracking.", "Admin")
    campaigns = numeric_columns(get_table("campaigns"), ["targetAmount", "collectedAmount", "allocatedAmount", "availableBalance", "progressPercent"])

    add_col, edit_col = st.columns([1, 1])
    with add_col:
        section_start("Add campaign")
        with st.form("add_campaign_form", clear_on_submit=True):
            title = st.text_input("Campaign Title")
            description = st.text_area("Description")
            target = st.number_input("Target Amount", min_value=1.0, value=100000.0, step=5000.0)
            start = st.date_input("Start Date", date(2026, 6, 1))
            end = st.date_input("End Date", date(2026, 7, 31))
            status = st.selectbox("Status", ["Active", "Completed", "Paused", "Closed"])
            submitted = st.form_submit_button("Add Campaign", type="primary")
        if submitted:
            ok, output = run_core(["add-campaign", title, description, float(target), str(start), str(end), status])
            show_result(output)
            if ok and output.startswith("SUCCESS|"):
                refresh()
        section_end()

    with edit_col:
        section_start("Update or delete campaign")
        options = id_options(campaigns, "title")
        if not options:
            st.info("No campaigns available.")
        else:
            option = st.selectbox("Select Campaign", options)
            campaign_id = selected_id(option)
            row = campaigns[campaigns["id"] == campaign_id].iloc[0]
            with st.form("update_campaign_form"):
                new_title = st.text_input("Campaign Title", row["title"])
                new_description = st.text_area("Description", row["description"])
                new_target = st.number_input("Target Amount", min_value=1.0, value=float(row["targetAmount"]), step=5000.0)
                new_start = st.text_input("Start Date (YYYY-MM-DD)", row["startDate"])
                new_end = st.text_input("End Date (YYYY-MM-DD)", row["endDate"])
                new_status = st.text_input("Status", row["status"])
                update_clicked = st.form_submit_button("Update Campaign")
            if update_clicked:
                ok, output = run_core(["update-campaign", campaign_id, new_title, new_description, float(new_target), new_start, new_end, new_status])
                show_result(output)
                if ok and output.startswith("SUCCESS|"):
                    refresh()
            if st.button("Delete Selected Campaign", key="delete_campaign"):
                ok, output = run_core(["delete-campaign", campaign_id])
                show_result(output)
                if ok and output.startswith("SUCCESS|"):
                    refresh()
        section_end()

    section_start("Campaign records")
    if campaigns.empty:
        st.info("No campaign records found.")
    else:
        st.dataframe(campaigns, use_container_width=True, hide_index=True)
    section_end()


def beneficiary_management_page() -> None:
    soft_header("Beneficiary Management", "Register beneficiaries and link each one to a specific campaign.", "Admin")
    beneficiaries = get_table("beneficiaries")
    campaigns = get_table("campaigns")
    campaign_options = id_options(campaigns, "title")

    add_col, edit_col = st.columns([1, 1])
    with add_col:
        section_start("Add beneficiary")
        with st.form("add_beneficiary_form", clear_on_submit=True):
            name = st.text_input("Name / Family Name")
            age = st.number_input("Age", min_value=1, max_value=120, value=30)
            contact = st.text_input("Contact", placeholder="+92 300 1234567")
            address = st.text_input("Address")
            family_size = st.number_input("Family Size", min_value=1, max_value=50, value=1)
            need_type = st.text_input("Need Type", placeholder="Food, Education, Medical...")
            campaign_option = st.selectbox("Linked Campaign", campaign_options) if campaign_options else ""
            submitted = st.form_submit_button("Add Beneficiary", type="primary")
        if submitted:
            if not campaign_options:
                st.error("Create a campaign first.")
            else:
                ok, output = run_core(["add-beneficiary", name, int(age), contact, address, int(family_size), need_type, selected_id(campaign_option)])
                show_result(output)
                if ok and output.startswith("SUCCESS|"):
                    refresh()
        section_end()

    with edit_col:
        section_start("Update or delete beneficiary")
        options = id_options(beneficiaries, "name")
        if not options:
            st.info("No beneficiary records available.")
        else:
            option = st.selectbox("Select Beneficiary", options)
            beneficiary_id = selected_id(option)
            row = beneficiaries[beneficiaries["id"] == beneficiary_id].iloc[0]
            default_campaign = next((opt for opt in campaign_options if opt.startswith(row["campaignId"])), campaign_options[0] if campaign_options else "")
            with st.form("update_beneficiary_form"):
                new_name = st.text_input("Name / Family Name", row["name"])
                new_age = st.number_input("Age", min_value=1, max_value=120, value=int(row["age"]))
                new_contact = st.text_input("Contact", row["contact"])
                new_address = st.text_input("Address", row["address"])
                new_family = st.number_input("Family Size", min_value=1, max_value=50, value=int(row["familySize"]))
                new_need = st.text_input("Need Type", row["needType"])
                new_campaign = st.selectbox("Linked Campaign", campaign_options, index=campaign_options.index(default_campaign) if default_campaign in campaign_options else 0)
                update_clicked = st.form_submit_button("Update Beneficiary")
            if update_clicked:
                ok, output = run_core(["update-beneficiary", beneficiary_id, new_name, int(new_age), new_contact, new_address, int(new_family), new_need, selected_id(new_campaign)])
                show_result(output)
                if ok and output.startswith("SUCCESS|"):
                    refresh()
            if st.button("Delete Selected Beneficiary", key="delete_beneficiary"):
                ok, output = run_core(["delete-beneficiary", beneficiary_id])
                show_result(output)
                if ok and output.startswith("SUCCESS|"):
                    refresh()
        section_end()

    section_start("Beneficiary records")
    st.dataframe(beneficiaries, use_container_width=True, hide_index=True)
    section_end()


def donations_page() -> None:
    soft_header("Donation Recording", "Record donations and automatically update donor/campaign totals.", "Admin")
    donors = get_table("donors")
    campaigns = get_table("campaigns")
    donations = numeric_columns(get_table("donations"), ["amount"])
    donor_options = id_options(donors, "name")
    campaign_options = id_options(campaigns, "title")

    section_start("Record new donation")
    with st.form("record_donation_form", clear_on_submit=True):
        col1, col2 = st.columns(2)
        donor_option = col1.selectbox("Donor", donor_options) if donor_options else ""
        campaign_option = col2.selectbox("Campaign", campaign_options) if campaign_options else ""
        amount = st.number_input("Donation Amount", min_value=1.0, value=1000.0, step=500.0)
        donation_date = st.date_input("Donation Date", date(2026, 6, 14))
        payment_method = st.selectbox("Payment Method", ["Cash", "Bank Transfer", "Cheque", "JazzCash", "EasyPaisa", "Other"])
        note = st.text_input("Note", value="General donation")
        submitted = st.form_submit_button("Record Donation", type="primary")
    if submitted:
        if not donor_options or not campaign_options:
            st.error("Add at least one donor and one campaign first.")
        else:
            ok, output = run_core(["record-donation", selected_id(donor_option), selected_id(campaign_option), float(amount), str(donation_date), payment_method, note])
            show_result(output)
            if ok and output.startswith("SUCCESS|"):
                refresh()
    section_end()

    section_start("Donation records")
    st.dataframe(donations, use_container_width=True, hide_index=True)
    section_end()


def fund_allocation_page() -> None:
    soft_header("Fund Allocation", "Distribute funds to beneficiaries without exceeding campaign balance.", "Admin")
    beneficiaries = get_table("beneficiaries")
    campaigns = numeric_columns(get_table("campaigns"), ["availableBalance", "collectedAmount", "allocatedAmount"])
    allocations = numeric_columns(get_table("allocations"), ["amount"])
    beneficiary_options = id_options(beneficiaries, "name")
    campaign_options = id_options(campaigns, "title")

    section_start("Allocate funds")
    with st.form("allocation_form", clear_on_submit=True):
        col1, col2 = st.columns(2)
        beneficiary_option = col1.selectbox("Beneficiary", beneficiary_options) if beneficiary_options else ""
        campaign_option = col2.selectbox("Campaign", campaign_options) if campaign_options else ""
        amount = st.number_input("Allocation Amount", min_value=1.0, value=1000.0, step=500.0)
        allocation_date = st.date_input("Allocation Date", date(2026, 6, 14))
        purpose = st.text_input("Purpose", value="Aid distribution")
        approved_by = st.text_input("Approved By", value="Admin")
        submitted = st.form_submit_button("Allocate Funds", type="primary")
    if submitted:
        if not beneficiary_options or not campaign_options:
            st.error("Add beneficiaries and campaigns first.")
        else:
            ok, output = run_core(["allocate-funds", selected_id(beneficiary_option), selected_id(campaign_option), float(amount), str(allocation_date), purpose, approved_by])
            show_result(output)
            if ok and output.startswith("SUCCESS|"):
                refresh()
    section_end()

    left, right = st.columns([1, 1])
    with left:
        section_start("Available campaign balances")
        if not campaigns.empty:
            st.dataframe(campaigns[["id", "title", "collectedAmount", "allocatedAmount", "availableBalance"]], use_container_width=True, hide_index=True)
        else:
            st.info("No campaigns found.")
        section_end()
    with right:
        section_start("Allocation records")
        st.dataframe(allocations, use_container_width=True, hide_index=True)
        section_end()


def reports_page() -> None:
    soft_header("Reports", "Generate monthly and campaign-level transparency reports.", "Admin")
    campaigns = get_table("campaigns")
    campaign_options = id_options(campaigns, "title")

    col1, col2 = st.columns(2)
    with col1:
        section_start("Monthly report")
        month = st.text_input("Month (YYYY-MM)", value="2026-06")
        if st.button("Generate Monthly Report", type="primary"):
            ok, output = run_core(["generate-report", "monthly", month])
            show_result(output)
            if ok and output.startswith("SUCCESS|"):
                refresh()
        section_end()
    with col2:
        section_start("Campaign report")
        if campaign_options:
            campaign_option = st.selectbox("Campaign", campaign_options)
            if st.button("Generate Campaign Report"):
                ok, output = run_core(["generate-report", "campaign", selected_id(campaign_option)])
                show_result(output)
                if ok and output.startswith("SUCCESS|"):
                    refresh()
        else:
            st.info("Add campaign data first.")
        section_end()

    reports = numeric_columns(get_table("reports"), ["totalDonations", "totalAllocations", "remainingBalance"])
    section_start("Generated reports")
    st.dataframe(reports, use_container_width=True, hide_index=True)
    section_end()


def validation_page() -> None:
    soft_header("Input Validation Evidence", "These test cases are produced by the C++ executable, not only by Streamlit.", "Rubric evidence")
    ok, output = run_core(["validation-demo"])
    section_start("Validation test cases")
    if not ok:
        st.error(output)
    else:
        df = parse_pipe_table(output)
        st.dataframe(df, use_container_width=True, hide_index=True)
        st.download_button("Download validation evidence", output, file_name="validation_evidence.txt")
    section_end()

    section_start("Manual negative donation test")
    donors = get_table("donors")
    campaigns = get_table("campaigns")
    if st.button("Run negative donation test"):
        if donors.empty or campaigns.empty:
            st.error("Seed demo data first.")
        else:
            donor_id = donors.iloc[0]["id"]
            campaign_id = campaigns.iloc[0]["id"]
            ok, output = run_core(["record-donation", donor_id, campaign_id, -100, "2026-06-14", "Cash", "negative test"])
            show_result(output)
    section_end()


def oop_page() -> None:
    soft_header("OOP Evidence", "Inheritance, polymorphism, constructors, operator overloading, and file handling remain in C++.", "Viva evidence")
    ok, output = run_core(["oop-demo"])
    section_start("C++ OOP demo output")
    if not ok:
        st.error(output)
    else:
        df = parse_pipe_table(output)
        st.dataframe(df, use_container_width=True, hide_index=True)
        st.code(output, language="text")
    section_end()


def tools_page() -> None:
    soft_header("System Tools", "Reset demo data, recompile the C++ core, and inspect CLI help.", "Admin")
    st.info("Admin login is a Streamlit-level demo guard. The C++ OOP core still contains the main project logic.")
    st.write(f"Data directory: `{DATA_DIR}`")
    st.write(f"Executable: `{EXE}`")

    col1, col2, col3 = st.columns(3)
    with col1:
        if st.button("Seed Demo Data", type="primary"):
            ok, output = run_core(["seed"])
            show_result(output)
            if ok:
                refresh()
    with col2:
        if st.button("Clear All Data"):
            ok, output = run_core(["clear"])
            show_result(output)
            if ok:
                refresh()
    with col3:
        if st.button("Recompile C++ Core"):
            compile_core.clear()
            ok, msg = compile_core()
            st.success(msg) if ok else st.error(msg)

    section_start("Command help")
    ok, help_output = run_core(["help"])
    st.code(help_output, language="text")
    section_end()


# Build status and navigation
compile_ok, compile_status = compile_core()
with st.sidebar:
    st.markdown("### Charity System")
    st.caption("C++ OOP core + Streamlit interface")
    if compile_ok:
        st.success("C++ core ready")
    else:
        st.error("C++ build failed")
        st.code(compile_status)

if not compile_ok:
    st.stop()

with st.sidebar:
    access_mode = st.radio("Choose access", ["Donor Portal", "Admin Area"], index=0)
    st.divider()

if access_mode == "Donor Portal":
    donor_portal_page()
    st.stop()

with st.sidebar:
    if st.session_state.get("admin_logged_in"):
        st.success("Admin logged in")
        if st.button("Logout"):
            st.session_state["admin_logged_in"] = False
            refresh()
    st.caption("Team: Ali Raza, Muhammad Ammar, Taha Ali")

if not login_admin():
    st.stop()

with st.sidebar:
    page = st.radio(
        "Admin navigation",
        [
            "Dashboard",
            "Donors",
            "Campaigns",
            "Beneficiaries",
            "Donations",
            "Fund Allocation",
            "Reports",
            "Validation Evidence",
            "OOP Evidence",
            "System Tools",
        ],
    )

if page == "Dashboard":
    admin_dashboard_page()
elif page == "Donors":
    donor_management_page()
elif page == "Campaigns":
    campaign_management_page()
elif page == "Beneficiaries":
    beneficiary_management_page()
elif page == "Donations":
    donations_page()
elif page == "Fund Allocation":
    fund_allocation_page()
elif page == "Reports":
    reports_page()
elif page == "Validation Evidence":
    validation_page()
elif page == "OOP Evidence":
    oop_page()
elif page == "System Tools":
    tools_page()
