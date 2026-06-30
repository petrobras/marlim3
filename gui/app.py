"""
Marlim3 GUI - Streamlit-based interface for simulation input/output.

Run with: streamlit run gui/app.py
"""
import locale
# Force dot as decimal separator regardless of system locale
locale.setlocale(locale.LC_NUMERIC, "C")

import streamlit as st
import json
import os
import sys
import tempfile
import atexit
import signal
import pandas as pd
import plotly.graph_objects as go
from pathlib import Path

# Add project root to path for marlim3 imports
PROJECT_ROOT = Path(__file__).parent.parent
sys.path.insert(0, str(PROJECT_ROOT))


def _kill_sim_on_exit():
    """Kill simulation subprocess when Streamlit app stops."""
    pid = st.session_state.get("_sim_pid") if "st" in dir() else None
    if pid:
        try:
            os.kill(pid, signal.SIGTERM)
        except (OSError, ProcessLookupError):
            pass


atexit.register(_kill_sim_on_exit)

st.set_page_config(
    page_title="Marlim3",
    page_icon="🛢️",
    layout="wide",
    initial_sidebar_state="expanded",
)

# Force dot decimal separator in browser number inputs
st.markdown(
    '<meta http-equiv="Content-Language" content="en-US">'
    '<script>document.documentElement.lang = "en-US";</script>',
    unsafe_allow_html=True,
)

# ─────────────────────────────────────────────────────────────────────────────
# CUSTOM THEME / BRAND COLORS
# ─────────────────────────────────────────────────────────────────────────────

st.markdown("""
<style>
    /* Primary color: #39C0E0  Secondary: #FFA933 */
    :root {
        --primary-color: #39C0E0;
        --secondary-color: #FFA933;
    }

    /* ─── Sidebar styling ─── */
    [data-testid="stSidebar"] {
        background: linear-gradient(180deg, #1b2e3b 0%, #162632 100%);
    }
    /* Default text in sidebar */
    [data-testid="stSidebar"] p,
    [data-testid="stSidebar"] span,
    [data-testid="stSidebar"] label,
    [data-testid="stSidebar"] .stMarkdown {
        color: #dce6ec !important;
    }

    /* ─── Expander headers (the ONLY section header style) ─── */
    [data-testid="stSidebar"] [data-testid="stExpander"] summary {
        background: rgba(57, 192, 224, 0.06);
        border-radius: 6px;
        padding: 0.35rem 0.6rem;
        border: 1px solid rgba(57, 192, 224, 0.15);
    }
    [data-testid="stSidebar"] [data-testid="stExpander"] summary span,
    [data-testid="stSidebar"] [data-testid="stExpander"] summary p {
        color: #39C0E0 !important;
        font-size: 0.78rem !important;
        font-weight: 700 !important;
        text-transform: uppercase;
        letter-spacing: 1.2px;
    }
    /* Expanded state — keep same color, no white override */
    [data-testid="stSidebar"] [data-testid="stExpander"] details[open] summary {
        background: rgba(57, 192, 224, 0.10);
        border-color: rgba(57, 192, 224, 0.25);
    }
    [data-testid="stSidebar"] [data-testid="stExpander"] details[open] summary span,
    [data-testid="stSidebar"] [data-testid="stExpander"] details[open] summary p {
        color: #39C0E0 !important;
    }
    /* Expander toggle icon (arrow) */
    [data-testid="stSidebar"] [data-testid="stExpander"] summary svg {
        color: #39C0E0 !important;
        fill: #39C0E0 !important;
    }
    /* Expander content area */
    [data-testid="stSidebar"] [data-testid="stExpander"] [data-testid="stExpanderDetails"] {
        border: none;
        padding-top: 0.4rem;
    }

    /* Sidebar dividers */
    [data-testid="stSidebar"] hr {
        border-color: rgba(255,255,255,0.08) !important;
        margin: 0.5rem 0;
    }
    /* Sidebar input fields — dark background, bright text */
    [data-testid="stSidebar"] input,
    [data-testid="stSidebar"] .stTextInput input,
    [data-testid="stSidebar"] .stTextInput div[data-baseweb="input"],
    [data-testid="stSidebar"] .stTextInput div[data-baseweb="base-input"],
    [data-testid="stSidebar"] .stNumberInput input,
    [data-testid="stSidebar"] .stNumberInput div[data-baseweb="input"],
    [data-testid="stSidebar"] .stSelectbox div[data-baseweb="select"],
    [data-testid="stSidebar"] .stSelectbox [data-baseweb="select"] > div,
    [data-testid="stSidebar"] [data-baseweb="select"] {
        background-color: #0f1f28 !important;
        border-color: rgba(57, 192, 224, 0.2) !important;
        color: #e8eff4 !important;
    }
    [data-testid="stSidebar"] .stTextInput input::placeholder {
        color: rgba(255,255,255,0.4) !important;
    }
    /* Selectbox value text */
    [data-testid="stSidebar"] [data-baseweb="select"] span,
    [data-testid="stSidebar"] [data-baseweb="select"] div[data-testid="stMarkdownContainer"],
    [data-testid="stSidebar"] .stSelectbox div[role="combobox"],
    [data-testid="stSidebar"] .stSelectbox [data-baseweb="select"] .css-1dimb5e-singleValue {
        color: #e8eff4 !important;
    }
    /* Sidebar file uploader — compact style */
    [data-testid="stSidebar"] [data-testid="stFileUploader"] section {
        background: #0f1f28 !important;
        border: 1px solid rgba(57, 192, 224, 0.2) !important;
        border-radius: 6px;
        padding: 0.4rem !important;
    }
    [data-testid="stSidebar"] [data-testid="stFileUploader"] button {
        color: #39C0E0 !important;
        border-color: rgba(57, 192, 224, 0.3) !important;
        background: rgba(57, 192, 224, 0.08) !important;
    }
    /* Sidebar captions */
    [data-testid="stSidebar"] .stCaption, [data-testid="stSidebar"] small {
        color: #7a95a6 !important;
    }
    /* Stop button in sidebar */
    [data-testid="stSidebar"] .stButton > button:not([kind="primary"]) {
        background-color: #0f1f28;
        border-color: rgba(57, 192, 224, 0.2);
        color: #dce6ec !important;
    }
    [data-testid="stSidebar"] .stButton > button:not([kind="primary"]):hover {
        background-color: rgba(255,80,80,0.15);
        border-color: #ff5050;
        color: #ff7070 !important;
    }

    /* ─── Primary buttons ─── */
    .stButton > button[kind="primary"] {
        background-color: #39C0E0;
        border-color: #39C0E0;
        color: white !important;
    }
    .stButton > button[kind="primary"]:hover {
        background-color: #2da8c5;
        border-color: #2da8c5;
    }
    /* Tabs */
    .stTabs [data-baseweb="tab"][aria-selected="true"] {
        border-bottom-color: #39C0E0;
        color: #39C0E0;
    }
    /* Links & accents */
    a {
        color: #39C0E0 !important;
    }
    /* Secondary accent on specific elements */
    .stDownloadButton > button {
        background-color: #FFA933;
        border-color: #FFA933;
        color: white !important;
    }
    .stDownloadButton > button:hover {
        background-color: #e6952b;
        border-color: #e6952b;
    }
    /* Header bar accent line */
    header[data-testid="stHeader"] {
        border-bottom: 3px solid #39C0E0;
    }
    /* Card grid for array items */
    .item-card {
        position: relative;
        padding: 0.75rem 1rem;
        border-radius: 8px;
        border: 1px solid #e0e0e0;
        background: #fafbfc;
        transition: box-shadow 0.2s;
        margin-bottom: 0.5rem;
    }
    .item-card .card-id {
        position: absolute;
        top: 0.5rem;
        right: 0.6rem;
        font-size: 0.72rem;
        font-weight: 600;
        color: #888;
    }
    .item-card:hover {
        box-shadow: 0 2px 8px rgba(57, 192, 224, 0.15);
        border-color: #39C0E0;
    }
    .item-card .card-title {
        font-weight: 600;
        font-size: 0.95rem;
        margin-bottom: 0.3rem;
        color: #1e3a4d;
    }
    .item-card .card-detail {
        font-size: 0.82rem;
        color: #555;
        margin-bottom: 0.2rem;
    }
    .item-card .badge-active {
        display: inline-block;
        padding: 1px 8px;
        border-radius: 10px;
        font-size: 0.72rem;
        font-weight: 500;
        background: #d4edda;
        color: #155724;
    }
    .item-card .badge-inactive {
        display: inline-block;
        padding: 1px 8px;
        border-radius: 10px;
        font-size: 0.72rem;
        font-weight: 500;
        background: #f8d7da;
        color: #721c24;
    }
</style>
""", unsafe_allow_html=True)

# ─────────────────────────────────────────────────────────────────────────────
# SESSION STATE INITIALIZATION
# ─────────────────────────────────────────────────────────────────────────────

def init_state():
    """Initialize session state with default values."""
    defaults = {
        "json_data": {},
        "simulation_results": None,
        "loaded_file": None,
        "work_dir": os.path.join(tempfile.gettempdir(), "marlim3_gui_workdir"),
        "sim_stdout": "",
        "sim_running": False,
        "data_version": 0,
    }
    for k, v in defaults.items():
        if k not in st.session_state:
            st.session_state[k] = v
    # Ensure work directory exists
    os.makedirs(st.session_state["work_dir"], exist_ok=True)


init_state()


# ─────────────────────────────────────────────────────────────────────────────
# JSON SANITIZER (must be defined before sidebar simulation runner)
# ─────────────────────────────────────────────────────────────────────────────

def _sanitize_json(obj):
    """Recursively remove nulls and empty arrays/dicts from JSON data for the C++ parser.

    Rules (from RapidJSON parser analysis):
    - null values crash GetBool/GetInt/GetDouble → remove key entirely
    - empty arrays [] are treated as absent → remove key entirely
    - empty strings for file paths → remove key entirely
    - NaN/Inf floats → remove (invalid JSON)
    """
    import math
    if isinstance(obj, dict):
        cleaned = {}
        for k, v in obj.items():
            v_clean = _sanitize_json(v)
            # Skip None, empty arrays, empty strings
            if v_clean is None:
                continue
            if isinstance(v_clean, list) and len(v_clean) == 0:
                continue
            if isinstance(v_clean, str) and v_clean == "":
                continue
            cleaned[k] = v_clean
        return cleaned
    elif isinstance(obj, list):
        # For arrays: sanitize each element but keep the array even with cleaned items
        cleaned = []
        for item in obj:
            item_clean = _sanitize_json(item)
            if item_clean is not None:
                cleaned.append(item_clean)
        return cleaned
    elif isinstance(obj, float):
        if math.isnan(obj) or math.isinf(obj):
            return None
        return obj
    elif isinstance(obj, bool):
        # bool must be checked before int (bool is subclass of int in Python)
        return obj
    elif isinstance(obj, int):
        return obj
    else:
        return obj


# ─────────────────────────────────────────────────────────────────────────────
# SIDEBAR - FILE I/O
# ─────────────────────────────────────────────────────────────────────────────

with st.sidebar:
    # Logo
    _logo_path = PROJECT_ROOT / "img" / "logo_marlim3.svg"
    if _logo_path.exists():
        st.image(str(_logo_path), use_container_width=True)
    else:
        st.title("Marlim3")
    st.markdown(
        '<p style="text-align:right; font-size:1.1rem; font-weight:600; '
        'color:#39C0E0; margin-top:-0.5rem; letter-spacing:2px;">LITE</p>',
        unsafe_allow_html=True,
    )

    # ─── Run Simulation (top of sidebar) ─────────────────────────────────
    with st.expander("SIMULATION", expanded=True):
        # Active model selector (at the top of simulation)
        work_dir = st.session_state["work_dir"]
        _all_wdir_files = sorted([f for f in os.listdir(work_dir)
                                   if os.path.isfile(os.path.join(work_dir, f))])
        _model_files = [f for f in _all_wdir_files if f.endswith((".json", ".mr3"))]

        if _model_files:
            _current = st.session_state.get("loaded_file", "")
            _default_idx = 0
            if _current in _model_files:
                _default_idx = _model_files.index(_current)
            _selected_model = st.selectbox(
                "Active model", _model_files, index=_default_idx,
                key="active_model_select"
            )
            if _selected_model and _selected_model != st.session_state.get("loaded_file"):
                _model_path = os.path.join(work_dir, _selected_model)
                try:
                    with open(_model_path, "r", encoding="utf-8") as _mf:
                        st.session_state["json_data"] = json.load(_mf)
                    st.session_state["loaded_file"] = _selected_model
                    st.session_state["data_version"] += 1
                    st.rerun()
                except (json.JSONDecodeError, UnicodeDecodeError) as _e:
                    st.error(f"Cannot load {_selected_model}: {_e}")
        else:
            _selected_model = None
            st.caption("No model files yet. Upload or load a demo below.")

        # Derive sim_label from the active model filename (without extension)
        file_name = st.session_state.get("loaded_file")
        if not file_name:  # This catches None, empty string, etc.
            file_name = "sim_gui"
        sim_label = os.path.splitext(str(file_name))[0]  # Convert to string just in case
        sim_dir = st.text_input("Output directory", value="results_gui")

        # Auto-detect executable from known locations
        _default_exec = ""
        _exec_candidates = [
            os.environ.get("MARLIM3_EXEC", ""),
            os.path.join(str(PROJECT_ROOT), "build", "Marlim3"),
            os.path.join(str(PROJECT_ROOT), "marlim3", "Marlim3"),
            os.path.join(str(PROJECT_ROOT), "build", "Marlim3.exe"),
            os.path.join(str(PROJECT_ROOT), "marlim3", "Marlim3.exe"),
        ]
        for _cand in _exec_candidates:
            if _cand and os.path.isfile(_cand):
                _default_exec = _cand
                break

        exec_override = st.text_input(
            "Executable path",
            value=_default_exec,
            help="Path to Marlim3.exe. Auto-detected from known locations."
        )

        _btn_col1, _btn_col2 = st.columns(2)
        with _btn_col1:
            _run_clicked = st.button("▶ Run", type="primary", use_container_width=True)
        with _btn_col2:
            _stop_clicked = st.button("⏹ Stop", use_container_width=True)

        # Stop button: kill running simulation
        if _stop_clicked and st.session_state.get("_sim_pid"):
            try:
                os.kill(st.session_state["_sim_pid"], signal.SIGTERM)
            except (OSError, ProcessLookupError):
                pass
            st.session_state.pop("_sim_pid", None)
            st.session_state["sim_running"] = False
            st.warning("Simulation stopped.")

        _output_container = st.empty()
        _status_container = st.empty()

    # Working Directory
    with st.expander("WORKING DIRECTORY", expanded=True):
        work_dir = st.session_state["work_dir"]

        # Minimal file uploader (CSS hides the drag area, shows only browse button)
        _uploaded_files = st.file_uploader(
            "Add files", type=None,
            accept_multiple_files=True, key="workdir_upload",
            label_visibility="collapsed"
        )
        if _uploaded_files:
            # Track which files we already processed to avoid rerun loop
            _upload_names = sorted([f.name for f in _uploaded_files])
            if _upload_names != st.session_state.get("_last_uploaded_names"):
                for _uf in _uploaded_files:
                    dest = os.path.join(work_dir, _uf.name)
                    with open(dest, "wb") as _wf:
                        _wf.write(_uf.getbuffer())
                st.session_state["_last_uploaded_names"] = _upload_names
                st.rerun()

        # Load demo files into work dir
        demos_dir = PROJECT_ROOT / "demos"
        if demos_dir.exists():
            demo_files = sorted([f.name for f in demos_dir.glob("*.json")])
            if demo_files:
                selected_demo = st.selectbox("Load a demo", ["(none)"] + demo_files)
                if selected_demo != "(none)" and st.button("Load Demo"):
                    import shutil as _shutil_demo
                    _src = demos_dir / selected_demo
                    _dst = os.path.join(work_dir, selected_demo)
                    _shutil_demo.copy2(str(_src), _dst)
                    st.session_state["loaded_file"] = selected_demo
                    with open(_dst, "r", encoding="utf-8") as _df:
                        st.session_state["json_data"] = json.load(_df)
                    st.session_state["data_version"] += 1
                    st.rerun()

        st.markdown("---")

        # Show all files in directory
        _all_files = sorted([f for f in os.listdir(work_dir)
                             if os.path.isfile(os.path.join(work_dir, f))])
        if _all_files:
            st.caption(f"{len(_all_files)} file(s):")
            for _f in _all_files:
                st.text(f"  {_f}")

        # Open in system file explorer (cross-platform)
        if st.button("📂 Open folder", use_container_width=True):
            import subprocess as _sp
            import platform as _platform
            _sys = _platform.system()
            if _sys == "Windows":
                os.startfile(work_dir)
            elif _sys == "Darwin":
                _sp.Popen(["open", work_dir])
            else:
                _sp.Popen(["xdg-open", work_dir])

    # ─── Simulation run logic (triggered by expander buttons above) ───────
    if _run_clicked:
        st.session_state["sim_stdout"] = ""
        st.session_state["sim_running"] = True
        try:
            exec_path = exec_override.strip()
            if not exec_path or not os.path.isfile(exec_path):
                _status_container.error("**Executable not found.**")
                st.session_state["sim_running"] = False
            else:
                import subprocess
                import shutil
                import time as _time
                import gc

                # Write sanitized JSON into work directory
                work_dir = st.session_state["work_dir"]
                input_json_path = os.path.join(work_dir, f"{sim_label}.json")
                _sim_data = _sanitize_json(st.session_state["json_data"])
                with open(input_json_path, "w", encoding="utf-8") as f:
                    _sim_data["language"] = "en"
                    json.dump(_sim_data, f, ensure_ascii=False)

                # Pre-cleanup output directory
                _out_dir = os.path.join(work_dir, sim_dir)
                if os.path.isdir(_out_dir):
                    gc.collect()
                    for _attempt in range(3):
                        try:
                            shutil.rmtree(_out_dir)
                            break
                        except PermissionError:
                            _time.sleep(0.5)
                os.makedirs(_out_dir, exist_ok=True)

                # Build command
                _sistema = _sim_data.get("system", "PROD")
                _cmd_list = [exec_path, "-d", sim_dir, "-i", f"{sim_label}.json"]
                if _sistema == "INJ":
                    _cmd_list += ["-s", "INJETOR"]

                # Run subprocess with live stdout streaming
                _status_container.info("⏳ Running simulation...")
                _lines = []
                process = subprocess.Popen(
                    _cmd_list,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT,
                    text=True,
                    cwd=work_dir,
                )
                st.session_state["_sim_pid"] = process.pid
                for line in iter(process.stdout.readline, ""):
                    _lines.append(line)
                    _tail = "".join(_lines[-200:])
                    _output_container.markdown(
                        f'<div style="max-height:300px; overflow-y:auto; background:#1e1e1e; '
                        f'color:#d4d4d4; padding:0.5rem; border-radius:4px; font-family:monospace; '
                        f'font-size:0.8rem; white-space:pre; display:flex; flex-direction:column-reverse;">'
                        f'<div>{_tail}</div></div>',
                        unsafe_allow_html=True,
                    )
                process.wait()
                st.session_state.pop("_sim_pid", None)

                # Move simulation output files from work_dir into output directory
                _stray_exts = (".log", ".dat", ".out", ".csv")
                for _sf in os.listdir(work_dir):
                    _sf_path = os.path.join(work_dir, _sf)
                    if (os.path.isfile(_sf_path)
                            and _sf.lower().endswith(_stray_exts)
                            and _sf_path != input_json_path):
                        shutil.move(_sf_path, os.path.join(_out_dir, _sf))

                _full_output = "".join(_lines)
                st.session_state["sim_stdout"] = _full_output
                _output_container.markdown(
                    f'<div style="max-height:300px; overflow-y:auto; background:#1e1e1e; '
                    f'color:#d4d4d4; padding:0.5rem; border-radius:4px; font-family:monospace; '
                    f'font-size:0.8rem; white-space:pre; display:flex; flex-direction:column-reverse;">'
                    f'<div>{_full_output}</div></div>',
                    unsafe_allow_html=True,
                )

                if process.returncode == 0:
                    _status_container.success("Simulation completed!")
                    try:
                        import marlim3
                        tramo = marlim3.Tramo()
                        tramo.from_json(input_json_path)
                        _results_dir = os.path.join(work_dir, sim_dir)
                        tramo.resultados = {}
                        try:
                            if tramo.perfilProducao is not None:
                                _perfis = tramo._process_profiles(_results_dir)
                                if _perfis is not None:
                                    tramo.resultados['productionProfile'] = _perfis
                            if tramo.tendP is not None:
                                _tend = tramo._process_trends(_results_dir)
                                if _tend:
                                    tramo.resultados['productionTrend'] = _tend
                        except Exception as _re:
                            _lines.append(f"\n[GUI] Results loading error: {_re}\n")
                        if not tramo.resultados:
                            _dat_files = []
                            if os.path.isdir(_results_dir):
                                for _r, _d, _f in os.walk(_results_dir):
                                    for _fn in _f:
                                        if _fn.endswith(".dat"):
                                            _dat_files.append(os.path.join(_r, _fn))
                            _lines.append(f"\n[GUI] Results dir: {_results_dir}\n")
                            _lines.append(f"[GUI] .dat files found: {_dat_files[:10]}\n")
                            _lines.append(f"[GUI] perfilProducao={tramo.perfilProducao is not None}, tendP={tramo.tendP is not None}\n")
                            _output_container.code("".join(_lines[-200:]), language="log")
                        st.session_state["simulation_results"] = tramo.resultados if tramo.resultados else None
                    except Exception as _e:
                        _lines.append(f"\n[GUI] Failed to load results: {_e}\n")
                        _output_container.code("".join(_lines[-200:]), language="log")
                        st.session_state["simulation_results"] = None
                else:
                    _status_container.error(
                        f"Simulation failed (exit code {process.returncode})")
        except FileNotFoundError as e:
            st.session_state["sim_stdout"] += f"\nERROR: {e}"
            _status_container.error(f"**Executable not found.** {e}")
        except Exception as e:
            st.session_state["sim_stdout"] += f"\nERROR: {e}"
            _status_container.error(f"Simulation failed: {e}")
        finally:
            st.session_state["sim_running"] = False


# ─────────────────────────────────────────────────────────────────────────────
# HELPER FUNCTIONS
# ─────────────────────────────────────────────────────────────────────────────

def get_val(path: list, default=None):
    """Get a nested value from json_data."""
    obj = st.session_state["json_data"]
    for key in path:
        if isinstance(obj, dict) and key in obj:
            obj = obj[key]
        elif isinstance(obj, list) and isinstance(key, int) and 0 <= key < len(obj):
            obj = obj[key]
        else:
            return default
    return obj


def _path_exists(path: list) -> bool:
    """Check if a path already exists in json_data (without creating it)."""
    obj = st.session_state["json_data"]
    for key in path:
        if isinstance(obj, dict) and key in obj:
            obj = obj[key]
        elif isinstance(obj, list) and isinstance(key, int) and 0 <= key < len(obj):
            obj = obj[key]
        else:
            return False
    return True


def set_val(path: list, value):
    """Set a nested value in json_data."""
    obj = st.session_state["json_data"]
    for i, key in enumerate(path[:-1]):
        if isinstance(obj, dict):
            next_key = path[i + 1]
            if key not in obj or obj[key] is None:
                obj[key] = {} if not isinstance(next_key, int) else []
            obj = obj[key]
        elif isinstance(obj, list) and isinstance(key, int):
            while len(obj) <= key:
                obj.append({})
            obj = obj[key]
    obj[path[-1]] = value


def _vkey(path):
    """Generate a versioned widget key to force re-init on data import."""
    v = st.session_state.get("data_version", 0)
    return f"v{v}_" + ".".join(str(p) for p in path)


def widget_bool(label, path, default=False, help_text=None):
    """Boolean toggle widget."""
    val = get_val(path, default)
    key = "bool_" + _vkey(path)
    new_val = st.checkbox(label, value=bool(val), help=help_text, key=key)
    if _path_exists(path) or new_val != default:
        set_val(path, new_val)
    return new_val


def widget_number(label, path, default=0.0, unit=None, help_text=None, fmt=None, min_val=None, max_val=None):
    """Numeric input widget."""
    val = get_val(path, default)
    display_label = f"{label} [{unit}]" if unit else label
    key = "num_" + _vkey(path)
    new_val = st.number_input(
        display_label, value=float(val) if val is not None else float(default),
        format=fmt or "%.6g", step=None, min_value=min_val, max_value=max_val, help=help_text, key=key
    )
    if _path_exists(path) or new_val != float(default):
        set_val(path, new_val)
    return new_val


def widget_int(label, path, default=0, help_text=None, min_val=None, max_val=None):
    """Integer input widget."""
    val = get_val(path, default)
    key = "int_" + _vkey(path)
    new_val = st.number_input(
        label, value=int(val) if val is not None else int(default),
        step=1, min_value=min_val, max_value=max_val, help=help_text, key=key
    )
    if _path_exists(path) or int(new_val) != int(default):
        set_val(path, int(new_val))
    return int(new_val)


def widget_select(label, path, options, default=None, help_text=None):
    """Selectbox widget."""
    val = get_val(path, default)
    idx = options.index(val) if val in options else 0
    key = "sel_" + _vkey(path)
    new_val = st.selectbox(label, options, index=idx, help=help_text, key=key)
    if _path_exists(path) or new_val != default:
        set_val(path, new_val)
    return new_val


def widget_int_select(label, path, options_map: dict, default=0, help_text=None):
    """Integer selectbox with labels. options_map: {int_val: 'label'}"""
    val = get_val(path, default)
    labels = list(options_map.values())
    keys = list(options_map.keys())
    idx = keys.index(val) if val in keys else 0
    key = "isel_" + _vkey(path)
    selected_label = st.selectbox(label, labels, index=idx, help=help_text, key=key)
    new_val = keys[labels.index(selected_label)]
    if _path_exists(path) or new_val != default:
        set_val(path, new_val)
    return new_val


def widget_text(label, path, default="", help_text=None):
    """Text input widget."""
    val = get_val(path, default)
    key = "txt_" + _vkey(path)
    new_val = st.text_input(label, value=str(val) if val else "", help=help_text, key=key)
    if _path_exists(path) or new_val != default:
        set_val(path, new_val)
    return new_val


