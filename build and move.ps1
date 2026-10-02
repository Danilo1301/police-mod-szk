$NDKPath = Get-Content "$PSScriptRoot/NDKPath.txt"

Write-Output "NDK located at: $NDKPath"

$buildScript = "$NDKPath/build/ndk-build"

if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

$envFile = "$PSScriptRoot/.env"

if (-not (Test-Path $envFile)) {
    Write-Output "[BUILD] .env not found!"
    Exit 1
}

$envContent = Get-Content $envFile

foreach ($line in $envContent) {
    $line = $line.Trim()

    if ($line -eq "" -or $line.StartsWith("#")) {
        continue
    }

    $parts = $line -split "=", 2

    if ($parts.Count -ne 2) {
        continue
    }

    $name = $parts[0].Trim()
    $value = $parts[1].Trim()

    if ($value.StartsWith('"') -and $value.EndsWith('"')) {
        $value = $value.Substring(1, $value.Length - 2)
    }

    Set-Item "Env:$name" $value
}

Write-Output "[BUILD] Starting NDK..."

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk NDK_DEBUG=0 -j4

if ($LASTEXITCODE -ne 0) {
    Write-Output "[BUILD] Failed!"
    Exit $LASTEXITCODE
}

Write-Output "[BUILD] Done!"

$filesList = "$PSScriptRoot/(files)/auto_move/files.txt"
$stateFile = "$PSScriptRoot/(files)/auto_move/.files-state.json"

$androidRoot = "/storage/emulated/0/Android_unprotected/data/com.rockstargames.gtasa"

$state = @{}

if (Test-Path $stateFile) {
    try {
        $loadedState = Get-Content $stateFile -Raw | ConvertFrom-Json

        foreach ($property in $loadedState.PSObject.Properties) {
            $state[$property.Name] = $property.Value
        }
    }
    catch {
        Write-Output "[ADB] Failed to read state file. Rebuilding all files."
    }
}

$newState = @{}

$entries = Get-Content $filesList |
Where-Object {
    $_.Trim() -ne "" -and
    -not $_.Trim().StartsWith("#")
}

foreach ($entry in $entries) {
    $parts = $entry -split "\s*>\s*", 2

    if ($parts.Count -ne 2) {
        Write-Output "[ADB] Invalid entry: $entry"
        continue
    }

    $source = $parts[0].Trim()
    $destination = $parts[1].Trim()

    $localPath = Join-Path $PSScriptRoot $source

    if (-not (Test-Path $localPath)) {
        Write-Output "[ADB] Not found: $source"
        continue
    }

    $item = Get-Item $localPath

    if ($item -is [System.IO.DirectoryInfo]) {
        $files = Get-ChildItem $localPath -File -Recurse

        foreach ($file in $files) {
            $relativePath = $file.FullName.Substring($localPath.Length).TrimStart("\", "/")
            $relativePath = $relativePath.Replace("\", "/")

            $remotePath = "$destination/$relativePath"
            $remotePath = $remotePath.Replace("\", "/")

            $fileKey = "$source/$relativePath"
            $fileKey = $fileKey.Replace("\", "/")

            $lastWriteTime = $file.LastWriteTimeUtc.Ticks
            $length = $file.Length

            $newState[$fileKey] = @{
                Length        = $length
                LastWriteTime = $lastWriteTime
            }

            $changed = $true

            if ($state.ContainsKey($fileKey)) {
                $old = $state[$fileKey]

                if (
                    [int64]$old.Length -eq $length -and
                    [int64]$old.LastWriteTime -eq $lastWriteTime
                ) {
                    $changed = $false
                }
            }

            if (-not $changed) {
                #Write-Output "[ADB] Skipping: $remotePath"
                continue
            }

            $remoteFile = "$androidRoot/$remotePath"
            $remoteDirectory = Split-Path $remoteFile -Parent
            $remoteDirectory = $remoteDirectory.Replace("\", "/")

            Write-Output "[ADB] Updating: $source/$relativePath -> $remotePath"

            & adb shell "mkdir -p '$remoteDirectory'"

            if ($LASTEXITCODE -ne 0) {
                Write-Output "[ADB] Failed creating directory!"
                Exit $LASTEXITCODE
            }

            & adb push $file.FullName $remoteFile

            if ($LASTEXITCODE -ne 0) {
                Write-Output "[ADB] Failed: $source/$relativePath"
                Exit $LASTEXITCODE
            }
        }
    }
    else {
        $remotePath = $destination.Replace("\", "/")

        $fileKey = $source.Replace("\", "/")

        $lastWriteTime = $item.LastWriteTimeUtc.Ticks
        $length = $item.Length

        $newState[$fileKey] = @{
            Length        = $length
            LastWriteTime = $lastWriteTime
        }

        $changed = $true

        if ($state.ContainsKey($fileKey)) {
            $old = $state[$fileKey]

            if (
                [int64]$old.Length -eq $length -and
                [int64]$old.LastWriteTime -eq $lastWriteTime
            ) {
                $changed = $false
            }
        }

        if (-not $changed) {
            Write-Output "[ADB] Skipping: $source"
            continue
        }

        $remoteFile = "$androidRoot/$remotePath"
        $remoteDirectory = Split-Path $remoteFile -Parent
        $remoteDirectory = $remoteDirectory.Replace("\", "/")

        Write-Output "[ADB] Updating: $source -> $remotePath"

        & adb shell "mkdir -p '$remoteDirectory'"

        if ($LASTEXITCODE -ne 0) {
            Write-Output "[ADB] Failed creating directory!"
            Exit $LASTEXITCODE
        }

        & adb push $item.FullName $remoteFile

        if ($LASTEXITCODE -ne 0) {
            Write-Output "[ADB] Failed: $source"
            Exit $LASTEXITCODE
        }
    }
}

$newState | ConvertTo-Json -Depth 5 | Set-Content $stateFile

Write-Output "[ADB] Done!"

Exit 0