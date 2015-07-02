import numpy as np
import theanets

class Controller:
    def __init__(self):
        self.net = theanets.Experiment(theanets.Regressor, layers=())
        self.net.load('data/default_net.dat')

    def steerControl(self, data):
#        return self.net.network.predict([[data.lateralErrors.error,
#            data.lateralErrors.deltaError, data.lateralErrors.deltaError2]])[0]
        return -(data.angularErrors.error * 0.025 + data.angularErrors.deltaError * 0.025)

#    def speedControl(self, data):
#        return 60

class Listener:
    def __init__(self):
        self.data = None

    def report(self, piddata, tnodedistance):
        item = [piddata.angularErrors.error, piddata.angularErrors.deltaError,
                piddata.angularErrors.deltaError2, piddata.steerControl, piddata.speedControl]

        if(self.data == None):
            self.data = np.array(item)
        else:
            self.data = np.vstack([self.data, item])

    def __del__(self):
        np.savetxt("data.csv", self.data, delimiter=',')

