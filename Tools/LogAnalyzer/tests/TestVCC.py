from LogAnalyzer import Test,TestResult
import DataflashLog

import collections


class TestVCC(Test):
	'''test for VCC within recommendations, or abrupt end to log in flight'''

	def __init__(self):
		self.name = "VCC"

	def run(self, logdata):
		self.result = TestResult()
		self.result.status = TestResult.StatusType.PASS

		if not "CURR" in logdata.channels:
			self.result.status = TestResult.StatusType.UNKNOWN
			self.result.statusMessage = "No CURR log data found"
			return

		# just a naive min/max test for now
		vccMin  = logdata.channels["CURR"]["Vcc"].min()
		vccMax  = logdata.channels["CURR"]["Vcc"].max()
		vccDiff = vccMax - vccMin;
		vccMinThreshold = 4.6 * 1000;
		vccMaxDiff      = 0.3 * 1000;
		if vccDiff > vccMaxDiff:
			self.result.status = TestResult.StatusType.WARN
			self.result.statusMessage = "VCC min/max diff %s, should be <%s" % (vccDiff/1000.0, vccMaxDiff/1000.0)
		elif vccMin < vccMinThreshold:
			self.result.status = TestResult.StatusType.FAIL
			self.result.statusMessage = "VCC below minimum of %s (%s)" % (`vccMinThreshold/1000.0`,`vccMin/1000.0`)
		
