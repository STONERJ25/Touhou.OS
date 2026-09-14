# Assembles a minimal FAT "ESP" folder and boots build/BOOTX64.EFI under QEMU + OVMF.

. "$PSScriptRoot\dev-shell.ps1"

$root       = Resolve-Path "$PSScriptRoot\.."
$buildDir   = Join-Path $root 'build'
$efiOut     = Join-Path $buildDir 'BOOTX64.EFI'
$espDir     = Join-Path $buildDir 'esp\EFI\BOOT'
$espRoot    = Join-Path $buildDir 'esp'
$ovmfCode   = 'C:\Program Files\qemu\share\edk2-x86_64-code.fd'
$varsSource = 'C:\Program Files\qemu\share\edk2-i386-vars.fd'
$varsCopy   = Join-Path $buildDir 'vars.fd'

if (-not (Test-Path $efiOut)) {
    throw 'BOOTX64.EFI not found -- run tools\build.ps1 first'
}

New-Item -ItemType Directory -Force -Path $espDir | Out-Null
Copy-Item $efiOut (Join-Path $espDir 'BOOTX64.EFI') -Force

if (-not (Test-Path $varsCopy)) {
    Copy-Item $varsSource $varsCopy
}

$driveCode = "if=pflash,format=raw,readonly=on,file=$ovmfCode"
$driveVars = "if=pflash,format=raw,file=$varsCopy"
$driveEsp  = "format=raw,file=fat:rw:$espRoot"

qemu-system-x86_64 -machine q35 -m 256M -drive $driveCode -drive $driveVars -drive $driveEsp -net none -serial stdio
