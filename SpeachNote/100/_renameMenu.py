# coding: UTF-8
import sys
import os
import glob
import re
a = 1
#�w�肷��t�H���_
path = os.getcwd()
print(path)
files = glob.glob(path + "/*.wav")
for file in files:
    os.rename(file, os.path.join(path, 'LcdM' + os.path.basename(file)))