def widget_array_table(label, path, columns: dict, help_text=None):
    """
    Editable array of objects as a table.
    columns: {col_name: {"type": "number"|"integer"|"boolean"|"string", "default": ...}}
    """
    val = get_val(path, [])
    if not isinstance(val, list):
        val = []

    df = pd.DataFrame(val) if val else pd.DataFrame(columns=list(columns.keys()))
    # Ensure all columns exist
    for col, spec in columns.items():
        if col not in df.columns:
            df[col] = spec.get("default", None)

    col_config = {}
    for col, spec in columns.items():
        if spec["type"] == "boolean":
            col_config[col] = st.column_config.CheckboxColumn(col)
        elif spec["type"] == "number":
            col_config[col] = st.column_config.NumberColumn(col, format="%.4g")
        elif spec["type"] == "integer":
            col_config[col] = st.column_config.NumberColumn(col, format="%d")

    st.caption(label)
    edited_df = st.data_editor(
        df, num_rows="dynamic", column_config=col_config,
        use_container_width=True, help=help_text, key=f"table_{_vkey(path)}"
    )
    result = edited_df.to_dict(orient="records")
    if _path_exists(path) or len(result) > 0:
        set_val(path, result)
    return result


def widget_time_series(label, path, value_cols: list, time_col="time", help_text=None):
    """
    Editable time-series data (time + value columns).
    value_cols: list of {"name": str, "unit": str}
    """
    val = get_val(path, {})
    if not isinstance(val, dict):
        val = {}

    times = val.get(time_col, [])
    data = {"time [s]": times}
    for vc in value_cols:
        col_label = f"{vc['name']} [{vc.get('unit', '')}]" if vc.get('unit') else vc['name']
        data[col_label] = val.get(vc["name"], [None] * len(times))

    # Pad to same length
    max_len = max(len(v) for v in data.values()) if data else 0
    for k in data:
        while len(data[k]) < max_len:
            data[k].append(None)

    df = pd.DataFrame(data)
    st.caption(label)
    edited_df = st.data_editor(
        df, num_rows="dynamic", use_container_width=True,
        key=f"ts_{_vkey(path)}_{label}"
    )

    # Write back
    result = {}
    cols_list = list(edited_df.columns)
    result[time_col] = edited_df[cols_list[0]].dropna().tolist()
    for i, vc in enumerate(value_cols):
        result[vc["name"]] = edited_df[cols_list[i + 1]].dropna().tolist()

    # Only write if path already exists or user added data
    has_data = any(len(v) > 0 for v in result.values())
    if _path_exists(path) or has_data:
        set_val(path, {**val, **result})
    return result


# ─────────────────────────────────────────────────────────────────────────────
# CARD RENDERING HELPER
# ─────────────────────────────────────────────────────────────────────────────

def render_card(title: str, details: list[str], active: bool = True, icon: str = "", id_number=None):
    """Render an HTML card with title, detail lines, and status badge."""
    badge = ('<span class="badge-active">Active</span>' if active
             else '<span class="badge-inactive">Inactive</span>')
    id_html = f'<span class="card-id">{id_number}</span>' if id_number is not None else ''
    detail_html = "".join(f'<div class="card-detail">{d}</div>' for d in details)
    st.markdown(
        f'<div class="item-card">'
        f'{id_html}'
        f'<div class="card-title">{icon} {title} {badge}</div>'
        f'{detail_html}'
        f'</div>',
        unsafe_allow_html=True
    )


# ─────────────────────────────────────────────────────────────────────────────
# DIALOG FUNCTIONS FOR ARRAY EDITING
# ─────────────────────────────────────────────────────────────────────────────

@st.dialog("Edit Production Fluid", width="large")
def _fluid_dialog(idx):
    """Full edit form for a single production fluid."""
    fluids = get_val(["productionFluid"], [])
    if idx >= len(fluids):
        st.error("Fluid not found.")
        return
    fluid = fluids[idx]

    st.subheader(f"Fluid {idx} (ID: {fluid.get('id', idx)})")

    widget_text("Label", ["productionFluid", idx, "label"],
                help_text="Display name for this fluid")

    # Basic properties
    st.markdown("**Basic Properties**")
    c1, c2, c3, c4 = st.columns(4)
    with c1:
        widget_bool("Active", ["productionFluid", idx, "active"], default=True)
        widget_int("ID", ["productionFluid", idx, "id"], default=idx)
    with c2:
        widget_number("API", ["productionFluid", idx, "api"], default=28.0,
                      help_text="Oil API gravity. Black oil only.")
        widget_number("GOR [Sm³/Sm³]", ["productionFluid", idx, "gor"], default=100.0,
                      help_text="Gas-oil ratio. Valid for all 3 fluid types.")
    with c3:
        widget_number("Gas density (rel.)", ["productionFluid", idx, "gasDensity"],
                      default=0.7, help_text="Relative to air at standard conditions. Used by black oil and compositional.")
        widget_number("BSW [m³/m³]", ["productionFluid", idx, "bsw"], default=0.0,
                      help_text="Basic Sediment and Water. Valid for all 3 fluid types.")
    with c4:
        widget_number("Water density (rel.)", ["productionFluid", idx, "waterDensity"],
                      default=1.0, help_text="Relative to pure water (1000 kg/m³).")
        widget_number("CO2 fraction", ["productionFluid", idx, "CO2Fraction"], default=0.0,
                      help_text="Molar fraction of CO2.")

    # Viscosity models
    st.markdown("**Viscosity & PVT Models**")
    c1, c2 = st.columns(2)
    with c1:
        widget_int_select("Dead oil viscosity model",
                          ["productionFluid", idx, "deadOilModel"], {
                              0: "0 - ASTM", 1: "1 - Beggs & Robinson",
                              2: "2 - B&R Modified", 3: "3 - Glaso",
                              4: "4 - Kartoatmodjo-Schmidt",
                              5: "5 - Petrosky-Farshad", 6: "6 - Beal",
                              7: "7 - T×viscosity table",
                          }, default=3, help_text="Valid for black oil, compositional, and flash table (when BO viscosity is enabled).")
        widget_int_select("Live oil viscosity",
                          ["productionFluid", idx, "liveOilModel"], {
                              0: "0 - Beggs & Robinson",
                              1: "1 - Kartoatmodjo-Schmidt",
                              2: "2 - Petrosky-Farshad",
                          }, default=0, help_text="Saturated oil viscosity correlation.")
        widget_int_select("Sub-saturated oil viscosity",
                          ["productionFluid", idx, "undersaturatedOilModel"], {
                              0: "0 - Vasquez & Beggs",
                              1: "1 - Kartoatmodjo-Schmidt",
                              2: "2 - Petrosky-Farshad",
                              3: "3 - Beal",
                              4: "4 - Khan",
                          }, default=0, help_text="Under-saturated oil viscosity correlation.")
    with c2:
        widget_int_select("RS/Pb model", ["productionFluid", idx, "RsPbModel"], {
            0: "0 - Vázquez/Beggs", 1: "1 - Lasater", 2: "2 - Standing",
            3: "3 - Glaso", 4: "4 - Lívia Fulchignoni",
        }, default=0, help_text="Solution gas-oil ratio / bubble point correlation. Black oil only.")
        widget_int_select("Critical correlation", ["productionFluid", idx, "criticalCorrelation"], {
            0: "0 - Correlation 0", 1: "1 - Brown et al",
            2: "2 - Piper et al",
        }, default=1, help_text="For gas critical P and T. 1 & 2 better for CO2.")
        widget_int_select("Visc. from BO (flash table)",
                          ["productionFluid", idx, "blackOilViscModel"], {
                              0: "0 - Use table viscosities",
                              1: "1 - Use BO correlations",
                          }, default=0, help_text="For flash table model only.")
        widget_int_select("Water model (flash table)",
                          ["productionFluid", idx, "blackOilWaterModel"], {
                              0: "0 - Use table JT coeff",
                              1: "1 - Use BO water correlations",
                          }, default=1, help_text="For flash table model only.")

    # ASTM viscosity points
    st.markdown("**ASTM Viscosity Points** (dead oil model = 0)")
    ca, cb = st.columns(2)
    with ca:
        widget_number("Temp 1 [°C]", ["productionFluid", idx, "temp1"],
                      help_text="Temperature of ASTM point 1.")
        widget_number("Visc 1 [cP]", ["productionFluid", idx, "visc1"],
                      help_text="Viscosity at ASTM point 1.")
    with cb:
        widget_number("Temp 2 [°C]", ["productionFluid", idx, "temp2"],
                      help_text="Temperature of ASTM point 2.")
        widget_number("Visc 2 [cP]", ["productionFluid", idx, "visc2"],
                      help_text="Viscosity at ASTM point 2.")

    # Dead oil table (model 7)
    st.markdown("**Dead Oil Table** (model = 7)")
    tbl_temps = fluid.get("deadOilTemp", [])
    tbl_viscs = fluid.get("deadOilVisc", [])
    max_len = max(len(tbl_temps), len(tbl_viscs)) if tbl_temps or tbl_viscs else 0
    while len(tbl_temps) < max_len:
        tbl_temps.append(None)
    while len(tbl_viscs) < max_len:
        tbl_viscs.append(None)
    tbl_df = pd.DataFrame({"tempOleoMorto [°C]": tbl_temps, "viscOleoMorto [cP]": tbl_viscs})
    edited_tbl = st.data_editor(tbl_df, num_rows="dynamic",
                                use_container_width=True,
                                key=f"dlg_dead_oil_tbl_{idx}")
    set_val(["productionFluid", idx, "deadOilTemp"],
            edited_tbl["tempOleoMorto [°C]"].dropna().tolist())
    set_val(["productionFluid", idx, "deadOilVisc"],
            edited_tbl["viscOleoMorto [cP]"].dropna().tolist())

    # Emulsion model
    st.markdown("**Emulsion Model**")
    widget_int_select("Emulsion model",
                      ["productionFluid", idx, "emulsionType"], {
                          0: "0 - Weighted mix", 1: "1 - Woeflin (weak)",
                          2: "2 - Woeflin (medium)", 3: "3 - Woeflin (strong)",
                          4: "4 - Exponential", 5: "5 - Pal-Rhodes",
                          6: "6 - BSW×multiplier", 7: "7 - Below BSW saturation",
                      }, default=0, help_text="Emulsion correlation for oil viscosity modification. Valid for all 3 fluid types.")
    widget_number("BSW inversion (bswCorte)", ["productionFluid", idx, "bswInversion"],
                  default=1.0, help_text="BSW at emulsion inversion point.")
    ce1, ce2 = st.columns(2)
    with ce1:
        st.caption("Model 4 (Exponential)")
        widget_number("Coef A", ["productionFluid", idx, "emulsionCoefA"],
                      help_text="Parameter 'a' of the exponential emulsion model.")
        widget_number("Coef B", ["productionFluid", idx, "emulsionCoefB"],
                      help_text="Parameter 'b' of the exponential emulsion model.")
    with ce2:
        st.caption("Model 5 (Pal-Rhodes)")
        widget_number("phi100", ["productionFluid", idx, "phi100"],
                      help_text="Emulsion multiplier for Pal-Rhodes model.")

    st.caption("Model 6 (BSW × multiplier) — editable table")
    bsw_vec = fluid.get("bswVec", [])
    emul_vec = fluid.get("emulVec", [])
    max_len = max(len(bsw_vec), len(emul_vec)) if bsw_vec or emul_vec else 0
    while len(bsw_vec) < max_len:
        bsw_vec.append(None)
    while len(emul_vec) < max_len:
        emul_vec.append(None)
    emul_df = pd.DataFrame({"bswVec": bsw_vec, "emulVec": emul_vec})
    edited_emul = st.data_editor(emul_df, num_rows="dynamic",
                                 use_container_width=True,
                                 key=f"dlg_emul_tbl_{idx}")
    set_val(["productionFluid", idx, "bswVec"],
            edited_emul["bswVec"].dropna().tolist())
    set_val(["productionFluid", idx, "emulVec"],
            edited_emul["emulVec"].dropna().tolist())

    # Compositional model options
    st.markdown("**Compositional Options**")
    widget_bool("User molar fractions",
                ["productionFluid", idx, "userMolarFraction"], default=False,
                help_text="If false, reads from .ctm file.")
    widget_bool("Correct GOR",
                ["productionFluid", idx, "userGORComp"], default=False,
                help_text="If true, corrects molar fractions to match GOR value.")
    st.caption("Molar fractions (same order as .ctm file)")
    frac_molar = fluid.get("molarFraction", [])
    frac_str = st.text_input(
        "Molar fractions (comma-sep)",
        value=", ".join(str(x) for x in frac_molar) if frac_molar else "",
        key=f"dlg_frac_molar_{idx}"
    )
    try:
        set_val(["productionFluid", idx, "molarFraction"],
                [float(x.strip()) for x in frac_str.split(",") if x.strip()])
    except ValueError:
        pass

    # Close button
    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_fluid_done_{idx}"):
        st.session_state.pop("_editing_fluid", None)
        st.rerun()


@st.dialog("Edit Material", width="large")
def _material_dialog(idx):
    """Full edit form for a material."""
    materials = get_val(["material"], [])
    if idx >= len(materials):
        st.error("Material not found.")
        return
    mat = materials[idx]

    st.subheader(f"Material {idx} (ID: {mat.get('id', idx)})")

    widget_text("Label", ["material", idx, "label"],
                help_text="Display name for this material")

    c1, c2 = st.columns(2)
    with c1:
        widget_bool("Active", ["material", idx, "active"], default=True)
        widget_int("ID", ["material", idx, "id"], default=idx)
        widget_int_select("Type", ["material", idx, "type"], {
            0: "0 - Solid", 1: "1 - Fluid (user)", 2: "2 - Water", 3: "3 - Air"
        }, default=0)
    with c2:
        widget_number("Conductivity k [W/m·K]", ["material", idx, "conductivity"], default=50.0)
        widget_number("Specific heat Cp [J/kg·K]", ["material", idx, "specificHeat"], default=500.0)
        widget_number("Density ρ [kg/m³]", ["material", idx, "rho"], default=7800.0)

    # Fields only needed for tipo=1 (fluid defined by user)
    tipo_val = get_val(["material", idx, "type"], 0)
    if tipo_val == 1:
        st.markdown("**Fluid properties** (tipo = 1)")
        cf1, cf2 = st.columns(2)
        with cf1:
            widget_number("Viscosity [cP]", ["material", idx, "visc"],
                          help_text="Required for tipo=1 (user-defined fluid)")
        with cf2:
            widget_number("Thermal expansivity β [1/K]", ["material", idx, "beta"],
                          help_text="Required for tipo=1 (user-defined fluid)", fmt="%.2e")

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_mat_done_{idx}"):
        st.session_state.pop("_editing_material", None)
        st.rerun()


def _draw_cross_section_figure(sec):
    """Create a Plotly figure showing the pipe cross-section with concentric layers."""
    import numpy as np

    is_anular = bool(sec.get("annular", False))
    d_interno = float(sec.get("innerDiameter", 0.1))
    d_externo = float(sec.get("outerDiameter", d_interno)) if is_anular else d_interno
    layers = sec.get("layers", [])

    # Compute the largest outer diameter for scaling
    current_diameter = d_externo if is_anular else d_interno
    layer_diameters = []  # list of (inner_d, outer_d) per layer
    for camada in layers:
        inner_d = current_diameter
        if camada.get("layerMeasurementType") == "DIAMETER":
            outer_d = float(camada.get("diameter", current_diameter))
        else:
            outer_d = current_diameter + 2 * float(camada.get("thickness", 0.01))
        layer_diameters.append((inner_d, outer_d))
        current_diameter = outer_d

    largest_diameter = current_diameter if layers else (d_externo if is_anular else d_interno * 1.5)
    if largest_diameter <= 0:
        largest_diameter = 0.2

    # Color palette for layers
    layer_colors = ['#39C0E0', '#FFA933', '#66BB6A', '#AB47BC', '#EF5350', '#26A69A']

    # Build figure
    fig = go.Figure()

    # Generate circle points
    theta = np.linspace(0, 2 * np.pi, 100)

    # Draw layers from outermost to innermost so the fill stacks correctly
    for i in range(len(layer_diameters) - 1, -1, -1):
        inner_d, outer_d = layer_diameters[i]
        r_outer = outer_d / 2
        x_outer = (r_outer * np.cos(theta)).tolist()
        y_outer = (r_outer * np.sin(theta)).tolist()
        color = layer_colors[i % len(layer_colors)]

        materials = get_val(["material"], [])
        mat_id = layers[i].get("materialId", 0)
        mat_label = f"Material {mat_id}"
        for m in materials:
            if m.get("id") == mat_id:
                mat_label = m.get("label", mat_label)
                break

        hover_text = (
            f"Layer {i}<br>"
            f"Material: {mat_label}<br>"
            f"D_inner: {inner_d:.4f} m<br>"
            f"D_outer: {outer_d:.4f} m<br>"
            f"Thickness: {(outer_d - inner_d) / 2:.4f} m"
        )

        fig.add_trace(go.Scatter(
            x=x_outer, y=y_outer,
            fill='toself',
            fillcolor=color,
            line=dict(color='#555', width=1),
            name=f"Layer {i} (Mat {mat_id})",
            hoverinfo='text',
            text=hover_text,
            mode='lines',
        ))

    # Draw annular flow region (white/light fill between innerDiameter and outerDiameter)
    if is_anular:
        r_ext = d_externo / 2
        x_ext = (r_ext * np.cos(theta)).tolist()
        y_ext = (r_ext * np.sin(theta)).tolist()
        fig.add_trace(go.Scatter(
            x=x_ext, y=y_ext,
            fill='toself',
            fillcolor='rgba(200, 230, 240, 0.5)',
            line=dict(color='#39C0E0', width=1.5, dash='dot'),
            name='Annular region',
            hoverinfo='text',
            text=f"Annular flow region<br>D_ext: {d_externo:.4f} m<br>D_int: {d_interno:.4f} m",
            mode='lines',
        ))

    # Draw internal diameter (flow bore)
    r_int = d_interno / 2
    x_int = (r_int * np.cos(theta)).tolist()
    y_int = (r_int * np.sin(theta)).tolist()
    fig.add_trace(go.Scatter(
        x=x_int, y=y_int,
        fill='toself',
        fillcolor='white',
        line=dict(color='#333', width=2),
        name='Flow bore',
        hoverinfo='text',
        text=f"Flow bore<br>D_int: {d_interno:.4f} m",
        mode='lines',
    ))

    # Layout
    plot_range = largest_diameter / 2 * 1.1
    fig.update_layout(
        xaxis=dict(
            scaleanchor="y", scaleratio=1,
            range=[-plot_range, plot_range],
            showgrid=False, zeroline=False, showticklabels=False,
        ),
        yaxis=dict(
            range=[-plot_range, plot_range],
            showgrid=False, zeroline=False, showticklabels=False,
        ),
        showlegend=True,
        legend=dict(orientation="h", yanchor="bottom", y=-0.15, xanchor="center", x=0.5),
        margin=dict(l=10, r=10, t=30, b=10),
        height=350,
        plot_bgcolor='white',
    )

    return fig


@st.dialog("Edit Cross Section", width="large")
def _section_dialog(idx):
    """Full edit form for a cross section with layers."""
    sections = get_val(["crossSection"], [])
    if idx >= len(sections):
        st.error("Section not found.")
        return
    sec = sections[idx]

    st.subheader(f"Cross Section {idx} (ID: {sec.get('id', idx)})")

    widget_text("Label", ["crossSection", idx, "label"],
                help_text="Display name for this section")

    c1, c2, c3, c4, c5 = st.columns(5)
    with c1:
        widget_bool("Active", ["crossSection", idx, "active"], default=True,
                    help_text="Whether this cross section is active.")
    with c2:
        widget_int("ID", ["crossSection", idx, "id"], default=idx,
                   help_text="Integer identifier for this cross section.")
    with c3:
        widget_bool("Annular", ["crossSection", idx, "annular"],
                    help_text="If true, the flow region is annular (two concentric diameters).")
    with c4:
        widget_number("D internal [m]", ["crossSection", idx, "innerDiameter"],
                      default=0.1,
                      help_text="Inner diameter of the flow section. For annular, the smaller diameter.")
    with c5:
        widget_number("Roughness [m]", ["crossSection", idx, "roughness"],
                      default=5e-5, fmt="%.2e",
                      help_text="Absolute wall roughness.")

    if sec.get("annular"):
        widget_number("D external [m]", ["crossSection", idx, "outerDiameter"],
                      default=0.15,
                      help_text="Outer diameter of the annular flow region.")

    # ── Cross-section visualization ──
    st.markdown("**Cross-Section View**")
    fig = _draw_cross_section_figure(sec)
    st.plotly_chart(fig, use_container_width=True, key=f"xsec_plot_{idx}")

    # Layers
    st.markdown("**Layers** (inner → outer)")
    layers = sec.get("layers", [])

    if st.button("➕ Add Layer", key=f"dlg_add_layer_{idx}"):
        layers.append({"layerMeasurementType": "THICKNESS", "thickness": 0.01,
                       "discretization": 1, "materialId": 0})
        sec["layers"] = layers
        set_val(["crossSection"], sections)
        st.rerun(scope="fragment")

    for j, layer in enumerate(layers):
        lc = st.columns([2, 2, 2, 2, 1])
        with lc[0]:
            meas_type = st.selectbox(
                "Measurement", ["THICKNESS", "DIAMETER"],
                index=0 if layer.get("layerMeasurementType", "THICKNESS") == "THICKNESS" else 1,
                key=f"dlg_layer_meas_{idx}_{j}",
                help="THICKNESS: radial thickness; DIAMETER: outer diameter from pipe center."
            )
            layers[j]["layerMeasurementType"] = meas_type
        with lc[1]:
            if meas_type == "THICKNESS":
                layers[j]["thickness"] = st.number_input(
                    "Thickness [m]", value=float(layer.get("thickness", 0.01)),
                    key=f"dlg_layer_esp_{idx}_{j}", format="%.4f",
                    help="Radial thickness of this layer."
                )
            else:
                layers[j]["diameter"] = st.number_input(
                    "Diameter [m]", value=float(layer.get("diameter", 0.12)),
                    key=f"dlg_layer_diam_{idx}_{j}", format="%.4f",
                    help="Diameter from pipe center to outer edge of this layer."
                )
        with lc[2]:
            layers[j]["discretization"] = int(st.number_input(
                "Discretization", value=int(layer.get("discretization", 1)),
                min_value=1, key=f"dlg_layer_disc_{idx}_{j}",
                help="Number of radial subdivisions for heat transfer calculation."
            ))
        with lc[3]:
            layers[j]["materialId"] = int(st.number_input(
                "Material ID", value=int(layer.get("materialId", 0)),
                min_value=0, key=f"dlg_layer_mat_{idx}_{j}",
                help="ID of the material (from Materials tab) composing this layer."
            ))
        with lc[4]:
            if st.button("🗑️", key=f"dlg_del_layer_{idx}_{j}"):
                layers.pop(j)
                sec["layers"] = layers
                set_val(["crossSection"], sections)
                st.rerun(scope="fragment")

    sec["layers"] = layers
    set_val(["crossSection"], sections)

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_sec_done_{idx}"):
        st.session_state.pop("_editing_section", None)
        st.rerun()


@st.dialog("Edit Rock Formation", width="large")
def _formation_dialog(idx):
    """Full edit form for a rock formation entry."""
    props = get_val(["initialConfig", "formation", "properties"], [])
    if idx >= len(props):
        st.error("Formation entry not found.")
        return
    rock = props[idx]
    path = ["initialConfig", "formation", "properties", idx]

    st.subheader(f"Rock {idx} (ID: {rock.get('id', idx)})")

    widget_text("Label", path + ["label"],
                help_text="Display name for this rock")

    c1, c2 = st.columns(2)
    with c1:
        widget_int("ID", path + ["id"], default=idx)
    with c2:
        widget_number("Conductivity [W/(m·°C)]", path + ["conductivity"])
    c3, c4 = st.columns(2)
    with c3:
        widget_number("Specific heat [J/(kg·°C)]", path + ["specificHeat"])
    with c4:
        widget_number("Density [kg/m³]", path + ["density"])

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_form_done_{idx}"):
        st.session_state.pop("_editing_formation", None)
        st.rerun()


