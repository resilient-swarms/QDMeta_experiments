

# import argparse
#
# parser = argparse.ArgumentParser(description='Process arguments.')
# parser.add_argument('-k', type=int,default=1000,
#                     help='number of data')

import numpy as np

import os

import csv
import pickle
HOME_DIR=os.environ["HOME"] 

def read_spacedelimited(path,periodicity=11):
    counter=1
    trajectories=[]
    with open(path) as f:
        reader = csv.reader(f, delimiter=" ")

        d = list(reader)
        traj=[]
        for i in range(len(d)):
            temp=np.array([float(item) for item in d[i] if item != ""],dtype=float)  # remove empty strings
            traj.append(temp)
            if counter % periodicity == 0:
                trajectories.append(traj)
                traj=[]
            counter += 1
    return trajectories

def get_normfactors_from_trajectoryfile():
    filename=HOME_DIR + "/RHex_experiments/Results/fulltrajectory_log.txt"
    trajectories=read_spacedelimited(filename)
    state_dim=5
    max_delta=np.zeros(state_dim)
    for trajectory in trajectories:

        for i in range(1,len(trajectory)):
            previous_state = trajectory[i-1]
            state=trajectory[i]
            delt=np.abs(state - previous_state)
            for i in range(state_dim):
                if delt[i] > max_delta[i]:
                    max_delta[i] = delt[i]
    print(max_delta)
    return max_delta

def read(path):
    with open(path) as f:
        reader = csv.reader(f, delimiter=" ")

        d = list(reader)
        traj=[]
        for i in range(len(d)):
            temp=np.array([float(item) for item in d[i] if item != ""],dtype=float)  # remove empty strings
            traj.append(temp)
        return traj

def get_batches():
    delta_trajectories=[]
    for i in range(0,10):
        filename = HOME_DIR + "/RHex_experiments/centroid_trajectories"+str(i)+".txt"
        batch = read(filename)
        pickle.dump(batch,open("batch"+str(i)+"","wb"))

if __name__ == "__main__":
    get_batches()
    #N=100000
    #batch_size=10000
    #BD=50
    #centroids=4096
    #os.system("python /home/david/sferes2/modules/cvt_map_elites/cvt.py -p="+str(N)+" -b="+str(batch_size)+" -d="+str(BD)+" -k="+str(centroids)+" -l="+HOME_DIR + "/RHex_experiments/scripts/batch")