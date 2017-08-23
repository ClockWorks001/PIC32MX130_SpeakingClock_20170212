# coding: UTF-8
import os
import glob
# import sys
# import re
#カレントフォルダ
path = os.getcwd()
print(path)
files = glob.glob(path + "/*.wav")
for file in files:
    os.rename(file, os.path.join(path, 'S_' + os.path.basename(file)))

