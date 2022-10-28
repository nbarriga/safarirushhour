#!/bin/sh
make
make lib
mv safari.a ../SafariRushHourEPCG/
cd ../SafariRushHourEPCG/
make clean
make
