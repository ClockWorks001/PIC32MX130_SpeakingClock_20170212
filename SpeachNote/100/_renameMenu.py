# coding: UTF-8
import sys
import os
import glob
import re
a = 1
#指定するフォルダ
path = os.getcwd()
print(path)
files = glob.glob(path + "/*.wav")
for file in files:
    os.rename(file, os.path.join(path, 'LcdM' + os.path.basename(file)))

