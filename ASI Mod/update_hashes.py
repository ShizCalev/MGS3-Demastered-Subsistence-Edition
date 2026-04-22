from __future__ import annotations

import hashlib
import re
import shutil
import subprocess
import sys
from pathlib import Path


# ==========================================================
# CONFIG
# ==========================================================
CPP_REL_PATH = Path(r"ASI Mod\src\verify_mod_installation.cpp")

TARGETS = [
    # (relative path from git root, constant name in cpp)
    (
        Path(r"Textures\Staging\textures\flatlist\ovr_stm\_win\sok_coat_lupe_himo.bmp.ctxr"),
        "MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1",
    ),
    (
        Path(r"Textures\Staging - 2x Upscaled\textures\flatlist\ovr_stm\_win\sok_coat_lupe_himo.bmp.ctxr"),
        "MGS3_Demaster_2x_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1",
    ),
    (
        Path(r"Textures\Staging - 4x Upscaled\textures\flatlist\ovr_stm\_win\sok_coat_lupe_himo.bmp.ctxr"),
        "MGS3_Demaster_4x_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1",
    ),
 
    (
        Path(r"Textures\Staging\textures\flatlist\ovr_stm\ovr_us\_win\sna_suit_tears_sub_ovl_alp.bmp.ctxr"),
        "MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1",
    ),
    (
        Path(r"Textures\Staging - 2x Upscaled\textures\flatlist\ovr_stm\ovr_us\_win\sna_suit_tears_sub_ovl_alp.bmp.ctxr"),
        "MGS3_Demaster_2x_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1",
    ),
    (
        Path(r"Textures\Staging - 4x Upscaled\textures\flatlist\ovr_stm\ovr_us\_win\sna_suit_tears_sub_ovl_alp.bmp.ctxr"),
        "MGS3_Demaster_4x_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1",
    ),
    
    (
        Path(r"Textures\Staging\textures\flatlist\ovr_stm\ovr_jp\_win\sna_suit_tears_sub_ovl_alp.bmp.ctxr"),
        "MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1",
    ),
    (
        Path(r"Textures\Staging - 2x Upscaled\textures\flatlist\ovr_stm\ovr_jp\_win\sna_suit_tears_sub_ovl_alp.bmp.ctxr"),
        "MGS3_Demaster_2x_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1",
    ),
    (
        Path(r"Textures\Staging - 4x Upscaled\textures\flatlist\ovr_stm\ovr_jp\_win\sna_suit_tears_sub_ovl_alp.bmp.ctxr"),
        "MGS3_Demaster_4x_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1",
    ), 
    
    
]

# ==========================================================
# HELPERS
# ==========================================================
def die(msg: str, code: int = 1) -> None:
    print(f"[ERROR] {msg}", file=sys.stderr)
    raise SystemExit(code)


def run_git_toplevel() -> Path:
    try:
        p = subprocess.run(
            ["git", "rev-parse", "--show-toplevel"],
            capture_output=True,
            text=True,
            check=True,
        )
    except FileNotFoundError:
        die("git is not installed or not on PATH.")
    except subprocess.CalledProcessError as ex:
        stderr = (ex.stderr or "").strip()
        die(f"Failed to get git root. Are you running inside a git repo?\n{stderr}")

    top = (p.stdout or "").strip()
    if not top:
        die("git rev-parse returned an empty path for the repo root.")

    return Path(top)


def sha1_file(path: Path) -> str:
    h = hashlib.sha1()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest().lower()


def update_cpp_constants(cpp_path: Path, updates: dict[str, str]) -> tuple[str, dict[str, tuple[str, str]], list[str]]:
    """
    Returns:
      new_text,
      changed: {name: (old_hash, new_hash)}
      missing: [names_not_found]
    """
    text = cpp_path.read_text(encoding="utf-8", errors="strict")

    changed: dict[str, tuple[str, str]] = {}
    missing: list[str] = []

    for name, new_hash in updates.items():
        # Match:
        # constexpr const char* NAME = "....";
        # allow whitespace and optional "constexpr" spacing variations
        pattern = re.compile(
            rf'(^\s*constexpr\s+const\s+char\s*\*\s*{re.escape(name)}\s*=\s*")([0-9a-fA-F]{{40}})(";\s*$)',
            re.MULTILINE,
        )

        m = pattern.search(text)
        if not m:
            missing.append(name)
            continue

        old_hash = m.group(2).lower()
        if old_hash != new_hash.lower():
            text = pattern.sub(rf'\g<1>{new_hash.lower()}\g<3>', text, count=1)
            changed[name] = (old_hash, new_hash.lower())

    return text, changed, missing


# ==========================================================
# MAIN
# ==========================================================
def main() -> int:
    git_root = run_git_toplevel()
    print(f"[INFO] git root: {git_root}")

    cpp_path = git_root / CPP_REL_PATH
    if not cpp_path.is_file():
        die(f"Missing file: {cpp_path}")

    updates: dict[str, str] = {}
    missing_files: list[Path] = []

    for rel_path, const_name in TARGETS:
        abs_path = git_root / rel_path
        if not abs_path.is_file():
            missing_files.append(abs_path)
            continue

        digest = sha1_file(abs_path)
        updates[const_name] = digest
        print(f"[SHA1] {const_name} = {digest}  ({abs_path})")

    if missing_files:
        print("[ERROR] One or more target files are missing:", file=sys.stderr)
        for p in missing_files:
            print(f"  - {p}", file=sys.stderr)
        return 2

    new_text, changed, missing_consts = update_cpp_constants(cpp_path, updates)

    if missing_consts:
        print("[ERROR] One or more constants were not found in the cpp file:", file=sys.stderr)
        for n in missing_consts:
            print(f"  - {n}", file=sys.stderr)
        return 3

    if not changed:
        print("[INFO] No changes needed. All hashes already match.")
        return 0

    cpp_path.write_text(new_text, encoding="utf-8", errors="strict")
    print(f"[OK] Updated: {cpp_path}")

    print("[CHANGES]")
    for name, (old_hash, new_hash) in changed.items():
        print(f"  - {name}: {old_hash} -> {new_hash}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())