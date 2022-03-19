#!/bin/bash
#-*-ENCODING: UTF-8-*-
flex --header-file=scanner.h -o scanner.cpp lexico.l ;
bison -o parser.cpp --defines=parser.h parser.y
exit
