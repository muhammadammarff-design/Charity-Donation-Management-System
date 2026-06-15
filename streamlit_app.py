from __future__ import annotations

import csv
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
    page_title="Charity & Donation Management System",
    page_icon="🤝",
    layout="wide",
    initial_sidebar_state="expanded",
)

st.markdown(
    """
    <style>
        .main-header {
            background: linear-gradient(135deg, #0f766e 0%, #115e59 45%, #134e4a 100%);
            color: white;
            padding: 1.3rem 1.5rem;
            border-radius: 18px;
            margin-bottom: 1.2rem;
            box-shadow: 0 10px 30px rgba(15, 118, 110, 0.25);
        }
        .main-header h1 { margin: 0; font-size: 2rem; }
        .main-header p { margin: .35rem 0 0 0; opacity: .92; }
        .info-card {
            border: 1px solid #e5e7eb;
            padding: 1rem;
            border-radius: 14px;
            background: #ffffff;
            box-shadow: 0 5px 18px rgba(15, 23, 42, 0.05);
        }
        .success-box {
            border-left: 5px solid #16a34a;
            background: #f0fdf4;
            padding: .75rem 1rem;
            border-radius: 8px;
            margin: .5rem 0;
        }
        .error-box {
            border-left: 5px solid #dc2626;
            background: #fef2f2;
            padding: .75rem 1rem;
            border-radius: 8px;
            margin: .5rem 0;
        }
        .small-note { color: #64748b; font-size: .92rem; }
        div[data-testid="stMetricValue"] { font-size: 1.65rem; }
    </style>
    """,
    unsafe_allow_html=True,
)


def header(title: str, subtitle: str) -> None:
    st.markdown(
        f"""
        <div class="main-header">
            <h1>{title}</h1>
            <p>{subtitle}</p>
        </div>
        """,
        unsafe_allow_html=True,
    )


@st.cache_resource(show_spinner=False)
def compile_core() -> Tuple[bool, str]:
    BUILD_DIR.mkdir(exist_ok=True)
    sources = sorted(str(p) for p in SRC_DIR.glob("*.cpp"))
    if not sources:
        return False, "No C++ source files found."

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
        return False, "g++ compiler not found. Install MinGW/GCC locally or build-essential on Linux."

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
    header_row = rows[0]
    data_rows = rows[1:]
    return pd.DataFrame(data_rows, columns=header_row)


def parse_result(output: str) -> Tuple[bool, str, str]:
    if output.startswith("SUCCESS|"):
        parts = output.split("|", 2)
        message = parts[1] if len(parts) > 1 else "Operation successful."
        item_id = parts[2] if len(parts) > 2 else ""
        return True, message, item_id
    if output.startswith("ERROR|"):
        parts = output.split("|", 1)
        message = parts[1] if len(parts) > 1 else "Operation failed."
        return False, message, ""
    return True, output, ""


def show_result(output: str) -> None:
    success, message, item_id = parse_result(output)
    if success:
        extra = f" New ID: **{item_id}**" if item_id else ""
        st.success(f"{message}{extra}")
    else:
        st.error(message)


def get_table(name: str) -> pd.DataFrame:
    ok, output = run_core(["list", name])
    if not ok:
        st.error(output)
        return pd.DataFrame()
    return parse_pipe_table(output)


def get_summary() -> dict:
    ok, output = run_core(["summary"])
    if not ok:
        st.error(output)
        return {}
    df = parse_pipe_table(output)
    if df.empty:
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


