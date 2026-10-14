param([string]$ReleaseVersion, [string]$ReleasePackage)

$ErrorActionPreference = 'Stop'
if ($ReleaseVersion -notmatch '^v[0-9]+\.[0-9]+\.[0-9]+$') {
    throw 'GimmiThemLegsInvalidReleaseTag'
}
$GitHubRelease = gh api "repos/$env:GITHUB_REPOSITORY/releases/tags/$ReleaseVersion" 2>$null
if ($LASTEXITCODE -ne 0) {
    gh release create $ReleaseVersion $ReleasePackage --title 'GimmiThemLegs [pre-alpha]' --notes '' --prerelease --verify-tag
    if ($LASTEXITCODE -ne 0) { throw 'GimmiThemLegsGitHubReleaseFailed' }
} else {
    $ReleasePackageName = Split-Path $ReleasePackage -Leaf
    $GitHubReleaseAsset = ($GitHubRelease | ConvertFrom-Json).assets | Where-Object name -eq $ReleasePackageName
    if (-not $GitHubReleaseAsset) {
        gh release upload $ReleaseVersion $ReleasePackage
        if ($LASTEXITCODE -ne 0) { throw 'GimmiThemLegsGitHubReleaseUploadFailed' }
    }
}
$GitHubRelease = gh api "repos/$env:GITHUB_REPOSITORY/releases/tags/$ReleaseVersion"
if ($LASTEXITCODE -ne 0) { throw 'GimmiThemLegsGitHubReleaseVerificationFailed' }
$ReleasePackageName = Split-Path $ReleasePackage -Leaf
$GitHubReleaseAsset = ($GitHubRelease | ConvertFrom-Json).assets | Where-Object name -eq $ReleasePackageName
$ReleasePackageHash = (Get-FileHash $ReleasePackage -Algorithm SHA256).Hash.ToLowerInvariant()
if ($GitHubReleaseAsset.digest -ne "sha256:$ReleasePackageHash") {
    throw 'GimmiThemLegsGitHubReleaseContentMismatch'
}
($GitHubRelease | ConvertFrom-Json).html_url
