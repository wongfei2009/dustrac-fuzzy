class Controller:
    def __init__(self):
        None

    def steerControl(self, data):
        return -(data.angularErrors.error * 0.025 + data.angularErrors.deltaError * 0.025)

#    def speedControl(self, data):
#        return 60

#class Listener:
#    def __init__(self):
#        None
#
#    def report(self, piddata):
#        None
#
#    def __del__(self):
#        None
