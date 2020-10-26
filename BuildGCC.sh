#!/bin/sh

set -e

TIMEFORMAT="took %Rs"

if [ "$1" == "Debug" ]; then
    FLAGS="-DLC_DEBUG -g"
elif [ "$1" == "Release" ]; then
    FLAGS="-DLC_RELEASE -O2 -msse2"
else
    printf "\033[31mInvalid configuration.\033[0m\n"
    printf "\033[31mUsage:\033[0m"
    printf "\033[31m    ./BuildGCC Debug\033[0m\n"
    printf "\033[31m        or\033[0m\n"
    printf "\033[31m    ./BuildGCC Release\033[0m\n"
    exit
fi

printf "\033[35mBuilding lcddl...\033[0m\n"
pushd lcddl > /dev/null
time gcc -O2 lcddl.c lcddlUserLayer.c -o ../bin/lcddl
popd > /dev/null
printf "\033[32mDone!\n\033[0m\n"

printf "\033[35mRunning lcddl...\033[0m\n"
time bin/lcddl Client/Assets/Meta/*.lcd
printf "\033[32mDone!\n\033[0m\n"

printf "\033[35mBuilding asset packer...\033[0m\n"
pushd AssetPacker > /dev/null
time gcc -g lcap.c -lm -o ../bin/AssetPacker
popd > /dev/null
printf "\033[32mDone!\n\033[0m\n"

printf "\033[35mPacking assets...\033[0m\n"
pushd bin > /dev/null
time ./AssetPacker assets.data
popd > /dev/null
printf "\033[32mDone!\n\033[0m\n"

printf "\033[35mBuilding lucerna...\033[0m\n"
time gcc -IClient/Source -IEngine/Source -IEngine/Include -DLC_PLATFORM_LINUX -Wall -pedantic $FLAGS Engine/Source/Lucerna.c Client/Source/Main.c -ldl -lpthread -lxcb -lX11 -lX11-xcb -lGL -lm -lasound -o bin/LucernaProject
printf "\033[32mDone!\n\033[0m\n"

exit 0