@st.dialog("Edit Pipe", width="large")
def _pipe_dialog(pipe_key, idx):
    """Full edit form for a single pipe (production or service)."""
    pipes = get_val([pipe_key], [])
    if idx >= len(pipes):
        st.error("Pipe not found.")
        return
    pipe = pipes[idx]

    kind = "Production" if pipe_key == "productionPipe" else "Service"
    st.subheader(f"{kind} Pipe {idx} (ID: {pipe.get('id', idx)})")

    widget_text("Label", [pipe_key, idx, "label"],
                help_text="Display name for this pipe")

    # ── Basic identification ─────────────────────────────────────────
    st.markdown("**Basic Properties**")
    c1, c2, c3, c4 = st.columns(4)
    with c1:
        widget_bool("Active", [pipe_key, idx, "active"], default=True,
                    help_text="Whether this pipe is active in the simulation.")
        widget_int("ID", [pipe_key, idx, "id"], default=idx,
                   help_text="Integer identifier for this pipe.")
    with c2:
        widget_number("Angle [rad]", [pipe_key, idx, "angle"], default=0.0,
                      help_text="Inclination from horizontal. Used when xyMode=false (default).")
        widget_int("Cross-section ID", [pipe_key, idx, "crossSectionId"], default=0,
                   help_text="ID of the cross section (from Cross Sections tab) composing this pipe.")
    with c3:
        widget_number("X end [m]", [pipe_key, idx, "xCoor"],
                      help_text="Final X coord (xyMode=true)")
        widget_number("Y end [m]", [pipe_key, idx, "yCoor"],
                      help_text="Final Y coord (xyMode=true)")
    with c4:
        widget_int("Formation ID", [pipe_key, idx, "formationId"], default=0,
                   help_text="Formation for heat exchange. 0 = none.")
        widget_int("numCellsXY", [pipe_key, idx, "numCellsXY"], default=0,
                   help_text="Number of cells (xyMode=true)")

    # ── Heat exchange & coupling ─────────────────────────────────────
    st.markdown("**Heat Exchange & Coupling**")
    c1, c2, c3, c4 = st.columns(4)
    with c1:
        widget_int_select("External environment", [pipe_key, idx, "environment"], {
            0: "0 - User defined", 1: "1 - Seawater", 2: "2 - Atmosphere"
        }, default=0, help_text="Infinite fluid for heat exchange. Not needed when formation is set or pipe is thermally coupled.")
    with c2:
        widget_int_select("Convection direction", [pipe_key, idx, "convectionDirection"], {
            0: "0 - Transverse", 1: "1 - Longitudinal"
        }, default=0, help_text="0: cross-flow (immersed pipe); 1: axial flow (annular coupling). Only valid without formation.")
    with c3:
        widget_int_select("Thermal coupling", [pipe_key, idx, "thermalCoupling"], {
            0: "0 - No", 1: "1 - Yes (coupled with counterpart pipe)"
        }, default=0, help_text="If 1, pipe is thermally coupled with its counterpart. Must match in position, length, and discretization.")
    with c4:
        widget_bool("Grouping", [pipe_key, idx, "grouping"], default=True,
                    help_text="If true, discretization uses cell groups (nCells×length). If false, each cell length is specified individually.")

    # ── Production-only / Service-only fields ────────────────────────
    if pipe_key == "productionPipe":
        st.markdown("**Advanced (Production only)**")
        c1, c2, c3 = st.columns(3)
        with c1:
            widget_int_select("2D thermal diffusion", [pipe_key, idx, "diffusion2d"], {
                0: "0 - No", 1: "1 - Yes"
            }, default=0, help_text="If 1, uses 2D heat transfer model for buried pipe defined in the JSON file below.")
            widget_text("2D diffusion JSON file", [pipe_key, idx, "diffusion2dJson"],
                        help_text="Filename of 2D thermal model JSON")
        with c2:
            widget_int("ssFlowModel", [pipe_key, idx, "ssFlowModel"], default=0,
                       help_text="Multiphase flow correlation override for this pipe.")
            widget_int_select("Parallel network coupling", [pipe_key, idx, "parallelNetworkThermalCoupling"], {
                0: "0 - No", 1: "1 - Yes"
            }, default=0, help_text="If 1, pipe is thermally coupled with a parallel network pipe.")
        with c3:
            widget_int("dPdLHidro correction idx", [pipe_key, idx, "correctionFactorHydro"], default=0,
                       help_text="Index in correction.correctionFactorHydro vector")
            widget_int("dPdLFric correction idx", [pipe_key, idx, "correctionFactorFric"], default=0,
                       help_text="Index in correction.correctionFactorFric vector")
            widget_int("dTdL correction idx", [pipe_key, idx, "correctionFactorTemp"], default=0,
                       help_text="Index in correction.correctionFactorTemp vector")
    else:
        st.markdown("**Correction Indices**")
        c1, c2, c3 = st.columns(3)
        with c1:
            widget_int("dPdLHidro correction idx", [pipe_key, idx, "correctionFactorHydro"], default=0,
                       help_text="Index in correction.correctionFactorHydro vector for hydrostatic pressure drop correction.")
        with c2:
            widget_int("dPdLFric correction idx", [pipe_key, idx, "correctionFactorFric"], default=0,
                       help_text="Index in correction.correctionFactorFric vector for friction pressure drop correction.")
        with c3:
            widget_int("dTdL correction idx", [pipe_key, idx, "correctionFactorTemp"], default=0,
                       help_text="Index in correction.correctionFactorTemp vector for temperature gradient correction.")

    # ── Discretization ───────────────────────────────────────────────
    st.markdown("**Discretization**")
    st.caption("Used when grouping=true and xyMode=false")
    disc = pipe.get("discretization", [])
    disc_df = pd.DataFrame(disc) if disc else pd.DataFrame(columns=["numCells", "length"])
    edited_disc = st.data_editor(
        disc_df, num_rows="dynamic", use_container_width=True,
        column_config={
            "numCells": st.column_config.NumberColumn("N Cells", format="%d"),
            "length": st.column_config.NumberColumn("Length [m]", format="%.2f"),
        },
        key=f"dlg_disc_{pipe_key}_{idx}"
    )
    pipes[idx]["discretization"] = edited_disc.to_dict(orient="records")

    # dxCelula
    ci_dx = pipe.get("cellDx", [])
    dx_str = st.text_input(
        "cellDx [m] (comma-sep, for grouping=false)",
        value=", ".join(str(x) for x in ci_dx) if ci_dx else "",
        key=f"dlg_dxcel_{pipe_key}_{idx}",
        help="Individual cell lengths when grouping=false"
    )
    if dx_str.strip():
        try:
            pipes[idx]["cellDx"] = [float(x.strip()) for x in dx_str.split(",") if x.strip()]
        except ValueError:
            pass
    elif "cellDx" in pipes[idx]:
        del pipes[idx]["cellDx"]

    # ── Initial & Boundary Conditions ────────────────────────────────
    st.markdown("**Initial Conditions & External Environment**")
    ci = pipe.get("initialConditions", pipe.get("initialAndAmbientConditions", {}))

    # Determine the key used in the original data for initial conditions
    _ci_key = "initialConditions" if "initialConditions" in pipe else "initialAndAmbientConditions"

    def _dlg_ci_input(label, field, default_val, col, help_txt=None):
        val = ci.get(field, default_val)
        txt = col.text_input(
            label,
            value=", ".join(str(x) for x in val) if val else "",
            key=f"dlg_ci_{field}_{pipe_key}_{idx}",
            help=help_txt
        )
        if txt.strip():
            try:
                pipes[idx].setdefault(_ci_key, {})[field] = [
                    float(x.strip()) for x in txt.split(",") if x.strip()
                ]
            except ValueError:
                pass

    st.caption("Interpolation positions (measuredPosition: 0 → 1)")
    _dlg_ci_input("measuredPosition", "measuredPosition", [], st,
                  help_txt="Relative pipe positions (must start at 0, end at 1). Other properties are linearly interpolated between these points.")

    c1, c2 = st.columns(2)
    _dlg_ci_input("Pressure [kgf/cm²]", "pressure", [], c1,
                  help_txt="Initial pressures at measuredPosition points. Transient only (initialCondition=0).")
    _dlg_ci_input("Temperature [°C]", "temp", [], c2,
                  help_txt="Initial temperatures at measuredPosition points. Transient only (initialCondition=0).")

    if pipe_key == "productionPipe":
        c1, c2, c3, c4 = st.columns(4)
        _dlg_ci_input("Holdup", "holdup", [], c1,
                      help_txt="Initial liquid holdup at measuredPosition points. Transient only.")
        _dlg_ci_input("Bet", "complementaryFluidFraction", [], c2,
                      help_txt="Complementary fluid volume fraction. Transient only (if complementary fluid is active).")
        _dlg_ci_input("uls [m/s]", "usl", [], c3,
                      help_txt="Superficial liquid velocity at measuredPosition points. Transient only.")
        _dlg_ci_input("ugs [m/s]", "usg", [], c4,
                      help_txt="Superficial gas velocity at measuredPosition points. Transient only.")
    else:
        _dlg_ci_input("Gas mass flow [kg/s]", "gasMassFlowRate", [], st,
                      help_txt="Gas mass flow rate at measuredPosition points. Transient only.")

    st.markdown("**External environment**")
    c1, c2, c3 = st.columns(3)
    _dlg_ci_input("External T [°C]", "ambientTemp", [4.0], c1,
                  help_txt="Ambient temperature at measuredPosition points. Not needed if thermally coupled.")
    _dlg_ci_input("External vel [m/s]", "ambientVel", [0.0], c2,
                  help_txt="External fluid velocity. Not needed if formation is set or thermally coupled.")
    _dlg_ci_input("k External [W/(m·K)]", "ambientConductivity", [], c3,
                  help_txt="External thermal conductivity. Not needed if formation is set or environment is seawater/atmosphere.")

    c1, c2, c3 = st.columns(3)
    _dlg_ci_input("Cp External [J/(kg·K)]", "ambientSpecificHeat", [], c1,
                  help_txt="External specific heat. Not needed if formation is set or environment is seawater/atmosphere.")
    _dlg_ci_input("ρ External [kg/m³]", "ambientDensity", [], c2,
                  help_txt="External fluid density. Not needed if formation is set or environment is seawater/atmosphere.")
    _dlg_ci_input("μ External [cP]", "ambientVisc", [], c3,
                  help_txt="External fluid viscosity. Not needed if formation is set or environment is seawater/atmosphere.")

    # ── 3D Thermal fields (production only) ──────────────────────────
    if pipe_key == "productionPipe":
        st.markdown("**3D Thermal Diffusion (advanced)**")
        dt3d = pipe.get("diffusion3d", [])
        dt3d_str = st.text_input(
            "difusTerm3D (int array, comma-sep)",
            value=", ".join(str(x) for x in dt3d) if dt3d else "",
            key=f"dlg_dt3d_{pipe_key}_{idx}",
            help="3D thermal diffusion flags per pipe segment."
        )
        if dt3d_str.strip():
            try:
                pipes[idx]["diffusion3d"] = [int(x.strip()) for x in dt3d_str.split(",") if x.strip()]
            except ValueError:
                pass

        dt3dfe = pipe.get("diffusion3dFE", [])
        dt3dfe_str = st.text_input(
            "difusTerm3DFE (float array, comma-sep)",
            value=", ".join(str(x) for x in dt3dfe) if dt3dfe else "",
            key=f"dlg_dt3dfe_{pipe_key}_{idx}",
            help="Finite element parameters for 3D thermal diffusion model."
        )
        if dt3dfe_str.strip():
            try:
                pipes[idx]["diffusion3dFE"] = [float(x.strip()) for x in dt3dfe_str.split(",") if x.strip()]
            except ValueError:
                pass

        dt3dacop = pipe.get("diffusion3dCoupling", [])
        dt3dacop_str = st.text_input(
            "difusTerm3DAcop (string array, comma-sep filenames)",
            value=", ".join(dt3dacop) if dt3dacop else "",
            key=f"dlg_dt3dacop_{pipe_key}_{idx}",
            help="JSON filenames for 3D thermally coupled segments."
        )
        if dt3dacop_str.strip():
            pipes[idx]["diffusion3dCoupling"] = [x.strip() for x in dt3dacop_str.split(",") if x.strip()]

    set_val([pipe_key], pipes)

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_pipe_done_{pipe_key}_{idx}"):
        st.session_state.pop("_editing_pipe", None)
        st.rerun()


# ─────────────────────────────────────────────────────────────────────────────
# ACCESSORY DIALOG FUNCTIONS
# ─────────────────────────────────────────────────────────────────────────────

@st.dialog("Edit IPR", width="large")
def _ipr_dialog(idx):
    """Full edit form for a single IPR entry."""
    iprs = get_val(["ipr"], [])
    if idx >= len(iprs):
        st.error("IPR not found.")
        return
    ipr = iprs[idx]
    path = ["ipr", idx]

    st.subheader(f"IPR {idx} (ID: {ipr.get('id', idx)})")
    widget_text("Label", path + ["label"], help_text="Display name for this IPR")
    c1, c2, c3 = st.columns(3)
    with c1:
        widget_bool("Active", path + ["active"], default=True,
                    help_text="Whether this IPR source is active.")
        widget_int("ID", path + ["id"], default=idx,
                   help_text="Integer identifier for this IPR.")
    with c2:
        widget_int("Fluid ID", path + ["prodFluidId"], default=0,
                   help_text="Production fluid ID to be produced by this source.")
        widget_number("Position [m]", path + ["measuredLength"],
                      help_text="Position along the production line.")
    with c3:
        widget_int_select("IPR type", path + ["iprType"], {
            0: "0 - Linear", 1: "1 - Vogel combined", 2: "2 - Vogel"
        }, default=1, help_text="0: linear (uses ip); 1: Vogel combined (ip+qMax); 2: pure Vogel (qMax only).")

    widget_time_series(f"IPR {idx} - Static pressure", path,
                       [{"name": "staticPressure", "unit": "kgf/cm²"}],
                       time_col="staticPressureTime")
    widget_time_series(f"IPR {idx} - Temperature", path,
                       [{"name": "temperatures", "unit": "°C"}],
                       time_col="temperaturesTime")
    widget_time_series(f"IPR {idx} - Productivity index (ip)", path,
                       [{"name": "ip", "unit": "(sm³/d)/(kgf/cm²)"}],
                       time_col="ipTime")
    widget_time_series(f"IPR {idx} - Max flow qMax (Vogel)", path,
                       [{"name": "qMax", "unit": "sm³/d"}],
                       time_col="qMaxTime")
    widget_time_series(f"IPR {idx} - Injectivity index (ii)", path,
                       [{"name": "ii", "unit": "(sm³/d)/(kgf/cm²)"}],
                       time_col="iiTime")

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_ipr_done_{idx}"):
        st.session_state.pop("_editing_ipr", None)
        st.rerun()


@st.dialog("Edit Liquid Source", width="large")
def _liquid_source_dialog(idx):
    """Full edit form for a single liquid source."""
    items = get_val(["liquidSource"], [])
    if idx >= len(items):
        st.error("Liquid source not found.")
        return
    item = items[idx]
    path = ["liquidSource", idx]

    st.subheader(f"Liquid Source {idx} (ID: {item.get('id', idx)})")
    widget_text("Label", path + ["label"], help_text="Display name for this liquid source")
    c1, c2, c3 = st.columns(3)
    with c1:
        widget_bool("Active", path + ["active"], default=True,
                    help_text="Whether this liquid source is active.")
        widget_int("ID", path + ["id"], default=idx,
                   help_text="Integer identifier for this liquid source.")
    with c2:
        widget_int("Fluid ID", path + ["prodFluidId"], default=0,
                   help_text="Production fluid ID to be produced by this source.")
    with c3:
        widget_number("Position [m]", path + ["measuredLength"],
                      help_text="Position along the production line.")

    widget_time_series(f"Liquid Source {idx}", path,
                       [{"name": "temperature", "unit": "°C"},
                        {"name": "beta", "unit": "-"},
                        {"name": "liquidFlowRate", "unit": "sm³/d"}])

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_liqsrc_done_{idx}"):
        st.session_state.pop("_editing_liq_src", None)
        st.rerun()


@st.dialog("Edit Mass Source", width="large")
def _mass_source_dialog(idx):
    """Full edit form for a single mass source."""
    items = get_val(["massSource"], [])
    if idx >= len(items):
        st.error("Mass source not found.")
        return
    item = items[idx]
    path = ["massSource", idx]

    st.subheader(f"Mass Source {idx} (ID: {item.get('id', idx)})")
    widget_text("Label", path + ["label"], help_text="Display name for this mass source")
    c1, c2, c3 = st.columns(3)
    with c1:
        widget_bool("Active", path + ["active"], default=True,
                    help_text="Whether this mass source is active.")
        widget_int("ID", path + ["id"], default=idx,
                   help_text="Integer identifier for this mass source.")
    with c2:
        widget_int("Fluid ID", path + ["prodFluidId"], default=0,
                   help_text="Production fluid ID to be produced by this source.")
        widget_int_select("Thermo type", path + ["thermType"], {
            0: "0 - Equilibrium", 1: "1 - User gas flow"
        }, default=0, help_text="0: gas flow from thermodynamic equilibrium; 1: user-defined via gasMassFlowRate.")
    with c3:
        widget_number("Position [m]", path + ["measuredLength"],
                      help_text="Position along the production line.")

    widget_time_series(f"Mass Source {idx}", path,
                       [{"name": "temperature", "unit": "°C"},
                        {"name": "totalMassFlowRate", "unit": "kg/s"},
                        {"name": "complementaryMassFlowRate", "unit": "kg/s"},
                        {"name": "gasMassFlow", "unit": "kg/s"}])

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_masssrc_done_{idx}"):
        st.session_state.pop("_editing_mass_src", None)
        st.rerun()


@st.dialog("Edit Gas Source", width="large")
def _gas_source_dialog(idx):
    """Full edit form for a single gas source."""
    items = get_val(["gasSource"], [])
    if idx >= len(items):
        st.error("Gas source not found.")
        return
    item = items[idx]
    path = ["gasSource", idx]

    st.subheader(f"Gas Source {idx} (ID: {item.get('id', idx)})")
    widget_text("Label", path + ["label"], help_text="Display name for this gas source")
    c1, c2, c3 = st.columns(3)
    with c1:
        widget_bool("Active", path + ["active"], default=True,
                    help_text="Whether this gas source is active.")
        widget_int("ID", path + ["id"], default=idx,
                   help_text="Integer identifier for this gas source.")
    with c2:
        widget_bool("Dry gas", path + ["dry"], default=True,
                    help_text="If true, produces dry gas. If false, rich gas requiring fluid definition.")
        widget_int("Fluid ID", path + ["prodFluidId"], default=0,
                   help_text="Production fluid ID. Valid when seco=false (rich gas).")
    with c3:
        widget_number("Position [m]", path + ["measuredLength"],
                      help_text="Position along the production line.")

    widget_time_series(f"Gas Source {idx}", path,
                       [{"name": "temperature", "unit": "°C"},
                        {"name": "gasFlowRate", "unit": "sm³/d"},
                        {"name": "complementaryFluidFlowRate", "unit": "ppm"}])

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_gassrc_done_{idx}"):
        st.session_state.pop("_editing_gas_src", None)
        st.rerun()


@st.dialog("Edit Pressure Source", width="large")
def _pressure_source_dialog(idx):
    """Full edit form for a single pressure source."""
    items = get_val(["pressureSource"], [])
    if idx >= len(items):
        st.error("Pressure source not found.")
        return
    item = items[idx]
    path = ["pressureSource", idx]

    st.subheader(f"Pressure Source {idx} (ID: {item.get('id', idx)})")
    widget_text("Label", path + ["label"], help_text="Display name for this pressure source")
    c1, c2, c3 = st.columns(3)
    with c1:
        widget_bool("Active", path + ["active"], default=True,
                    help_text="Whether this pressure source is active.")
        widget_int("ID", path + ["id"], default=idx,
                   help_text="Integer identifier for this pressure source.")
        widget_number("Position [m]", path + ["measuredLength"],
                      help_text="Position along the production line.")
    with c2:
        widget_int_select("Fluid type", path + ["fluidType"], {
            0: "0 - User fluid", 1: "1 - In-pipe fluid"}, default=1,
            help_text="1: uses in-situ hydrocarbon; 0: uses prodFluidId.")
        widget_int("Fluid ID", path + ["prodFluidId"], default=0,
                   help_text="External fluid ID. Valid for fluidType=0.")
        widget_bool("Gas environment", path + ["gasAmbient"], default=False,
                    help_text="If true, only gas fraction is admitted. Valid for fluidType=0.")
    with c3:
        widget_int_select("Check type", path + ["check"], {
            0: "0 - Normal", 1: "1 - Vacuum breaker", -1: "-1 - Check valve"}, default=0,
            help_text="0: normal hole; 1: vacuum breaker; -1: check valve (one-way).")
        widget_int_select("Opening type", path + ["openingType"], {
            0: "0 - Area ratio", 1: "1 - Diameter ratio"}, default=0,
            help_text="0: opening as hole/pipe area ratio; 1: as hole/pipe diameter ratio.")
        widget_number("Cd", path + ["cd"], default=0.84,
                      help_text="Discharge coefficient (default 0.84).")

    c4, c5, c6 = st.columns(3)
    with c4:
        widget_number("Beta", path + ["beta"], default=0.0,
                      help_text="Complementary fluid fraction in the external fluid.")
    with c5:
        widget_number("Ext. temperature [°C]", path + ["temperature"],
                      help_text="External fluid temperature.")
    with c6:
        widget_number("Ext. pressure [kgf/cm²]", path + ["pressure"],
                      help_text="External fluid pressure.")

    widget_time_series(f"Pressure Source {idx} - Opening", path,
                       [{"name": "opening", "unit": "ratio"}])

    st.markdown("**Additional parameters**")
    c7, c8 = st.columns(2)
    with c7:
        widget_number("External title (titAmb)", path + ["ambientFluidQuality"],
                      help_text="Ambient fluid title (gas mass fraction).")
    with c8:
        _tchk = get_val(path + ["chokeTime"], [])
        _tchk_df = pd.DataFrame({"tempoChk [s]": _tchk})
        _tchk_ed = st.data_editor(_tchk_df, num_rows="dynamic", use_container_width=True,
                                  key=f"fpsrc_tchk_{idx}")
        _tchk_list = _tchk_ed["tempoChk [s]"].dropna().tolist()
        if _path_exists(path + ["chokeTime"]) or _tchk_list:
            set_val(path + ["chokeTime"], _tchk_list)

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_fpsrc_done_{idx}"):
        st.session_state.pop("_editing_fp_src", None)
        st.rerun()


@st.dialog("Edit Radial Pore Source", width="large")
def _poro_rad_dialog(idx):
    """Full edit form for a single radial pore source."""
    items = get_val(["porousRadialSource"], [])
    if idx >= len(items):
        st.error("Radial pore source not found.")
        return
    item = items[idx]
    path = ["porousRadialSource", idx]

    st.subheader(f"Radial Pore Source {idx} (ID: {item.get('id', idx)})")
    widget_text("Label", path + ["label"], help_text="Display name for this source")
    c1, c2, c3 = st.columns(3)
    with c1:
        widget_bool("Active", path + ["active"], default=True,
                    help_text="Whether this radial pore source is active.")
        widget_int("ID", path + ["id"], default=idx,
                   help_text="Integer identifier for this source.")
    with c2:
        widget_number("Position [m]", path + ["measuredLength"],
                      help_text="Position along the production line.")
    with c3:
        _json_files = [f for f in os.listdir(st.session_state["work_dir"])
                       if f.lower().endswith(".json")]
        _cur_arq = get_val(path + ["file"], "")
        if _cur_arq and _cur_arq not in _json_files:
            _json_files.insert(0, _cur_arq)
        _arq_options = [""] + _json_files
        _arq_idx = _arq_options.index(_cur_arq) if _cur_arq in _arq_options else 0
        _arq_sel = st.selectbox(
            "JSON file", _arq_options, index=_arq_idx,
            help="JSON file detailing this radial pore source (upload in sidebar).",
            key=f"dlg_poro_rad_arq_{idx}"
        )
        if _path_exists(path + ["file"]) or _arq_sel:
            set_val(path + ["file"], _arq_sel if _arq_sel else None)

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_poro_rad_done_{idx}"):
        st.session_state.pop("_editing_poro_rad", None)
        st.rerun()


@st.dialog("Edit 2D Pore Source", width="large")
def _poro_2d_dialog(idx):
    """Full edit form for a single 2D pore source."""
    items = get_val(["porous2DSource"], [])
    if idx >= len(items):
        st.error("2D pore source not found.")
        return
    item = items[idx]
    path = ["porous2DSource", idx]

    st.subheader(f"2D Pore Source {idx} (ID: {item.get('id', idx)})")
    widget_text("Label", path + ["label"], help_text="Display name for this source")
    c1, c2, c3 = st.columns(3)
    with c1:
        widget_bool("Active", path + ["active"], default=True,
                    help_text="Whether this 2D pore source is active.")
        widget_int("ID", path + ["id"], default=idx,
                   help_text="Integer identifier for this source.")
    with c2:
        widget_number("Position [m]", path + ["measuredLength"],
                      help_text="Position along the production line.")
    with c3:
        _json_files = [f for f in os.listdir(st.session_state["work_dir"])
                       if f.lower().endswith(".json")]
        _cur_arq = get_val(path + ["file"], "")
        if _cur_arq and _cur_arq not in _json_files:
            _json_files.insert(0, _cur_arq)
        _arq_options = [""] + _json_files
        _arq_idx = _arq_options.index(_cur_arq) if _cur_arq in _arq_options else 0
        _arq_sel = st.selectbox(
            "JSON file", _arq_options, index=_arq_idx,
            help="JSON file detailing this 2D pore source (upload in sidebar).",
            key=f"dlg_poro_2d_arq_{idx}"
        )
        if _path_exists(path + ["file"]) or _arq_sel:
            set_val(path + ["file"], _arq_sel if _arq_sel else None)

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_poro_2d_done_{idx}"):
        st.session_state.pop("_editing_poro_2d", None)
        st.rerun()


@st.dialog("Edit Valve", width="large")
def _valve_dialog(idx):
    """Full edit form for a single generic valve."""
    items = get_val(["valve"], [])
    if idx >= len(items):
        st.error("Valve not found.")
        return
    item = items[idx]
    path = ["valve", idx]

    st.subheader(f"Valve {idx} (ID: {item.get('id', idx)})")
    widget_text("Label", path + ["label"], help_text="Display name for this valve")
    c1, c2, c3 = st.columns(3)
    with c1:
        widget_bool("Active", path + ["active"], default=True,
                    help_text="Whether this valve is active.")
        widget_int("ID", path + ["id"], default=idx,
                   help_text="Integer identifier for this valve.")
    with c2:
        widget_number("Position [m]", path + ["measuredLength"],
                      help_text="Position along the production line.")
        widget_number("Cd", path + ["cd"], default=0.84,
                      help_text="Discharge coefficient (Sachdeva model). Default 0.84.")
    with c3:
        widget_int_select("CV curve type", path + ["cvCurve"], {
            0: "0 - Area ratio", 1: "1 - Stem displacement"}, default=0,
            help_text="0: opening as free area / pipe area; 1: stem displacement with CV curve.")

    widget_time_series(f"Valve {idx} - Opening", path,
                       [{"name": "opening", "unit": "ratio"}])

    # CV curve (x1/cv1) — only relevant when curvaCV=1
    cv_type = get_val(path + ["cvCurve"], 0)
    if cv_type == 1:
        st.markdown("**CV Curve** (stem displacement → flow coefficient)")
        x1_vals = item.get("x1", [])
        cv1_vals = item.get("cv1", [])
        max_cv_len = max(len(x1_vals), len(cv1_vals)) if x1_vals or cv1_vals else 0
        while len(x1_vals) < max_cv_len:
            x1_vals.append(None)
        while len(cv1_vals) < max_cv_len:
            cv1_vals.append(None)
        cv_df = pd.DataFrame({"x1 (% stem)": x1_vals, "cv1 (gal/min)/psi": cv1_vals})
        edited_cv = st.data_editor(cv_df, num_rows="dynamic",
                                    use_container_width=True, key=f"valve_cv_{idx}")
        items[idx]["x1"] = edited_cv["x1 (% stem)"].dropna().tolist()
        items[idx]["cv1"] = edited_cv["cv1 (gal/min)/psi"].dropna().tolist()
        set_val(["valve"], items)

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_valve_done_{idx}"):
        st.session_state.pop("_editing_valve", None)
        st.rerun()


