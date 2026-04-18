# TINY Lexical Scanner (Compiler Project)

A desktop GUI lexical analyzer for a simplified TINY programming language, built with C++ and the Win32 API.

This project demonstrates the **lexical analysis** phase of a compiler by scanning source code and producing a token table (`Token#`, `Lexeme`, `Type`).

## Features

- Win32 GUI application with input/output panels
- Lexical analysis for TINY language tokens
- Case-insensitive keyword recognition (`if`, `IF`, `If`, etc.)
- Support for comments in `{ ... }` format (ignored by scanner)
- Tokenized output with fixed-width columns for readability
- Responsive window layout with modernized color theme

## Token Rules Implemented

### Reserved Words

- `if`
- `then`
- `else`
- `end`
- `repeat`
- `until`
- `read`
- `write`

### Operators

- Arithmetic: `+`, `-`, `*`, `/`
- Relational: `=`, `<`, `>`, `<=`, `>=`
- Assignment: `:=`

### Symbols

- `;`
- `(`
- `)`
- `,`

### Identifiers

- Regex form: `Letter (Letter | Digit)*`

### Numbers

- Regex form: `Digit+`

### Comments

- Regex form: `\{[^}]*\}`

## Project Structure

- `Project/mainwindow.cpp` - main source file (scanner + GUI)
- `Documentation.pdf` - language and regex specification

## Requirements

- Windows
- MinGW g++ (or compatible GCC toolchain)

## Build and Run

From the project root:

```powershell
cd "Project"
g++ "mainwindow.cpp" -o "mainwindow.exe" -mwindows -lgdi32 -luser32
.\mainwindow.exe
```

If `mainwindow.exe` is currently running, close it before rebuilding.

## Sample Test Input

```txt
{ scanner full test }
read x;
read y;
if x <= 10 then
    z := x + y * 2;
else
    z := y - 3 / 1;
end
repeat
    x := x + 1;
until x >= y
write z, x;
if x > y then
    write x;
end
```

## Expected Behavior

- Keywords are classified as keyword tokens (not identifiers)
- Variables like `x`, `y`, `z` are `IDENTIFIER`
- Numbers like `10`, `2`, `3` are `NUMBER`
- Operators and symbols are recognized according to the token rules
- Comment text inside `{ ... }` is ignored

## Team

- Zeyad Yasser Salah
- Seif-Eldeen Taha
- Seif Mujahid
- Zeyad Magdy
- Ibrahim Mokhtar

## Supervisor

- Eng. Crestien

