import ast
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def _load_gui_helper(helper_name):
    source_path = ROOT / "gui" / "app.py"
    tree = ast.parse(source_path.read_text(encoding="utf-8"))
    selected_nodes = [
        node
        for node in tree.body
        if (
            isinstance(node, ast.Assign)
            and any(
                isinstance(target, ast.Name) and target.id == "MODEL_FILE_SUFFIXES"
                for target in node.targets
            )
        )
        or (isinstance(node, ast.FunctionDef) and node.name == helper_name)
    ]
    namespace = {}
    exec(compile(ast.Module(selected_nodes, []), str(source_path), "exec"), namespace)
    return namespace[helper_name]


def test_streamlit_gui_does_not_import_desktop_wrapper():
    source = (ROOT / "gui" / "app.py").read_text(encoding="utf-8")

    assert "import marlim3_desktop" not in source
    assert "from marlim3_desktop" not in source
    assert "sys._MEIPASS" not in source
    assert "MARLIM3_DESKTOP_ROOT" not in source


def test_desktop_wrapper_consumes_gui_as_a_data_file():
    spec = (
        ROOT / "marlim3_desktop" / "packaging" / "marlim3-desktop.spec"
    ).read_text(encoding="utf-8")

    assert 'root / "gui" / "app.py"' in spec
    assert 'desktop_root / "__main__.py"' in spec


def test_demo_loader_lists_mr3_models_but_not_auxiliary_json(tmp_path):
    discover_demo_files = _load_gui_helper("_discover_demo_files")
    demos_dir = tmp_path / "demos"
    case_dir = demos_dir / "case"
    case_dir.mkdir(parents=True)
    (demos_dir / "english.mr3").write_text("{}", encoding="utf-8")
    (demos_dir / "manifest.json").write_text("[]", encoding="utf-8")
    (case_dir / "portuguese.mr3").write_text("{}", encoding="utf-8")
    (case_dir / "VFP.json").write_text("{}", encoding="utf-8")
    (case_dir / "PVTSIM-MARLIM.tab").write_text("", encoding="utf-8")

    assert discover_demo_files(demos_dir) == [
        "case/portuguese.mr3",
        "english.mr3",
    ]


def test_relevant_simulator_launches_do_not_use_shell():
    sources = [
        ROOT / "gui" / "app.py",
        ROOT / "marlim3" / "_tramo" / "_branch.py",
        ROOT / "marlim3" / "_rede" / "_rede.py",
        ROOT / "marlim3" / "_conversores" / "_conversor_mr2" / "_mr2_simulator.py",
    ]

    for source_path in sources:
        assert "shell=True" not in source_path.read_text(encoding="utf-8-sig")
