from marlim_regression.bootstrap import ensure_frontend_assets, ensure_supported_python
from marlim_regression.server import main


if __name__ == "__main__":
    ensure_supported_python()
    ensure_frontend_assets()
    main()
