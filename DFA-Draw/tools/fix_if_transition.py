"""Set q1 + 'f' -> q2 in DFA_States.xlsx so keyword IF can reach a final state."""
from pathlib import Path

import openpyxl

ROOT = Path(__file__).resolve().parents[1]
path = ROOT / "DFA_States.xlsx"
wb = openpyxl.load_workbook(path)
ws = wb.active
rows = list(ws.iter_rows(values_only=False))
header = [c.value for c in rows[0]]
try:
    fi = header.index("f")
except ValueError:
    raise SystemExit("column 'f' not found")

for row in rows[1:]:
    if row[0].value == "q1":
        row[fi].value = "q2"
        print("Updated q1[f] -> q2")
        break
else:
    raise SystemExit("row q1 not found")

wb.save(path)
print("Saved", path)
