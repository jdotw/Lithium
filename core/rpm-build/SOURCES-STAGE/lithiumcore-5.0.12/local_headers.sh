#!/bin/sh

sed -i '' -e 's/^\#include <induction\(.*\)>/#include "induction\1"/g' *.c
sed -i '' -e 's/^\#include <lithium\/\(.*\)>/#include "device\/\1"/g' *.c
