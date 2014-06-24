#
# Code to abstract the parsing of APM Dataflash log files, currently only used by the LogAnalyzer
#
# Initial code by Andrew Chapman (chapman@skymount.com), 16th Jan 2014
#

# TODO: log reading needs to be much more robust, only compatible with AC3.0.1-AC3.1 logs at this point, try to be compatible at least back to AC2.9.1, and APM:Plane too (skipping copter-only tests)
# TODO: rethink data storage, dictionaries are good for specific line lookups, but we don't end up doing a lot of that

import pprint  # temp
import collections
import os

class Format:
	'''Channel format as specified by the FMT lines in the log file'''
	msgType = 0
	msgLen  = 0
	name    = ""
	types   = ""
	labels  = []
	def __init__(self,msgType,msgLen,name,types,labels):
		self.msgType = msgType
		self.msgLen  = msgLen
		self.name    = name
		self.types   = types
		self.labels  = labels.split(',')
	def __str__(self):
		return "%8s %s" % (self.name, `self.labels`)


class Channel:
	'''storage for a single stream of data, i.e. all GPS.RelAlt values'''
	dictData = None #  dict of linenum->value      # store dupe data in dict and list for now, until we decide which is the better way to go
	listData = None #  list of (linenum,value)
	def __init__(self):
		self.dictData = {}
		self.listData = []
	def getSegment(self, startLine, endLine):
		'''returns a segment of this data (from startLine to endLine, inclusive) as a new Channel instance'''
		segment = Channel()
		segment.dictData = {k:v for k,v in self.dictData.iteritems() if k >= startLine and k <= endLine}
		return segment
	def min(self):
		return min(self.dictData.values())
	def max(self):
		return max(self.dictData.values())
	def avg(self):
		return avg(self.dictData.values())
	def getNearestValue(self, lineNumber, lookForwards=True):
		'''return the nearest data value to the given lineNumber'''
		if lineNumber in self.dictData:
			return self.dictData[lineNumber]
		offset = 1
		if not lookForwards:
			offset = -1
		minLine = min(self.dictData.keys())
		maxLine = max(self.dictData.keys())
		line = max(minLine,lineNumber)
		line = min(maxLine,line)
		while line >= minLine and line <= maxLine:
			if line in self.dictData:
				return self.dictData[line]
			line = line + offset
		raise Exception("Error finding nearest value for line %d" % lineNumber)


class DataflashLogHelper:
	@staticmethod
	def getTimeAtLine(logdata, lineNumber):
		'''returns the nearest GPS timestamp in milliseconds after the given line number'''
		if not "GPS" in logdata.channels:
			raise Exception("no GPS log data found")
		# older logs use 'TIme', newer logs use 'TimeMS'
		timeLabel = "TimeMS"
		if "Time" in logdata.channels["GPS"]:
			timeLabel = "Time"
		while lineNumber <= logdata.lineCount:
			if lineNumber in logdata.channels["GPS"][timeLabel].dictData:
				return logdata.channels["GPS"][timeLabel].dictData[lineNumber]
			lineNumber = lineNumber + 1

	@staticmethod
	def findLoiterChunks(logdata, minLengthSeconds=0, noRCInputs=True):
		'''returns a list of (to,from) pairs defining sections of the log which are in loiter mode. Ordered from longest to shortest in time. If noRCInputs == True it only returns chunks with no control inputs'''
		# TODO: implement noRCInputs handling when identifying stable loiter chunks, for now we're ignoring it

		def chunkSizeCompare(chunk1, chunk2):
			chunk1Len = chunk1[1]-chunk1[0]
			chunk2Len = chunk2[1]-chunk2[0]
			if chunk1Len == chunk2Len:
				return 0
			elif chunk1Len > chunk2Len:
				return -1
			else:
				return 1

		od = collections.OrderedDict(sorted(logdata.modeChanges.items(), key=lambda t: t[0]))
		chunks = []
		for i in range(len(od.keys())):
			if od.values()[i][0] == "LOITER":
				startLine = od.keys()[i]
				endLine   = None
				if i == len(od.keys())-1:
					endLine = logdata.lineCount
				else:
					endLine = od.keys()[i+1]-1
				chunkTimeSeconds = (DataflashLogHelper.getTimeAtLine(logdata,endLine)-DataflashLogHelper.getTimeAtLine(logdata,startLine)+1) / 1000.0
				if chunkTimeSeconds > minLengthSeconds:
					chunks.append((startLine,endLine))
					#print "LOITER chunk: %d to %d, %d lines" % (startLine,endLine,endLine-startLine+1)
					#print "  (time %d to %d, %d seconds)" % (DataflashLogHelper.getTimeAtLine(logdata,startLine), DataflashLogHelper.getTimeAtLine(logdata,endLine), chunkTimeSeconds)
		chunks.sort(chunkSizeCompare)
		return chunks

	@staticmethod
	def isLogEmpty(logdata):
		'''returns an human readable error string if the log is essentially empty, otherwise returns None'''
		# naive check for now, see if the throttle output was ever above 20%
		throttleThreshold = 200
		if "CTUN" in logdata.channels:
			maxThrottle = logdata.channels["CTUN"]["ThrOut"].max()
			if maxThrottle < throttleThreshold:
				return "Throttle never above 20%"
		return None


