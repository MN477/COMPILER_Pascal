# Mini Pascal Compiler in C

A compact educational compiler pipeline written in C, including lexical analysis, syntax analysis, basic semantic checks, and stack-machine style intermediate code generation.

## Project Overview

This repository contains several C programs used to experiment with compiler construction concepts:

- lexical analysis (tokenization)
- recursive-descent syntax analysis
- simple semantic checks (identifier/type validation)
- intermediate code generation to a stack-machine style representation

The project also includes Pascal-like input examples and generated tables/output files.

## Repository Structure

- `compil.c`: integrated compiler (lexical + syntax + semantic + intermediate code)
- `compil_test.c`: variant with additional in-code explanations/comments
- `Analyseur_Lexical.c`: standalone lexical analyzer
- `Analyseur_Syntaxique.c`: standalone syntax analyzer
- `test.pas`, `Test_Pascal.pas`: sample source programs
- `table_ID.txt`, `table_key_words.txt`: generated lexical tables
- `code_machine.txt`: generated intermediate code

## Requirements

- C compiler (GCC recommended)
- Windows PowerShell, CMD, or Linux/macOS shell

## Build

### Windows (MinGW GCC)

```bash
gcc -Wall -Wextra -std=c11 compil.c -o compil.exe
gcc -Wall -Wextra -std=c11 Analyseur_Lexical.c -o Analyseur_Lexical.exe
gcc -Wall -Wextra -std=c11 Analyseur_Syntaxique.c -o Analyseur_Syntaxique.exe
```

### Linux/macOS

```bash
gcc -Wall -Wextra -std=c11 compil.c -o compil
gcc -Wall -Wextra -std=c11 Analyseur_Lexical.c -o Analyseur_Lexical
gcc -Wall -Wextra -std=c11 Analyseur_Syntaxique.c -o Analyseur_Syntaxique
```

## Run

### Integrated compiler

The current program asks for the source filename interactively:

```bash
./compil
# then type: test.pas
```

### Lexical analyzer

```bash
./Analyseur_Lexical
# then type: test.pas
```

### Syntax analyzer

```bash
./Analyseur_Syntaxique < tokens.txt
```

## Typical Workflow

1. Compile one of the analyzers.
2. Run with a Pascal-like input file.
3. Inspect generated artifacts:
   - `table_ID.txt`
   - `table_key_words.txt`
   - `code_machine.txt`

## Notes

- This project is educational and intentionally lightweight.
- Error messages are primarily designed for learning/debugging.
- Some files may overlap in functionality as different project stages.

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) before opening a pull request.

## Security

See [SECURITY.md](SECURITY.md) for reporting guidance.

## License

Distributed under the MIT License. See [LICENSE](LICENSE).
