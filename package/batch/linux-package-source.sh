#!/bin/bash
# package
echo "... directory ..."
rm  -rf ${PWD}/../source
rm  -f  ${PWD}/../source-linux.7z
echo "... export ..."
git clone $1 ${PWD}/../source
rm  -rf ${PWD}/../source/.git
echo "... tar ..."
7z  a -r ${PWD}/../source-linux.7z ${PWD}/../source/*
echo "... directory cleanup .."
rm -rf ${PWD}/../source
echo "... finished ..."
