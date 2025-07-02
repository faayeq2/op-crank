# x86 Assembler

A **two-pass** assembler written in **C** for the x86 architecture.

It generates executable machine code of the **supported** Assembly language code in the provided .asm file.

## Usage:
```bash
make
```
## Alternatives:

Other ways to view the created machine code:
```bash
xxd bin/output.bin
```

```bash
hexdump -C bin/output.bin
```