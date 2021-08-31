
import numpy as np



















if __name__ == "__main__":
    # given a large database of base-features, we want it to map to a feature map with large coverage

    # case 1: random base-features across space
    def sigmoid(x):
        return 1./(1. + np.exp(-x))
    x=np.linspace(-5,5.,100)
    print(sigmoid(5.))
