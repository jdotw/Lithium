#!/bin/sh

cat template.h | sed "s/template/$1/g" > $1.h
cat template.c | sed "s/template/$1/g" > $1.c
cat template_item.c | sed "s/template/$1/g" > $1_item.c
cat template_objfact.c | sed "s/template/$1/g" > $1_objfact.c

