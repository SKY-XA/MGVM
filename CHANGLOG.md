## 0.2.1
### Fixed
- Bug: bytecode symbol table boundary read exception
- disasm: occasional variable symbol truncation output issue
### Added
- Bytecode: add basic validation for variable‑symbol table segment
- disasm: add warning alert for corrupted symbol table
### Optimized
- Improve variable symbol parsing robustness during bytecode loading

## 0.2.0
### Breaking Changes
- Bytecode format upgraded to v3, old v3‑previous(v2) bytecode files cannot be loaded.

### Fixed
- Bug: bytecode runtime lost variable names, causing incorrect execution
- disasm: restore original variable names in disassembly output
- Fixed wrong implementation for `je` / `jne` conditional jump

### Added
- Bytecode: new variable‑symbol table segment stores name‑to‑index mapping