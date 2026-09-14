# Compiles boot/main.c into build/BOOTX64.EFI using Clang + LLD (see docs/decisions/ADR-0005).

. "$PSScriptRoot\dev-shell.ps1"

$root     = Resolve-Path "$PSScriptRoot\.."
$buildDir = Join-Path $root 'build'
$srcMain  = Join-Path $root 'boot\main.c'
$objMain  = Join-Path $buildDir 'main.obj'
$efiOut   = Join-Path $buildDir 'BOOTX64.EFI'

New-Item -ItemType Directory -Force -Path $buildDir | Out-Null

Write-Output 'Compiling boot/main.c ...'
clang -target x86_64-unknown-windows -ffreestanding -fshort-wchar -fno-stack-protector -Wall -Wextra -c $srcMain -o $objMain
if ($LASTEXITCODE -ne 0) { throw 'Compile failed' }

Write-Output 'Linking BOOTX64.EFI ...'
lld-link /subsystem:efi_application /entry:efi_main /nodefaultlib /machine:x64 "/out:$efiOut" $objMain
if ($LASTEXITCODE -ne 0) { throw 'Link failed' }

Write-Output "Built $efiOut"
