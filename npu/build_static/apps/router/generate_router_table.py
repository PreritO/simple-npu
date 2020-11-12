#!/usr/bin/python

import sys
import csv
import operator
import itertools
import time

if __name__=="__main__":
    filename="router_table.txt"
    with open(filename, 'w') as file_object: 
        ## IPv4 LPM first
        text = "insert_entry ipv4_lpm 0.0.0.0/8 set_nhop 10.0.1.1 1\n"
        file_object.write(text)

        for i,j,k in itertools.product(range(0,32,2),range(0,32,2), range(0,32,2)):
            # if file has been successfully opened. 
            text = "insert_entry ipv4_lpm " + "0.{0}.{1}.{2}/32".format(i,j,k) + " set_nhop 10.0.1.2    2\n" 
            file_object.write(text) # write function to write text to file
        
        #Forward
        text = "\n\n\ninsert_entry forward 10.0.1.1 set_dmac FF:FF:FF:FF:FF:01"
        file_object.write(text)
        text = "\ninsert_entry forward 10.0.1.2 set_dmac BB:BB:BB:BB:BB:01"
        file_object.write(text)
        
        text = "\n\n\ninsert_entry send_frame 1'2 rewrite_mac AA:AA:AA:AA:AA:01"
        file_object.write(text)
        text = "\n\n\ninsert_entry send_frame 2'2 rewrite_mac AA:AA:AA:AA:AA:02"
        file_object.write(text)