@st.dialog("Edit Gas Lift Valve", width="large")
def _vgl_dialog(idx):
    """Full edit form for a single gas lift valve."""
    items = get_val(["gasLiftSource"], [])
    if idx >= len(items):
        st.error("VGL not found.")
        return
    item = items[idx]
    path = ["gasLiftSource", idx]

    st.subheader(f"VGL {idx} (ID: {item.get('id', idx)})")
    widget_text("Label", path + ["label"], help_text="Display name for this gas-lift valve")
    c1, c2 = st.columns(2)
    with c1:
        widget_bool("Active", path + ["active"], default=True,
                    help_text="Whether this gas-lift valve is active.")
        widget_int("ID", path + ["id"], default=idx,
                   help_text="Integer identifier for this VGL.")
        widget_bool("Col. & annular", path + ["annulusColumnFlag"], default=True,
                    help_text="If true, service line position is auto-calculated from annular geometry.")
        widget_number("Prod. position [m]", path + ["prodMeasuredLength"],
                      help_text="Position on the production line.")
        widget_number("Serv. position [m]", path + ["serviceMeasuredLength"],
                      help_text="Position on the service line. Valid when colunaEanular=false.")
    with c2:
        widget_int_select("Valve type", path + ["valveType"], {
            0: "0 - Orifice", 1: "1 - Pressure", 2: "2 - Venturi"}, default=0,
            help_text="0: orifice; 1: pressure (bellows); 2: Venturi.")
        widget_number("Orifice D [m]", path + ["orificeDiameter"], default=0.01,
                      help_text="Orifice diameter.")
        widget_number("Cd (gas)", path + ["vglDischCoef"], default=0.84,
                      help_text="Discharge coefficient for gas flow (monophasic model). Default 0.84.")
        widget_number("Cd (liquid)", path + ["liquidDischCoef"], default=0.9,
                      help_text="Discharge coefficient for liquid flow (gas-lift discharge). Default 0.9.")
        widget_number("Area ratio", path + ["areaRatio"],
                      help_text="Port-to-diaphragm area ratio (for pressure valve, type 1).")
        widget_number("Calib. pressure [psi]", path + ["calibrationPressure"],
                      help_text="Bellows calibration pressure (pressure valve, type 1).")
        widget_number("Calib. temp [°F]", path + ["calibrationTemperature"],
                      help_text="Bellows calibration temperature (pressure valve, type 1).")
        widget_number("Port diameter [m]", path + ["outerDiameter"],
                      help_text="Port diameter (pressure valve, type 1).")

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_vgl_done_{idx}"):
        st.session_state.pop("_editing_vgl", None)
        st.rerun()


@st.dialog("Edit ESP Pump", width="large")
def _bcs_dialog(idx):
    """Full edit form for a single ESP pump."""
    items = get_val(["esp"], [])
    if idx >= len(items):
        st.error("ESP not found.")
        return
    item = items[idx]
    path = ["esp", idx]

    st.subheader(f"ESP {idx} (ID: {item.get('id', idx)})")
    widget_text("Label", path + ["label"], help_text="Display name for this ESP pump")
    c1, c2, c3 = st.columns(3)
    with c1:
        widget_bool("Active", path + ["active"], default=True,
                    help_text="Whether this ESP pump is active.")
        widget_int("ID", path + ["id"], default=idx,
                   help_text="Integer identifier for this ESP.")
        widget_number("Position [m]", path + ["measuredLength"],
                      help_text="Position along the production line.")
    with c2:
        widget_number("Ref. frequency [Hz]", path + ["referenceFreq"], default=60.0,
                      help_text="Frequency at which the performance curve was measured.")
        widget_int("N stages", path + ["stage"], default=1, min_val=1,
                   help_text="Number of pump stages.")
        widget_int("N stages (mfg)", path + ["manufacturerStage"], default=1, min_val=1,
                   help_text="Number of stages used by manufacturer to measure the curve.")
    with c3:
        widget_number("Motor efficiency", path + ["motorEfficiency"], default=1.0,
                      help_text="Motor efficiency (fraction, 0-1).")
        widget_number("Min freq [Hz]", path + ["minFrequency"], default=30.0,
                      help_text="Minimum operating frequency.")
        widget_bool("H-I correction", path + ["hiCorrection"], default=False,
                    help_text="Head-Impeller degradation correction.")

    widget_time_series(f"ESP {idx} - Frequency", path,
                       [{"name": "frequency", "unit": "Hz"}])

    st.markdown("**Performance Curve**")
    curve_data = {
        "flow rate [BPD]": item.get("flowRate", []),
        "head [ft]": item.get("pumpHead", []),
        "power [hp]": item.get("power", []),
        "efficiency": item.get("efficiency", []),
    }
    max_len = max(len(v) for v in curve_data.values()) if any(curve_data.values()) else 0
    for k in curve_data:
        while len(curve_data[k]) < max_len:
            curve_data[k].append(None)
    curve_df = pd.DataFrame(curve_data)
    edited_curve = st.data_editor(curve_df, num_rows="dynamic",
                                  use_container_width=True, key=f"bcs_curve_{idx}")
    items[idx]["flowRate"] = edited_curve["flow rate [BPD]"].dropna().tolist()
    items[idx]["pumpHead"] = edited_curve["head [ft]"].dropna().tolist()
    items[idx]["power"] = edited_curve["power [hp]"].dropna().tolist()
    items[idx]["efficiency"] = edited_curve["efficiency"].dropna().tolist()
    set_val(["esp"], items)

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_bcs_done_{idx}"):
        st.session_state.pop("_editing_bcs", None)
        st.rerun()


@st.dialog("Edit Volumetric Pump", width="large")
def _vol_pump_dialog(idx):
    """Full edit form for a single volumetric pump."""
    items = get_val(["volumetricPump"], [])
    if idx >= len(items):
        st.error("Pump not found.")
        return
    item = items[idx]
    path = ["volumetricPump", idx]

    st.subheader(f"Vol. Pump {idx} (ID: {item.get('id', idx)})")
    widget_text("Label", path + ["label"], help_text="Display name for this volumetric pump")
    c1, c2, c3 = st.columns(3)
    with c1:
        widget_bool("Active", path + ["active"], default=True,
                    help_text="Whether this volumetric pump is active.")
        widget_int("ID", path + ["id"], default=idx,
                   help_text="Integer identifier for this pump.")
    with c2:
        widget_number("Position [m]", path + ["measuredLength"],
                      help_text="Position along the production line.")
        widget_number("Capacity [cm³]", path + ["capacity"], default=100.0,
                      help_text="Volumetric chamber capacity. Flow = capacity × frequency.")
    with c3:
        widget_number("Polytropic factor", path + ["polyFactor"], default=1.4,
                      help_text="Polytropic factor for temperature gain during compression.")

    widget_time_series(f"Vol. Pump {idx} - Frequency", path,
                       [{"name": "frequency", "unit": "Hz"}])

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_bv_done_{idx}"):
        st.session_state.pop("_editing_vol_pump", None)
        st.rerun()


@st.dialog("Edit Pressure Increment", width="large")
def _delta_p_dialog(idx):
    """Full edit form for a single pressureDrop entry."""
    items = get_val(["pressureDrop"], [])
    if idx >= len(items):
        st.error("ΔP not found.")
        return
    item = items[idx]
    path = ["pressureDrop", idx]

    st.subheader(f"ΔP {idx} (ID: {item.get('id', idx)})")
    widget_text("Label", path + ["label"], help_text="Display name for this pressure increment")
    c1, c2, c3 = st.columns(3)
    with c1:
        widget_bool("Active", path + ["active"], default=True,
                    help_text="Whether this pressure increment is active.")
        widget_int("ID", path + ["id"], default=idx,
                   help_text="Integer identifier for this pressure increment.")
        widget_number("Position [m]", path + ["measuredLength"],
                      help_text="Position along the production line.")
    with c2:
        widget_int_select("Gas compression", path + ["gasCompType"], {
            0: "0 - Adiabatic", 1: "1 - Polytropic", 2: "2 - Isothermal"}, default=0,
            help_text="Gas compression process type.")
        widget_number("Polytropic/adiab. factor", path + ["polyFacOrAdiabConst"],
                      help_text="If polytropic: polytropic factor. Otherwise: adiabatic constant.")
    with c3:
        widget_number("Liquid efficiency [%]", path + ["liquidEfficiency"], default=100.0,
                      help_text="Liquid-phase pumping efficiency (default 100%).")
        widget_number("Gas efficiency [%]", path + ["gasEfficiency"], default=100.0,
                      help_text="Gas-phase pumping efficiency (default 100%).")

    widget_time_series(f"ΔP {idx} - Pressure increment", path,
                       [{"name": "pressureDrop", "unit": "kgf/cm²"}])

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_dp_done_{idx}"):
        st.session_state.pop("_editing_delta_p", None)
        st.rerun()


@st.dialog("Edit Pig", width="large")
def _pig_dialog(idx):
    """Full edit form for a single pig entry."""
    items = get_val(["pig"], [])
    if idx >= len(items):
        st.error("Pig not found.")
        return
    item = items[idx]
    path = ["pig", idx]

    st.subheader(f"Pig {idx} (ID: {item.get('id', idx)})")
    widget_text("Label", path + ["label"], help_text="Display name for this pig")
    c1, c2, c3 = st.columns(3)
    with c1:
        widget_bool("Active", path + ["active"], default=True,
                    help_text="Whether this pig is active.")
        widget_int("ID", path + ["id"], default=idx,
                   help_text="Integer identifier for this pig.")
    with c2:
        widget_number("Launcher position [m]", path + ["launcher"],
                      help_text="Position where the pig is launched.")
        widget_number("Receiver position [m]", path + ["receiver"],
                      help_text="Position where the pig is received.")
    with c3:
        widget_number("Launch time [s]", path + ["time"],
                      help_text="Time instant at which the pig is launched.")

    st.markdown("---")
    if st.button("Done", type="primary", use_container_width=True, key=f"dlg_pig_done_{idx}"):
        st.session_state.pop("_editing_pig", None)
        st.rerun()


# ─────────────────────────────────────────────────────────────────────────────
# MAIN TABS
# ─────────────────────────────────────────────────────────────────────────────

tab_names = [
    "💧 Fluids",
    "🧱 Materials",
    "⭕ Cross Sections",
    "🪨 Rock Formation",
    "📏 Pipes",
    "🔧 Accessories",
    "🔀 Boundary Conditions",
    "⏱️ Time",
    "⚙️ General",
    "📊 Results",
]

tabs = st.tabs(tab_names)

