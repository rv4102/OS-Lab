import os
import glob

folder_name = "Assgn1_24_20CS10056_20CS10087_20CS30030_20CS30045"
os.makedirs(folder_name)
for folder in glob.glob('*'):
    for file in glob.glob(os.path.join(folder, '*.sh')):
        os.system('cp ' + file + ' ./'+folder_name+'/')

os.system('zip -r ' + folder_name + '.zip ' + folder_name)
os.system('rm -rf ' + folder_name)