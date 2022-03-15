import random
import string
import sys

nFiles = int(sys.argv[1])
nLines = int(sys.argv[2])
nChars = int(sys.argv[3])
output = sys.argv[4]
while nFiles > 0:
	counter = nLines
	with open(output+str(nFiles-1), 'w') as f:
		while counter > 0:
			line = ""
			for i in range(0, nChars):
				line += random.choice(string.ascii_letters)
			line += "\n"
			f.write(line)
			counter -= 1
	nFiles -= 1