# ═══════════════════════════════════════════════════════════════════════════════
# TAB 9: GENERAL CONFIGURATIONS
# ═══════════════════════════════════════════════════════════════════════════════
with tabs[8]:
    st.header("General Configurations")
    st.caption("Global settings: system type, slip models, correlations, and numerical parameters")

    col1, col2 = st.columns(2)
    with col1:
        widget_select("System Type", ["system"], ["MULTIFASICO", "INJETOR"],
                      default="MULTIFASICO", help_text="Production (MULTIFASICO) or injection (INJETOR)")
    with col2:
        widget_text("JSON Version", ["versaoJSON"], default="3.0.0",
                    help_text="Software version referenced by this JSON")

    # initialConfig
    ci_path = ["initialConfig"]

    with st.expander("🔧 Basic Settings", expanded=True):
        c1, c2, c3 = st.columns(3)
        with c1:
            widget_bool("Sensitivity analysis (AS)", ci_path + ["sensitivityAnalysis"],
                        help_text="If true, performs sensitivity analysis.")
        with c2:
            widget_bool("Classic output", ci_path + ["classicOutput"],
                        help_text="Controls the format of simulation output text.")
            widget_bool("Detailed screen output", ci_path + ["screenPrint"],
                        help_text="Controls verbosity of screen output during simulation.")

    with st.expander("📐 Slip & Correlations"):
        c1, c2 = st.columns(2)
        with c1:
            widget_bool("Slip in steady-state", ci_path + ["steadyStateSlip"], default=True,
                        help_text="If true, considers slip (phase velocity differences) in steady-state solver.")
            widget_bool("Slip in transient", ci_path + ["transientSlip"], default=True,
                        help_text="If true, considers slip in transient solver.")
            widget_bool("Drift-type model", ci_path + ["driftModel"], default=True,
                        help_text="If true, uses drift-flux model. If false, uses black-box correlations.")
        with c2:
            widget_int_select("Flow pattern map", ci_path + ["flowPatternMap"], {
                0: "0 - Simplified Barnea",
                1: "1 - Complete Barnea",
            }, default=0, help_text="Flow pattern map for flow regime identification.")
            widget_int_select("Mass transfer model", ci_path + ["massTransfer"], {
                0: "0 - Complete (implicit)",
                1: "1 - Complete (explicit)",
                2: "2 - Simplified isothermal",
                3: "3 - No mass transfer",
            }, default=0, help_text="Model for phase mass transfer calculation.")
        st.markdown("**Slip correlations by flow pattern**")
        cc1, cc2, cc3 = st.columns(3)
        with cc1:
            widget_int_select("Stratified", ci_path + ["correlationsByPattern", "stratified"], {
                0: "0 - Choi et al",
                1: "1 - Bhagwat & Ghajar",
                2: "2 - França & Lahey",
                4: "4 - Bhagwat & Ghajar mod.",
            }, default=1, help_text="Slip correlation for stratified flow.")
        with cc2:
            widget_int_select("Bubble/Slug", ci_path + ["correlationsByPattern", "slugBubble"], {
                0: "0 - Choi et al",
                1: "1 - Bhagwat & Ghajar",
                4: "4 - Bhagwat & Ghajar mod.",
            }, default=1, help_text="Slip correlation for bubble/slug flow.")
        with cc3:
            widget_int_select("Annular/Churn", ci_path + ["correlationsByPattern", "annularChurn"], {
                0: "0 - Choi et al",
                1: "1 - Bhagwat & Ghajar",
                3: "3 - Hibiki & Ishii",
                4: "4 - Bhagwat & Ghajar mod.",
            }, default=1, help_text="Slip correlation for annular/churn flow.")

    with st.expander("🔥 Heat Transfer"):
        c1, c2 = st.columns(2)
        with c1:
            widget_bool("Thermal equilibrium", ci_path + ["thermalEquilibrium"],
                        help_text="Strategy for pipe wall temperature distribution. Linear interpolation vs iterative.")
            widget_bool("Latent heat in condensation", ci_path + ["latentHeatCond"], default=True,
                        help_text="If false, disables latent heat in condensation mass transfer processes.")
            widget_number("Mass transfer limit", ci_path + ["advanced", "massTransferLimit"],
                          default=10.0, unit="kg/(s.m)",
                          help_text="Maximum allowed mass transfer rate per meter.")
        with c2:
            widget_number("Reverse T (tempReves)", ci_path + ["reverseTemp"], unit="°C",
                          help_text="Gas return temperature for reverse flow.")
            widget_int("Col-annular coupling steps (perm)",
                       ci_path + ["advanced", "strongAnnularColCoupling"], default=0, min_val=0,
                       help_text="Strong coupling iterations between column and annular in steady-state.")

    with st.expander("🌡️ Geometry & Coordinates"):
        c1, c2 = st.columns(2)
        with c1:
            widget_bool("Geometry follows flow direction",
                        ci_path + ["geometryFollowsFlow"], default=True,
                        help_text="If true, geometry filling direction matches the flow direction.")
            widget_number("X prod start", ci_path + ["xProdStart"], unit="m",
                          help_text="Initial X coordinate of the production line (xyMode=true).")
            widget_number("Y prod start", ci_path + ["yProdStart"], unit="m",
                          help_text="Initial Y coordinate of the production line (xyMode=true).")
        with c2:
            widget_number("X service start", ci_path + ["xServiceStart"], unit="m",
                          help_text="Initial X coordinate of the service line (xyMode=true).")
            widget_number("Y service start", ci_path + ["yServiceStart"], unit="m",
                          help_text="Initial Y coordinate of the service line (xyMode=true).")

    with st.expander("🔬 Numerical Aspects"):
        c1, c2 = st.columns(2)
        with c1:
            widget_bool("Track GOR", ci_path + ["trackGOR"],
                        help_text="If true, updates GOR along the pipe using mixing rules.")
            widget_bool("Track gas density", ci_path + ["trackGasDensity"],
                        help_text="If true, updates gas density along the pipe.")
            widget_bool("Gas density BO correction", ci_path + ["freeGasDensityCorrectionBO"],
                        help_text="If true, considers in-situ deviation for free gas density.")
            widget_bool("RS/PB table", ci_path + ["RsPbTable"],
                        help_text="Pre-builds RS table using BO before simulation.")
            widget_bool("Parallelize AS", ci_path + ["parallelizeSA"],
                        help_text="If true, parallelizes sensitivity analysis runs.")
            widget_bool("3D diffusion", ci_path + ["diffusion3dMode"],
                        help_text="If true, activates 3D thermal diffusion model.")
        with c2:
            widget_int("3D threads", ci_path + ["diffusion3dThreads"], default=1, min_val=1,
                       help_text="Number of threads for 3D thermal calculations.")
            widget_text("3D diffusion JSON", ci_path + ["diffusion3dJson"],
                        help_text="JSON filename for 3D thermal diffusion model.")
            widget_number("Steady-state guess (chutePerm)", ci_path + ["steadyGuess"], default=-1.0,
                          help_text="Initial estimate for steady-state solver (-1 = automatic).")
            widget_int_select("Check valve at end", ci_path + ["checkValve"], {
                0: "0 - Off (allow reverse flow)",
                1: "1 - On (prevent reverse flow)",
            }, default=0, help_text="If 1, places a check valve at system end preventing backflow.")
            widget_int_select("Numerical order (perm.)", ci_path + ["steadyStateOrder"], {
                1: "1 - First order",
                2: "2 - Runge-Kutta 2nd order",
            }, default=1, help_text="Numerical integration order for steady-state solver.")

    with st.expander("⚡ Advanced Numerical Aspects"):
        adv = ci_path + ["advanced"]
        c1, c2, c3 = st.columns(3)
        with c1:
            widget_number("Monophase criterion", adv + ["monophasicCriterion"], default=1e-5,
                          fmt="%.1e", help_text="Threshold below which flow is treated as monophasic.")
            widget_number("Condensation criterion", adv + ["condensationCriterion"], default=1e-3,
                          fmt="%.1e", help_text="Threshold for condensation initiation.")
            widget_number("False chord search", adv + ["regulaFalsiSearchCriterion"], default=0.1,
                          help_text="Criterion for false chord root-finding method.")
            widget_number("Depress. rate threshold", adv + ["despressRate"], default=0.01,
                          unit="kgf/(cm².s)", help_text="Depressurization rate below which special handling activates.")
        with c2:
            widget_bool("Simple avg press at boundary", adv + ["simplePressureFrontier"], default=True,
                        help_text="If true, uses simple average pressure at boundary cells.")
            widget_bool("Relax DT at choke", adv + ["relaxChokeTimestep"],
                        help_text="Relaxes time step control at choke vicinity.")
            widget_number("Perm. convergence", adv + ["steadyConvergenceCriterion"], default=0.001,
                          help_text="Convergence criterion for steady-state solver.")
        with c3:
            widget_bool("Accelerate perm. convergence", adv + ["accelerateSteadyConvergence"], default=True,
                        help_text="If true, uses acceleration techniques for steady-state convergence.")
            widget_bool("Slip at boundary cell", adv + ["slipBoundaryCell"],
                        default=True, help_text="If true, considers slip at boundary cells.")
            widget_bool("Counter-current corr. (perm)", adv + ["counterflowCorrectionSteady"],
                        help_text="Counter-current correction for steady-state solver.")
            widget_bool("Stable column", adv + ["columnStabilization"],
                        help_text="Stabilization feature for liquid column.")
            widget_int("Threads", adv + ["threads"], default=1, min_val=1,
                       help_text="Number of computational threads.")
            widget_int("Matrix threads", adv + ["matrixThreads"], default=1, min_val=1,
                       help_text="Number of threads for matrix operations.")
            widget_int_select("Area change losses", adv + ["areaChange"], {
                0: "0 - Off", 1: "1 - On"
            }, default=0, help_text="If 1, accounts for pressure losses at area changes.")

        c1b, c2b, c3b = st.columns(3)
        with c1b:
            widget_bool("Correct separator BC", adv + ["correctSepCondition"], default=True,
                        help_text="Corrects comparison between last-cell pressure and separator pressure.")
            widget_bool("Disable DT penalization", adv + ["disablePenalizeTimestep"],
                        help_text="Disables DT penalization logic.")
            widget_bool("DT valve control", adv + ["valveTimestepControl"],
                        help_text="Time step control at valve vicinity.")
            widget_bool("Disable dRho/dT in mass transfer", adv + ["disableMassTransferTempDeriv"],
                        help_text="Disables density-temperature derivative in mass transfer.")
        with c2b:
            widget_int("MiniTab delay (steps)", adv + ["dynTableMinDelay"], default=0, min_val=0,
                       help_text="Every N steps, build mini flash table per cell (composit. transient).")
            widget_number("MiniTab ΔP [kgf/cm²]", adv + ["dynTableMinDp"], default=0.0,
                          help_text="Pressure increment for mini flash table generation.")
            widget_number("MiniTab ΔT [°C]", adv + ["dynTableMinDt"], default=0.0,
                          help_text="Temperature increment for mini flash table generation.")
            widget_number("Min DT criterion", adv + ["minTimestepCriterion"],
                          help_text="Minimum time step criterion.")
            widget_number("JT Liquid Simple", adv + ["liquidJTSimple"],
                          help_text="Simplified Joule-Thomson coefficient for liquid.")
        with c3b:
            st.caption("Complete model time switching")
            _tcmc = get_val(adv + ["compModelCorrectionTime"], [])
            _cmc = get_val(adv + ["compModelCorrectionFlag"], [])
            _tcmc_df = pd.DataFrame({"tempo [s]": _tcmc, "active": _cmc + [None]*(len(_tcmc)-len(_cmc)) if len(_tcmc)>len(_cmc) else _cmc[:len(_tcmc)]})
            _tcmc_ed = st.data_editor(_tcmc_df, num_rows="dynamic", use_container_width=True, key="adv_tcmc")
            _tcmc_list = _tcmc_ed["tempo [s]"].dropna().tolist()
            _cmc_list = _tcmc_ed["active"].dropna().tolist()
            if _path_exists(adv + ["compModelCorrectionTime"]) or _tcmc_list:
                set_val(adv + ["compModelCorrectionTime"], _tcmc_list)
            if _path_exists(adv + ["compModelCorrectionFlag"]) or _cmc_list:
                set_val(adv + ["compModelCorrectionFlag"], [bool(v) for v in _cmc_list])

            st.caption("Sonic timestep switching")
            _tson = get_val(adv + ["sonicTime"], [])
            _son = get_val(adv + ["sonicFlag"], [])
            _tson_df = pd.DataFrame({"tempo [s]": _tson, "sonic": _son + [None]*(len(_tson)-len(_son)) if len(_tson)>len(_son) else _son[:len(_tson)]})
            _tson_ed = st.data_editor(_tson_df, num_rows="dynamic", use_container_width=True, key="adv_tson")
            _tson_list = _tson_ed["tempo [s]"].dropna().tolist()
            _son_list = _tson_ed["sonic"].dropna().tolist()
            if _path_exists(adv + ["sonicTime"]) or _tson_list:
                set_val(adv + ["sonicTime"], _tson_list)
            if _path_exists(adv + ["sonicFlag"]) or _son_list:
                set_val(adv + ["sonicFlag"], [int(v) for v in _son_list])

    with st.expander("🚀 Gas-Lift Discharge"):
        ci_path = ["initialConfig"]
        widget_number("Salinity", ci_path + ["fluidSalinity"],
                      unit="g/kg water", default=0.0,
                      help_text="Completion fluid salinity for gas-lift discharge.")
        c1, c2 = st.columns(2)
        with c1:
            widget_number("Interface position - gas line [m]",
                          ci_path + ["gasLineInterfaceLength"],
                          help_text="Initial gas/completion fluid interface position on the service line (from platform).")
            widget_number("Interface position - prod line [m]",
                          ci_path + ["prodLineInterfaceLength"],
                          help_text="Initial gas/completion fluid interface position on the production line (from platform).")
        with c2:
            widget_bool("Automated discharge control", ci_path + ["dischargeControl"],
                        help_text="If true, automatically controls gas injection to avoid erosional velocities during discharge.")

        _ctrl_descarga = get_val(ci_path + ["dischargeControl"], False)
        if _ctrl_descarga:
            st.markdown("**Discharge Control Parameters**")
            pd_path = ci_path + ["dischargeParameters"]
            c1, c2, c3 = st.columns(3)
            with c1:
                widget_number("Max liquid flow at VGL [sm³/d]",
                              pd_path + ["maxDischargeFlowRate"],
                              help_text="Maximum liquid flow rate at the valve (erosional limit).")
                widget_number("Max prod choke pressure [kgf/cm²]",
                              pd_path + ["maxDischargePressure"],
                              help_text="Maximum downstream pressure at production choke during discharge.")
                widget_number("Min prod choke pressure [kgf/cm²]",
                              pd_path + ["minDischargePressure"],
                              help_text="Minimum downstream pressure at production choke during discharge.")
            with c2:
                widget_number("Gas discharge start P [kgf/cm²]",
                              pd_path + ["workGasChargePressure"],
                              help_text="Downstream prod choke pressure at which discharge starts.")
                widget_number("Max injection P [kgf/cm²]",
                              pd_path + ["maxGasChargePressure"],
                              help_text="Maximum injection pressure during discharge control.")
                widget_number("Min injection P [kgf/cm²]",
                              pd_path + ["minGasChargePressure"],
                              help_text="Minimum injection pressure during discharge control.")
            with c3:
                widget_number("Initial gas line P [kgf/cm²]",
                              pd_path + ["initialGasDischargePressure"],
                              help_text="Initial pressure in the gas line when discharge starts.")
                widget_number("Discharge temperature [°C]",
                              pd_path + ["dischargeTemperature"],
                              help_text="Temperature at which discharge is performed.")
                widget_number("Latency time [s]",
                              pd_path + ["latencyTime"],
                              help_text="Time between simulation stabilization and start of discharge.")

    with st.expander("📏 Correction Factors"):
        widget_bool("Active", ["correction", "active"], default=True,
                    help_text="Enable correction factors for dP/dL and dT/dL.")
        st.markdown("**Hydrostatic dP/dL multipliers**")
        _corr_hidro = get_val(["correction", "correctionFactorHydro"], [])
        _corr_hidro_str = st.text_input(
            "dPdLHidro (comma-sep multipliers)",
            value=", ".join(str(x) for x in _corr_hidro) if _corr_hidro else "",
            key=f"corr_hidro_v{st.session_state.get('data_version', 0)}",
            help="List of hydrostatic pressure drop correction multipliers. Referenced by index in duct settings."
        )
        try:
            _corr_hidro_list = [float(x.strip()) for x in _corr_hidro_str.split(",") if x.strip()]
            if _path_exists(["correction", "correctionFactorHydro"]) or _corr_hidro_list:
                set_val(["correction", "correctionFactorHydro"], _corr_hidro_list)
        except ValueError:
            pass

        st.markdown("**Friction dP/dL multipliers**")
        _corr_fric = get_val(["correction", "correctionFactorFric"], [])
        _corr_fric_str = st.text_input(
            "dPdLFric (comma-sep multipliers)",
            value=", ".join(str(x) for x in _corr_fric) if _corr_fric else "",
            key=f"corr_fric_v{st.session_state.get('data_version', 0)}",
            help="List of friction pressure drop correction multipliers. Referenced by index in duct settings."
        )
        try:
            _corr_fric_list = [float(x.strip()) for x in _corr_fric_str.split(",") if x.strip()]
            if _path_exists(["correction", "correctionFactorFric"]) or _corr_fric_list:
                set_val(["correction", "correctionFactorFric"], _corr_fric_list)
        except ValueError:
            pass

        st.markdown("**Temperature gradient dT/dL multipliers**")
        _corr_dt = get_val(["correction", "correctionFactorTemp"], [])
        _corr_dt_str = st.text_input(
            "dTdL (comma-sep multipliers)",
            value=", ".join(str(x) for x in _corr_dt) if _corr_dt else "",
            key=f"corr_dt_v{st.session_state.get('data_version', 0)}",
            help="List of temperature gradient correction multipliers. Referenced by index in duct settings."
        )
        try:
            _corr_dt_list = [float(x.strip()) for x in _corr_dt_str.split(",") if x.strip()]
            if _path_exists(["correction", "correctionFactorTemp"]) or _corr_dt_list:
                set_val(["correction", "correctionFactorTemp"], _corr_dt_list)
        except ValueError:
            pass

    with st.expander("📊 Output Profiles"):
        st.caption("Select which variables to record as profiles along the production line")
        widget_bool("Active", ["productionProfile", "active"], default=True,
                    help_text="Enable production profile recording.")

        # Profile time instants (transient)
        _pf_tempos = get_val(["productionProfile", "time"], [])
        _pf_tempos_str = st.text_input(
            "Profile times [s] (comma-sep, transient only)",
            value=", ".join(str(x) for x in _pf_tempos) if _pf_tempos else "",
            key=f"perfil_tempos_v{st.session_state.get('data_version', 0)}",
            help="Time instants at which profiles are recorded."
        )
        try:
            _pf_t_list = [float(x.strip()) for x in _pf_tempos_str.split(",") if x.strip()]
            if _path_exists(["productionProfile", "time"]) or _pf_t_list:
                set_val(["productionProfile", "time"], _pf_t_list)
        except ValueError:
            pass

        st.markdown("**Variables to record:**")
        _profile_vars = [
            "pressure", "temperature", "holdup", "complementaryFluidFraction", "usg", "usl", "ug", "ul",
            "flowPattern", "liquidViscosity", "gasViscosity", "gasInSituDensity", "liquidInSituDensity", "mixtureDensity",
            "SR", "gasMassFlowRate", "liquidMassFlowRate", "c0", "driftVelocity",
            "gasOilRatio", "gasStdDensity", "yCO2", "heat", "massTransferRate", "cpGas", "cpLiquid",
            "stdLiqFlowRate", "stdLiqWaterFlowRate", "stdTotalLiqFlowRate", "stdGasFlowRate", "api", "bsw",
            "hydrostaticPressureGradient", "frictionPressureGradient", "term1", "term2", "dissolvedGasDensity", "freeGasDensity",
            "internalReynolds", "externalReynolds", "internalGrashof", "externalGrashof",
            "internalNusselt", "externalNusselt", "internalPrandtl", "externalPrandtl",
            "froude", "formationVolumeFactor", "internalHeatCoef", "externalHeatCoef", "ambientTemperature",
            "gasPrandtl", "liquidPrandtl",
            "pseudoLiquid", "pseudoGas", "pseudoMist",
            "residenceTime", "frictionReducer", "angle",
        ]
        # Show as multiselect for convenience
        _current_active = [v for v in _profile_vars
                           if get_val(["productionProfile", v], False)]
        _selected_vars = st.multiselect(
            "Profile variables", _profile_vars, default=_current_active,
            key=f"perfil_vars_v{st.session_state.get('data_version', 0)}",
            help="Select variables to include in the output profiles."
        )
        for _pv in _profile_vars:
            _pv_val = _pv in _selected_vars
            if _path_exists(["productionProfile", _pv]) or _pv_val:
                set_val(["productionProfile", _pv], _pv_val)

    with st.expander("📈 Output Trends"):
        st.caption("Configure trend (time-series) recording points on the production line")
        _tendp = get_val(["productionTrend"], [])
        if not _tendp:
            _tendp = []

        col_add_t, _ = st.columns([1, 3])
        with col_add_t:
            if st.button("➕ Add Trend Point", key="add_tendp"):
                _tendp.append({"active": True, "measuredLength": 0.0, "dt": 1.0,
                               "label": f"Trend {len(_tendp)}",
                               "pressure": True, "temperature": True})
                set_val(["productionTrend"], _tendp)
                st.rerun()

        _tend_vars = [
            "pressure", "temperature", "holdup", "complementaryFluidFraction", "usg", "usl", "ug", "ul",
            "flowPattern", "liquidViscosity", "gasViscosity", "gasInSituDensity", "liquidInSituDensity",
            "mixtureDensity", "SR", "gasMassFlowRate", "liquidMassFlowRate", "c0", "driftVelocity",
            "gasOilRatio", "gasStdDensity", "yCO2", "heat", "massTransferRate", "cpGas", "cpLiquid",
            "stdLiqFlowRate", "stdLiqWaterFlowRate", "stdTotalLiqFlowRate", "stdGasFlowRate", "api", "bsw",
            "hydrostaticPressureGradient", "frictionPressureGradient", "dissolvedGasDensity", "freeGasDensity",
            "internalReynolds", "externalReynolds", "internalGrashof", "externalGrashof",
            "internalNusselt", "externalNusselt", "internalPrandtl", "externalPrandtl",
            "froude", "formationVolumeFactor", "internalHeatCoef", "externalHeatCoef", "ambientTemperature", "angle",
            "sourceLiqMassFlow", "sourceGasMassFlow", "sourceCompMassFlow", "chokeDownstreamTemp",
            "pumpDeltaP", "pumpPower",
        ]

        for _ti, _tp in enumerate(_tendp):
            with st.container():
                _tc1, _tc2, _tc3, _tc4 = st.columns([2, 2, 2, 1])
                with _tc1:
                    _tendp[_ti]["active"] = st.checkbox(
                        "Active", value=_tp.get("active", True),
                        key=f"tendp_act_{_ti}_v{st.session_state.get('data_version', 0)}")
                    _tendp[_ti]["label"] = st.text_input(
                        "Label", value=_tp.get("label", ""),
                        key=f"tendp_lbl_{_ti}_v{st.session_state.get('data_version', 0)}")
                with _tc2:
                    _tendp[_ti]["measuredLength"] = st.number_input(
                        "Position [m]", value=float(_tp.get("measuredLength", 0)),
                        format="%g", key=f"tendp_pos_{_ti}_v{st.session_state.get('data_version', 0)}")
                with _tc3:
                    _tendp[_ti]["dt"] = st.number_input(
                        "Δt [s]", value=float(_tp.get("dt", 1.0)), min_value=0.001,
                        format="%g", key=f"tendp_dt_{_ti}_v{st.session_state.get('data_version', 0)}")
                with _tc4:
                    if st.button("🗑️", key=f"del_tendp_{_ti}"):
                        _tendp.pop(_ti)
                        set_val(["productionTrend"], _tendp)
                        st.rerun()

                _tp_current = [v for v in _tend_vars if _tp.get(v, False)]
                _tp_sel = st.multiselect(
                    "Variables", _tend_vars, default=_tp_current,
                    key=f"tendp_vars_{_ti}_v{st.session_state.get('data_version', 0)}"
                )
                for _tv in _tend_vars:
                    _tendp[_ti][_tv] = _tv in _tp_sel
                st.markdown("---")

        set_val(["productionTrend"], _tendp)

    # ── Service Line Profile Output (perfilServico) ──────────────────────────
    with st.expander("📊 Service Line Profile Output (perfilServico)"):
        st.caption("Select which variables to record as profiles along the service line")
        widget_bool("Active", ["serviceProfile", "active"], default=True,
                    help_text="Enable service line profile recording.")

        # Profile time instants (transient)
        _ps_tempos = get_val(["serviceProfile", "time"], [])
        _ps_tempos_str = st.text_input(
            "Profile times [s] (comma-sep, transient only)",
            value=", ".join(str(x) for x in _ps_tempos) if _ps_tempos else "",
            key=f"perfil_serv_tempos_v{st.session_state.get('data_version', 0)}",
            help="Time instants at which service line profiles are recorded."
        )
        try:
            _ps_t_list = [float(x.strip()) for x in _ps_tempos_str.split(",") if x.strip()]
            if _path_exists(["serviceProfile", "time"]) or _ps_t_list:
                set_val(["serviceProfile", "time"], _ps_t_list)
        except ValueError:
            pass

        st.markdown("**Variables to record:**")
        _profile_serv_vars = [
            "pressure", "temperature", "usg", "ug",
            "shearStress", "gasViscosity", "gasInSituDensity", "gasMassFlowRate",
            "stdGasFlowRate", "frictionPressureGradient", "heat", "hydrostaticPressureGradient",
            "internalReynolds", "externalReynolds", "internalGrashof", "externalGrashof",
            "internalNusselt", "externalNusselt", "internalPrandtl", "externalPrandtl",
            "internalHeatCoef", "externalHeatCoef", "ambientTemperature", "angle",
        ]
        _current_active_s = [v for v in _profile_serv_vars
                             if get_val(["serviceProfile", v], False)]
        _selected_vars_s = st.multiselect(
            "Service profile variables", _profile_serv_vars, default=_current_active_s,
            key=f"perfil_serv_vars_v{st.session_state.get('data_version', 0)}",
            help="Select variables to include in the service line output profiles."
        )
        for _pv in _profile_serv_vars:
            _pv_val = _pv in _selected_vars_s
            if _path_exists(["serviceProfile", _pv]) or _pv_val:
                set_val(["serviceProfile", _pv], _pv_val)

    # ── Service Line Trends (tendS) ──────────────────────────────────────────
    with st.expander("📈 Service Line Trends (tendS)"):
        st.caption("Configure trend (time-series) recording points on the service line")
        _tends = get_val(["serviceTrend"], [])
        if not _tends:
            _tends = []

        col_add_ts, _ = st.columns([1, 3])
        with col_add_ts:
            if st.button("➕ Add Service Trend Point", key="add_tends"):
                _tends.append({"active": True, "measuredLength": 0.0, "dt": 1.0,
                               "label": f"Trend S{len(_tends)}",
                               "pressure": True, "temperature": True})
                set_val(["serviceTrend"], _tends)
                st.rerun()

        _tend_serv_vars = [
            "pressure", "temperature", "usg", "ug",
            "shearStress", "gasViscosity", "gasInSituDensity", "gasMassFlowRate",
            "hydrostaticPressureGradient", "frictionPressureGradient", "heat", "stdGasFlowRate",
            "vglStagePressure", "vglStageTemp", "vglThroatPressure", "vglThroatTemp",
            "vglFlowRate", "maxThroatVelocity",
            "internalReynolds", "externalReynolds", "internalGrashof", "externalGrashof",
            "internalNusselt", "externalNusselt", "internalPrandtl", "externalPrandtl",
            "froude", "internalHeatCoef", "externalHeatCoef",
        ]

        for _si, _sp in enumerate(_tends):
            with st.container():
                _sc1, _sc2, _sc3, _sc4 = st.columns([2, 2, 2, 1])
                with _sc1:
                    _tends[_si]["active"] = st.checkbox(
                        "Active", value=_sp.get("active", True),
                        key=f"tends_act_{_si}_v{st.session_state.get('data_version', 0)}")
                    _tends[_si]["label"] = st.text_input(
                        "Label", value=_sp.get("label", ""),
                        key=f"tends_lbl_{_si}_v{st.session_state.get('data_version', 0)}")
                with _sc2:
                    _tends[_si]["measuredLength"] = st.number_input(
                        "Position [m]", value=float(_sp.get("measuredLength", 0)),
                        format="%g", key=f"tends_pos_{_si}_v{st.session_state.get('data_version', 0)}")
                with _sc3:
                    _tends[_si]["dt"] = st.number_input(
                        "Δt [s]", value=float(_sp.get("dt", 1.0)), min_value=0.001,
                        format="%g", key=f"tends_dt_{_si}_v{st.session_state.get('data_version', 0)}")
                with _sc4:
                    if st.button("🗑️", key=f"del_tends_{_si}"):
                        _tends.pop(_si)
                        set_val(["serviceTrend"], _tends)
                        st.rerun()

                _sp_current = [v for v in _tend_serv_vars if _sp.get(v, False)]
                _sp_sel = st.multiselect(
                    "Variables", _tend_serv_vars, default=_sp_current,
                    key=f"tends_vars_{_si}_v{st.session_state.get('data_version', 0)}"
                )
                for _sv in _tend_serv_vars:
                    _tends[_si][_sv] = _sv in _sp_sel
                st.markdown("---")

        set_val(["serviceTrend"], _tends)

    # ── Transverse Temperature Profiles (Production) ─────────────────────────
    with st.expander("🌡️ Transverse Profiles – Production (perfisTransP)"):
        st.caption("Radial temperature profiles at specific cross-sections on the production line")
        widget_bool("Active", ["crossProductionProfile", "active"], default=True,
                    help_text="Enable transverse profile recording (production).")

        st.markdown("**Times [s]** – instants at which profiles are recorded (transient only)")
        _ptp_tempos = get_val(["crossProductionProfile", "time"], [])
        _ptp_df = pd.DataFrame({"tempo [s]": _ptp_tempos})
        _ptp_edited = st.data_editor(_ptp_df, num_rows="dynamic", use_container_width=True,
                                     key="perfisTransP_tempo")
        _ptp_t_list = _ptp_edited["tempo [s]"].dropna().tolist()
        if _path_exists(["crossProductionProfile", "time"]) or _ptp_t_list:
            set_val(["crossProductionProfile", "time"], _ptp_t_list)

        st.markdown("**Positions [m]** – measured lengths on the production line")
        _ptp_pos = get_val(["crossProductionProfile", "measuredLength"], [])
        _ptp_pos_df = pd.DataFrame({"comprimentoMedido [m]": _ptp_pos})
        _ptp_pos_edited = st.data_editor(_ptp_pos_df, num_rows="dynamic", use_container_width=True,
                                         key="perfisTransP_pos")
        _ptp_pos_list = _ptp_pos_edited["comprimentoMedido [m]"].dropna().tolist()
        if _path_exists(["crossProductionProfile", "measuredLength"]) or _ptp_pos_list:
            set_val(["crossProductionProfile", "measuredLength"], _ptp_pos_list)

    # ── Transverse Temperature Profiles (Service) ────────────────────────────
    with st.expander("🌡️ Transverse Profiles – Service (perfisTransS)"):
        st.caption("Radial temperature profiles at specific cross-sections on the service line")
        widget_bool("Active", ["crossServiceProfile", "active"], default=True,
                    help_text="Enable transverse profile recording (service).")

        st.markdown("**Times [s]** – instants at which profiles are recorded (transient only)")
        _pts_tempos = get_val(["crossServiceProfile", "time"], [])
        _pts_df = pd.DataFrame({"tempo [s]": _pts_tempos})
        _pts_edited = st.data_editor(_pts_df, num_rows="dynamic", use_container_width=True,
                                     key="perfisTransS_tempo")
        _pts_t_list = _pts_edited["tempo [s]"].dropna().tolist()
        if _path_exists(["crossServiceProfile", "time"]) or _pts_t_list:
            set_val(["crossServiceProfile", "time"], _pts_t_list)

        st.markdown("**Positions [m]** – measured lengths on the service line")
        _pts_pos = get_val(["crossServiceProfile", "measuredLength"], [])
        _pts_pos_df = pd.DataFrame({"comprimentoMedido [m]": _pts_pos})
        _pts_pos_edited = st.data_editor(_pts_pos_df, num_rows="dynamic", use_container_width=True,
                                         key="perfisTransS_pos")
        _pts_pos_list = _pts_pos_edited["comprimentoMedido [m]"].dropna().tolist()
        if _path_exists(["crossServiceProfile", "measuredLength"]) or _pts_pos_list:
            set_val(["crossServiceProfile", "measuredLength"], _pts_pos_list)

    # ── Transverse Temperature Trends (Production) ───────────────────────────
    with st.expander("📈 Transverse Trends – Production (tendTransP)"):
        st.caption("Time-series of temperatures at specific cross-section points (production line)")
        _ttp = get_val(["crossProductionTrend"], [])
        if not _ttp:
            _ttp = []

        col_add_ttp, _ = st.columns([1, 3])
        with col_add_ttp:
            if st.button("➕ Add Point", key="add_tendTransP"):
                _ttp.append({"active": True, "measuredLength": 0.0,
                             "layerIndex": 0, "discretization": 0, "dt": 1.0,
                             "label": f"TransP {len(_ttp)}"})
                set_val(["crossProductionTrend"], _ttp)
                st.rerun()

        for _tti, _tt in enumerate(_ttp):
            with st.container():
                _c1, _c2, _c3, _c4, _c5, _c6 = st.columns([1, 2, 1, 1, 2, 0.5])
                with _c1:
                    _ttp[_tti]["active"] = st.checkbox(
                        "Active", value=_tt.get("active", True), key=f"ttp_act_{_tti}")
                with _c2:
                    _ttp[_tti]["label"] = st.text_input(
                        "Label", value=_tt.get("label", ""), key=f"ttp_lbl_{_tti}")
                with _c3:
                    _ttp[_tti]["measuredLength"] = st.number_input(
                        "Pos [m]", value=float(_tt.get("measuredLength", 0)), format="%g", key=f"ttp_pos_{_tti}")
                with _c4:
                    _ttp[_tti]["layerIndex"] = int(st.number_input(
                        "Layer", value=int(_tt.get("layerIndex", 0)), step=1, key=f"ttp_cam_{_tti}"))
                    _ttp[_tti]["discretization"] = int(st.number_input(
                        "Discret.", value=int(_tt.get("discretization", 0)), step=1, key=f"ttp_disc_{_tti}"))
                with _c5:
                    _ttp[_tti]["dt"] = st.number_input(
                        "Δt [s]", value=float(_tt.get("dt", 1.0)), min_value=0.001, format="%g", key=f"ttp_dt_{_tti}")
                with _c6:
                    if st.button("🗑️", key=f"del_ttp_{_tti}"):
                        _ttp.pop(_tti); set_val(["crossProductionTrend"], _ttp); st.rerun()
                st.markdown("---")

        set_val(["crossProductionTrend"], _ttp)

    # ── Transverse Temperature Trends (Service) ──────────────────────────────
    with st.expander("📈 Transverse Trends – Service (tendTransS)"):
        st.caption("Time-series of temperatures at specific cross-section points (service line)")
        _tts = get_val(["crossServiceTrend"], [])
        if not _tts:
            _tts = []

        col_add_tts, _ = st.columns([1, 3])
        with col_add_tts:
            if st.button("➕ Add Point", key="add_tendTransS"):
                _tts.append({"active": True, "measuredLength": 0.0,
                             "layerIndex": 0, "discretization": 0, "dt": 1.0,
                             "label": f"TransS {len(_tts)}"})
                set_val(["crossServiceTrend"], _tts)
                st.rerun()

        for _tsi, _ts in enumerate(_tts):
            with st.container():
                _c1, _c2, _c3, _c4, _c5, _c6 = st.columns([1, 2, 1, 1, 2, 0.5])
                with _c1:
                    _tts[_tsi]["active"] = st.checkbox(
                        "Active", value=_ts.get("active", True), key=f"tts_act_{_tsi}")
                with _c2:
                    _tts[_tsi]["label"] = st.text_input(
                        "Label", value=_ts.get("label", ""), key=f"tts_lbl_{_tsi}")
                with _c3:
                    _tts[_tsi]["measuredLength"] = st.number_input(
                        "Pos [m]", value=float(_ts.get("measuredLength", 0)), format="%g", key=f"tts_pos_{_tsi}")
                with _c4:
                    _tts[_tsi]["layerIndex"] = int(st.number_input(
                        "Layer", value=int(_ts.get("layerIndex", 0)), step=1, key=f"tts_cam_{_tsi}"))
                    _tts[_tsi]["discretization"] = int(st.number_input(
                        "Discret.", value=int(_ts.get("discretization", 0)), step=1, key=f"tts_disc_{_tsi}"))
                with _c5:
                    _tts[_tsi]["dt"] = st.number_input(
                        "Δt [s]", value=float(_ts.get("dt", 1.0)), min_value=0.001, format="%g", key=f"tts_dt_{_tsi}")
                with _c6:
                    if st.button("🗑️", key=f"del_tts_{_tsi}"):
                        _tts.pop(_tsi); set_val(["crossServiceTrend"], _tts); st.rerun()
                st.markdown("---")

        set_val(["crossServiceTrend"], _tts)

    # ── Screen Output Columns (tela) ─────────────────────────────────────────
    with st.expander("🖥️ Screen Output Columns (tela)"):
        st.caption("Columns printed on screen during simulation execution")
        _tela = get_val(["screenConfig"], [])
        if not _tela:
            _tela = []

        _var_prod_labels = {
            1: "1 – Full model indicator",
            2: "2 – Temperature",
            3: "3 – Void fraction",
            4: "4 – Complementary fluid fraction",
            5: "5 – Superficial gas velocity",
            6: "6 – Superficial liquid velocity",
        }
        _var_serv_labels = {
            1: "1 – Pressure",
            2: "2 – Temperature",
            3: "3 – Gas velocity",
        }

        col_add_tela, _ = st.columns([1, 3])
        with col_add_tela:
            if st.button("➕ Add Column", key="add_tela"):
                _tela.append({"active": True, "tubing": True, "cellIndex": 0, "variable": 1})
                set_val(["screenConfig"], _tela)
                st.rerun()

        for _tei, _te in enumerate(_tela):
            with st.container():
                _c1, _c2, _c3, _c4, _c5 = st.columns([0.8, 1.5, 1.2, 2.5, 0.5])
                with _c1:
                    _tela[_tei]["active"] = st.checkbox(
                        "Active", value=_te.get("active", True), key=f"tela_act_{_tei}")
                with _c2:
                    _is_prod = st.selectbox(
                        "Line", ["Production", "Service"],
                        index=0 if _te.get("tubing", True) else 1,
                        key=f"tela_col_{_tei}")
                    _tela[_tei]["tubing"] = (_is_prod == "Production")
                with _c3:
                    _tela[_tei]["cellIndex"] = int(st.number_input(
                        "Cell", value=int(_te.get("cellIndex", 0)), step=1, key=f"tela_cel_{_tei}"))
                with _c4:
                    _vlabels = _var_prod_labels if _tela[_tei]["tubing"] else _var_serv_labels
                    _cur_var = _te.get("variable", 1)
                    if _cur_var not in _vlabels:
                        _cur_var = list(_vlabels.keys())[0]
                    _var_sel = st.selectbox(
                        "Variable", list(_vlabels.keys()),
                        index=list(_vlabels.keys()).index(_cur_var),
                        format_func=lambda x: _vlabels.get(x, str(x)),
                        key=f"tela_var_{_tei}")
                    _tela[_tei]["variable"] = _var_sel
                with _c5:
                    if st.button("🗑️", key=f"del_tela_{_tei}"):
                        _tela.pop(_tei); set_val(["screenConfig"], _tela); st.rerun()
            st.markdown("---")

        set_val(["screenConfig"], _tela)


