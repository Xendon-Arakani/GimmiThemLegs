$ErrorActionPreference = 'Stop'
$NativeOutputDirectory = Join-Path $PSScriptRoot 'native\build'
$PluginOutputDirectory = Join-Path $PSScriptRoot 'plugins'
New-Item -ItemType Directory -Path $NativeOutputDirectory -Force | Out-Null
New-Item -ItemType Directory -Path $PluginOutputDirectory -Force | Out-Null
$NativeObjectPath = Join-Path $NativeOutputDirectory 'FirstPersonBodyBones.obj'
$ReleaseModManifest = Get-Content -Raw -LiteralPath (Join-Path $PSScriptRoot 'mods\GimmiThemLegs\mod.json') | ConvertFrom-Json
$ReleasePluginName = $ReleaseModManifest.Name + ' ' + $ReleaseModManifest.Version
$ReleaseMetadataPath = Join-Path $NativeOutputDirectory 'FirstPersonBodyReleaseMetadata.h'
[IO.File]::WriteAllText($ReleaseMetadataPath, ('static constexpr char FirstPersonBodyPluginName[] = ' + (ConvertTo-Json -Compress $ReleasePluginName) + ';'), [Text.UTF8Encoding]::new($false))
$NativeCompilerCommand = Get-Command clang-cl.exe -ErrorAction SilentlyContinue
$NativeLinkerCommand = Get-Command lld-link.exe -ErrorAction SilentlyContinue
$NativeCompilerPath = if ($NativeCompilerCommand) { $NativeCompilerCommand.Path } else { Join-Path $env:ProgramFiles 'LLVM\bin\clang-cl.exe' }
$NativeLinkerPath = if ($NativeLinkerCommand) { $NativeLinkerCommand.Path } else { Join-Path $env:ProgramFiles 'LLVM\bin\lld-link.exe' }
$NativeSdkLibraryRoot = Join-Path ${env:ProgramFiles(x86)} 'Windows Kits\10\Lib'
$NativeKernelLibrary = Get-ChildItem -LiteralPath $NativeSdkLibraryRoot -Directory | Sort-Object { [Version]$_.Name } -Descending | ForEach-Object { Join-Path $_.FullName 'um\x64\kernel32.lib' } | Where-Object { Test-Path -LiteralPath $_ } | Select-Object -First 1
if (-not $NativeKernelLibrary) { throw 'FirstPersonBodyWindowsSdkUnavailable' }
& $NativeCompilerPath /c /O2 /GS- /GR- /EHs-c- /Zl (Join-Path $PSScriptRoot 'native\FirstPersonBodyBones.cpp') "/Fo$NativeObjectPath"
if ($LASTEXITCODE -ne 0) { throw 'FirstPersonBodyBonesCompilationFailed' }
& $NativeLinkerPath /dll /nodefaultlib /entry:DllMain /machine:x64 "/out:$(Join-Path $PluginOutputDirectory 'FirstPersonBodyBones.dll')" $NativeObjectPath $NativeKernelLibrary
if ($LASTEXITCODE -ne 0) { throw 'FirstPersonBodyBonesLinkFailed' }
