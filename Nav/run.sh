#!/bin/bash

./ObsDoppler $1
./NavDoppler $2

matlab -nodesktop -nosplash -r "run Gapher.m;quit;"
