import sys
import os

print 'OK, let us start generating the files'
os.system("date")
print '\n'

bucketsize = [5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
#seeds = [1, 6, 11, 16, 21, 26, 31, 36, 41, 46, 51]
#files= [5, 5, 5, 5, 5, 5, 5, 5, 5 ,5 ,5]

seeds = [1, 95, 208, 351, 478, 589, 688, 776, 848, 905, 958]
files = [94, 113, 143, 127, 111, 99, 88, 72, 57, 53, 43]

for run in range(0, 11):
	#seeds = files[run]+1
	gridstatement = "./runDPSim.py -t beamProfile_%sK_100M.conf -i /e906/data/users/dkleinja/pythiabkg/output/E906beamProf_ignoreWARN_100M_%%s.root -o rateDep_%sK_%%s -w /e906/data/users/dkleinja/dpsim/%sK/ -s %s -n %s -g" % (bucketsize[run], bucketsize[run], bucketsize[run], seeds[run], files[run]) 
	print gridstatement
	os.system(gridstatement)
        os.system("date")
print 'OK, we are done running'
os.system("date")
