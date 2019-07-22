#!/usr/bin/env bash
make
rm namedb
./mmaparray --index 20 --create
./mmaparray --index 10 --name Minas --age 10 --set
./mmaparray --i 4 --n Guston --a 110 --set
./mmaparray --ind 3 --nam George --ag 3 --se --p
strings namedb
./mmaparray --ind 3 --get
./mmaparray --ind 10 --del --p