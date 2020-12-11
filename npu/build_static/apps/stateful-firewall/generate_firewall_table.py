#!/usr/bin/python

import sys
import csv
import operator
import itertools
import time

if __name__=="__main__":
    filename="firewall_table.txt"
    with open(filename, 'w') as file_object: 
        ## IPv4 LPM first
        # text = "insert_entry ipv4_lpm 0.0.0.0/8 set_nhop 10.0.1.1 1\n"
        # file_object.write(text)

        for i,j,k in itertools.product(range(0,32,2),range(0,32,2), range(0,32,2)):
            # if file has been successfully opened.
            # insert_entry firewall 10.0.0.5/32 forward 3 
            text = "insert_entry firewall " + "0.{0}.{1}.{2}".format(i,j,k) + " forward 3\n" 
            file_object.write(text) # write function to write text to file
    