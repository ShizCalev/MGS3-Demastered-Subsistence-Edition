from __future__ import annotations

import os
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed
from threading import Lock

# ==========================================================
# CONFIG
# ==========================================================
ROOT = Path(__file__).resolve().parent
THREADS = os.cpu_count() or 8

# ==========================================================
# GLOBALS
# ==========================================================
deleted_count = 0
deleted_count_lock = Lock()

# ==========================================================
# WORKER
# ==========================================================
def delete_file(path: Path) -> None:
    global deleted_count

    try:
        path.unlink()

        with deleted_count_lock:
            deleted_count += 1

    except Exception as e:
        print(f"[Error] {path}: {e}")

# ==========================================================
# MAIN
# ==========================================================
def main() -> None:
    ctxr_files = []

    # Fast recursive scan
    for root, _, files in os.walk(ROOT):
        for name in files:
            if name.lower().endswith(".ctxr"):
                ctxr_files.append(Path(root) / name)

    total = len(ctxr_files)
    print(f"[INFO] Found {total} .ctxr files")

    if total == 0:
        return

    with ThreadPoolExecutor(max_workers=THREADS) as executor:
        futures = [executor.submit(delete_file, p) for p in ctxr_files]

        for i, _ in enumerate(as_completed(futures), 1):
            if i % 1000 == 0 or i == total:
                print(f"[Progress] {i}/{total} deleted")

    print(f"[DONE] Deleted {deleted_count} files")


# ==========================================================
if __name__ == "__main__":
    main()