# ═══════════════════════════════════════════════════════════════════════════════
# TAB 1: FLUIDS
# ═══════════════════════════════════════════════════════════════════════════════
with tabs[0]:
    st.header("Fluids")
    st.caption("Configure production fluids, PVT models, and complementary/gas fluid properties")

    ci_path = ["initialConfig"]
    with st.expander("🧪 Fluid Model", expanded=True):
        c1, c2 = st.columns(2)
        with c1:
            widget_bool("Compositional model", ci_path + ["compositionalFluidModel"],
                        help_text="If true, uses compositional models. Only considered when flash table is disabled.")
            widget_bool("Flash table model", ci_path + ["flashTableFluidModel"],
                        help_text="If true, uses PVT flash table for fluid modeling instead of black oil or compositional.")
            widget_bool("Dynamic table model", ci_path + ["dynamicTableModel"],
                        help_text="Post-simulation table from compositional model, reusing BO P/T mapping. Steady-state only. Can significantly speed up network simulations.")
            widget_bool("Latent heat from PVTSim", ci_path + ["latentHeat"],
                        help_text="If true, uses PVTSim table interpolation for enthalpy. Requires PVTSim file.")
        with c2:
            widget_int_select("Fluid type", ci_path + ["fluidType"], {
                0: "0 - Liquid dominated (BO)",
                1: "1 - Gas dominated (title ratio)",
            }, default=0, help_text="0: mass transfer via black oil logic; 1: mass transfer via title ratio (gas-dominated systems).")
            widget_int_select("Cp model", ci_path + ["cpModel"], {
                0: "0 - Black oil",
                1: "1 - PVTSim table interpolation",
            }, default=0, help_text="Specific heat calculation method for black oil fluids.")
            widget_int_select("JTL model", ci_path + ["jtlModel"], {
                0: "0 - Standard",
                1: "1 - d(rho_l)/dT from PVTSim",
            }, default=0, help_text="If 1, uses d(rho_l)/dT from PVTSim table even in black oil mode.")
            # PVTSim file dropdown from uploaded auxiliary files
            _pvt_files = [f for f in os.listdir(st.session_state["work_dir"])
                          if f.lower().endswith((".tab", ".ctm"))]
            _cur_pvt = get_val(ci_path + ["pvtFile"], "")
            if _cur_pvt and _cur_pvt not in _pvt_files:
                _pvt_files.insert(0, _cur_pvt)
            _pvt_options = [""] + _pvt_files
            _pvt_idx = _pvt_options.index(_cur_pvt) if _cur_pvt in _pvt_options else 0
            _pvt_sel = st.selectbox("PVTSim file", _pvt_options, index=_pvt_idx,
                                    help=".tab or .ctm file (upload in sidebar)",
                                    key=f"pvtsim_file_select_v{st.session_state.get('data_version', 0)}")
            if _path_exists(ci_path + ["pvtFile"]) or _pvt_sel:
                set_val(ci_path + ["pvtFile"], _pvt_sel if _pvt_sel else None)
            widget_bool("Table P (compress. table)", ci_path + ["pressureTable"])
            widget_bool("Table G (gas compress.)", ci_path + ["gasTable"])

    # --- Production Fluids ---
    with st.expander("🛢️ Production Fluids", expanded=True):
        fluids = get_val(["productionFluid"], [])
        if not fluids:
            fluids = []

        n_fluids = len(fluids)
        st.caption(f"{n_fluids} fluid(s) defined")

        col_add, _ = st.columns([1, 3])
        with col_add:
            if st.button("➕ Add Fluid", key="add_fluid"):
                fluids.append({"active": True, "id": n_fluids, "api": 28.0, "gor": 100.0,
                               "gasDensity": 0.7, "bsw": 0.0})
                set_val(["productionFluid"], fluids)
                st.rerun()

        # Render cards in a grid (3 per row)
        for row_start in range(0, n_fluids, 3):
            cols = st.columns(3)
            for col_idx, i in enumerate(range(row_start, min(row_start + 3, n_fluids))):
                fluid = fluids[i]
                with cols[col_idx]:
                    active = fluid.get("active", True)
                    _ftitle = fluid.get("label", f"Fluid {i}")
                    render_card(
                        title=_ftitle,
                        id_number=fluid.get('id', i),
                        details=[
                            f"API: {fluid.get('api', '—')} &nbsp;|&nbsp; GOR: {fluid.get('gor', '—')}",
                            f"Gas dens: {fluid.get('gasDensity', '—')} &nbsp;|&nbsp; BSW: {fluid.get('bsw', '—')}",
                        ],
                        active=active,
                        icon="🛢️",
                    )
                    bc1, bc2 = st.columns(2)
                    with bc1:
                        if st.button("✏️", key=f"edit_fluid_{i}", use_container_width=True):
                            st.session_state.pop("_editing_material", None)
                            st.session_state.pop("_editing_section", None)
                            st.session_state.pop("_editing_pipe", None)
                            st.session_state["_editing_fluid"] = i
                            st.rerun()
                    with bc2:
                        if st.button("🗑️", key=f"del_fluid_{i}", use_container_width=True):
                            fluids.pop(i)
                            set_val(["productionFluid"], fluids)
                            st.rerun()

        # Open dialog if editing (only one dialog at a time)
        if "_editing_fluid" in st.session_state:
            st.session_state.pop("_editing_material", None)
            st.session_state.pop("_editing_section", None)
            st.session_state.pop("_editing_pipe", None)
            st.session_state.pop("_editing_formation", None)
            _fluid_dialog(st.session_state.pop("_editing_fluid"))

    # --- Gas Fluid ---
    with st.expander("💨 Gas Fluid"):
        widget_bool("Active", ["gasFluid", "active"], default=True,
                    help_text="Gas fluid for service line or dry gas source.")
        c1, c2, c3 = st.columns(3)
        with c1:
            widget_number("Gas density (rel.)", ["gasFluid", "gasDensity"], default=0.7,
                          help_text="Relative to air at standard conditions.")
        with c2:
            widget_number("CO2 fraction", ["gasFluid", "CO2Fraction"], default=0.0,
                          help_text="Molar fraction of CO2 in the gas.")
        with c3:
            widget_int_select("Critical correlation", ["gasFluid", "criticalCorrelation"], {
                1: "1 - Brown et al", 2: "2 - Piper et al",
            }, default=1, help_text="Correlation for pseudo-critical P and T. Options 1 & 2 are better for CO2-rich gas.")
        st.markdown("**Compositional Options**")
        widget_bool("User molar fractions",
                    ["gasFluid", "userMolarFraction"], default=False,
                    help_text="If true, uses molarFraction below. If false, reads from .ctm file.")
        st.caption("Molar fractions (same order as .ctm file)")
        frac_molar_gas = get_val(["gasFluid", "molarFraction"], [])
        frac_gas_str = st.text_input(
            "Molar fractions (comma-sep)",
            value=", ".join(str(x) for x in frac_molar_gas) if frac_molar_gas else "",
            key=f"gas_frac_molar_v{st.session_state.get('data_version', 0)}"
        )
        try:
            _frac_list = [float(x.strip()) for x in frac_gas_str.split(",") if x.strip()]
            if _path_exists(["gasFluid", "molarFraction"]) or _frac_list:
                set_val(["gasFluid", "molarFraction"], _frac_list)
        except ValueError:
            pass

    # --- Complementary Fluid ---
    with st.expander("🧪 Complementary Fluid"):
        widget_bool("Active", ["complementaryFluid", "active"], default=True,
                    help_text="Third liquid phase (inert, non-soluble in gas). E.g. glycol, ethanol, inhibitor, completion fluid.")
        c1, c2, c3 = st.columns(3)
        with c1:
            widget_number("Density [kg/m³]", ["complementaryFluid", "density"],
                          help_text="Liquid density at standard conditions.")
            widget_number("Compress. [1/Pa]", ["complementaryFluid", "compressibility"], fmt="%.2e",
                          help_text="Liquid compressibility.")
            widget_number("Thermal exp. [1/K]", ["complementaryFluid", "thermalExpansivity"], fmt="%.2e",
                          help_text="Liquid thermal expansivity.")
        with c2:
            widget_number("Surface tension [N/m]", ["complementaryFluid", "surfaceTension"],
                          help_text="Surface tension of the liquid.")
            widget_number("Specific heat [J/kg·K]", ["complementaryFluid", "specificHeat"],
                          help_text="Specific heat capacity of the liquid.")
            widget_number("Conductivity [W/m·K]", ["complementaryFluid", "conductivity"],
                          help_text="Thermal conductivity of the liquid.")
        with c3:
            widget_number("Temp 1 [°C]", ["complementaryFluid", "temp1"],
                          help_text="Temperature of ASTM viscosity point 1.")
            widget_number("Visc 1 [cP]", ["complementaryFluid", "visc1"],
                          help_text="Viscosity at ASTM point 1.")
            widget_number("Temp 2 [°C]", ["complementaryFluid", "temp2"],
                          help_text="Temperature of ASTM viscosity point 2.")
            widget_number("Visc 2 [cP]", ["complementaryFluid", "visc2"],
                          help_text="Viscosity at ASTM point 2.")
            widget_int_select("Type", ["complementaryFluid", "complementaryFluidType"], {
                0: "0 - Standard", 1: "1 - Water (salinity only)", 2: "2 - Analog (drag reducer)"
            }, default=0, help_text="0: requires all properties; 1: only salinity needed; 2: like 0 but computes drag reducer internally.")
        widget_number("Salinity [g/kg water]", ["complementaryFluid", "salinity"],
                      help_text="Required when type is 1 (Water).")

    # --- Wax Deposition ---
    with st.expander("🕯️ Wax Deposition"):
        st.caption("Wax (paraffin) deposition model parameters.")
        _wax_path = ["wax"]

        widget_bool("Wax model active", ["initialConfig", "waxMode"],
                    help_text="If true, activates wax deposition model.")

        # Wax file dropdown
        _wax_files = [f for f in os.listdir(st.session_state["work_dir"])
                      if f.lower().endswith((".wax", ".tab", ".txt", ".json"))]
        _cur_wax = get_val(_wax_path + ["waxFile"], "")
        if _cur_wax and _cur_wax not in _wax_files:
            _wax_files.insert(0, _cur_wax)
        _wax_options = [""] + _wax_files
        _wax_idx = _wax_options.index(_cur_wax) if _cur_wax in _wax_options else 0
        _wax_sel = st.selectbox(
            "Wax file", _wax_options, index=_wax_idx,
            help="Wax properties file (upload in sidebar).",
            key="wax_file_sel")
        if _path_exists(_wax_path + ["waxFile"]) or _wax_sel:
            set_val(_wax_path + ["waxFile"], _wax_sel if _wax_sel else None)

        c1, c2, c3 = st.columns(3)
        with c1:
            widget_bool("User porosity", _wax_path + ["userPorosity"],
                        help_text="If true, uses user-defined porosity instead of default.")
            widget_number("Porosity", _wax_path + ["porosity"],
                          help_text="User-defined deposit porosity (valid when userPorosity=true).")
            widget_bool("User C2/C3", _wax_path + ["userC2C3"],
                        help_text="If true, uses user-defined C2 and C3 constants.")
            widget_number("C2", _wax_path + ["c2"],
                          help_text="C2 constant (valid when userC2C3=true).")
            widget_number("C3", _wax_path + ["c3"],
                          help_text="C3 constant (valid when userC2C3=true).")
        with c2:
            widget_bool("User diffusivity", _wax_path + ["userDiffusion"],
                        help_text="If true, uses user-defined diffusivity.")
            widget_number("Diffusivity", _wax_path + ["diffusivity"],
                          help_text="User-defined diffusivity (valid when userDiffusion=true).")
            widget_bool("Alter fluid viscosity", _wax_path + ["changeFluidVisc"],
                        help_text="If true, wax deposition modifies the fluid viscosity.")
            widget_number("Roughness [m]", _wax_path + ["roughness"], unit="m",
                          help_text="Surface roughness for wax deposit layer.")
        with c3:
            widget_number("Viscosity multiplier", _wax_path + ["viscMultiplier"],
                          help_text="Multiplier applied to viscosity due to wax.")
            widget_number("D multipWax", _wax_path + ["waxMultD"],
                          help_text="D parameter for wax multiplier correlation.")
            widget_number("E multipWax", _wax_path + ["waxMultE"],
                          help_text="E parameter for wax multiplier correlation.")
            widget_number("F multipWax", _wax_path + ["waxMultF"],
                          help_text="F parameter for wax multiplier correlation.")

    # --- Compressibility Table ---
    with st.expander("📊 Compressibility Table Parameters"):
        widget_bool("Active", ["compTable", "active"], default=True,
                    help_text="Parameters for compressibility tables (when requested in initialConfig).")
        c1, c2, c3 = st.columns(3)
        with c1:
            widget_int("N points", ["compTable", "numPoints"], default=50, min_val=2,
                       help_text="Number of P and T points in the table grid.")
        with c2:
            widget_number("P min [kgf/cm²]", ["compTable", "minPressure"],
                          help_text="Minimum pressure in the table.")
            widget_number("P max [kgf/cm²]", ["compTable", "maxPressure"],
                          help_text="Maximum pressure in the table.")
        with c3:
            widget_number("T min [°C]", ["compTable", "minTemperature"],
                          help_text="Minimum temperature in the table.")
            widget_number("T max [°C]", ["compTable", "maxTemperature"],
                          help_text="Maximum temperature in the table.")


# ═══════════════════════════════════════════════════════════════════════════════
# TAB 2: MATERIALS
# ═══════════════════════════════════════════════════════════════════════════════
with tabs[1]:
    st.header("Materials")
    st.caption("Define materials for pipe wall layers (steel, insulation, concrete, etc.)")

    materials = get_val(["material"], [])
    if not materials:
        materials = []

    _type_labels = {0: "Solid", 1: "Fluid (user)", 2: "Water", 3: "Air"}

    col_add, _ = st.columns([1, 3])
    with col_add:
        if st.button("➕ Add Material", key="add_mat"):
            materials.append({"active": True, "id": len(materials), "type": 0,
                              "conductivity": 50.0, "specificHeat": 500.0, "rho": 7800.0})
            set_val(["material"], materials)
            st.rerun()

    # Render cards in a grid (4 per row)
    n_mats = len(materials)
    for row_start in range(0, n_mats, 4):
        cols = st.columns(4)
        for col_idx, i in enumerate(range(row_start, min(row_start + 4, n_mats))):
            mat = materials[i]
            with cols[col_idx]:
                _mtitle = mat.get("label", f"Material {i}")
                render_card(
                    title=_mtitle,
                    id_number=mat.get('id', i),
                    details=[
                        f"Type: {_type_labels.get(mat.get('type', 0), '?')}",
                        f"k={mat.get('conductivity', '—')} &nbsp; Cp={mat.get('specificHeat', '—')} &nbsp; ρ={mat.get('rho', '—')}",
                    ],
                    active=mat.get("active", True),
                    icon="🧱",
                )
                bc1, bc2 = st.columns(2)
                with bc1:
                    if st.button("✏️", key=f"edit_mat_{i}", use_container_width=True):
                        st.session_state.pop("_editing_fluid", None)
                        st.session_state.pop("_editing_section", None)
                        st.session_state.pop("_editing_pipe", None)
                        st.session_state["_editing_material"] = i
                        st.rerun()
                with bc2:
                    if st.button("🗑️", key=f"del_mat_{i}", use_container_width=True):
                        materials.pop(i)
                        set_val(["material"], materials)
                        st.rerun()

    # Open dialog if editing (only one dialog at a time)
    if "_editing_material" in st.session_state:
        st.session_state.pop("_editing_fluid", None)
        st.session_state.pop("_editing_section", None)
        st.session_state.pop("_editing_pipe", None)
        st.session_state.pop("_editing_formation", None)
        _material_dialog(st.session_state.pop("_editing_material"))


# ═══════════════════════════════════════════════════════════════════════════════
# TAB 3: CROSS SECTIONS
# ═══════════════════════════════════════════════════════════════════════════════
with tabs[2]:
    st.header("Cross Sections")
    st.caption("Define pipe cross-sections with material layers (inner to outer)")

    sections = get_val(["crossSection"], [])
    if not sections:
        sections = []

    col_add, _ = st.columns([1, 3])
    with col_add:
        if st.button("➕ Add Cross Section", key="add_sec"):
            sections.append({
                "active": True, "id": len(sections), "annular": False,
                "innerDiameter": 0.1, "roughness": 0.00005,
                "layers": [{"layerMeasurementType": "THICKNESS", "thickness": 0.01,
                             "discretization": 1, "materialId": 0}]
            })
            set_val(["crossSection"], sections)
            st.rerun()

    # Render cards in a grid (3 per row)
    n_secs = len(sections)
    for row_start in range(0, n_secs, 3):
        cols = st.columns(3)
        for col_idx, i in enumerate(range(row_start, min(row_start + 3, n_secs))):
            sec = sections[i]
            with cols[col_idx]:
                n_layers = len(sec.get("layers", []))
                ann_label = "Annular" if sec.get("annular") else "Circular"
                _stitle = sec.get("label", f"Section {i}")
                render_card(
                    title=_stitle,
                    id_number=sec.get('id', i),
                    details=[
                        f"D<sub>int</sub>: {sec.get('innerDiameter', '—')} m &nbsp;|&nbsp; {ann_label}",
                        f"Layers: {n_layers} &nbsp;|&nbsp; Roughness: {sec.get('roughness', '—')}",
                    ],
                    active=sec.get("active", True),
                    icon="⭕",
                )
                bc1, bc2 = st.columns(2)
                with bc1:
                    if st.button("✏️", key=f"edit_sec_{i}", use_container_width=True):
                        st.session_state.pop("_editing_fluid", None)
                        st.session_state.pop("_editing_material", None)
                        st.session_state.pop("_editing_pipe", None)
                        st.session_state["_editing_section"] = i
                        st.rerun()
                with bc2:
                    if st.button("🗑️", key=f"del_sec_{i}", use_container_width=True):
                        sections.pop(i)
                        set_val(["crossSection"], sections)
                        st.rerun()

    # Open dialog if editing (only one dialog at a time)
    if "_editing_section" in st.session_state:
        st.session_state.pop("_editing_fluid", None)
        st.session_state.pop("_editing_material", None)
        st.session_state.pop("_editing_pipe", None)
        st.session_state.pop("_editing_formation", None)
        _section_dialog(st.session_state.pop("_editing_section"))


# ═══════════════════════════════════════════════════════════════════════════════
# TAB 4: ROCK FORMATION
# ═══════════════════════════════════════════════════════════════════════════════
with tabs[3]:
    st.header("Rock Formation")
    st.caption("Define rock lithology properties for heat exchange with the formation")

    # Only ensure structure if initialConfig already exists
    if _path_exists(["initialConfig"]) and get_val(["initialConfig", "formation"], None) is None:
        set_val(["initialConfig", "formation"], {"properties": [], "productionTime": 0})
    formation = get_val(["initialConfig", "formation"], {})
    rocks = formation.get("properties", [])

    # Production time (top-level Formacao field)
    widget_number("Production time [days]",
                  ["initialConfig", "formation", "productionTime"],
                  help_text="Important for determining the heating radius of the formation")

    col_add, _ = st.columns([1, 3])
    with col_add:
        if st.button("➕ Add Rock", key="add_rock"):
            rocks.append({"id": len(rocks), "conductivity": 2.0,
                          "specificHeat": 800.0, "density": 2500.0})
            set_val(["initialConfig", "formation", "properties"], rocks)
            st.rerun()

    # Render cards in a grid (4 per row)
    n_rocks = len(rocks)
    for row_start in range(0, n_rocks, 4):
        cols = st.columns(4)
        for col_idx, i in enumerate(range(row_start, min(row_start + 4, n_rocks))):
            rock = rocks[i]
            with cols[col_idx]:
                _rtitle = rock.get("label", f"Rock {i}")
                render_card(
                    title=_rtitle,
                    id_number=rock.get('id', i),
                    details=[
                        f"k={rock.get('conductivity', '—')} W/(m·°C)",
                        f"Cp={rock.get('specificHeat', '—')} &nbsp; ρ={rock.get('density', '—')}",
                    ],
                    active=True,
                    icon="🪨",
                )
                bc1, bc2 = st.columns(2)
                with bc1:
                    if st.button("✏️", key=f"edit_rock_{i}", use_container_width=True):
                        st.session_state.pop("_editing_fluid", None)
                        st.session_state.pop("_editing_material", None)
                        st.session_state.pop("_editing_section", None)
                        st.session_state.pop("_editing_pipe", None)
                        st.session_state["_editing_formation"] = i
                        st.rerun()
                with bc2:
                    if st.button("🗑️", key=f"del_rock_{i}", use_container_width=True):
                        rocks.pop(i)
                        set_val(["initialConfig", "formation", "properties"], rocks)
                        st.rerun()

    # Open dialog if editing
    if "_editing_formation" in st.session_state:
        st.session_state.pop("_editing_fluid", None)
        st.session_state.pop("_editing_material", None)
        st.session_state.pop("_editing_section", None)
        st.session_state.pop("_editing_pipe", None)
        _formation_dialog(st.session_state.pop("_editing_formation"))


# ═══════════════════════════════════════════════════════════════════════════════
# TAB 5: PIPES
# ═══════════════════════════════════════════════════════════════════════════════
with tabs[4]:
    st.header("Pipes")
    st.caption("Define production and service line geometry, segments, and initial conditions")

    with st.expander("⚙️ Pipe Settings", expanded=True):
        ci_duct = ["initialConfig"]
        c1, c2 = st.columns(2)
        with c1:
            widget_bool("Gas service line", ci_duct + ["gasLine"],
                        help_text="Enable gas lift / service line")
        with c2:
            widget_bool("XY coordinate mode", ci_duct + ["xyMode"],
                        help_text="Infer inclinations from XY coordinates")

    # ── Geometry Plot (first, reactive to duct data) ─────────────────────
    #st.subheader("📐 Geometry Preview")

    def _build_geometry_figure():
        """Build a plotly figure of production/service duct geometry from current data."""
        import math

        json_data = st.session_state["json_data"]
        config_inicial = json_data.get("initialConfig", {})
        modo_xy = config_inicial.get("xyMode", False)

        ducts_prod = get_val(["productionPipe"], [])
        ducts_serv = get_val(["servicePipe"], [])

        def _get_length(duto):
            agrupamento = duto.get("grouping", True)
            if agrupamento:
                disc = duto.get("discretization", [])
                if not disc:
                    return 0
                return sum(
                    (item.get("numCells", 0) or 0) * (item.get("length", 0) or 0)
                    for item in disc if isinstance(item, dict)
                )
            else:
                dx_celula = duto.get("cellDx", [])
                if not dx_celula:
                    return 0
                return sum(float(x) for x in dx_celula)

        x_prod = [0.0]
        y_prod = [0.0]
        tooltips_prod = []

        for duto in (ducts_prod or []):
            if not duto.get("active", True):
                continue
            if modo_xy and "xCoor" in duto and "yCoor" in duto:
                x_prod.append(float(duto["xCoor"]))
                y_prod.append(float(duto["yCoor"]))
            elif "angle" in duto:
                comprimento = _get_length(duto)
                ang = float(duto["angle"])
                dx = comprimento * math.cos(ang)
                dy = comprimento * math.sin(ang)
                x_prod.append(x_prod[-1] + dx)
                y_prod.append(y_prod[-1] + dy)
            else:
                continue
            tooltips_prod.append(
                f"ID: {duto.get('id', '?')}<br>"
                f"Section: {duto.get('crossSectionId', 'N/A')}"
            )

        # Service ducts start from end of production (platform)
        platform_x = x_prod[-1]
        platform_y = y_prod[-1]
        x_serv = [platform_x]
        y_serv = [platform_y]
        tooltips_serv = []

        serv_list = ducts_serv or []
        for _si, duto in enumerate(serv_list):
            if not duto.get("active", True):
                continue
            if modo_xy and "xCoor" in duto and "yCoor" in duto:
                if _si == 0:
                    prev_x, prev_y = 0.0, 0.0
                else:
                    prev_duto = serv_list[_si - 1]
                    prev_x = float(prev_duto.get("xCoor", 0))
                    prev_y = float(prev_duto.get("yCoor", 0))
                dx = float(duto["xCoor"]) - prev_x
                dy = float(duto["yCoor"]) - prev_y
                x_serv.append(x_serv[-1] + dx)
                y_serv.append(y_serv[-1] + dy)
            elif "angle" in duto:
                comprimento = _get_length(duto)
                ang = float(duto["angle"])
                dx = comprimento * math.cos(ang)
                dy = comprimento * math.sin(ang)
                x_serv.append(x_serv[-1] - dx)
                y_serv.append(y_serv[-1] + dy)
            else:
                continue
            tooltips_serv.append(
                f"ID: {duto.get('id', '?')}<br>"
                f"Section: {duto.get('crossSectionId', 'N/A')}"
            )

        fig = go.Figure()

        if len(x_prod) > 1:
            fig.add_trace(go.Scatter(
                x=x_prod, y=y_prod, mode="lines+markers",
                name="Production", hovertext=tooltips_prod, hoverinfo="text",
                line=dict(color="#39C0E0", width=3),
                marker=dict(size=8, color="#39C0E0")
            ))

        if len(x_serv) > 1:
            fig.add_trace(go.Scatter(
                x=x_serv, y=y_serv, mode="lines+markers",
                name="Service", hovertext=tooltips_serv, hoverinfo="text",
                line=dict(color="#FFA933", width=3),
                marker=dict(size=8, color="#FFA933")
            ))

        fig.update_layout(
            xaxis_title="x (m)", yaxis_title="y (m)",
            xaxis=dict(scaleanchor="y"), yaxis=dict(scaleanchor="x"),
            height=400, margin=dict(t=30, b=30),
        )
        return fig

    prod_ducts = get_val(["productionPipe"], [])
    serv_ducts = get_val(["servicePipe"], [])
    if prod_ducts or serv_ducts:
        fig_geo = _build_geometry_figure()
        st.plotly_chart(fig_geo, use_container_width=True)
    else:
        st.info("Add production or service pipes to see the geometry preview.")

    # ── Duct list (cards) ────────────────────────────────────────────────
    st.markdown("---")
    pipe_type = st.radio("Pipe type", ["Production", "Service"],
                         horizontal=True)
    pipe_key = "productionPipe" if "Production" in pipe_type else "servicePipe"

    pipes = get_val([pipe_key], [])
    if not pipes:
        pipes = []

    col_add, _ = st.columns([1, 3])
    with col_add:
        if st.button("➕ Add Pipe", key=f"add_pipe_{pipe_key}"):
            pipes.append({
                "active": True, "id": len(pipes), "angle": 0.0, "crossSectionId": 0,
                "environment": 0, "grouping": True,
                "discretization": [{"numCells": 10, "length": 100.0}],
                "initialConditions": {"ambientTemp": [4.0], "ambientVel": [0.0]}
            })
            set_val([pipe_key], pipes)
            st.rerun()

    n_pipes = len(pipes)
    st.caption(f"{n_pipes} pipe(s) defined")

    # Render cards in a grid (4 per row)
    for row_start in range(0, n_pipes, 4):
        cols = st.columns(4)
        for col_idx, i in enumerate(range(row_start, min(row_start + 4, n_pipes))):
            pipe = pipes[i]
            with cols[col_idx]:
                # Build summary details
                has_xy = "xCoor" in pipe and "yCoor" in pipe
                if has_xy:
                    geom_info = f"X: {pipe.get('xCoor', 0):.0f} &nbsp; Y: {pipe.get('yCoor', 0):.0f}"
                else:
                    ang = pipe.get("angle", 0)
                    geom_info = f"Angle: {ang:.3f} rad"
                _dtitle = pipe.get("label", f"Pipe {i}")
                render_card(
                    title=_dtitle,
                    id_number=pipe.get('id', i),
                    details=[
                        f"Section: {pipe.get('crossSectionId', '—')} &nbsp;|&nbsp; {geom_info}",
                    ],
                    active=pipe.get("active", True),
                    icon="📏",
                )
                bc1, bc2 = st.columns(2)
                with bc1:
                    if st.button("✏️", key=f"edit_pipe_{pipe_key}_{i}", use_container_width=True):
                        st.session_state.pop("_editing_fluid", None)
                        st.session_state.pop("_editing_material", None)
                        st.session_state.pop("_editing_section", None)
                        st.session_state["_editing_pipe"] = (pipe_key, i)
                        st.rerun()
                with bc2:
                    if st.button("🗑️", key=f"del_pipe_{pipe_key}_{i}", use_container_width=True):
                        pipes.pop(i)
                        set_val([pipe_key], pipes)
                        st.rerun()

    set_val([pipe_key], pipes)

    # Open dialog if editing (only one dialog at a time)
    if "_editing_pipe" in st.session_state:
        st.session_state.pop("_editing_fluid", None)
        st.session_state.pop("_editing_material", None)
        st.session_state.pop("_editing_section", None)
        st.session_state.pop("_editing_formation", None)
        _dk, _di = st.session_state.pop("_editing_pipe")
        _pipe_dialog(_dk, _di)


