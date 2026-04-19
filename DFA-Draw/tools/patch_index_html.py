"""Insert exported DFA block into index.html between DATA markers."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
html = (ROOT / "index.html").read_text(encoding="utf-8")
exp = (ROOT / "_export_out.txt").read_text(encoding="utf-8-sig")
lines = [
    ln
    for ln in exp.splitlines()
    if ln.strip()
    and not ln.strip().startswith("// ---")
    and "paste into index.html" not in ln
]
block = "\n".join(lines).strip()
if not block.startswith("const FINAL"):
    raise SystemExit("bad export: " + repr(block[:80]))
bl = block.split("\n")
bl.insert(1, "const START = 'q0', DEAD = 'qd';")
block = "\n".join(bl)

start = html.index("const FINAL = new Set")
end = html.index("// ════════════════════════════════════════════════\n//  LAYOUT")
new_html = html[:start] + block + "\n\n" + html[end:]
(ROOT / "index.html").write_text(new_html, encoding="utf-8")
print("Patched index.html DATA block")
