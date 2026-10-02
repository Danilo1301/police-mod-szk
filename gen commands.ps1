& "C:\android-ndk-r27d\build\ndk-build.cmd" `
    APP_BUILD_SCRIPT="$PSScriptRoot/Android.mk" `
    NDK_APPLICATION_MK="$PSScriptRoot/Application.mk" `
    NDK_PROJECT_PATH="$PSScriptRoot" `
    compile_commands.json