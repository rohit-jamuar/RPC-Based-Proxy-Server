#!/usr/bin/python

from sys import argv
with open(argv[1],'r') as fi:
	data,fin={},[]
	for line in fi:
		if len(line)>1 and 'curl_easy_perform()' not in line:
			val=line[line.index('(')+5:line.index(')')]
			data[val]=data.get(val,[])+[float(line[line.index('= ')+2:])]
	for i in sorted(data):
		k=sum(data[i])/len(data[i])
		fin.append(k)
		print 'Average time taken to serve ',i,' = ',k,"s"
	if fin:
		print '\nNet average time taken to serve = ',sum(fin)/len(fin),'s'	
