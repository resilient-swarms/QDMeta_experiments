

# import argparse
#
# parser = argparse.ArgumentParser(description='Process arguments.')
# parser.add_argument('-k', type=int,default=1000,
#                     help='number of data')

import numpy as np
import pickle
import os
from matplotlib import pyplot as PLT
home=os.environ["HOME"] + "/dynamicplanar_metacmaes"

N = 1000000  # as usual take 100000 datapoints
batch_size = 20000  # avoid too big memory requirement
centroids = 4096
prefix = home + "/meta-cmaes/centroids/batch"
L=0.62  # full length
S = 0.0775 # segment length
import sys


#https://math.stackexchange.com/questions/1365622/adding-two-polar-vectors
def sum_polar(r,theta,r2,theta2):
    r_summed = np.sqrt(r ** 2 + r2 ** 2 + 2 * r * r2 * np.cos(theta2 - theta))
    theta_summed = theta+np.arctan2(r2*np.sin(theta2 -theta), r+r2*np.cos(theta2 - theta))
    return r_summed, theta_summed

def print_final_trajectory(trajectory,i):
    colors=["grey","red","green","blue"]
    legend_labels=["t=0","t=1","t=2","t=3"]

    # starting position
    x=[0.0 for j in range(9)]
    y=[-j*S for j in range(9)]
    PLT.xlim(-L, L)
    PLT.ylim(0, -L)
    lines=PLT.plot(x, y, marker="o", color=colors[0])
    for t in range(1,4):
        print("at time step t=", t)
        start=(t-1)*16
        x=[0.0]
        y=[0.0]
        for j in range(0,8):
            r_index = start + j*2
            theta_index = start + j*2 + 1
            r = trajectory[r_index]
            theta=trajectory[theta_index]
            xx=r*np.cos(theta)
            yy=r*np.sin(theta)
            print("polar coord ", r, " ", theta)
            print("xy coord ", xx, " ", yy)
            x.append(xx)
            y.append(yy)
        lines.append(PLT.plot(x, y, marker="o",color=colors[t]))

    leg = PLT.legend(lines, labels=legend_labels, loc="best",
                    fancybox=True)
    PLT.savefig("trajectory_"+str(i)+"pdf")
    PLT.close()
    input()
def generate_single_trajectory(index):
    # print("point "+str(i))
    trajectory = []
    theta_previous = [0 for i in range(8)]
    for t in range(3):
        # joint 1
        theta = np.pi + np.random.random() * np.pi
        temp = theta - 1.5 * np.pi  # relative angle, straight is 1.5 PI
        # check 0: angle did not change too much
        delta_theta = abs(temp - theta_previous[0])
        if delta_theta > np.pi / 2:
            #print("delta_theta 0", delta_theta)
            return None
        theta_previous[0] = temp
        sum_theta = temp
        r = S
        final_angle = theta  # angle of final segment
        trajectory.append(r)
        trajectory.append(theta)
        # joint 2 to 8
        for j in range(1, 8):
            temp = -np.pi / 2 + np.random.random() * np.pi  # random joint orientation within [-PI/2,PI/2]; automatically angle in feasible range
            # check 0: angle did not change too much
            delta_theta = abs(temp - theta_previous[j])
            if delta_theta > np.pi / 2:
                #print("delta_theta ", j, " ", delta_theta)
                return None
            theta_previous[j] = temp # change to new joint angle for doing check 0 in next time step

            final_angle = final_angle + temp  # compute final_angle to compute displacement vector
            r, theta = sum_polar(r, theta, S, final_angle)
            # check 1: not hitting wall, y < 0
            y = r * np.sin(theta)
            if y >= 0:  # hitting the wall
                #print("y ", j, " ", y)
                return None
            # check 2: sum of angles < 360 deg
            sum_theta += temp
            #print("sum_theta ", j , sum_theta)
            if abs(sum_theta) >= 2 * np.pi:
                #print("sum ", j, " ", sum_theta)
                return None
            trajectory.append(r)
            trajectory.append(theta)
    #print("successfull generated trajectory of length ", len(trajectory))
    #print_final_trajectory(trajectory,index)
    return trajectory

def generate_data(seed,size):
    np.random.seed(seed)
    final_trajectories=[]
    count=0
    import time
    start=time.time()
    while len(final_trajectories) <size:
        trajectory = generate_single_trajectory(len(final_trajectories))
        if trajectory is not None:
            final_trajectories.append(trajectory)
        count+=1
        # if checks(point):
        #     final_trajectories.append(point)
    delta_t=time.time() - start
    print("efficiency: ",size/count*100 ,"%")
    print("took ", delta_t, " seconds")
    print("batch done")
    return final_trajectories
def make_batches(j=0):
    #assert batch_size>BD
    data=generate_data(j,batch_size)
    pickle.dump(data,open(prefix+str(j),"wb"))
def normalise_radius(r,i):
    l = (i+1) * S
    temp = r / l
    return min(max(0.0, temp), 1.0)

def normalise_angle(angle):
    temp = (angle - np.pi) / np.pi # [PI,2PI] --> [0,1]
    return min(max(0.0, temp), 1.0)

def normalise(trajectory):
    for t in range(3):
        start = t*16
        for j in range(8):
            index = start + j*2
            trajectory[index] = normalise_radius(trajectory[index],j)
            trajectory[index+1] = normalise_angle(trajectory[index+1])
    return trajectory
def normalise_batch(batch):
    # from trajectory to the base-behavioural descriptor
    for i in range(len(batch)):
        batch[i] = normalise(batch[i])
    return batch
def normalise_batches():
    for i in range(1,51):
        filename=prefix+str(i)
        batch = pickle.load(open(filename,"rb"))
        batch = normalise_batch(batch)
        pickle.dump(batch,open(prefix+"normalised"+str(i),"wb"))


if __name__ == "__main__":
    #import multiprocessing as mp
    #num_cpus=mp.cpu_count()
    #pool = mp.Pool(num_cpus)
    #start=6
    #end=50
    #num_per_proc=int(np.ceil((end-start)/float(num_cpus)))
    #pool.starmap(make_batches, [(i,range(i*batch_size,(i+num_per_proc)*batch_size)) for i in range(start,end,num_per_proc)])
    #pool.close()
    #i=int(sys.argv[1])
    #print("starting batch")
    #make_batches(i)
    #normalise_batches()
    BD=48
    os.system("python /home/dmb1m19/argos-sferes/sferes2/modules/cvt_map_elites/cvt.py -p="+str(N)+" -b="+str(batch_size)+" -d="+str(BD)+" -k="+str(centroids)+" -l="+prefix+"normalised")
