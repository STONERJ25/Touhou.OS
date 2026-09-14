# TouhouOS

TouhouOS is an early-stage hobby operating system project inspired by Touhou Project. It aims to build an independent x86-64 operating system that boots through UEFI and can be developed and tested in QEMU.

UEFI loads `BOOTX64.EFI`, which prints a welcome banner through the firmware's text console, then goes on to run independently of firmware entirely:

```text
TouhouOS
Gensokyo Kernel 0.0.1

Welcome to Gensokyo.
```

Since then it has grown its own framebuffer graphics (via the Graphics Output Protocol), a bitmap-font text console (word-wrap, scrolling, backspace), serial (COM1) output, PS/2 keyboard input, and an interactive command shell — `gapsh` — supporting `HELP`, `CLEAR`, `VERSION`, `ECHO`, `PANIC`, `MEMMAP`, `CPUINFO`, and `REBOOT`. It calls `ExitBootServices` and keeps running afterward, so past that point none of this depends on UEFI anymore.

## Build and run

On Windows, install Clang/LLVM and QEMU, then run these commands from the repository root in PowerShell:

```powershell
Set-ExecutionPolicy -Scope Process RemoteSigned -Force
.\tools\build.ps1
.\tools\run-qemu.ps1
```

The execution-policy setting applies only to the current PowerShell session. The build script creates `build/BOOTX64.EFI`; the run script boots it with QEMU and bundled UEFI firmware. Close QEMU to stop the application, which currently stays in a loop after printing.

## Disclaimer

This is largely Vibe/AI coded, I do not claim any of this code is original to myself, nor do I intend this to be taken in anyway seriously. I mean this as a an fun educational experience for myself and nothing else.

### Code and use

This project is experimental and includes AI-assisted code. It is provided for learning and demonstration, without any promise that it is correct, secure, or suitable for use on real hardware. Review and test the code yourself before using it.

There is currently no `LICENSE` file for this repository. Its public availability does not, by itself, grant general permission to reuse, modify, or redistribute the code. Any third-party code, tools, or assets remain subject to their own licenses and attribution requirements.

### Touhou Project

TouhouOS is an unofficial fan work based on **Touhou Project**. It is not affiliated with, approved by, or endorsed by ZUN or Team Shanghai Alice. Touhou Project and its associated names, characters, settings, and other original material belong to their respective rights holders. This repository does not grant rights to that material.

The project intends to use original or properly licensed assets rather than material extracted from official Touhou games. Fan creators should consult the [official Touhou Project fan-content guidelines](https://touhou-project.news/guidelines_en/) before distributing derivative work.
