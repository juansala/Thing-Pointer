import requests
import json
import datetime

r = requests.get("""https://www.celestrak.com/NORAD/elements/stations.txt""")
#response = json.loads(r)
print str(r.text[0:167])
print "Header"
print r.text[0:11]
print "First Col"
print r.text[26:95]
print "Second Col"
print r.text[97:167]

