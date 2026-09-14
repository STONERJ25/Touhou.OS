# TouhouOS

TouhouOS is an early-stage hobby operating system project inspired by Touhou Project. It aims to build an independent x86-64 operating system that boots through UEFI and can be developed and tested in QEMU.

The first boot milestone is working in QEMU: UEFI loads `BOOTX64.EFI`, which prints a welcome message through the firmware's text console:

```text
TouhouOS
Gensokyo Kernel 0.0.1

Welcome to Gensokyo.
```

This is a small UEFI application, not yet an independent kernel. It has not initialized its own framebuffer or taken over the machine from UEFI.

## Build and run

On Windows, install Clang/LLVM and QEMU, then run these commands from the repository root in PowerShell:

```powershell
Set-ExecutionPolicy -Scope Process RemoteSigned -Force
.\tools\build.ps1
.\tools\run-qemu.ps1
```

The execution-policy setting applies only to the current PowerShell session. The build script creates `build/BOOTX64.EFI`; the run script boots it with QEMU and bundled UEFI firmware. Close QEMU to stop the application, which currently stays in a loop after printing.
