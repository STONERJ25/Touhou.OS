# Compiles every boot/*.c into build/BOOTX64.EFI using Clang + LLD (see docs/decisions/ADR-0005).
# Auto-discovers source files so adding a new boot/*.c doesn't require editing this script.

. "$PSScriptRoot\dev-shell.ps1"

$root     = Resolve-Path "$PSScriptRoot\.."
$buildDir = Join-Path $root 'build'
$bootDir  = Join-Path $root 'boot'
$efiOut   = Join-Path $buildDir 'BOOTX64.EFI'

New-Item -ItemType Directory -Force -Path $buildDir | Out-Null

$sources = Get-ChildItem -Path $bootDir -Filter '*.c'
$objects = @()

foreach ($src in $sources) {
    $obj = Join-Path $buildDir ($src.BaseName + '.obj')
    Write-Output "Compiling boot/$($src.Name) ..."
    clang -target x86_64-unknown-windows -ffreestanding -fshort-wchar -fno-stack-protector -Wall -Wextra -c $src.FullName -o $obj
    if ($LASTEXITCODE -ne 0) { throw "Compile failed: $($src.Name)" }
    $objects += $obj
}

Write-Output 'Linking BOOTX64.EFI ...'
lld-link /subsystem:efi_application /entry:efi_main /nodefaultlib /machine:x64 "/out:$efiOut" $objects
if ($LASTEXITCODE -ne 0) { throw 'Link failed' }

Write-Output "Built $efiOut"
