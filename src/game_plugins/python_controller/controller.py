import numpy as np
import theanets

class PIDData:
    def __init__(self, error = 0, deltaError = 0,
        deltaError2 = 0, prevControl = 0
    ):
        self.error = error
        self.deltaError = deltaError
        self.deltaError2 = deltaError2
        self.prevControl = prevControl

class Controller:
    def __init__(self):
        self.net = theanets.Experiment(theanets.Regressor, layers=())
        self.net.load('network.dat')
        self.net.network([[1, 1, 1]])[0]

    def steerControl(self, data):
        return self.net.network([[data.error, data.deltaError, data.deltaError2]])[0]
#        return -(data.error * 0.025 + data.deltaError * 0.025)

    def speedControl(self, data):
        return 60

class Listener:
    def __init__(self):
        self.data = None

    def report(self, data1, data2):
        item = [data1.error, data1.deltaError, data1.deltaError2, data1.prevControl, data2.prevControl]

        if(self.data == None):
            self.data = np.array(item)
        else:
            self.data = np.vstack([self.data, item])

    def __del__(self):
        np.savetxt("data.csv", self.data, delimiter=',')

