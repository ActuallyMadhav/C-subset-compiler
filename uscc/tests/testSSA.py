#---------------------------------------------------------
# Copyright (c) 2014, Sanjay Madhav
# All rights reserved.
#
# This file is distributed under the BSD license.
# See LICENSE.TXT for details.
#---------------------------------------------------------
import subprocess
import os
import sys

import unittest
uscc = "../bin/uscc"
import os
lli = "../../bin/lli"

__unittest = True

class EmitTests(unittest.TestCase):

	def setUp(self):
		self.maxDiff = None
		if not os.path.isfile(uscc):
			raise Exception("Can't run without uscc")
		if not os.path.isfile(lli):
			raise Exception("lli not found at ../../bin/lli")

	def checkEmit(self, fileName):
		# read in expected
		expectFile = open("expected/" + fileName + ".ssa", "r")
		expectedStr = expectFile.read()
		expectFile.close()
		# first compile the .bc using uscc
		try:
			result = subprocess.check_output([uscc, "-p", fileName + ".usc"], stderr=subprocess.STDOUT).decode("utf-8")
			self.assertMultiLineEqual(expectedStr, result)
		except subprocess.CalledProcessError as e:
			self.fail("\n" + (e.output.decode("utf-8") if e.output.decode("utf-8") else ""))

		# now run it in lli and compare the output

	def test_Emit_emit04(self):
		self.checkEmit("emit04")

	def test_Emit_emit06(self):
		self.checkEmit("emit06")

	def test_Emit_emit07(self):
		self.checkEmit("emit07")

	def test_Emit_emit08(self):
		self.checkEmit("emit08")

	def test_Emit_emit09(self):
		self.checkEmit("emit09")

	def test_Emit_emit11(self):
		self.checkEmit("emit11")

	def test_Emit_emit12(self):
		self.checkEmit("emit12")

	def test_Emit_quicksort(self):
		self.checkEmit("quicksort")

	def test_Emit_015(self):
		self.checkEmit("test015")

	def test_Emit_016(self):
		self.checkEmit("test016")

	def test_Emit_opt01(self):
		self.checkEmit("opt01")

	def test_Emit_opt02(self):
		self.checkEmit("opt02")

	def test_Emit_opt03(self):
		self.checkEmit("opt03")

	def test_Emit_opt04(self):
		self.checkEmit("opt04")

	def test_Emit_opt05(self):
		self.checkEmit("opt05")

	def test_Emit_opt06(self):
		self.checkEmit("opt06")

	def test_Emit_opt07(self):
		self.checkEmit("opt07")

	def test_Emit_opt08(self):
		self.checkEmit("opt08")

	def test_Emit_opt09(self):
		self.checkEmit("opt09")

	def test_Emit_opt10(self):
		self.checkEmit("opt10")

	def test_Emit_opt11(self):
		self.checkEmit("opt11")

	def test_Emit_opt12(self):
		self.checkEmit("opt12")

	def test_Emit_opt13(self):
		self.checkEmit("opt13")

	def test_Emit_opt14(self):
		self.checkEmit("opt14")

	def test_Emit_opt15(self):
		self.checkEmit("opt15")

	def test_Emit_opt16(self):
		self.checkEmit("opt16")

	def test_Emit_opt17(self):
		self.checkEmit("opt17")

	def test_Emit_opt18(self):
		self.checkEmit("opt18")

	def test_Emit_opt19(self):
		self.checkEmit("opt19")

	def test_Emit_opt20(self):
		self.checkEmit("opt20")
if __name__ == '__main__':
	unittest.main(verbosity=2)
