"""
/*
 * Copyright (C) 2000-2001 QuantLib Group
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
    Revision 1.9  2001/01/08 16:19:29  nando
    more homogeneous format

    Revision 1.8  2001/01/08 15:33:23  nando
    improved

"""

from QuantLib import Statistics
import time
startTime = time.time()
print 'Testing Statistics'

tol = 1e-9

data =    [  3,   4,   5,   2,   3,   4,   5,   6,   4,   7]
weights = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]

s = Statistics()
s.addWeightedSequence(data, weights)

print 'samples', s.samples(),
if (s.samples()==10):
  print 'OK'
else:
  print 'wrong'

print 'Sum of the weights', s.weightSum(),
if (s.weightSum()==10):
  print 'OK'
else:
  print 'wrong'

print 'min', s.min(),
if (s.min()==2):
  print 'OK'
else:
  print 'wrong'

print 'max', s.max(),
if (s.max()==7):
  print 'OK'
else:
  print 'wrong'

print 'mean', s.mean(),
if (abs(s.mean()-4.3)<tol):
  print 'OK'
else:
  print 'wrong'

print 'variance', s.variance(),
if (abs(s.variance()-2.23333333333)<tol):
  print 'OK'
else:
  print 'wrong'

print 'stdDev', s.standardDeviation(),
if (abs(s.standardDeviation()-1.4944341181)<tol):
  print 'OK'
else:
  print 'wrong'

print 'skew', s.skewness(),
if (abs(s.skewness()-0.359543071407)<tol):
  print 'OK'
else:
  print 'wrong'

print 'kurt', s.kurtosis(), 
if (abs(s.kurtosis()+0.151799637209)<tol):
  print 'OK'
else:
  print 'wrong'

print 'error estimate', s.errorEstimate(), 'not checked' 
#if (abs(s.kurtosis()+0.151799637209)<tol):
#  print 'OK'
#else:
#  print 'wrong'

print
print 'Test passed (elapsed time', time.time() - startTime, ')'
print 'Press return to end this test'
raw_input()
