#!/bin/bash
# Program:
#	This program is used to update svn server

svn status | awk '$1=="?"{cmd="svn add "$2;system(cmd)}'

