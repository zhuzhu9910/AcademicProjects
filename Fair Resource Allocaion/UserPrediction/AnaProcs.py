import operator
import re
import os
import simplejson as json
from multiprocessing import Process, Lock 

FIELD_TIME = 0
FIELD_EVENT = 3
FIELD_USER = 4




if __name__ == '__main__':
    rdir = './data'
    utrack = dict()

    for dfile in os.listdir(rdir):
        if dfile.endswith('.csv'):
            f = open(rdir + '/' + dfile, 'r')
            for line in f:
                fields = line.split(',')
                if fields[FIELD_EVENT] == '0':
                    if fields[FIELD_USER] in utrack:
                        utrack[fields[FIELD_USER]].append(fields[FIELD_TIME])
                    else:
                        utrack[fields[FIELD_USER]] = [fields[FIELD_TIME]]
            f.close()
            
    
    fo = open('user_jobtime','w')
    fo.write(json.dumps(utrack))
    fo.close()


