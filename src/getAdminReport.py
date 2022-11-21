#!/usr/bin/python3

# 
# This script retrieves the last admin report received by OWM
# for a wind station and decodes data from this report
# 
# getAdminReport.py <stationID> <nbr of reports>
# 
import sys
import os
import json
import time


arglen=len(sys.argv)
if (arglen==1):
    print("Usage: getAdminReport.py <stationID> [<nbr of reports>]")
    exit(0)

stationID=sys.argv[1]
if (arglen>2):
    reportNbr=int(sys.argv[2])
else:
    reportNbr=1


#################################### 
# 
# get json file from OpenWindMap
# 
#################################### 
def getDataFile(filename,stationID,offset):

    # get data from OWM
    url=" \"https://api.pioupiou.fr/v1/sigfox-messages/"+stationID+"?offset="+str(offset)+"\" "
    if os.path.exists(filename):
        os.remove(filename)
    # print("URL %s" % url)
    # print("getting %s" % filename)
    command="wget -O "+filename+url+" 1>/dev/null 2>/dev/null"
    os.system(command)

    return(os.stat(filename).st_size)


#################################### 
# 
# Parse the json file to find out admin message
# 
#################################### 
def parseFile(filename):

    found=0
    with open(filename) as f:
        content = json.load(f)
    for item in content:
        data=item['data']
        # search 12 bytes messages
        if len(data)>16:
            # print(data)
            decode(data,item['time'])
            found+=1

    return(found)
            

#################################### 
# 
# Decode the admin message
# 
#################################### 
def decode(entry,timestamp):

    print("--------------------------------------------------------")
    print(timestamp)
    print(entry)
    if len(entry)>16:
        print("Vbatt = %f  (resol 10mV)" % ((int(entry[0:2],16)+199.5)/100.0))
        print("Soft = %d"  %   int(entry[22:24],16))
        print("Sensor = %d" %  int(entry[20:22],16))
        print("Temp = %d"  %  (int(entry[18:20],16)-50.5))
        print("Vcc = %f"  %  ((int(entry[16:18],16)+199.5)/100.0))
    else:
        # first record
        print("Min = %f " % decodeSpeed(int(entry[0:2] ,16)))
        print("Avg = %f " % decodeSpeed(int(entry[4:6] ,16)))
        print("Max = %f " % decodeSpeed(int(entry[8:10],16)))
 
        print("Dir = %d " % decodeDir(int(entry[12:14],16)))
 
        # second record
        print("Min = %f " % decodeSpeed(int(entry[2:4]  ,16)))
        print("Avg = %f " % decodeSpeed(int(entry[6:8]  ,16)))
        print("Max = %f " % decodeSpeed(int(entry[10:12],16)))
 
        print("Dir = %d " % decodeDir(int(entry[14:16],16)))


  
def decodeSpeed(es):
    if es<39:
        s=es/4
    elif es<179:
        s=(es-20)/2
    elif es<219:
        s=es-100
    else:
        s=(es-160)*2
    return(s)

def decodeDir(ed):
    
    d=2*ed
    return(d)


#################################### 
# 
# Main program
# 
#################################### 
found=0
offset=0
tmpfilename="/tmp/pp"+stationID+".json"

while(found<reportNbr):
    # get data from OWM
    l=getDataFile(tmpfilename,stationID,offset)
    if (l<100): break
    # parse data
    ret=parseFile(tmpfilename)
    time.sleep(1)
    found+=ret
    offset+=100
