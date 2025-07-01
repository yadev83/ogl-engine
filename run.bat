:: Configuration
set "BUILD_DIR=build"
set "VCPKG_DIR=C:/vcpkg"
set "PROJECT_NAME=ogl-engine"
set "EXECUTABLE=%PROJECT_NAME%.exe"
set "WORKDIR=%BUILD_DIR%\Debug"

@echo Building Debug...
@cmake -G "Visual Studio 17 2022" -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=%VCPKG_DIR%/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
@cmake --build %BUILD_DIR% --config Debug

@if exist %WORKDIR% (
    @echo Running %EXECUTABLE%...
    @cd %WORKDIR%
    @echo =======================
    @%EXECUTABLE%
    @echo =======================
    @cd %DEFAULT_WORKDIR%
) else (
    @echo Debug Executable not found. Build the project in debug mode first.
)