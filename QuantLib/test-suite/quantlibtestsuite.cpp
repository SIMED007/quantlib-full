
/*
 Copyright (C) 2003 RiskMap srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <ql/qldefines.hpp>
#include <iostream>
#include <string>
#include "qltestlistener.hpp"
#include "calendars.hpp"
#include "capfloor.hpp"
#include "compoundforward.hpp"
#include "covariance.hpp"
#include "dates.hpp"
#include "daycounters.hpp"
#include "distributions.hpp"
#include "europeanoption.hpp"
#include "instruments.hpp"
#include "integrals.hpp"
#include "lowdiscrepancysequences.hpp"
#include "marketelements.hpp"
#include "matrices.hpp"
#include "mersennetwister.hpp"
#include "operators.hpp"
#include "piecewiseflatforward.hpp"
#include "riskstats.hpp"
#include "solvers.hpp"
#include "stats.hpp"
#include "swap.hpp"
#include "swaption.hpp"
#include "termstructures.hpp"
#include "old_pricers.hpp"

int main() {
    CppUnit::TextUi::TestRunner runner;
    QLTestListener qlListener;
    runner.eventManager().addListener(&qlListener);

    runner.addTest(new CalendarTest);
    runner.addTest(CompoundForwardTest::suite());
    runner.addTest(CovarianceTest::suite());
    runner.addTest(new DateTest);
    runner.addTest(new DayCounterTest);
    runner.addTest(new DistributionTest);
    runner.addTest(InstrumentTest::suite());
    runner.addTest(new IntegralTest);
    runner.addTest(LDSTest::suite());
    runner.addTest(MarketElementTest::suite());
    runner.addTest(new MatricesTest());
    runner.addTest(new MersenneTwisterTest());
    runner.addTest(new OperatorTest);
    runner.addTest(new RiskStatisticsTest);
    runner.addTest(new Solver1DTest);
    runner.addTest(new StatisticsTest);
    runner.addTest(SwaptionTest::suite());

    // to be deprecated
    runner.addTest(OldPricerTest::suite());

    // don't work under Borland
    runner.addTest(TermStructureTest::suite());
    runner.addTest(PiecewiseFlatForwardTest::suite());
    runner.addTest(SimpleSwapTest::suite());
    runner.addTest(CapFloorTest::suite());
    runner.addTest(EuropeanOptionTest::suite());


    std::string quote = "\n"
        "                    Testing can never demonstrate the absence   \n"
        "                    of errors in software, only their presence. \n"
        "                                           -- W.E. Dijkstra \n";
    std::string header = "Testing QuantLib " QL_VERSION " using CppUnit " CPPUNIT_VERSION;

    std::cerr << quote << std::endl;
    std::cerr << std::string(header.length(),'=') << std::endl;
    std::cerr << header << std::endl;
    std::cerr << std::string(header.length(),'=');
    std::cerr.flush();
    bool succeeded = runner.run();
    return succeeded ? 0 : 1;
}
