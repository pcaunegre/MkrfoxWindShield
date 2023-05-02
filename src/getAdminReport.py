#!/usr/bin/python3

# 
# This script retrieves the last admin report received by OWM
# for a wind station and decodes data from this report
# 
# getAdminReport.py <stationID> [<nbr of reports>] [-all] [-csv <file>]
# 
import sys
import os
import json
import time
import re
import datetime

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
    # command="wget -O "+filename+url+" 1>/dev/null 2>/dev/null"
    command="wget -O "+filename+url+" 1>/dev/null 2>/tmp/errmsg"
    os.system(command)
    print(url)
    err=open("/tmp/errmsg").read()
    err=re.sub('\n','',err,re.IGNORECASE)
    if re.match(".*ERROR.*",err):
        print("server returns: %s" % ret)

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
        # report all measures
        if reportall:
            decode(data,item['time'])
        else:
            # report only admin reports
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
    
    global ln, tim0, firstitem, itemnbr, curday, dailyrep
    print("--------------------------------------------------------")
    tim1=datetime.datetime.strptime(timestamp,'%Y-%m-%dT%H:%M:%S.%fZ')
    
    if itemnbr>1:
        dt=tim0-tim1
        if curday==datetime.datetime.strftime(tim1,'%m%d'):
            dailyrep+=1
        else:
            dailyrep=1
        print("%s  %d  %d  %d" % (timestamp,dailyrep,itemnbr,dt.total_seconds()))
    else:
        dailyrep=1
        print("%s  %d  %d" %     (timestamp,dailyrep,itemnbr))
    
    curday=datetime.datetime.strftime(tim1,'%m%d')
    tim0=tim1
    itemnbr+=1
    print(entry)
#     print("len=%d" % len(entry))
    if len(entry)>16:
        rnb=int(entry[14:16],16)-1
        if rnb==0:
            print("REPORT Number %d (Start-Up)" % rnb)
        else:
            print("REPORT Number %d " % rnb)
        print("vin Max = %1.2f " % ((int(entry[0:2],16)+250)/100.0))
        print("vin Avg = %1.2f " % ((int(entry[4:6],16)+250)/100.0))
        print("vin Min = %1.2f " % ((int(entry[2:4],16)+250)/100.0))
        
        print("Tmp Max = %1.1f " % ((int(entry[6:8],16)-50.5))) 
        print("Tmp Avg = %1.1f " % ((int(entry[10:12],16)-50.5)))
        print("Tmp Min = %1.1f " % ((int(entry[8:10],16)-50.5)))
        
        print("Samples = %d " % int(entry[12:14],16))
 
        print("Vin     = %1.2f  (resol 10mV)"  % ((int(entry[16:18],16)+250)/100.0))
        print("Vcc     = %1.2f  (resol 10mV)"  % ((int(entry[18:20],16)+250)/100.0))
        print("Temp    = %1.1f"  %  (int(entry[20:22],16)-50.5))
        b=int(entry[22:24],16)
        print("Sensor  = %d"  %   (b>>6))
        print("Soft    = %d"  %   (b&63))
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
 
        if printcsv:
            print("%d, %f, %f, %f," % (ln,decodeSpeed(int(entry[0:2],16)), \
                decodeSpeed(int(entry[4:6]  ,16)), \
                decodeSpeed(int(entry[8:10],16))),file=fout)
            ln=ln+1
            print("%d, %f, %f, %f," % (ln,decodeSpeed(int(entry[2:4],16)), \
                decodeSpeed(int(entry[6:8]  ,16)), \
                decodeSpeed(int(entry[10:12],16))),file=fout)
            ln=ln+1

  
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

# 
#  args reading
# 
printcsv=0
arglen=len(sys.argv)
if (arglen==1):
    print("Usage: getAdminReport.py <stationID> [<nbr of reports>] [-all] [-csv <file>]")
    exit(0)

stationID=sys.argv[1]
reportall=0
if (arglen>3):
    if (sys.argv[3]=="-all"): reportall=1  

if (arglen>5):
    if (sys.argv[4]=="-csv"): 
        csvoutfile=sys.argv[5]
        printcsv=1

if (arglen>2):
   reportNbr=int(sys.argv[2])
else:
    reportNbr=1

# 
#  main
# 
found=0
offset=0
ln=0
itemnbr=1
tmpfilename="/tmp/pp"+stationID+".json"

if printcsv:
    fout=open(csvoutfile,'w')    
    print("N, Min, Avg, Max",file=fout)

while(found<reportNbr):
    # get data from OWM
    l=getDataFile(tmpfilename,stationID,offset)
    if (l<100): 
        print("Reached void logs, exiting")
        break
    # parse data
    ret=parseFile(tmpfilename)
    time.sleep(1)
    found+=ret
    offset+=100
