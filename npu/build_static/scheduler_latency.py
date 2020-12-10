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
    processedPDs = {}
    with open(schedulFile,"w") as schedulFileh :
        for i in range(0, len(IngressTrace)):
            # keep running total here for each packet here
            timeInSched = 0
            for j in range(0, len(sortedSchedulerTrace)):
                #print " pkt: " + str(i) + ", trace: " + str(sortedSchedulerTrace[j][0])
                if (i != int(sortedSchedulerTrace[j][0])):
                    continue
                if (i not in processedPDs):
                    processedPDs[i] = sortedSchedulerTrace[j][2]
                    continue
                else:
                    timeInSched += float(sortedSchedulerTrace[j][2]) - float(processedPDs[i])
                    processedPDs.pop(i)
                    
            schedulFileh.write(str(i) + "," + str(timeInSched) + "\n")