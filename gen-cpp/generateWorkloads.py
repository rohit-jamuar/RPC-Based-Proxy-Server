#!/usr/bin/python

from random import choice

l=["http://www.google.com","http://www.bing.com","http://www.yahoo.com","http://www.hotmail.com","http://www.ebay.com","http://www.gmail.com","http://www.yelp.com","http://www.mit.edu","http://www.amazon.com","http://www.youtube.com"]

with open("workloadA",'w') as fo:
	for i in range(10):
		for j in l:
			fo.write(j+"\n")
			
with open("workloadB",'w') as fo:
	for i in range(100):
		fo.write(choice(l)+"\n")
