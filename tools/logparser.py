# Released under GPLv2
# Berardi Davide, Matteo Martelli, Marco Melletti, Federico Montori

import sys
import re

def parsefile(p, avgp):
	f = open(p, "r")
	i = 0
	for l in f.readlines():
		num = float(l.split(':')[-1].rstrip())
		try:
			avgp[i] += num
		except:
			avgp.append(num)
		i += 1
	f.close()
avgprobs = []
size = len(sys.argv[1::])
for path in sys.argv[1::]:
	parsefile(path, avgprobs)
for e in avgprobs:
	print(e / size)
