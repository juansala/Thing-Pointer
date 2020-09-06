# Juan Salazar 5/31/2018
# Uses PyEphem Package to compute the position of the ISS
# and to write it in terms of altitude (degrees above horizon, +N) and azimuth (degrees along horizon, +E) 
# Altitude & Azimuth are measured from a particular location at a particular time (the observer)
# Sends message w/ altitude and azimuth to Arduino via UART

import ephem
import serial
import math
from time import time, sleep
import requests
from datetime import datetime

ser = serial.Serial('/dev/serial0',9600) #initializes connection with the arduino
#time.sleep(5.0)

lastPullTime = 0

home = ephem.Observer()
# Long. and Lat. of observer
home.lon, home.lat = '-71.19', '42.42'
# Boston Long and Lat: '-71.19', '42.42'
# Miami Long and Lat: '-80.1918' , '25.7617'   
# Approximate elevation of observer in meters
home.elevation = 6

# NORAD TLE (Two Line Element) taken from https://www.celestrak.com/NORAD/elements/stations.txt, website updates several times daily
# TLE's were invented to package all of the important information about the Keplerian orbits of artificial satellites, 
# initially designed for 80-hole punch cards

#iss = ephem.readtle("ISS (ZARYA)",
#"1 25544U 98067A   20249.67279622  .00000384  00000-0  15023-4 0  9998",
#"2 25544  51.6467 310.1651 0002005  85.3959  52.3313 15.49214300244493"
#)

# Initialize TLE by doing GET request, parse ISS elements
r = requests.get("""https://www.celestrak.com/NORAD/elements/stations.txt""")
tle = r.text
iss = ephem.readtle(str(r.text[0:11]), str(r.text[26:95]), str(r.text[97:167]))

while True:
        if time() - lastPullTime > 30*60: # If 30 min passed since last TLE update, update it 
            r = requests.get("""https://www.celestrak.com/NORAD/elements/stations.txt""")
            #print r.text[0:167]
            iss = ephem.readtle(str(r.text[0:11]), str(r.text[26:95]), str(r.text[97:167]))
            lastPullTime = time()

	home.date = datetime.now()
	iss.compute(home)
	alt = math.degrees(iss.alt)
	az = math.degrees(iss.az)
        #ser.flushInput()
        #ser.flushOutput()
	string_pos = str(alt) + "," + str(az) + "\n"
	# print("%s %s" % (ephem.degrees(iss.sublong), ephem.degrees(iss.sublat)))
	ser.write(string_pos)
	ser.flush()
	print('Altitude: %f - Azimuth: %f' % (alt, az))
	print(home.date)
	sleep(1)
