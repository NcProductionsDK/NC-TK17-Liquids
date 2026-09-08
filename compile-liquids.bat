@echo off
setlocal

set "GCC=C:\msys64\mingw32\bin\gcc.exe"
set "PATH=C:\msys64\mingw32\bin;%PATH%"
set "PROJECT_DIR=%~dp0"
set "BUILD_DIR=%PROJECT_DIR%build"

if not exist "%GCC%" (
  echo MinGW GCC was not found at "%GCC%".
  exit /b 1
)

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

if not defined LIQUIDS_OUTPUT set "LIQUIDS_OUTPUT=%BUILD_DIR%\NC-TK17-Liquids.dll"

"%GCC%" ^
  -m32 ^
  -shared ^
  -O2 ^
  -s ^
  -Wall ^
  -Wextra ^
  -static-libgcc ^
  -o "%LIQUIDS_OUTPUT%" ^
  "%PROJECT_DIR%NC-TK17-Liquids.c" ^
  -lole32

if errorlevel 1 (
  echo Compilation failed.
  exit /b 1
)

echo Built "%LIQUIDS_OUTPUT%".
