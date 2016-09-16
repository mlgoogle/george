#!/usr/bin/python2.6  
# -*- coding: utf-8 -*-  
#encoding=utf-8

'''
Created on 2015年9月9日

@author: kerry
'''
import codecs
import os
import random
import re
import sys
import cpplint

def __init__():
    cpplint.ResetNolintSuppressions()
    cpplint._cpplint_state.ResetErrorCounts()

def checkstyle(check_file,verbose_level):
    cpplint.ProcessFile(check_file,verbose_level)

if __name__ == '__main__':
    __init__()
    print "start...."
    for root, dirs, files in os.walk("./"):
        for name in files:
            ext = os.path.splitext(name)[1]
            if  ext  in ('.h', '.cc', '.cpp'):
                checkstyle(os.path.join(root, name),cpplint._cpplint_state.verbose_level)
    cpplint._cpplint_state.PrintErrorCounts()