class DataflashLog:
	'''APM Dataflash log file reader and container class. Keep this simple, add more advanced or specific functions to DataflashLogHelper class'''
	filename = None

	vehicleType     = None # ArduCopter, ArduPlane, ArduRover, etc, verbatim as given by header
	firmwareVersion = None
	firmwareHash    = None
	freeRAM         = None
	hardwareType    = None # APM 1, APM 2, PX4, MPNG, etc What is VRBrain? BeagleBone, etc? Needs more testing

	formats     = {} # name -> Format
	parameters  = {} # token -> value
	messages    = {} # lineNum -> message
	modeChanges = {} # lineNum -> (mode,value)
	channels    = {} # lineLabel -> {dataLabel:Channel}

	filesizeKB   = None
	durationSecs = None
	lineCount    = None

	def getCopterType(self):
		if self.vehicleType != "ArduCopter":
			return None
		motLabels = []
		if "MOT" in self.formats: # not listed in PX4 log header for some reason?
			motLabels = self.formats["MOT"].labels
	 	if "GGain" in motLabels:
			return "tradheli"
		elif len(motLabels) == 4:
			return "quad"
		elif len(motLabels) == 6:
			return "hex"
		elif len(motLabels) == 8:
			return "octo"
		else:
			return ""

	def __castToFormatType(self,value,valueType):
		'''using format characters from libraries/DataFlash/DataFlash.h to cast to basic python int/float/string types'''
	  	intTypes   = "bBhHiIM"
	  	floatTypes = "fcCeEL"
	  	charTypes  = "nNZ"
		if valueType in floatTypes:
			return float(value)
		elif valueType in intTypes:
			return int(value)
		elif valueType in charTypes:
			return str(value)
		else:
			raise Exception("Unknown value type of '%s' specified to __castToFormatType()" % valueType)

	def __init__(self, logfile, ignoreBadlines=False):
		self.read(logfile, ignoreBadlines)

	def read(self, logfile, ignoreBadlines=False):
		# TODO: dataflash log parsing code is *SUPER* hacky, should re-write more methodically
		self.filename = logfile
		f = open(self.filename, 'r')
		lineNumber = 0
		copterLogPre3Header = False
		copterLogPre3Params = False
		knownHardwareTypes = ["APM", "PX4", "MPNG"]
		for line in f:
			lineNumber = lineNumber + 1
			try:
				#print "Reading line: %d" % lineNumber
				line = line.strip('\n\r')
				tokens = line.split(', ')
				# first handle the log header lines
				if copterLogPre3Header and line[0:15] == "SYSID_SW_MREV: ":
					copterLogPre3Header = False
					copterLogPre3Params = True
				if len(tokens) == 1:
					tokens2 = line.split(' ')
					if line == "":
						pass
					elif len(tokens2) == 1 and tokens2[0].isdigit(): # log index
						pass
					elif len(tokens2) == 3 and tokens2[0] == "Free" and tokens2[1] == "RAM:":
						self.freeRAM = int(tokens2[2])
					elif tokens2[0] in knownHardwareTypes:
						self.hardwareType = line      # not sure if we can parse this more usefully, for now only need to report it back verbatim
					elif line[0:8] == "FW Ver: ":
						copterLogPre3Header = True
					elif copterLogPre3Header:
						pass   # just skip over all that until we hit the PARM lines
					elif (len(tokens2) == 2 or len(tokens2) == 3) and tokens2[1][0] == "V":  # e.g. ArduCopter V3.1 (5c6503e2)
						self.vehicleType     = tokens2[0]
						self.firmwareVersion = tokens2[1]
						if len(tokens2) == 3:
							self.firmwareHash    = tokens2[2][1:-1]
					elif len(tokens2) == 2 and copterLogPre3Params:
						pName = tokens2[0]
						self.parameters[pName] = float(tokens2[1])
					else:
						errorMsg = "Error parsing line %d of log file: %s" % (lineNumber, self.filename)
						if ignoreBadlines:
							print errorMsg + " (skipping line)"
						else:
							raise Exception(errorMsg)
				# now handle the non-log data stuff, format descriptions, params, etc
				elif tokens[0] == "FMT":
					format = None
					if len(tokens) == 6:
						format = Format(tokens[1],tokens[2],tokens[3],tokens[4],tokens[5])
					elif len(tokens) == 5:  # some logs have FMT STRT with no labels
						format = Format(tokens[1],tokens[2],tokens[3],tokens[4],"")
					else:
						raise Exception("FMT error on line %d, nTokens: %d" % (lineNumber, len(tokens)))
					#print format  # TEMP
					self.formats[tokens[3]] = format
				elif tokens[0] == "PARM":
					pName = tokens[1]
					self.parameters[pName] = float(tokens[2])
				elif tokens[0] == "MSG":
					self.messages[lineNumber] = tokens[1]
				elif tokens[0] == "MODE":
					self.modeChanges[lineNumber] = (tokens[1],int(tokens[2]))
				# anything else must be the log data
				elif not copterLogPre3Header:
					groupName = tokens[0]
					tokens2 = line.split(', ')
					# first time seeing this type of log line, create the channel storage
					if not groupName in self.channels:
						self.channels[groupName] = {}
						for label in self.formats[groupName].labels:
							self.channels[groupName][label] = Channel()
						#print "Channel definition for group %s, data at address %s" % (groupName, hex(id(self.channels[groupName][label].dictData)))
						#pprint.pprint(self.channels[groupName]) # TEMP!!!
					# check the number of tokens matches between the line and what we're expecting from the FMT definition
					if len(tokens2) != (len(self.formats[groupName].labels) + 1):
						errorMsg = "Error on line %d of log file: %s, %s line's value count (%d) not matching FMT specification (%d)" % (lineNumber, self.filename, groupName, len(tokens2), len(self.formats[groupName].labels))
						if ignoreBadlines:
							print errorMsg + " (skipping line)"
							continue
						else:
							raise Exception(errorMsg)
					# store each token in its relevant channel
					for (index,label) in enumerate(self.formats[groupName].labels):
						#value = float(tokens2[index+1])  # simple read without handling datatype
						value = self.__castToFormatType(tokens2[index+1], self.formats[groupName].types[index])  # handling datatype via this call slows down ready by about 50%
						channel = self.channels[groupName][label]
						#print "Set data {%s,%s} for line %d to value %s, of type %c, stored at address %s" % (groupName, label, lineNumber, `value`, self.formats[groupName].types[index], hex(id(channel.dictData)))
						channel.dictData[lineNumber] = value
						channel.listData.append((lineNumber,value))
			except:
				print "Error parsing line %d of log file %s" % (lineNumber,self.filename)
				raise

		# gather some general stats about the log
		self.lineCount  = lineNumber
		self.filesizeKB = os.path.getsize(self.filename) / 1024.0
		if "GPS" in self.channels:
			# the GPS time label changed at some point, need to handle both
			timeLabel = "TimeMS"
			if timeLabel not in self.channels["GPS"]:
				timeLabel = "Time"
			firstTimeGPS = self.channels["GPS"][timeLabel].listData[0][1]
			lastTimeGPS  = self.channels["GPS"][timeLabel].listData[-1][1]
			self.durationSecs = (lastTimeGPS-firstTimeGPS) / 1000

