#!/usr/bin/env bash

GAME_INVADER_ENTRY="invaders-sample"
TEST_ENTRY="process"

binary=""

print_possibility()
{
    echo "Usage: ./run [invader]/[test]"
    echo ""
    echo -e "\t- invader: game sample."
    echo -e "\t- test: test sample displaying camera with arrow overlay."
}

case $1 in
    "invader"|"invaders"|"game")
        binary="$GAME_INVADER_ENTRY"
    ;;
    "process"|"test")
        binary="$TEST_ENTRY"
    
    ;;
    *)
        echo -e "Invalid Argument: this sample does not exist.\n"
        print_possibility
        exit 1
    ;;
esac

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make

./build/"$binary"
