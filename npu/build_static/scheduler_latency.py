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