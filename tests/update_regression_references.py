#!/usr/bin/env python3
"""
Update reference DataFrames for regression tests.

Runs all demo simulations and saves results as CSV files in
tests/comparison/<model-name>/.

Usage:
    python tests/update_regression_references.py              # all models
    python tests/update_regression_references.py model1 ...   # specific models

Requirements:
    - Compiled Marlim3 executable available in the marlim3 package
"""

import argparse
import os
import shutil
import sys

# Add project root to path
PROJECT_ROOT = os.path.normpath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.insert(0, PROJECT_ROOT)

from test_regression import (
    COMPARISON_DIR,
    DEMOS,
    DEMO_IDS,
    _run_simulation,
    _save_results,
)


def main():
    parser = argparse.ArgumentParser(
        description="Update reference DataFrames for regression tests."
    )
    parser.add_argument(
        "models",
        nargs="*",
        default=DEMO_IDS,
        help="Model names to update (default: all)",
    )
    parser.add_argument(
        "--force",
        action="store_true",
        help="Overwrite existing references without confirmation",
    )
    args = parser.parse_args()

    # Validate names
    invalid = [m for m in args.models if m not in DEMOS]
    if invalid:
        print(f"ERROR: Unknown models: {invalid}")
        print(f"Available models: {DEMO_IDS}")
        sys.exit(1)

    print(f"Updating references for {len(args.models)} model(s)...")
    print(f"Reference directory: {COMPARISON_DIR}")
    print()

    failed = []
    for model_name in args.models:
        model_dir = os.path.join(COMPARISON_DIR, model_name)

        if os.path.isdir(model_dir) and os.listdir(model_dir) and not args.force:
            resp = input(
                f"  Reference already exists for '{model_name}'. Overwrite? [y/N] "
            )
            if resp.lower() not in ("y", "yes"):
                print(f"  Skipping '{model_name}'")
                continue

        print(f"  [{model_name}] Running simulation...")
        try:
            resultados = _run_simulation(model_name)
            if not resultados:
                print(f"  [{model_name}] WARNING: simulation produced no results!")
                failed.append(model_name)
                continue

            # Clean existing directory
            if os.path.isdir(model_dir):
                shutil.rmtree(model_dir)

            _save_results(model_name, resultados)
            n_files = len([f for f in os.listdir(model_dir) if f.endswith(".csv")])
            print(f"  [{model_name}] OK — {n_files} CSV file(s) saved")

        except Exception as e:
            print(f"  [{model_name}] ERROR: {e}")
            failed.append(model_name)

    print()
    if failed:
        print(f"FAILURES: {failed}")
        sys.exit(1)
    else:
        print("All references updated successfully!")


if __name__ == "__main__":
    main()
