# Sets up PATH for TouhouOS build tools in the CURRENT PowerShell session only.
# Must be dot-sourced to affect your interactive shell:
#
#   . .\tools\dev-shell.ps1
#
# Running it normally (.\tools\dev-shell.ps1) would only change the PATH of a
# throwaway child process and have no effect on your terminal.

$ToolPaths = @(
    'C:\Program Files\LLVM\bin',
    'C:\Program Files\qemu',
    "$env:LOCALAPPDATA\bin\NASM"
)

foreach ($p in $ToolPaths) {
    if ((Test-Path $p) -and ($env:Path -notlike "*$p*")) {
        $env:Path = "$p;$env:Path"
    }
}

Write-Output 'TouhouOS dev environment:'
foreach ($tool in 'clang', 'lld-link', 'lldb', 'nasm', 'qemu-system-x86_64') {
    $cmd = Get-Command $tool -ErrorAction SilentlyContinue
    if ($cmd) {
        Write-Output "  $tool -> $($cmd.Source)"
    } else {
        Write-Output "  $tool -> NOT FOUND"
    }
}
