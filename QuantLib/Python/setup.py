"""
/*
 * Copyright (C) 2001 QuantLib Group
 * 
 * This file is part of QuantLib.
 * QuantLib is a C++ open source library for financial quantitative
 * analysts and developers --- http://quantlib.sourceforge.net/
 *
 * QuantLib is free software and you are allowed to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of it under the conditions stated 
 * in the QuantLib License.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You should have received a copy of the license along with this file;
 * if not, contact ferdinando@ametrano.net
 *
 * QuantLib license is also available at:
 * http://quantlib.sourceforge.net/LICENSE.TXT
*/
"""

""" 
    $Source$
    $Log$
    Revision 1.2  2001/03/06 13:10:07  marmar
    Copyright notice added

"""

from distutils.core import setup, Extension

predir = "/usr/local/include/QuantLib"

setup ( name = "pyQuantLib", \
	version = "0.1.1", \
	maintainer = "Enrico Sirola", \
	maintainer_email = "enri@users.sourceforge.net", \
	url = "http://quantlib.sourceforge.net", \
	py_modules = ["QuantLib"],
	ext_modules = [ \
		Extension ( "QuantLibc", \
			["quantlib_wrap.cpp"], \
			libraries = ["QuantLib"], \
			include_dirs = [predir, \
					"%s/Calendars" % predir, \
					"%s/Currencies" % predir, \
					"%s/DayCounters" % predir, \
					"%s/FiniteDifferences" % predir, \
					"%s/Instruments" % predir, \
					"%s/Math" % predir, \
					"%s/Patterns" % predir, \
					"%s/Pricers" % predir, \
					"%s/Solvers1D" % predir, \
					"%s/TermStructures" % predir, \
					"%s/MonteCarlo" % predir, \
					"%s/Utilities" % predir \
					] \
				) \
			] \
		)
