#!/usr/bin/env bash
a=$(which gcc)
${a} run.c -o lash.run
./lash.run