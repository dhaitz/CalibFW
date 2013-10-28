#!/bin/bash

GT=$1

for i in ${GT}*
do
  a=${i/${GT}_/}
  a=${a/.txt/}
  l=${a%%_*}
  a=${a#*_}
  git mv $i "${GT}_${a}_${l}.txt"
  #echo $i $a $l  "${GT}_${a}_${l}.txt"
done
