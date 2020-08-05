#!/bin/sh

set -e

build_lcddl(){
    echo -e "\033[35mBuilding lcddl...\033[0m"
    pushd lcddl
    gcc -O2 lcddl.c lcddlUserLayer.c -o ../bin/lcddl
    popd
    echo -e "\033[32mDone!\n\033[0m"
}

run_lcddl(){
    echo -e "\033[35mRunning lcddl...\033[0m"
    bin/lcddl Client/Assets/Meta/*.lcd
    echo -e "\033[32mDone!\n\033[0m"
}

build_glad(){
    echo -e "\033[35mBuilding glad...\033[0m"
    pushd Engine/Vendor/glad
    gcc -c -O2 -Iinclude src/glad.c -ldl -o bin-int/glad.o
    ar rcs bin/libglad.a bin-int/glad.o
    popd
    echo -e "\033[32mDone!\n\033[0m"
}

build_glfw_debug(){
    echo -e "\033[35mBuilding glfw...\033[0m"
    pushd Engine/Vendor/glfw
    gcc -c -D_GLFW_X11 -g src/egl_context.c -o bin-int/egl_context.o
    gcc -c -D_GLFW_X11 -g src/context.c -o bin-int/context.o
    gcc -c -D_GLFW_X11 -g src/init.c -o bin-int/init.o
    gcc -c -D_GLFW_X11 -g src/input.c -o bin-int/input.o
    gcc -c -D_GLFW_X11 -g src/monitor.c -o bin-int/monitor.o
    gcc -c -D_GLFW_X11 -g src/osmesa_context.c -o bin-int/osmesa_context.o
    gcc -c -D_GLFW_X11 -g src/vulkan.c -o bin-int/vulkan.o
    gcc -c -D_GLFW_X11 -g src/window.c -o bin-int/window.o
    gcc -c -D_GLFW_X11 -g src/glx_context.c -o bin-int/glx_context.o
    gcc -c -D_GLFW_X11 -g src/linux_joystick.c -o bin-int/linux_joystick.o
    gcc -c -D_GLFW_X11 -g src/posix_thread.c -o bin-int/posix_thread.o
    gcc -c -D_GLFW_X11 -g src/posix_time.c -o bin-int/posix_time.o
    gcc -c -D_GLFW_X11 -g src/x11_init.c -o bin-int/x11_init.o
    gcc -c -D_GLFW_X11 -g src/x11_monitor.c -o bin-int/x11_monitor.o
    gcc -c -D_GLFW_X11 -g src/x11_window.c -o bin-int/x11_window.o
    gcc -c -D_GLFW_X11 -g src/xkb_unicode.c -o bin-int/xkb_unicode.o
    ar rcs bin/libglfw.a bin-int/*.o
    popd
    echo -e "\033[32mDone!\n\033[0m"
}

build_glfw_release(){
    echo -e "\033[35mBuilding glfw...\033[0m"
    pushd Engine/Vendor/glfw
    gcc -c -D_GLFW_X11 -O2 src/egl_context.c -o bin-int/egl_context.o
    gcc -c -D_GLFW_X11 -O2 src/context.c -o bin-int/context.o
    gcc -c -D_GLFW_X11 -O2 src/init.c -o bin-int/init.o
    gcc -c -D_GLFW_X11 -O2 src/input.c -o bin-int/input.o
    gcc -c -D_GLFW_X11 -O2 src/monitor.c -o bin-int/monitor.o
    gcc -c -D_GLFW_X11 -O2 src/osmesa_context.c -o bin-int/osmesa_context.o
    gcc -c -D_GLFW_X11 -O2 src/vulkan.c -o bin-int/vulkan.o
    gcc -c -D_GLFW_X11 -O2 src/window.c -o bin-int/window.o
    gcc -c -D_GLFW_X11 -O2 src/glx_context.c -o bin-int/glx_context.o
    gcc -c -D_GLFW_X11 -O2 src/linux_joystick.c -o bin-int/linux_joystick.o
    gcc -c -D_GLFW_X11 -O2 src/posix_thread.c -o bin-int/posix_thread.o
    gcc -c -D_GLFW_X11 -O2 src/posix_time.c -o bin-int/posix_time.o
    gcc -c -D_GLFW_X11 -O2 src/x11_init.c -o bin-int/x11_init.o
    gcc -c -D_GLFW_X11 -O2 src/x11_monitor.c -o bin-int/x11_monitor.o
    gcc -c -D_GLFW_X11 -O2 src/x11_window.c -o bin-int/x11_window.o
    gcc -c -D_GLFW_X11 -O2 src/xkb_unicode.c -o bin-int/xkb_unicode.o
    ar rcs bin/libglfw.a bin-int/*
    popd
    echo -e "\033[32mDone!\n\033[0m"
}

build_lucerna_debug(){
    echo -e "\033[35mBuilding lucerna...\033[0m"
    gcc -IEngine/Vendor/glfw/include -IEngine/Vendor/glad/include -IClient/Source -IEngine/Source -IEngine/Include -DGLFW_INCLUDE_NONE -DLC_PLATFORM_LINUX -DLC_DEBUG  -LEngine/Vendor/glfw/bin -LEngine/Vendor/glad/bin -std=c89 -g Engine/Source/Lucerna.c Client/Source/Main.c -ldl -lm -lpthread -lX11 -lGL -lglfw -lglad -o bin/LucernaProject
    echo -e "\033[32mDone!\n\033[0m"
}

build_lucerna_release(){
    echo -e "\033[35mBuilding lucerna...\033[0m"
    gcc -IEngine/Vendor/glfw/include -IEngine/Vendor/glad/include -IClient/Source -IEngine/Source -IEngine/Include -DGLFW_INCLUDE_NONE -DLC_PLATFORM_LINUX -DLC_RELEASE -LEngine/Vendor/glfw/bin -LEngine/Vendor/glad/bin -std=c89 -O2 Engine/Source/Lucerna.c Client/Source/Main.c -ldl -lm -lpthread -lX11 -lGL -lglfw -lglad -o bin/LucernaProject
    echo -e "\033[32mDone!\n\033[0m"
}

#------------------------------------------------------------------------------

debug(){
    build_lcddl
    build_glad
    build_glfw_debug
    run_lcddl
    build_lucerna_debug
}

release(){
    build_lcddl
    build_glad
    build_glfw_release
    run_lcddl
    build_lucerna_release
}


if [ "$1" == "Debug" ]; then
    TIMEFORMAT="compiled in %Rs"; time debug
elif [ "$1" == "Release" ]; then
    TIMEFORMAT="compiled in %Rs"; time release
else
    echo -e "\033[31mInvalid configuration.\033[0m"
    echo -e "\033[31mUsage:\033[0m"
    echo -e "\033[31m    ./BuildGCC Debug\033[0m"
    echo -e "\033[31m        or\033[0m"
    echo -e "\033[31m    ./BuildGCC Release\033[0m"
    exit
fi

exit 0
