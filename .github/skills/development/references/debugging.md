# Debugging

## A crash

1. Reproduce it with `scripts/repro_case.py` and keep the run folder.
2. Build a debug engine in its own folder: `cmake --preset gcc-debug -B build-debug` and `cmake --build build-debug -j`. The build also copies this engine into `tools/marlim3/`; rebuild the release preset afterwards.
3. Run it under gdb from the run folder: `gdb --args <engine> -d . -i <model>`, then `run` and `bt`. gdb stops at the signal before `signalHandler` logs it.
4. For memory errors, add `-DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer" -DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address` to the configure step. An optimized build with `-g` keeps the timing close to the release one, which matters for races.
5. Without a debugger, the `causa` field of `simulacao.log` names the signal.

## A hang

- Start the engine under gdb and interrupt it (Ctrl+C), then `thread apply all bt`. Attaching to a running process can be blocked by the system's ptrace policy.
- Run with `-t 1` (or `OMP_NUM_THREADS=1`) to tell a race from a numerical loop.
- Long steady-state loops usually sit in a root search (`SProd::buscaProdPfundoPerm` and its variants) or in the false-position solver of the fluid properties (`ProFlu::FalsaCorda`).

## OpenMP

- The parametric analysis (`leituraAPparalelo`) and the networks run in parallel. `Logger::log` is serialized; other shared objects are not.
- ThreadSanitizer may fail to link with older GCC versions. AddressSanitizer with gdb found the logger race of issue #136.

## Encoding (Python)

- On Windows, files opened without `encoding=` use the code page (cp1252) and garble accents.
- Reproduce it on Linux with `python -X warn_default_encoding -W error::EncodingWarning ...` (Python 3.10+): every implicit encoding becomes an error.
