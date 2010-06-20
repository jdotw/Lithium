#!/bin/sh

grep -R i_debug * | grep '\.c:' | grep -v check_debug | grep -v 'printf.c:'
