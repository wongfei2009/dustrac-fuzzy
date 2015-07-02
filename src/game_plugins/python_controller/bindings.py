class DiffStore:
    def __init__(self, error, deltaError, deltaError2):
        self.error = error
        self.deltaError = deltaError
        self.deltaError2 = deltaError2  

class PIDData:
    def __init__(self, angularErrors, lateralErrors, steerControl, speedControl):
        self.angularErrors = angularErrors
        self.lateralErrors = lateralErrors
        self.steerControl = steerControl
        self.speedControl = speedControl