# ═══════════════════════════════════════════════════════════════════════════════
# TAB 6: ACCESSORIES
# ═══════════════════════════════════════════════════════════════════════════════
with tabs[5]:
    st.header("Accessories")
    st.caption("Configure flow sources, valves, pumps, and pig launchers along the pipeline")

    acc_tabs = st.tabs(["🛢️ Sources", "🔩 Valves", "⚙️ Pumps", "🐷 Pig"])

    # Helper: all accessory editing keys for mutual exclusion
    _ACC_EDIT_KEYS = [
        "_editing_ipr", "_editing_liq_src", "_editing_mass_src",
        "_editing_gas_src", "_editing_fp_src", "_editing_poro_rad",
        "_editing_poro_2d", "_editing_valve",
        "_editing_vgl", "_editing_bcs", "_editing_vol_pump",
        "_editing_delta_p", "_editing_pig",
    ]

    def _pop_all_acc_edit(exclude=None):
        for k in _ACC_EDIT_KEYS:
            if k != exclude:
                st.session_state.pop(k, None)

    # ─────────────────────────────────────────────────────────────────────────
    # SOURCES: ipr, liquidSource, massSource, gasSource, pressureSource, radialPore, 2DPore
    # ─────────────────────────────────────────────────────────────────────────
    with acc_tabs[0]:
        src_tabs = st.tabs(["IPR", "Liquid Source", "Mass Source", "Gas Source",
                            "Pressure Source", "Radial Pore", "2D Pore"])

        # --- IPR ---
        with src_tabs[0]:
            st.subheader("Inflow Performance Relationship")
            iprs = get_val(["ipr"], [])
            if not iprs:
                iprs = []

            col_add, _ = st.columns([1, 3])
            with col_add:
                if st.button("➕ Add IPR", key="add_ipr"):
                    iprs.append({
                        "active": True, "id": len(iprs), "prodFluidId": 0,
                        "measuredLength": 0.0, "iprType": 1,
                        "staticPressureTime": [0], "staticPressure": [250.0],
                        "temperaturesTime": [0], "temperatures": [80.0],
                        "ipTime": [0], "ip": [10.0],
                        "iiTime": [0], "ii": [10.0],
                    })
                    set_val(["ipr"], iprs)
                    st.rerun()

            _type_ipr = {0: "Linear", 1: "Vogel comb.", 2: "Vogel"}
            for row_start in range(0, len(iprs), 4):
                cols = st.columns(4)
                for col_idx, i in enumerate(range(row_start, min(row_start + 4, len(iprs)))):
                    item = iprs[i]
                    with cols[col_idx]:
                        render_card(
                            title=f"{item.get('label') or f'IPR {i}'}",
                            id_number=item.get('id', i),
                            details=[
                                f"Type: {_type_ipr.get(item.get('iprType', 1), '?')}",
                                f"Pos: {item.get('measuredLength', 0)} m &nbsp; Fluid: {item.get('prodFluidId', 0)}",
                            ],
                            active=item.get("active", True), icon="📈",
                        )
                        bc1, bc2 = st.columns(2)
                        with bc1:
                            if st.button("✏️", key=f"edit_ipr_{i}", use_container_width=True):
                                _pop_all_acc_edit()
                                st.session_state["_editing_ipr"] = i
                                st.rerun()
                        with bc2:
                            if st.button("🗑️", key=f"del_ipr_{i}", use_container_width=True):
                                iprs.pop(i); set_val(["ipr"], iprs); st.rerun()

            if "_editing_ipr" in st.session_state:
                _pop_all_acc_edit("_editing_ipr")
                _ipr_dialog(st.session_state.pop("_editing_ipr"))

        # --- Liquid Source ---
        with src_tabs[1]:
            st.subheader("Liquid Sources")
            liq_srcs = get_val(["liquidSource"], [])
            if not liq_srcs:
                liq_srcs = []

            col_add, _ = st.columns([1, 3])
            with col_add:
                if st.button("➕ Add Liquid Source", key="add_liq_src"):
                    liq_srcs.append({
                        "active": True, "id": len(liq_srcs), "prodFluidId": 0,
                        "measuredLength": 0.0,
                        "time": [0], "temperature": [80.0], "beta": [0.0],
                        "liquidFlowRate": [100.0],
                    })
                    set_val(["liquidSource"], liq_srcs)
                    st.rerun()

            for row_start in range(0, len(liq_srcs), 4):
                cols = st.columns(4)
                for col_idx, i in enumerate(range(row_start, min(row_start + 4, len(liq_srcs)))):
                    item = liq_srcs[i]
                    with cols[col_idx]:
                        render_card(
                            title=f"{item.get('label') or f'Liq. Src {i}'}",
                            id_number=item.get('id', i),
                            details=[
                                f"Pos: {item.get('measuredLength', 0)} m",
                                f"Fluid: {item.get('prodFluidId', 0)}",
                            ],
                            active=item.get("active", True), icon="💧",
                        )
                        bc1, bc2 = st.columns(2)
                        with bc1:
                            if st.button("✏️", key=f"edit_liq_src_{i}", use_container_width=True):
                                _pop_all_acc_edit()
                                st.session_state["_editing_liq_src"] = i
                                st.rerun()
                        with bc2:
                            if st.button("🗑️", key=f"del_liq_src_{i}", use_container_width=True):
                                liq_srcs.pop(i); set_val(["liquidSource"], liq_srcs); st.rerun()

            if "_editing_liq_src" in st.session_state:
                _pop_all_acc_edit("_editing_liq_src")
                _liquid_source_dialog(st.session_state.pop("_editing_liq_src"))

        # --- Mass Source ---
        with src_tabs[2]:
            st.subheader("Mass Sources")
            mass_srcs = get_val(["massSource"], [])
            if not mass_srcs:
                mass_srcs = []

            col_add, _ = st.columns([1, 3])
            with col_add:
                if st.button("➕ Add Mass Source", key="add_src"):
                    mass_srcs.append({
                        "active": True, "id": len(mass_srcs), "prodFluidId": 0,
                        "measuredLength": 0.0, "thermType": 0,
                        "time": [0], "temperature": [80.0],
                        "totalMassFlowRate": [10.0], "complementaryMassFlowRate": [0.0],
                    })
                    set_val(["massSource"], mass_srcs)
                    st.rerun()

            for row_start in range(0, len(mass_srcs), 4):
                cols = st.columns(4)
                for col_idx, i in enumerate(range(row_start, min(row_start + 4, len(mass_srcs)))):
                    item = mass_srcs[i]
                    with cols[col_idx]:
                        render_card(
                            title=f"{item.get('label') or f'Mass Src {i}'}",
                            id_number=item.get('id', i),
                            details=[
                                f"Pos: {item.get('measuredLength', 0)} m",
                                f"Fluid: {item.get('prodFluidId', 0)} &nbsp; Thermo: {item.get('thermType', 0)}",
                            ],
                            active=item.get("active", True), icon="⚡",
                        )
                        bc1, bc2 = st.columns(2)
                        with bc1:
                            if st.button("✏️", key=f"edit_mass_src_{i}", use_container_width=True):
                                _pop_all_acc_edit()
                                st.session_state["_editing_mass_src"] = i
                                st.rerun()
                        with bc2:
                            if st.button("🗑️", key=f"del_src_{i}", use_container_width=True):
                                mass_srcs.pop(i); set_val(["massSource"], mass_srcs); st.rerun()

            if "_editing_mass_src" in st.session_state:
                _pop_all_acc_edit("_editing_mass_src")
                _mass_source_dialog(st.session_state.pop("_editing_mass_src"))

        # --- Gas Source ---
        with src_tabs[3]:
            st.subheader("Gas Sources")
            gas_srcs = get_val(["gasSource"], [])
            if not gas_srcs:
                gas_srcs = []

            col_add, _ = st.columns([1, 3])
            with col_add:
                if st.button("➕ Add Gas Source", key="add_gas_src"):
                    gas_srcs.append({
                        "active": True, "id": len(gas_srcs), "dry": True,
                        "measuredLength": 0.0,
                        "time": [0], "temperature": [30.0], "gasFlowRate": [1000.0],
                    })
                    set_val(["gasSource"], gas_srcs)
                    st.rerun()

            for row_start in range(0, len(gas_srcs), 4):
                cols = st.columns(4)
                for col_idx, i in enumerate(range(row_start, min(row_start + 4, len(gas_srcs)))):
                    item = gas_srcs[i]
                    with cols[col_idx]:
                        render_card(
                            title=f"{item.get('label') or f'Gas Src {i}'}",
                            id_number=item.get('id', i),
                            details=[
                                f"Pos: {item.get('measuredLength', 0)} m",
                                f"{'Dry' if item.get('dry', True) else 'Rich'} gas",
                            ],
                            active=item.get("active", True), icon="💨",
                        )
                        bc1, bc2 = st.columns(2)
                        with bc1:
                            if st.button("✏️", key=f"edit_gas_src_{i}", use_container_width=True):
                                _pop_all_acc_edit()
                                st.session_state["_editing_gas_src"] = i
                                st.rerun()
                        with bc2:
                            if st.button("🗑️", key=f"del_gas_src_{i}", use_container_width=True):
                                gas_srcs.pop(i); set_val(["gasSource"], gas_srcs); st.rerun()

            if "_editing_gas_src" in st.session_state:
                _pop_all_acc_edit("_editing_gas_src")
                _gas_source_dialog(st.session_state.pop("_editing_gas_src"))

        # --- Pressure Source ---
        with src_tabs[4]:
            st.subheader("Pressure Sources")
            st.caption("Hole between production pipe and an external fluid reservoir")
            fp_list = get_val(["pressureSource"], [])
            if not fp_list:
                fp_list = []

            col_add, _ = st.columns([1, 3])
            with col_add:
                if st.button("➕ Add Pressure Source", key="add_fp"):
                    fp_list.append({
                        "active": True, "id": len(fp_list),
                        "measuredLength": 0.0, "fluidType": 1,
                        "check": 0, "openingType": 0,
                        "cd": 0.84, "beta": 0.0,
                        "temperature": 30.0, "pressure": 100.0,
                        "time": [0], "opening": [0.0],
                    })
                    set_val(["pressureSource"], fp_list)
                    st.rerun()

            _check_labels = {0: "Normal", 1: "Vacuum br.", -1: "Check vlv."}
            for row_start in range(0, len(fp_list), 4):
                cols = st.columns(4)
                for col_idx, i in enumerate(range(row_start, min(row_start + 4, len(fp_list)))):
                    item = fp_list[i]
                    with cols[col_idx]:
                        render_card(
                            title=f"{item.get('label') or f'Press. Src {i}'}",
                            id_number=item.get('id', i),
                            details=[
                                f"Pos: {item.get('measuredLength', 0)} m",
                                f"{_check_labels.get(item.get('check', 0), '?')} &nbsp; Cd={item.get('cd', 0.84)}",
                            ],
                            active=item.get("active", True), icon="🕳️",
                        )
                        bc1, bc2 = st.columns(2)
                        with bc1:
                            if st.button("✏️", key=f"edit_fp_{i}", use_container_width=True):
                                _pop_all_acc_edit()
                                st.session_state["_editing_fp_src"] = i
                                st.rerun()
                        with bc2:
                            if st.button("🗑️", key=f"del_fp_{i}", use_container_width=True):
                                fp_list.pop(i); set_val(["pressureSource"], fp_list); st.rerun()

            if "_editing_fp_src" in st.session_state:
                _pop_all_acc_edit("_editing_fp_src")
                _pressure_source_dialog(st.session_state.pop("_editing_fp_src"))

        # --- Poro Radial Source ---
        with src_tabs[5]:
            st.subheader("Radial Pore Sources")
            st.caption("Radial pore model — each source references an external JSON file")
            _poro_rad = get_val(["porousRadialSource"], [])
            if not _poro_rad:
                _poro_rad = []

            col_add, _ = st.columns([1, 3])
            with col_add:
                if st.button("➕ Add Poro Radial", key="add_poro_rad"):
                    _poro_rad.append({"active": True, "id": len(_poro_rad),
                                      "measuredLength": 0.0, "file": ""})
                    set_val(["porousRadialSource"], _poro_rad)
                    st.rerun()

            for row_start in range(0, len(_poro_rad), 4):
                cols = st.columns(4)
                for col_idx, i in enumerate(range(row_start, min(row_start + 4, len(_poro_rad)))):
                    item = _poro_rad[i]
                    with cols[col_idx]:
                        render_card(
                            title=f"{item.get('label') or f'Poro Rad {i}'}",
                            id_number=item.get('id', i),
                            details=[
                                f"Pos: {item.get('measuredLength', 0)} m",
                                f"File: {item.get('file', '—') or '—'}",
                            ],
                            active=item.get("active", True), icon="🔵",
                        )
                        bc1, bc2 = st.columns(2)
                        with bc1:
                            if st.button("✏️", key=f"edit_poro_rad_{i}", use_container_width=True):
                                _pop_all_acc_edit()
                                st.session_state["_editing_poro_rad"] = i
                                st.rerun()
                        with bc2:
                            if st.button("🗑️", key=f"del_poro_rad_{i}", use_container_width=True):
                                _poro_rad.pop(i); set_val(["porousRadialSource"], _poro_rad); st.rerun()

            if "_editing_poro_rad" in st.session_state:
                _pop_all_acc_edit("_editing_poro_rad")
                _poro_rad_dialog(st.session_state.pop("_editing_poro_rad"))

        # --- Poro 2D Source ---
        with src_tabs[6]:
            st.subheader("2D Pore Sources")
            st.caption("2D pore model — each source references an external JSON file")
            _poro_2d = get_val(["porous2DSource"], [])
            if not _poro_2d:
                _poro_2d = []

            col_add, _ = st.columns([1, 3])
            with col_add:
                if st.button("➕ Add Poro 2D", key="add_poro_2d"):
                    _poro_2d.append({"active": True, "id": len(_poro_2d),
                                     "measuredLength": 0.0, "file": ""})
                    set_val(["porous2DSource"], _poro_2d)
                    st.rerun()

            for row_start in range(0, len(_poro_2d), 4):
                cols = st.columns(4)
                for col_idx, i in enumerate(range(row_start, min(row_start + 4, len(_poro_2d)))):
                    item = _poro_2d[i]
                    with cols[col_idx]:
                        render_card(
                            title=f"{item.get('label') or f'Poro 2D {i}'}",
                            id_number=item.get('id', i),
                            details=[
                                f"Pos: {item.get('measuredLength', 0)} m",
                                f"File: {item.get('file', '—') or '—'}",
                            ],
                            active=item.get("active", True), icon="🟢",
                        )
                        bc1, bc2 = st.columns(2)
                        with bc1:
                            if st.button("✏️", key=f"edit_poro_2d_{i}", use_container_width=True):
                                _pop_all_acc_edit()
                                st.session_state["_editing_poro_2d"] = i
                                st.rerun()
                        with bc2:
                            if st.button("🗑️", key=f"del_poro_2d_{i}", use_container_width=True):
                                _poro_2d.pop(i); set_val(["porous2DSource"], _poro_2d); st.rerun()

            if "_editing_poro_2d" in st.session_state:
                _pop_all_acc_edit("_editing_poro_2d")
                _poro_2d_dialog(st.session_state.pop("_editing_poro_2d"))

    # ─────────────────────────────────────────────────────────────────────────
    # VALVES: valve, gasLiftSource, master1, master2, surfaceChoke, injectionChoke
    # ─────────────────────────────────────────────────────────────────────────
    with acc_tabs[1]:
        vlv_tabs = st.tabs(["DHSV / Generic", "Gas Lift (VGL)", "Master 1 (prod)",
                            "Master 2 (serv)", "Surface Choke", "Injection Choke"])

        # --- DHSV / Generic Valves ---
        with vlv_tabs[0]:
            st.subheader("Valves")
            st.caption("Biphasic valve — Sachdeva model (e.g. DHSV)")
            valves = get_val(["valve"], [])
            if not valves:
                valves = []

            col_add, _ = st.columns([1, 3])
            with col_add:
                if st.button("➕ Add Valve", key="add_valve"):
                    valves.append({"active": True, "id": len(valves), "measuredLength": 0.0,
                                   "cd": 0.84, "cvCurve": 0, "time": [0], "opening": [1.0]})
                    set_val(["valve"], valves)
                    st.rerun()

            for row_start in range(0, len(valves), 4):
                cols = st.columns(4)
                for col_idx, i in enumerate(range(row_start, min(row_start + 4, len(valves)))):
                    item = valves[i]
                    with cols[col_idx]:
                        render_card(
                            title=f"{item.get('label') or f'Valve {i}'}",
                            id_number=item.get('id', i),
                            details=[
                                f"Pos: {item.get('measuredLength', 0)} m",
                                f"Cd={item.get('cd', 0.84)}",
                            ],
                            active=item.get("active", True), icon="🔩",
                        )
                        bc1, bc2 = st.columns(2)
                        with bc1:
                            if st.button("✏️", key=f"edit_valve_{i}", use_container_width=True):
                                _pop_all_acc_edit()
                                st.session_state["_editing_valve"] = i
                                st.rerun()
                        with bc2:
                            if st.button("🗑️", key=f"del_valve_{i}", use_container_width=True):
                                valves.pop(i); set_val(["valve"], valves); st.rerun()

            if "_editing_valve" in st.session_state:
                _pop_all_acc_edit("_editing_valve")
                _valve_dialog(st.session_state.pop("_editing_valve"))

        # --- Gas Lift Valves ---
        with vlv_tabs[1]:
            st.subheader("Gas Lift Valves")
            vgls = get_val(["gasLiftSource"], [])
            if not vgls:
                vgls = []

            col_add, _ = st.columns([1, 3])
            with col_add:
                if st.button("➕ Add VGL", key="add_vgl"):
                    vgls.append({
                        "active": True, "id": len(vgls), "annulusColumnFlag": True,
                        "valveType": 0, "prodMeasuredLength": 0.0,
                        "orificeDiameter": 0.01, "vglDischCoef": 0.84, "liquidDischCoef": 0.9,
                    })
                    set_val(["gasLiftSource"], vgls)
                    st.rerun()

            _vgl_types = {0: "Orifice", 1: "Pressure", 2: "Venturi"}
            for row_start in range(0, len(vgls), 4):
                cols = st.columns(4)
                for col_idx, i in enumerate(range(row_start, min(row_start + 4, len(vgls)))):
                    item = vgls[i]
                    with cols[col_idx]:
                        render_card(
                            title=f"{item.get('label') or f'VGL {i}'}",
                            id_number=item.get('id', i),
                            details=[
                                f"Type: {_vgl_types.get(item.get('valveType', 0), '?')}",
                                f"Pos: {item.get('prodMeasuredLength', 0)} m &nbsp; D={item.get('orificeDiameter', '—')}",
                            ],
                            active=item.get("active", True), icon="⛽",
                        )
                        bc1, bc2 = st.columns(2)
                        with bc1:
                            if st.button("✏️", key=f"edit_vgl_{i}", use_container_width=True):
                                _pop_all_acc_edit()
                                st.session_state["_editing_vgl"] = i
                                st.rerun()
                        with bc2:
                            if st.button("🗑️", key=f"del_vgl_{i}", use_container_width=True):
                                vgls.pop(i); set_val(["gasLiftSource"], vgls); st.rerun()

            if "_editing_vgl" in st.session_state:
                _pop_all_acc_edit("_editing_vgl")
                _vgl_dialog(st.session_state.pop("_editing_vgl"))

        # --- Master 1 (production) ---
        with vlv_tabs[2]:
            st.subheader("Master Valve 1 — Production")
            st.caption("ANM valve on production line (fixed Cd=0.84)")
            widget_bool("Active", ["masterValve", "active"], default=True,
                        help_text="Whether Master 1 valve is active.")
            c1, c2 = st.columns(2)
            with c1:
                widget_number("Position [m]", ["masterValve", "measuredLength"],
                              help_text="Position along the production line.")
                widget_int_select("CV curve type", ["masterValve", "cvCurve"], {
                    0: "0 - Area ratio", 1: "1 - Stem displacement"
                }, default=0, help_text="0: opening as free area / pipe area; 1: stem displacement with CV curve.")
            with c2:
                widget_number("Active area ratio threshold", ["masterValve", "activeAreaRatio"],
                              default=0.4,
                              help_text="Below this threshold the valve model is used (default 0.4).")

            widget_time_series("Master1 opening schedule", ["masterValve"],
                               [{"name": "opening", "unit": "ratio"}])

            # CV curve (x1/cv1) for master1
            _m1_cv = get_val(["masterValve", "cvCurve"], 0)
            if _m1_cv == 1:
                st.markdown("**CV Curve** (stem displacement → flow coefficient)")
                _m1_x1 = get_val(["masterValve", "x1"], [])
                _m1_cv1 = get_val(["masterValve", "cv1"], [])
                _m1_len = max(len(_m1_x1), len(_m1_cv1)) if _m1_x1 or _m1_cv1 else 0
                while len(_m1_x1) < _m1_len:
                    _m1_x1.append(None)
                while len(_m1_cv1) < _m1_len:
                    _m1_cv1.append(None)
                _m1_df = pd.DataFrame({"x1 (% stem)": _m1_x1, "cv1 (gal/min)/psi": _m1_cv1})
                _m1_edited = st.data_editor(_m1_df, num_rows="dynamic",
                                            use_container_width=True, key="master1_cv_table")
                set_val(["masterValve", "x1"], _m1_edited["x1 (% stem)"].dropna().tolist())
                set_val(["masterValve", "cv1"], _m1_edited["cv1 (gal/min)/psi"].dropna().tolist())

        # --- Master 2 (service) ---
        with vlv_tabs[3]:
            st.subheader("Master Valve 2 — Service")
            st.caption("ANM valve on service line (binary sphere model)")
            widget_bool("Active", ["masterValve2", "active"], default=True,
                        help_text="Whether Master 2 valve is active.")
            widget_number("Position [m]", ["masterValve2", "measuredLength"],
                          help_text="Position along the service line.")
            widget_time_series("Master2 opening schedule", ["masterValve2"],
                               [{"name": "opening", "unit": "ratio"}])

        # --- Surface Choke ---
        with vlv_tabs[4]:
            st.subheader("Surface Choke")
            c1, c2 = st.columns(2)
            with c1:
                widget_int_select("CV curve type", ["surfaceChoke", "cvCurve"], {
                    0: "0 - Area ratio", 1: "1 - Stem displacement"
                }, default=0, help_text="0: opening as free area / pipe area; 1: stem displacement with CV curve.")
                widget_number("Discharge coeff.", ["surfaceChoke", "dischargeCoefficient"], default=0.84,
                              help_text="Choke discharge coefficient (default 0.84).")
            with c2:
                widget_int_select("Model", ["surfaceChoke", "model"], {0: "0 - Sachdeva"}, default=0,
                                  help_text="Valve model for the surface choke.")

            widget_time_series("Choke opening schedule", ["surfaceChoke"],
                               [{"name": "opening", "unit": "ratio"}])

            # CV curve (x1/cv1) for chokeSup
            _chk_cv = get_val(["surfaceChoke", "cvCurve"], 0)
            if _chk_cv == 1:
                st.markdown("**CV Curve** (stem displacement → flow coefficient)")
                _chk_x1 = get_val(["surfaceChoke", "x1"], [])
                _chk_cv1 = get_val(["surfaceChoke", "cv1"], [])
                _chk_len = max(len(_chk_x1), len(_chk_cv1)) if _chk_x1 or _chk_cv1 else 0
                while len(_chk_x1) < _chk_len:
                    _chk_x1.append(None)
                while len(_chk_cv1) < _chk_len:
                    _chk_cv1.append(None)
                _chk_df = pd.DataFrame({"x1 (% stem)": _chk_x1, "cv1 (gal/min)/psi": _chk_cv1})
                _chk_edited = st.data_editor(_chk_df, num_rows="dynamic",
                                             use_container_width=True, key="chokeSup_cv_table")
                set_val(["surfaceChoke", "x1"], _chk_edited["x1 (% stem)"].dropna().tolist())
                set_val(["surfaceChoke", "cv1"], _chk_edited["cv1 (gal/min)/psi"].dropna().tolist())

        # --- Injection Choke ---
        with vlv_tabs[5]:
            st.subheader("Injection Choke")
            st.caption("Monophasic gas flow — no CV curve")
            widget_bool("Active", ["injectionChoke", "active"], default=True,
                        help_text="Whether the injection choke is active.")
            widget_number("Discharge coeff.", ["injectionChoke", "dischargeCoefficient"], default=0.9,
                          help_text="Injection choke discharge coefficient (default 0.9).")
            widget_time_series("Injection choke opening", ["injectionChoke"],
                               [{"name": "opening", "unit": "ratio"}])

    # ─────────────────────────────────────────────────────────────────────────
    # PUMPS: esp, volumetricPump, pressureDrop
    # ─────────────────────────────────────────────────────────────────────────
    with acc_tabs[2]:
        pump_tabs = st.tabs(["ESP", "Volumetric Pump", "Delta Pressure"])

        # --- ESP ---
        with pump_tabs[0]:
            st.subheader("ESP Pumps")
            bcs_list = get_val(["esp"], [])
            if not bcs_list:
                bcs_list = []

            col_add, _ = st.columns([1, 3])
            with col_add:
                if st.button("➕ Add ESP", key="add_bcs"):
                    bcs_list.append({
                        "active": True, "id": len(bcs_list), "measuredLength": 0.0,
                        "referenceFreq": 60.0, "stage": 1, "manufacturerStage": 1,
                        "motorEfficiency": 1.0, "minFrequency": 30.0,
                        "time": [0], "frequency": [60.0],
                        "flowRate": [], "pumpHead": [], "power": [], "efficiency": []
                    })
                    set_val(["esp"], bcs_list)
                    st.rerun()

            for row_start in range(0, len(bcs_list), 4):
                cols = st.columns(4)
                for col_idx, i in enumerate(range(row_start, min(row_start + 4, len(bcs_list)))):
                    item = bcs_list[i]
                    with cols[col_idx]:
                        render_card(
                            title=f"{item.get('label') or f'ESP {i}'}",
                            id_number=item.get('id', i),
                            details=[
                                f"Pos: {item.get('measuredLength', 0)} m",
                                f"Stages: {item.get('stage', 1)} &nbsp; f_ref={item.get('referenceFreq', 60)} Hz",
                            ],
                            active=item.get("active", True), icon="🔄",
                        )
                        bc1, bc2 = st.columns(2)
                        with bc1:
                            if st.button("✏️", key=f"edit_bcs_{i}", use_container_width=True):
                                _pop_all_acc_edit()
                                st.session_state["_editing_bcs"] = i
                                st.rerun()
                        with bc2:
                            if st.button("🗑️", key=f"del_bcs_{i}", use_container_width=True):
                                bcs_list.pop(i); set_val(["esp"], bcs_list); st.rerun()

            if "_editing_bcs" in st.session_state:
                _pop_all_acc_edit("_editing_bcs")
                _bcs_dialog(st.session_state.pop("_editing_bcs"))

        # --- Volumetric Pump ---
        with pump_tabs[1]:
            st.subheader("Volumetric Pump")
            st.caption("Isochoric compression model")
            bv_list = get_val(["volumetricPump"], [])
            if not bv_list:
                bv_list = []

            col_add, _ = st.columns([1, 3])
            with col_add:
                if st.button("➕ Add Volumetric Pump", key="add_bv"):
                    bv_list.append({
                        "active": True, "id": len(bv_list), "measuredLength": 0.0,
                        "capacity": 100.0, "polyFactor": 1.4,
                        "time": [0], "frequency": [60.0],
                    })
                    set_val(["volumetricPump"], bv_list)
                    st.rerun()

            for row_start in range(0, len(bv_list), 4):
                cols = st.columns(4)
                for col_idx, i in enumerate(range(row_start, min(row_start + 4, len(bv_list)))):
                    item = bv_list[i]
                    with cols[col_idx]:
                        render_card(
                            title=f"{item.get('label') or f'Vol. Pump {i}'}",
                            id_number=item.get('id', i),
                            details=[
                                f"Pos: {item.get('measuredLength', 0)} m",
                                f"Cap: {item.get('capacity', '—')} cm³",
                            ],
                            active=item.get("active", True), icon="🔧",
                        )
                        bc1, bc2 = st.columns(2)
                        with bc1:
                            if st.button("✏️", key=f"edit_bv_{i}", use_container_width=True):
                                _pop_all_acc_edit()
                                st.session_state["_editing_vol_pump"] = i
                                st.rerun()
                        with bc2:
                            if st.button("🗑️", key=f"del_bv_{i}", use_container_width=True):
                                bv_list.pop(i); set_val(["volumetricPump"], bv_list); st.rerun()

            if "_editing_vol_pump" in st.session_state:
                _pop_all_acc_edit("_editing_vol_pump")
                _vol_pump_dialog(st.session_state.pop("_editing_vol_pump"))

        # --- Delta Pressure ---
        with pump_tabs[2]:
            st.subheader("Pressure Increment")
            st.caption("Localized pressure boost — can model a volumetric pump analogy")
            dp_list = get_val(["pressureDrop"], [])
            if not dp_list:
                dp_list = []

            col_add, _ = st.columns([1, 3])
            with col_add:
                if st.button("➕ Add ΔP", key="add_dp"):
                    dp_list.append({
                        "active": True, "id": len(dp_list), "measuredLength": 0.0,
                        "gasCompType": 0, "liquidEfficiency": 100.0, "gasEfficiency": 100.0,
                        "time": [0], "pressureDrop": [10.0],
                    })
                    set_val(["pressureDrop"], dp_list)
                    st.rerun()

            _comp_types = {0: "Adiabatic", 1: "Polytropic", 2: "Isothermal"}
            for row_start in range(0, len(dp_list), 4):
                cols = st.columns(4)
                for col_idx, i in enumerate(range(row_start, min(row_start + 4, len(dp_list)))):
                    item = dp_list[i]
                    with cols[col_idx]:
                        render_card(
                            title=f"{item.get('label') or f'ΔP {i}'}",
                            id_number=item.get('id', i),
                            details=[
                                f"Pos: {item.get('measuredLength', 0)} m",
                                f"{_comp_types.get(item.get('gasCompType', 0), '?')} &nbsp; η_L={item.get('liquidEfficiency', 100)}%",
                            ],
                            active=item.get("active", True), icon="📐",
                        )
                        bc1, bc2 = st.columns(2)
                        with bc1:
                            if st.button("✏️", key=f"edit_dp_{i}", use_container_width=True):
                                _pop_all_acc_edit()
                                st.session_state["_editing_delta_p"] = i
                                st.rerun()
                        with bc2:
                            if st.button("🗑️", key=f"del_dp_{i}", use_container_width=True):
                                dp_list.pop(i); set_val(["pressureDrop"], dp_list); st.rerun()

            if "_editing_delta_p" in st.session_state:
                _pop_all_acc_edit("_editing_delta_p")
                _delta_p_dialog(st.session_state.pop("_editing_delta_p"))

    # ─────────────────────────────────────────────────────────────────────────
    # PIG
    # ─────────────────────────────────────────────────────────────────────────
    with acc_tabs[3]:
        st.subheader("Pig Launcher/Receiver")
        pig_list = get_val(["pig"], [])
        if not pig_list:
            pig_list = []

        col_add, _ = st.columns([1, 3])
        with col_add:
            if st.button("➕ Add Pig", key="add_pig"):
                pig_list.append({
                    "active": True, "id": len(pig_list),
                    "launcher": 0.0, "receiver": 1000.0, "time": 0.0,
                })
                set_val(["pig"], pig_list)
                st.rerun()

        for row_start in range(0, len(pig_list), 4):
            cols = st.columns(4)
            for col_idx, i in enumerate(range(row_start, min(row_start + 4, len(pig_list)))):
                item = pig_list[i]
                with cols[col_idx]:
                    render_card(
                        title=f"{item.get('label') or f'Pig {i}'}",
                        id_number=item.get('id', i),
                        details=[
                            f"Launch: {item.get('launcher', 0)} m → {item.get('receiver', 0)} m",
                            f"t={item.get('time', 0)} s",
                        ],
                        active=item.get("active", True), icon="🐷",
                    )
                    bc1, bc2 = st.columns(2)
                    with bc1:
                        if st.button("✏️", key=f"edit_pig_{i}", use_container_width=True):
                            _pop_all_acc_edit()
                            st.session_state["_editing_pig"] = i
                            st.rerun()
                    with bc2:
                        if st.button("🗑️", key=f"del_pig_{i}", use_container_width=True):
                            pig_list.pop(i); set_val(["pig"], pig_list); st.rerun()

        if "_editing_pig" in st.session_state:
            _pop_all_acc_edit("_editing_pig")
            _pig_dialog(st.session_state.pop("_editing_pig"))


