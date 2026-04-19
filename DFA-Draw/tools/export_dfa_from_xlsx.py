"""
Read DFA_States.xlsx and print JS snippets for index.html:
- FINAL as Set initializer
- RAW_TRANS as array of [from, to, symbol]
- KEYWORD_PHASES with simulation results
"""
import json
import sys
from pathlib import Path

import pandas as pd

ROOT = Path(__file__).resolve().parents[1]
XLSX = ROOT / "DFA_States.xlsx"


def is_yes(v):
    if pd.isna(v):
        return False
    return str(v).strip().lower() in ("yes", "y", "true", "1")


def load():
    df = pd.read_excel(XLSX, sheet_name=0)
    state_col = df.columns[0]
    symbols = [
        c
        for c in df.columns
        if c not in (state_col, "is_final", "Characters")
    ]
    finals = []
    raw = []
    for _, row in df.iterrows():
        fr = str(row[state_col]).strip()
        if is_yes(row.get("is_final", "")):
            finals.append(fr)
        for sym in symbols:
            if sym not in row.index:
                continue
            to = row[sym]
            if pd.isna(to):
                continue
            raw.append((fr, str(to).strip(), str(sym)))

    # Merge edges with same from→to; use *all when all outgoing symbols from that state go to qd
    merged = {}
    targets_from = {}
    for fr, to, sym in raw:
        key = (fr, to)
        if key not in merged:
            merged[key] = []
        merged[key].append(sym)
        targets_from.setdefault(fr, set()).add(to)

    trans = []
    for (fr, to), syms in sorted(merged.items()):
        if to == "qd" and targets_from.get(fr) == {"qd"}:
            label = "*all"
        else:
            label = ",".join(sorted(syms, key=lambda s: (len(s) > 1, s)))
        trans.append([fr, to, label])
    return sorted(finals), trans, raw


# Reserved words / tokens to show in Phases panel (order matters for display)
PHASE_WORDS = [
    ("IF", "if"),
    ("THEN", "then"),
    ("ELSE", "else"),
    ("END", "end"),
    ("REPEAT", "repeat"),
    ("UNTIL", "until"),
    ("READ", "read"),
    ("WRITE", "write"),
]


def simulate(raw_trans, finals_set, word):
    index = {(t[0], t[2]): t[1] for t in raw_trans}
    s = "q0"
    steps = [{"state": s, "ch": None}]
    for ch in word:
        key = (s, ch)
        if key not in index:
            return {
                "ok": False,
                "reason": "no_transition",
                "steps": steps,
                "last": s,
                "missing": ch,
            }
        s = index[key]
        steps.append({"state": s, "ch": ch})
    return {
        "ok": s in finals_set,
        "reason": "accept" if s in finals_set else "not_final",
        "steps": steps,
        "last": s,
        "missing": None,
    }


def main():
    if not XLSX.exists():
        print("Missing", XLSX, file=sys.stderr)
        sys.exit(1)
    finals, trans, raw = load()
    fset = set(finals)
    phases = []
    for label, w in PHASE_WORDS:
        r = simulate(raw, fset, w)
        phases.append(
            {
                "label": label,
                "word": w,
                "pass": r["ok"],
                "reason": r["reason"],
                "steps": r["steps"],
                "last": r["last"],
                "missing": r["missing"],
            }
        )

    # Reachability of final states from q0
    reach = {"q0"}
    changed = True
    while changed:
        changed = False
        for fr, to, _ in raw:
            if fr in reach and to not in reach:
                reach.add(to)
                changed = True
    unreachable_finals = [x for x in finals if x not in reach]

    print("// --- paste into index.html (replace DFA_EXPORT block) ---")
    print("const FINAL = new Set(" + json.dumps(finals) + ");")
    print("const RAW_TRANS = " + json.dumps(trans, indent=2) + ";")
    print(
        "const KEYWORD_PHASES = "
        + json.dumps(phases, indent=2)
        + ";"
    )
    print(
        "const UNREACHABLE_FINALS = "
        + json.dumps(unreachable_finals)
        + ";"
    )
    print("// --- end ---")


if __name__ == "__main__":
    main()
