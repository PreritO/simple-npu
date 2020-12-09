#!/usr/bin/python
#
# simple-npu: Example NPU simulation model using the PFPSim Framework
#
# Copyright (C) 2016 Concordia Univ., Montreal
#     Samar Abdi
#     Umair Aftab
#     Gordon Bailey
#     Faras Dewal
#     Shafigh Parsazad
#     Eric Tremblay
#
# Copyright (C) 2016 Ericsson
#     Bochra Boughzala
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
#

import sys
import csv
import operator
import itertools
import time

if __name__=="__main__":

    IngressTrace = list(csv.reader(open("IngressTrace.csv"), delimiter=","))
    SchedulerTrace = list(csv.reader(open("SchedulerTrace.csv"), delimiter=","))
    sortedSchedulerTrace = sorted(SchedulerTrace, key=lambda x: float(x[0]), reverse=False)
    # print "---- SortedSchedulerTrace Sort----"
    # for line in sortedSchedulerTrace:
    #    print line

    # Get the first instance of the key and the last instance (takes care of recirculation packets as well)
    # very inefficient 

    schedulFile = "scheduler_diff.txt"
    with open(schedulFile,"w") as schedulFileh :
        for i in range(0, len(IngressTrace)):
            first = -1
            last = -1
            for j in range(0, len(sortedSchedulerTrace)):
                #print " pkt: " + str(i) + ", trace: " + str(sortedSchedulerTrace[j][0])
                if (i != int(sortedSchedulerTrace[j][0])):
                    continue
                if (first == -1):
                    first = j
                last = j
            #print "pkt id: " + i + ", first: " + first + ", last " + last
            #schedulFileh.write("pkt id: " + str(i) + ", first: " + str(first) + ", last " + str(last) + "\n")
            if (first != -1 and last != -1):
                #schedulFileh.write("pkt id: " + str(i) + ", Ingress: " + str(sortedSchedulerTrace[first][2]) + ", Egress " + str(sortedSchedulerTrace[last][2]) + "\n")
                # just write the difference here...
                diff = int(sortedSchedulerTrace[last][2])-int(sortedSchedulerTrace[first][2])
                schedulFileh.write(str(i) + "," + str(diff) + "\n")