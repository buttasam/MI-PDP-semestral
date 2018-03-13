#!/bin/bash
FILES=data/*
for f in $FILES
do
  ./a.out < $f
done