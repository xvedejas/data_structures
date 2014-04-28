#!/usr/bin/env bash
set -o nounset
set -o errexit

gcc -pipe -m64 -std=c99 -fPIC -g -O3 -fstack-protector -Wl,-z,relro -Wl,-z,now \
    -fvisibility=internal -W -Wall -Wno-unused-parameter -Wno-unused-function \
    -Wno-unused-label -Wpointer-arith -Wformat -Wreturn-type -Wsign-compare \
    -Wmultichar -Wformat-nonliteral -Winit-self -Wuninitialized -Wno-deprecated\
    -Wformat-security -Werror data_structures.c -lcunit -o data_structures_test

# In future versions of GCC, -fdiagnostics-color=auto
