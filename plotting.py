#!/usr/bin/env python

import sys

#Run a test with cat log1Mote5.txt | awk 'BEGIN {FS=","}{print $10; fflush()}' | ./plotting.py | ./driveGnuPlots.pl 3 500 500 50 "X" "Z" "Y"



#from the HMC5883 datasheet, for gain mode 5

HMC_SCALE = 2.56

ignore =0
moteId=[1,2,3,4,5]
hexList =[]
X_values=[]
Y_values=[]
Z_values=[]

def paddedHex ( number ):
    return "{0:#0{1}x}".format(int(number),4);


def toHex( asciiList ):    
    for i in range(0,len(asciiList),2):
	hexList.append(paddedHex(asciiList[i])+paddedHex(asciiList[i+1])[2:])
	
    return

def toInt ( hList ):
    for i in range(0,len(hList)):
	hList[i]=int(str(hList[i]),16)
        if hList[i] >= 2**15:
           hList[i] -= 2**16
    return

def arrangeValues( numList):
    for i in range(0,len(numList),3):
        X_values.append(numList[i])
	Y_values.append(numList[i+1])
	Z_values.append(numList[i+2])
    return

def scaleValues (values):
    for i in range(0,len(values)):
	values[i]=values[i]*HMC_SCALE
    return


         


for line in sys.stdin:
    
    if ignore == 0:#ignore first input
       ignore = ignore +1
    else:
       data = line.split('-')
       toHex(data)
       toInt(hexList)
       arrangeValues(hexList)
       scaleValues(X_values)
       scaleValues(Y_values)
       scaleValues(Z_values)
       for i in range(0,len(X_values)):
	   print "0:"+str(X_values[i])
           sys.stdout.flush()
           print "1:"+str(Y_values[i])
           sys.stdout.flush()
           print "2:"+str(Z_values[i])
           sys.stdout.flush()
       X_values=[]
       Y_values=[]
       Z_values=[]
       hexList=[]
    
     
    
   
    
        
    
