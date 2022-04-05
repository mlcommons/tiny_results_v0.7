#!/bin/bash
ls -R | grep "^[.]/" | sed -e "s/:$//" -e "s/[^\/]*\//    /g" -e "s/^//" -e "s/\(.*\)    /\1+-- /" > struct.txt
