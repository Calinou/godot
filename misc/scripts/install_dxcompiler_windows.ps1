echo "Downloading DXCompiler..."
Invoke-WebRequest -Uri https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.7.2212/dxc_2022_12_16.zip -OutFile "$env:TEMP\dxc_2022_12_16.zip"

if (Test-Path "$PSScriptRoot\..\..\platform\windows\dxc") {
    Echo "Removing existing local DXCompiler installation in $PSScriptRoot\..\..\platform\windows\dxc ..."
    # Remove existing local DXCompiler if present to allow for updates.
    Remove-Item -Recurse -Force "$PSScriptRoot\..\..\platform\windows\dxc"
}

echo "Extracting DXCompiler..."
Expand-Archive "$env:TEMP\dxc_2022_12_16.zip" -DestinationPath "$PSScriptRoot\..\..\platform\windows\dxc"

# Remove the original archive as it's not required anymore.
Remove-Item -Recurse -Force "$env:TEMP\dxc_2022_12_16.zip"

Echo 'DirectX Shader Compiler installed successfully! You can now build Godot with Direct3D 12 support enabled by running "scons d3d12=yes".'
