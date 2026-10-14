param([string]$Version)

$ErrorActionPreference = 'Stop'
$ReleaseArguments = @((Join-Path $PSScriptRoot 'tools\build_release_package.py'))
if ($Version) { $ReleaseArguments += @('--version', $Version.TrimStart('v')) }
& python @ReleaseArguments --synchronize-only
if ($LASTEXITCODE -ne 0) { throw 'GimmiThemLegsVersionSynchronizationFailed' }
& (Join-Path $PSScriptRoot 'Build-FirstPersonBodyBones.ps1')
& python @ReleaseArguments
if ($LASTEXITCODE -ne 0) { throw 'GimmiThemLegsPackageBuildFailed' }