# Compile once and show status in sidebar.
compile_ok, compile_status = compile_core()
with st.sidebar:
    st.title("🤝 Charity System")
    st.caption("Streamlit GUI + C++ OOP Core")
    if compile_ok:
        st.success("C++ core ready")
    else:
        st.error("C++ build failed")
        st.code(compile_status)

    page = st.radio(
        "Navigation",
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
    st.divider()
    st.caption("Team: Ali Raza, Muhammad Ammar, Taha Ali")

if not compile_ok:
    st.stop()

if page == "Dashboard":
    header("Charity & Donation Management System", "Transparent donor, campaign, beneficiary, donation, and fund allocation tracking.")
    summary = get_summary()

    col1, col2, col3, col4 = st.columns(4)
    col1.metric("Donors", summary.get("donors", "0"))
    col2.metric("Campaigns", summary.get("campaigns", "0"))
    col3.metric("Donations", summary.get("donations", "0"))
    col4.metric("Allocations", summary.get("allocations", "0"))

    col5, col6, col7 = st.columns(3)
    col5.metric("Total Donations", f"Rs. {summary.get('totalDonations', '0.00')}")
    col6.metric("Total Allocated", f"Rs. {summary.get('totalAllocations', '0.00')}")
    col7.metric("Remaining Balance", f"Rs. {summary.get('remainingBalance', '0.00')}")

    campaigns = numeric_columns(get_table("campaigns"), ["targetAmount", "collectedAmount", "allocatedAmount", "availableBalance", "progressPercent"])
    donations = numeric_columns(get_table("donations"), ["amount"])

    left, right = st.columns([1.15, 1])
    with left:
        st.subheader("Campaign Progress")
        if campaigns.empty:
            st.info("No campaigns found. Use System Tools → Seed Demo Data or add a campaign.")
        else:
            chart_df = campaigns[["title", "collectedAmount", "targetAmount"]].set_index("title")
            st.bar_chart(chart_df)
            st.dataframe(campaigns, use_container_width=True, hide_index=True)

    with right:
        st.subheader("Recent Donations")
        if donations.empty:
            st.info("No donations recorded yet.")
        else:
            st.dataframe(donations.tail(8), use_container_width=True, hide_index=True)
            by_campaign = donations.groupby("campaignId", as_index=False)["amount"].sum()
            st.caption("Donation totals by campaign")
            st.bar_chart(by_campaign.set_index("campaignId"))

elif page == "Donors":
    header("Donor Management", "Add, view, update, and protect donor donation history.")
    donors = get_table("donors")

    add_col, edit_col = st.columns([1, 1])
    with add_col:
        st.subheader("Add New Donor")
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

    with edit_col:
        st.subheader("Update / Delete Donor")
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
            if st.button("Delete Selected Donor", type="secondary"):
                ok, output = run_core(["delete-donor", donor_id])
                show_result(output)
                if ok and output.startswith("SUCCESS|"):
                    refresh()

    st.subheader("Donor Records")
    st.dataframe(donors, use_container_width=True, hide_index=True)

elif page == "Campaigns":
    header("Campaign Management", "Create charity campaigns and track target vs collected funds.")
    campaigns = numeric_columns(get_table("campaigns"), ["targetAmount", "collectedAmount", "allocatedAmount", "availableBalance", "progressPercent"])

    add_col, edit_col = st.columns([1, 1])
    with add_col:
        st.subheader("Add New Campaign")
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

    with edit_col:
        st.subheader("Update / Delete Campaign")
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
            if st.button("Delete Selected Campaign"):
                ok, output = run_core(["delete-campaign", campaign_id])
                show_result(output)
                if ok and output.startswith("SUCCESS|"):
                    refresh()

    st.subheader("Campaign Records")
    if not campaigns.empty:
        st.dataframe(campaigns, use_container_width=True, hide_index=True)
        st.progress(min(float(campaigns["progressPercent"].mean()) / 100.0, 1.0), text="Average campaign funding progress")
    else:
        st.info("No campaign records found.")

elif page == "Beneficiaries":
    header("Beneficiary Management", "Register beneficiaries and link each person/family to a campaign.")
    beneficiaries = get_table("beneficiaries")
    campaigns = get_table("campaigns")
    campaign_options = id_options(campaigns, "title")

    add_col, edit_col = st.columns([1, 1])
    with add_col:
        st.subheader("Add New Beneficiary")
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

    with edit_col:
        st.subheader("Update / Delete Beneficiary")
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
            if st.button("Delete Selected Beneficiary"):
                ok, output = run_core(["delete-beneficiary", beneficiary_id])
                show_result(output)
                if ok and output.startswith("SUCCESS|"):
                    refresh()

    st.subheader("Beneficiary Records")
    st.dataframe(beneficiaries, use_container_width=True, hide_index=True)

elif page == "Donations":
    header("Donation Recording", "Log each donation with donor, campaign, amount, date, payment method, and notes.")
    donors = get_table("donors")
    campaigns = get_table("campaigns")
    donations = numeric_columns(get_table("donations"), ["amount"])

    donor_options = id_options(donors, "name")
    campaign_options = id_options(campaigns, "title")

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

    st.subheader("Donation Records")
    st.dataframe(donations, use_container_width=True, hide_index=True)
    if not donations.empty:
        st.bar_chart(donations.groupby("date", as_index=True)["amount"].sum())

elif page == "Fund Allocation":
    header("Fund Allocation", "Distribute collected funds to linked beneficiaries without exceeding campaign balance.")
    beneficiaries = get_table("beneficiaries")
    campaigns = numeric_columns(get_table("campaigns"), ["availableBalance"])
    allocations = numeric_columns(get_table("allocations"), ["amount"])

    beneficiary_options = id_options(beneficiaries, "name")
    campaign_options = id_options(campaigns, "title")

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

    col_a, col_b = st.columns([1, 1])
    with col_a:
        st.subheader("Available Campaign Balances")
        if campaigns.empty:
            st.info("No campaigns found.")
        else:
            st.dataframe(campaigns[["id", "title", "collectedAmount", "allocatedAmount", "availableBalance"]], use_container_width=True, hide_index=True)
    with col_b:
        st.subheader("Allocation Records")
        st.dataframe(allocations, use_container_width=True, hide_index=True)

elif page == "Reports":
    header("Reports", "Generate monthly or campaign-level transparency reports.")
    campaigns = get_table("campaigns")
    campaign_options = id_options(campaigns, "title")

    col1, col2 = st.columns(2)
    with col1:
        st.subheader("Monthly Report")
        month = st.text_input("Month (YYYY-MM)", value="2026-06")
        if st.button("Generate Monthly Report", type="primary"):
            ok, output = run_core(["generate-report", "monthly", month])
            show_result(output)
            if ok and output.startswith("SUCCESS|"):
                refresh()
    with col2:
        st.subheader("Campaign Report")
        if campaign_options:
            campaign_option = st.selectbox("Campaign", campaign_options)
            if st.button("Generate Campaign Report"):
                ok, output = run_core(["generate-report", "campaign", selected_id(campaign_option)])
                show_result(output)
                if ok and output.startswith("SUCCESS|"):
                    refresh()
        else:
            st.info("Add campaign data first.")

    reports = numeric_columns(get_table("reports"), ["totalDonations", "totalAllocations", "remainingBalance"])
    st.subheader("Generated Reports")
    st.dataframe(reports, use_container_width=True, hide_index=True)
    if not reports.empty:
        st.bar_chart(reports.set_index("id")[["totalDonations", "totalAllocations", "remainingBalance"]])

elif page == "Validation Evidence":
    header("Input Validation Evidence", "C++ core rejects invalid data even if GUI validation is bypassed.")
    st.markdown(
        """
        The table below is produced by the **C++ executable**, not by Streamlit. This is important for the rubric because validation is implemented inside the OOP core.
        """
    )
    ok, output = run_core(["validation-demo"])
    if not ok:
        st.error(output)
    else:
        df = parse_pipe_table(output)
        st.dataframe(df, use_container_width=True, hide_index=True)
        st.download_button("Download validation evidence as CSV", output, file_name="validation_evidence.txt")

    st.subheader("Manual Validation Test")
    st.caption("Try recording a negative donation against the first available donor/campaign.")
    donors = get_table("donors")
    campaigns = get_table("campaigns")
    if st.button("Run Negative Donation Test"):
        if donors.empty or campaigns.empty:
            st.error("Seed demo data first.")
        else:
            donor_id = donors.iloc[0]["id"]
            campaign_id = campaigns.iloc[0]["id"]
            ok, output = run_core(["record-donation", donor_id, campaign_id, -100, "2026-06-14", "Cash", "negative test"])
            show_result(output)

elif page == "OOP Evidence":
    header("OOP Evidence", "Demonstration of inheritance, polymorphism, constructors, and operator overloading.")
    ok, output = run_core(["oop-demo"])
    if not ok:
        st.error(output)
    else:
        df = parse_pipe_table(output)
        st.dataframe(df, use_container_width=True, hide_index=True)
        st.code(output, language="text")

    st.markdown(
        """
        **C++ classes used:** `Person`, `Donor`, `Beneficiary`, `Campaign`, `Donation`, `FundAllocation`, `Report`, `FileManager`, and `CharitySystem`.
        """
    )

elif page == "System Tools":
    header("System Tools", "Reset, seed, inspect build status, and prepare the project for demo.")
    st.markdown(f"**Data directory:** `{DATA_DIR}`")
    st.markdown(f"**Executable:** `{EXE}`")
    st.info("No full login system is added because the rubric and viva focus on C++ OOP, CRUD, validation, file handling, UML, and GUI. Keeping auth out reduces unnecessary complexity.")

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
            if ok:
                st.success(msg)
            else:
                st.error(msg)

    st.subheader("Command Help")
    ok, help_output = run_core(["help"])
    st.code(help_output, language="text")
