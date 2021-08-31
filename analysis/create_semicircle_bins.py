import numpy as np
L=0.62
from matplotlib import pyplot as PLT
def generate_linspace():
    # create a grid based on a rectangle encompassing the semi-circle
    xdimensions = np.linspace(-L,L, num=31)
    ydimensions = np.linspace(-L, 0, num=16)
    dx = xdimensions[1] - xdimensions[0]
    print("dx =", dx)
    dy = ydimensions[1] - ydimensions[0]
    print("dy =", dy)
    combinations = []
    for x in xdimensions[:-1]: # don't need the last one
        for y in ydimensions[:-1]:  # don't need the last one
            combinations.append((x,y))

    return combinations

def reject_outside(data):
    newdata = []
    for (x,y) in data:
        r = np.sqrt(x**2 + y**2)
        if r < L:
            # accept
            newdata.append(np.array([x,y]))
    print("number of bins =", len(newdata))
    delta=L/15
    print("delta = ",delta)
    print("area filled = ", len(newdata) * delta**2 )
    print("area ideally = ", np.pi*L**2 / 2)

    spw = 0
    for i in range(0,len(newdata)-1):
        for j in range(i+1,len(newdata)):
            spw+=np.sqrt(np.sum(np.square(newdata[i] - newdata[j])))
    print("summed pairwise distance=",spw)
    return newdata
def create_bins():
    space = generate_linspace()
    space = reject_outside(space)
    x=[data[0] for data in space]
    y=[data[1] for data in space]
    PLT.scatter(x,y)
    PLT.show()
    return space

if __name__ == "__main__":
    bins = create_bins()
    writefile = open("bin_locations.txt","w")
    for (x,y) in bins:
        writefile.write("%.4f %.4f \n"%(x,y))
