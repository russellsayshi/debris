#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd "$DIR"
gcc -lncurses debris.c -o debris
gcc rmt.c -o rmt
