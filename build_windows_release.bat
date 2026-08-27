@echo off
echo ========================================================
echo   SecureBrowser Windows x64 Build and Packaging Script
echo ========================================================

:: Check for CMake
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [Error] CMake is not installed or not in PATH.
    exit /b 1
)

:: Check for NSIS
where makensis >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [Error] NSIS (makensis) is not installed or not in PATH.
    echo Please install NSIS from https://nsis.sourceforge.io/ to create the installer.
    exit /b 1
)

echo [1/3] Generating CMake Build Files...
cd Browser
if not exist build mkdir build
cd build
cmake .. -A x64
if %ERRORLEVEL% NEQ 0 (
    echo [Error] CMake generation failed.
    exit /b 1
)

echo [2/3] Compiling Release Binaries...
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [Error] Compilation failed.
    exit /b 1
)
cd ..

echo [3/3] Building NSIS Installer...
cd packaging
makensis installer.nsi
if %ERRORLEVEL% NEQ 0 (
    echo [Error] Installer packaging failed.
    exit /b 1
)

echo ========================================================
echo   Success! Installer created at:
echo   Browser\packaging\SecureBrowser_Setup_x64.exe
echo ========================================================
exit /b 0
