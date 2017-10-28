#!/usr/bin/env bash
for d in ./*/
do 
    for i in $d*.c
    do
        echo "#########################################################"
        echo "make ref TARGET=$i"
        make ref TARGET=$i
    done

    for i in $d*.ref
    do
        echo "cp $i ../Driver/"
        cp $i ../Driver/
    done
done