# ═══════════════════════════════════════════════════════════════════════════════
# TAB 7: BOUNDARY CONDITIONS
# ═══════════════════════════════════════════════════════════════════════════════
with tabs[6]:
    st.header("Boundary Conditions")
    st.caption("Set pressure and flow boundary conditions at system inlet and outlet")

    bc_tabs = st.tabs(["Downstream Pressure", "Downstream Flow", "Upstream Pressure",
                       "Upstream Flow+Pressure", "Upstream Service Line", "Injection Well"])

    # --- Downstream Pressure (Separator) ---
    with bc_tabs[0]:
        st.subheader("Downstream Pressure BC")
        st.caption("Downstream pressure dynamics")
        widget_bool("Active", ["separator", "active"], default=True,
                    help_text="Whether the downstream pressure BC is active.")
        widget_time_series("Separator pressure schedule", ["separator"],
                           [{"name": "pressure", "unit": "kgf/cm²"}])

    # --- Downstream Flow (info only) ---
    with bc_tabs[1]:
        st.info(
            "To define a downstream flow or IPR boundary condition, simply add the "
            "corresponding accessory (mass source, liquid, gas, or IPR) at a small "
            "(but non-zero!) measured depth of the pipe, in the **Accessories** tab.\n\n"
            "Once the corresponding accessory is added, it will automatically be used "
            "as a boundary condition — no additional configuration is needed here."
        )

    # --- Upstream Pressure BC ---
    with bc_tabs[2]:
        st.subheader("Upstream Pressure BC")
        st.caption("Pressure condition at pipe upstream (from initialConfig)")
        widget_bool("Active", ["initialConfig", "pressureCondition", "active"], default=True,
                    help_text="Whether the upstream pressure BC is active.")
        widget_time_series(
            "Pressure BC time series",
            ["initialConfig", "pressureCondition"],
            [
                {"name": "pressure", "unit": "kgf/cm²"},
                {"name": "temperature", "unit": "°C"},
                {"name": "fluidQuality", "unit": "-"},
                {"name": "betaRatio", "unit": "-"},
            ]
        )

    # --- Upstream Flow + Pressure BC ---
    with bc_tabs[3]:
        st.subheader("Upstream Flow + Pressure BC")
        st.caption("Determines the system entirely from the upstream (steady-state only)")
        widget_bool("Active", ["initialConfig", "flowRatePressureCondition", "active"], default=True,
                    help_text="Whether the upstream flow+pressure BC is active.")
        widget_time_series(
            "Flow+Pressure BC time series",
            ["initialConfig", "flowRatePressureCondition"],
            [
                {"name": "pressure", "unit": "kgf/cm²"},
                {"name": "temperature", "unit": "°C"},
                {"name": "massFlowRate", "unit": "kg/s"},
                {"name": "betaRatio", "unit": "-"},
            ]
        )

    # --- Gas Injection BC ---
    with bc_tabs[4]:
        st.subheader("Gas Injection BC")
        st.caption("Boundary condition for gas-lift injection on service line")
        widget_bool("Active", ["gasInj", "active"], default=True,
                    help_text="Whether gas injection BC is active.")
        widget_int_select("BC type", ["gasInj", "bcType"], {
            0: "0 - Injection pressure", 1: "1 - Injection flow rate"
        }, default=0, help_text="0: specifies injection pressure; 1: specifies injection flow rate.")
        widget_bool("Guess initial flow rate", ["gasInj", "initialFlowRateGuess"], default=False,
                    help_text="For pressure BC: if true, first element of gasFlowRate is used as initial guess. Helps convergence.")
        widget_time_series(
            "Gas injection schedule",
            ["gasInj"],
            [
                {"name": "temperature", "unit": "°C"},
                {"name": "gasFlowRate", "unit": "sm³/d"},
                {"name": "injectionPressures", "unit": "kgf/cm²"},
            ]
        )

    # --- Injection Well ---
    with bc_tabs[5]:
        st.subheader("Injection Well BC")
        widget_bool("Active", ["injectionWellBC", "active"], default=True,
                    help_text="Whether the injection well BC is active.")
        widget_int_select("Fluid type", ["injectionWellBC", "fluidType"], {
            0: "0 - User fluid (complementar)",
            1: "1 - Water (salinity required)",
            2: "2 - CO₂-rich gas (flash table)",
            3: "3 - CO₂-rich gas (compositional)",
        }, default=1, help_text="0: user-defined liquid; 1: water (set salinity); 2/3: CO₂-rich gas via flash or compositional.")
        widget_number("Salinity [g/(kg water)]", ["injectionWellBC", "salinity"],
                      help_text="Injected water salinity. Required when fluid type is 1 (water).")
        # PVTSim file dropdown from uploaded auxiliary files
        _pvt_inj_files = [f for f in os.listdir(st.session_state["work_dir"])
                          if f.lower().endswith((".tab", ".ctm"))]
        _cur_pvt_inj = get_val(["injectionWellBC", "pvtsimFile"], "")
        if _cur_pvt_inj and _cur_pvt_inj not in _pvt_inj_files:
            _pvt_inj_files.insert(0, _cur_pvt_inj)
        _pvt_inj_options = [""] + _pvt_inj_files
        _pvt_inj_idx = _pvt_inj_options.index(_cur_pvt_inj) if _cur_pvt_inj in _pvt_inj_options else 0
        _pvt_inj_sel = st.selectbox("PVTSim file", _pvt_inj_options, index=_pvt_inj_idx,
                                    help=".tab or .ctm file (upload in sidebar)",
                                    key=f"pvtsim_inj_file_select_v{st.session_state.get('data_version', 0)}")
        set_val(["injectionWellBC", "pvtsimFile"], _pvt_inj_sel if _pvt_inj_sel else None)
        widget_int_select("BC combination", ["injectionWellBC", "boundaryCondition"], {
            0: "0 - Flow + IPR",
            1: "1 - Inj. pressure + IPR",
            2: "2 - Bottom pressure + IPR",
            3: "3 - Inj. + bottom pressure",
            4: "4 - Flow + inj. pressure",
            5: "5 - Flow + bottom pressure",
        }, default=0, help_text="Combination of boundary conditions for the injection well.")
        c1, c2, c3 = st.columns(3)
        with c1:
            widget_number("Inj. temperature [°C]", ["injectionWellBC", "injectionTemp"],
                          help_text="Injection temperature.")
        with c2:
            widget_number("Flow rate [sm³/d]", ["injectionWellBC", "stdLiquidFlowRate"],
                          help_text="Injection flow rate. Used by BC combinations 0, 4, 5.")
        with c3:
            widget_number("Inj. pressure [kgf/cm²]", ["injectionWellBC", "injectionPressure"],
                          help_text="Injection pressure. Used by BC combinations 1, 3, 4.")
        widget_number("Bottom pressure [kgf/cm²]", ["injectionWellBC", "bottomholePressure"],
                      help_text="Bottom-hole pressure. Used by BC combinations 2, 3, 5.")


# ═══════════════════════════════════════════════════════════════════════════════
# TAB 8: TIME
# ═══════════════════════════════════════════════════════════════════════════════
with tabs[7]:
    st.header("Time Configuration")
    st.caption("Set simulation time, initial conditions, and time step controls for transient simulations")

    widget_bool("Transient simulation", ["initialConfig", "transient"],
                help_text="true: transient simulation; false: steady-state.")

    c1, c2 = st.columns(2)
    with c1:
        widget_number("Final time [s]", ["time", "finalTime"], default=100.0,
                      help_text="Total simulation time.")
        widget_int("Initial fluid ID (prod)", ["initialConfig", "initialFluidId"], default=0,
                   help_text="ID of the fluid that initially fills the production system.")
    with c2:
        widget_int_select("Initial condition", ["initialConfig", "initialCondition"], {
            0: "0 - User defined",
            1: "1 - Steady-state solution",
            2: "2 - Snapshot file (.snp)",
            3: "3 - Gas-lift discharge",
        }, default=1, help_text="0: user initial conditions; 1: steady-state first; 2: from .snp file; 3: gas-lift discharge procedure.")

    # Snapshot file selector (only relevant when initialCondition=2)
    _ci_val = get_val(["initialConfig", "initialCondition"], 1)
    if _ci_val == 2:
        _snp_files = [f for f in os.listdir(st.session_state["work_dir"])
                      if f.lower().endswith(".snp")]
        _cur_snp = get_val(["initialConfig", "snapshotFile"], "")
        if _cur_snp and _cur_snp not in _snp_files:
            _snp_files.insert(0, _cur_snp)
        _snp_options = [""] + _snp_files
        _snp_idx = _snp_options.index(_cur_snp) if _cur_snp in _snp_options else 0
        _snp_sel = st.selectbox(
            "Snapshot file (.snp)", _snp_options, index=_snp_idx,
            help="Select the .snp file to use as initial condition (upload in sidebar).",
            key=f"snp_file_select_v{st.session_state.get('data_version', 0)}"
        )
        if _path_exists(["initialConfig", "snapshotFile"]) or _snp_sel:
            set_val(["initialConfig", "snapshotFile"], _snp_sel if _snp_sel else None)

    st.caption("Define time breakpoints and corresponding max time steps")

    # Time steps table
    tempo_data = get_val(["time"], {})
    tempos = tempo_data.get("times", [0])
    dtmax = tempo_data.get("maxDT", [5])
    max_len = max(len(tempos), len(dtmax))
    while len(tempos) < max_len:
        tempos.append(None)
    while len(dtmax) < max_len:
        dtmax.append(None)

    ts_df = pd.DataFrame({"tempos [s]": tempos, "dtmax [s]": dtmax})
    st.subheader("Maximum Time Increments")
    edited_ts = st.data_editor(ts_df, num_rows="dynamic", use_container_width=True,
                               key=f"time_steps_editor_v{st.session_state.get('data_version', 0)}")
    _tempos_list = edited_ts["tempos [s]"].dropna().tolist()
    _dtmax_list = edited_ts["dtmax [s]"].dropna().tolist()
    if _path_exists(["time"]) or _tempos_list or _dtmax_list:
        set_val(["time", "times"], _tempos_list)
        set_val(["time", "maxDT"], _dtmax_list)

    # Snapshot moments
    with st.expander("📸 Snapshot Recording (gravaMomento)"):
        grava = tempo_data.get("saveSnapshot", [])
        grava_str = st.text_input(
            "Snapshot times [s] (comma-separated)",
            value=", ".join(str(x) for x in grava) if grava else ""
        )
        try:
            _grava_list = [float(x.strip()) for x in grava_str.split(",") if x.strip()]
            if _path_exists(["time", "saveSnapshot"]) or _grava_list:
                set_val(["time", "saveSnapshot"], _grava_list)
        except ValueError:
            pass

    # Segregation control
    with st.expander("🔄 Segregation Control"):
        seg_tempos = tempo_data.get("segregationTime", [])
        seg_vals = tempo_data.get("segregation", [])
        max_len = max(len(seg_tempos), len(seg_vals)) if seg_tempos or seg_vals else 0
        while len(seg_tempos) < max_len:
            seg_tempos.append(None)
        while len(seg_vals) < max_len:
            seg_vals.append(None)
        seg_df = pd.DataFrame({"tempoSegrega [s]": seg_tempos, "segrega [0/1]": seg_vals})
        edited_seg = st.data_editor(seg_df, num_rows="dynamic", use_container_width=True,
                                    key=f"segregation_editor_v{st.session_state.get('data_version', 0)}")
        _seg_tempos = edited_seg["tempoSegrega [s]"].dropna().tolist()
        _seg_vals = [int(x) for x in edited_seg["segrega [0/1]"].dropna().tolist()]
        if _path_exists(["time", "segregationTime"]) or _seg_tempos:
            set_val(["time", "segregationTime"], _seg_tempos)
        if _path_exists(["time", "segregation"]) or _seg_vals:
            set_val(["time", "segregation"], _seg_vals)


# ═══════════════════════════════════════════════════════════════════════════════
# ═══════════════════════════════════════════════════════════════════════════════
# ═══════════════════════════════════════════════════════════════════════════════
# TAB 10: RESULTS
# ═══════════════════════════════════════════════════════════════════════════════
with tabs[9]:
    st.header("Simulation Results")
    st.caption("View profiles, trends, and transverse temperature results after running the simulation")

    results = st.session_state.get("simulation_results")
    if results is None:
        st.info("No simulation results yet. Configure your input and click **Run** in the sidebar.")
    else:
        # Filter out None values from results
        result_keys = [k for k, v in results.items() if v is not None] if isinstance(results, dict) else []
        if not result_keys:
            st.warning("Simulation completed but no result data found.")
        else:
            selected_result = st.selectbox("Select result set", result_keys)
            data = results[selected_result]

            if isinstance(data, pd.DataFrame):
                st.subheader("📈 Profiles")

                # Check if this is a transient profile (MultiIndex with time)
                _is_transient = (isinstance(data.index, pd.MultiIndex)
                                 and data.index.nlevels >= 2)

                if _is_transient:
                    _times = sorted(data.index.get_level_values(0).unique())
                    _has_multiple_times = len(_times) > 1
                else:
                    _has_multiple_times = False

                # Use first time or full data for column detection
                _sample = data.loc[_times[0]] if _is_transient else data

                # Determine x-axis column (measured depth)
                _x_candidates = [c for c in _sample.columns
                                 if "Comprimento" in c and "Centro" in c]
                if not _x_candidates:
                    _x_candidates = [c for c in _sample.columns
                                     if "Comprimento" in c or "measuredLength" in c]

                numeric_cols = _sample.select_dtypes(include="number").columns.tolist()
                if not numeric_cols:
                    st.warning("No numeric columns found in the result data.")
                else:
                    # Default x-axis: measured depth column
                    _default_x_idx = 0
                    if _x_candidates:
                        _default_x_idx = numeric_cols.index(_x_candidates[0]) \
                            if _x_candidates[0] in numeric_cols else 0

                    x_col = st.selectbox("X axis", numeric_cols,
                                         index=_default_x_idx,
                                         key=f"profile_x_{selected_result}")

                    # Exclude geometry/auxiliary columns from default Y
                    _skip_cols = {"Comprimento (m) Fronteira F",
                                  "Comprimento (m) Centro Volume C",
                                  "Elevacao (m) F", "Elevacao (m) C",
                                  "Profundidade (m) F", "Profundidade (m) C",
                                  "Unidade de Producao", "Unidade de Servico"}
                    _y_candidates = [c for c in numeric_cols
                                     if c != x_col and c not in _skip_cols]

                    default_y = _y_candidates[:3]

                    y_cols = st.multiselect("Y axis variables", _y_candidates,
                                           default=default_y,
                                           key=f"profile_y_{selected_result}")

                    if y_cols:
                        # Slider + plot in a fragment for responsive sliding
                        @st.fragment
                        def _profile_plot_fragment():
                            if _has_multiple_times:
                                _time_val = st.select_slider(
                                    "Time instant (s)",
                                    options=_times,
                                    value=_times[0],
                                    key=f"profile_time_slider_{selected_result}",
                                    help="Select time step to visualize the profile."
                                )
                                st.caption(f"t = {_time_val} s "
                                           f"({_time_val / 3600:.4g} h)")
                                _slice = data.loc[_time_val]
                            elif _is_transient:
                                _slice = data.loc[_times[0]]
                            else:
                                _slice = data

                            fig = go.Figure()
                            for y_col in y_cols:
                                fig.add_trace(go.Scatter(
                                    x=_slice[x_col], y=_slice[y_col],
                                    mode="lines", name=y_col))
                            fig.update_layout(
                                xaxis_title=x_col,
                                yaxis_title="Value",
                                legend=dict(orientation="h", yanchor="bottom", y=1.02),
                                height=500,
                            )
                            st.plotly_chart(fig, use_container_width=True)

                        _profile_plot_fragment()

                # Raw data table
                with st.expander("📋 Raw Data"):
                    _raw = data.loc[_times[0]] if _is_transient else data
                    st.dataframe(_raw, use_container_width=True)

            elif isinstance(data, dict):
                # Dict of DataFrames (e.g. tendP — trends with time as index)
                st.subheader("📈 Trends (Time Series)")
                sub_keys = list(data.keys())
                sel_sub = st.selectbox("Location/ID", sub_keys)
                sub_data = data[sel_sub]
                if isinstance(sub_data, pd.DataFrame):
                    # Show metadata if available
                    _attrs = sub_data.attrs
                    if _attrs.get("label"):
                        st.caption(f"Label: **{_attrs['label']}** | "
                                   f"Cell: {_attrs.get('indice_celula', '?')} | "
                                   f"MD: {_attrs.get('comprimento_medido', '?')} m")

                    # X-axis is always time (the DataFrame index)
                    _time_col = sub_data.index.name or "Tempo (s)"
                    _time_display = "Time (s)"
                    _plot_df = sub_data.reset_index()
                    numeric_cols = [c for c in _plot_df.columns if c != _time_col
                                    and pd.api.types.is_numeric_dtype(_plot_df[c])]

                    y_cols = st.multiselect("Y axis variables", numeric_cols,
                                           default=numeric_cols[:3],
                                           key=f"tend_y_{selected_result}")
                    if y_cols:
                        fig = go.Figure()
                        for y_col in y_cols:
                            fig.add_trace(go.Scatter(
                                x=_plot_df[_time_col], y=_plot_df[y_col],
                                mode="lines", name=y_col))
                        fig.update_layout(
                            xaxis_title=_time_display,
                            yaxis_title="Value",
                            legend=dict(orientation="h", yanchor="bottom", y=1.02),
                            height=500,
                        )
                        st.plotly_chart(fig, use_container_width=True)
                    with st.expander("📋 Raw Data"):
                        st.dataframe(_plot_df, use_container_width=True)
                else:
                    st.dataframe(pd.DataFrame([sub_data]) if isinstance(sub_data, dict)
                                 else pd.DataFrame(sub_data), use_container_width=True)
            else:
                try:
                    st.json(json.loads(json.dumps(data, default=str)))
                except Exception:
                    st.write(data)



# ─────────────────────────────────────────────────────────────────────────────
# FOOTER
# ─────────────────────────────────────────────────────────────────────────────
st.markdown("---")
st.markdown(
    '<p style="text-align:center; color:#39C0E0; font-size:0.85em;">'
    'Marlim3 | Built with Streamlit</p>',
    unsafe_allow_html=True
)