:: Configuration
set "BUILD_DIR=build"
set "VCPKG_DIR=C:/vcpkg"
set "PROJECT_NAME=ogl_engine"
set "EXECUTABLE=%PROJECT_NAME%.exe"
set "DEBUG_WORKDIR=%BUILD_DIR%\Debug"
set "RELEASE_WORKDIR=%BUILD_DIR%\Release"
set "DEFAULT_WORKDIR=%~dp0"

:: Menu
:menu
cls
@echo ===========================
@echo       Build Menu
@echo ===========================
@echo [1] Build Debug
@echo [2] Build Release
@echo [3] Clean Data
@echo [4] Run in Debug mode
@echo [5] Run in Release mode
@echo [9] Clean Builds
@echo [0] Exit
@echo.

@set /p choice="Choose an option: "

@if "%choice%"=="1" @goto build_debug
@if "%choice%"=="2" @goto build_release
@if "%choice%"=="3" @goto clean_data
@if "%choice%"=="4" @goto run_debug
@if "%choice%"=="5" @goto run_release
@if "%choice%"=="9" @goto clean_builds
@if "%choice%"=="0" exit
@goto menu

:build_debug
@echo Building Debug...
@cmake -G "Visual Studio 17 2022" -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=%VCPKG_DIR%/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
@cmake --build %BUILD_DIR% --config Debug
@goto done

:build_release
@echo Building Release...
@cmake -G "Visual Studio 17 2022" -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=%VCPKG_DIR%/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
@cmake --build %BUILD_DIR% --config Release
@goto done

:clean_data
@echo Cleaning data directory...
@rmdir /S /Q "%BUILD_DIR%/Debug/data"
@rmdir /S /Q "%BUILD_DIR%/Release/data"
@goto done

:clean_builds
@echo Cleaning Builds
@rmdir /S /Q %BUILD_DIR%
@goto done

:run_debug
cls
@if exist %DEBUG_WORKDIR% (
    @echo Running %EXECUTABLE%...
    @cd %DEBUG_WORKDIR%
    @echo =======================
    @%EXECUTABLE%
    @echo =======================
    @cd %DEFAULT_WORKDIR%
) else (
    @echo Debug Executable not found. Build the project in debug mode first.
)
@pause
@goto menu

:run_release
cls
@if exist %RELEASE_WORKDIR% (
    @echo Running %EXECUTABLE%...
    @cd %RELEASE_WORKDIR%
    @echo =======================
    @%EXECUTABLE%
    @echo =======================
    @cd %DEFAULT_WORKDIR%
) else (
    @echo Release Executable not found. Build the project in release mode first.
)
@pause
@goto menu

:done
@echo.
@echo Done.
@pause
@echo.
@goto menu