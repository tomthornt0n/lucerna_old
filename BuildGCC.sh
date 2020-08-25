#!/bin/sh

set -e

build_and_run_lcddl(){
    echo -e "\033[35mBuilding lcddl...\033[0m"
    pushd lcddl
    gcc -O2 lcddl.c lcddlUserLayer.c -o ../bin/lcddl
    popd
    echo -e "\033[32mDone!\n\033[0m"

    echo -e "\033[35mRunning lcddl...\033[0m"
    bin/lcddl Client/Assets/Meta/*.lcd
    echo -e "\033[32mDone!\n\033[0m"
}

build_debug(){
    build_and_run_lcddl
    echo -e "\033[35mBuilding lucerna...\033[0m"
    gcc -IEngine/Vendor/glad/include -IClient/Source -IEngine/Source -IEngine/Include -DLC_PLATFORM_LINUX -DLC_DEBUG  -LEngine/Vendor/glad/bin -std=c89 -g Engine/Source/Lucerna.c Client/Source/Main.c -ldl -lpthread -lxcb -lX11 -lX11-xcb -lGL -o bin/LucernaProject
    echo -e "\033[32mDone!\n\033[0m"
}

build_release(){
    build_and_run_lcddl
    echo -e "\033[35mBuilding lucerna...\033[0m"
    gcc -IEngine/Vendor/glad/include -IClient/Source -IEngine/Source -IEngine/Include -DLC_PLATFORM_LINUX -DLC_RELEASE -LEngine/Vendor/glad/bin -std=c89 -O2 Engine/Source/Lucerna.c Client/Source/Main.c -ldl -lpthread -lxcb -lX11 -lX11-xcb -lGL -o bin/LucernaProject
    echo -e "\033[32mDone!\n\033[0m"
}


if [ "$1" == "Debug" ]; then
    TIMEFORMAT="compiled in %Rs"; time build_debug
elif [ "$1" == "Release" ]; then
    TIMEFORMAT="compiled in %Rs"; time build_release
else
    echo -e "\033[31mInvalid configuration.\033[0m"
    echo -e "\033[31mUsage:\033[0m"
    echo -e "\033[31m    ./BuildGCC Debug\033[0m"
    echo -e "\033[31m        or\033[0m"
    echo -e "\033[31m    ./BuildGCC Release\033[0m"
    exit
fi

exit 0
