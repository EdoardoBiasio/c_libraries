#!/bin/bash

cd visualization/
make
cd ..
cd data_structures/
make
cd ..
cd numerical/
make
cd ..
cd memory/
make
cd ..
gcc test.c -o test -lclib_num -lclib_ds -lclib_vis -lclib_mem -Ldata_structures -Lnumerical -Lmemory -Lvisualization -lX11 -lm

