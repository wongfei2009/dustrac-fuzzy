class PIDData:
	def __init__(self, error = 0, deltaError = 0,
		deltaError2 = 0, prevControl = 0
	):
		self.error = error
		self.deltaError = deltaError
		self.deltaError2 = deltaError2
		self.prevControl = prevControl

class Controller:
	def steerControl(self, data):
		return -(data.error * 0.025 + data.deltaError * 0.025)

	def speedControl(self, data):
		return 60

#class Listener:
#	def report(self, data1, data2):
#		print(data1.error)
#		print(data2.error)
