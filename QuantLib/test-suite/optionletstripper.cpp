/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2008 Ferdinando Ametrano
 Copyright (C) 2007 Laurent Hoffmann

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include "optionletstripper.hpp"
#include "utilities.hpp"
#include <ql/termstructures/volatility/optionlet/optionletstripper1.hpp>
#include <ql/termstructures/volatility/optionlet/strippedoptionletadapter.hpp>
#include <ql/termstructures/volatility/capfloor/constantcapfloortermvol.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/indexes/ibor/euribor.hpp>
#include <ql/pricingengines/capfloor/blackcapfloorengine.hpp>
#include <ql/instruments/makecapfloor.hpp>

using namespace QuantLib;
using namespace boost::unit_test_framework;

namespace {

    // TODO: use CommonVars
    Calendar calendar_;
    DayCounter dayCounter_;

    RelinkableHandle<YieldTermStructure> yieldTermStructure_;

    std::vector<Rate> strikes_;
    std::vector<Period> optionTenors_;
    Matrix termV_;

    boost::shared_ptr<CapFloorTermVolSurface> capFloorVolSurface_;
    boost::shared_ptr<CapFloorTermVolSurface> flatTermVolSurface_;

    Real accuracy_ = 1.0e-6;
    Real tolerance_ = 1.0e-5;

    void setTermStructure(){

        calendar_ = TARGET();
        Date today = calendar_.adjust(Date::todaysDate());
        Settings::instance().evaluationDate() = today;

        dayCounter_ = Actual365Fixed();

        Rate flatFwdRate = 0.04;
        yieldTermStructure_.linkTo(boost::shared_ptr<FlatForward>(new
            FlatForward(0,
                        calendar_,
                        flatFwdRate,
                        dayCounter_)));
    }

    void setFlatTermVolSurface() {

        setTermStructure();

        optionTenors_.resize(10);
        for (Size i = 0; i < optionTenors_.size(); ++i)
            optionTenors_[i] = Period(i + 1, Years);

        strikes_.resize(10);
        for (Size j = 0; j < strikes_.size(); ++j)
            strikes_[j] = Real(j + 1) / 100.0;

        Volatility flatVol = .18;
        termV_ = Matrix(optionTenors_.size(), strikes_.size(), flatVol);
        flatTermVolSurface_ = boost::shared_ptr<CapFloorTermVolSurface>(new
            CapFloorTermVolSurface(0, calendar_, Following,
                                   optionTenors_, strikes_,
                                   termV_, dayCounter_));
    }


    void setCapFloorTermVolSurface() {

        setTermStructure();

        //cap volatility smile matrix
        optionTenors_ = std::vector<Period>();
        optionTenors_.push_back(Period(1, Years));
        optionTenors_.push_back(Period(18, Months));
        optionTenors_.push_back(Period(2, Years));
        optionTenors_.push_back(Period(3, Years));
        optionTenors_.push_back(Period(4, Years));
        optionTenors_.push_back(Period(5, Years));
        optionTenors_.push_back(Period(6, Years));
        optionTenors_.push_back(Period(7, Years));
        optionTenors_.push_back(Period(8, Years));
        optionTenors_.push_back(Period(9, Years));
        optionTenors_.push_back(Period(10, Years));
        optionTenors_.push_back(Period(12, Years));
        optionTenors_.push_back(Period(15, Years));
        optionTenors_.push_back(Period(20, Years));
        optionTenors_.push_back(Period(25, Years));
        optionTenors_.push_back(Period(30, Years));

        strikes_ = std::vector<Rate>();
        strikes_.push_back(0.015);
        strikes_.push_back(0.0175);
        strikes_.push_back(0.02);
        strikes_.push_back(0.0225);
        strikes_.push_back(0.025);
        strikes_.push_back(0.03);
        strikes_.push_back(0.035);
        strikes_.push_back(0.04);
        strikes_.push_back(0.05);
        strikes_.push_back(0.06);
        strikes_.push_back(0.07);
        strikes_.push_back(0.08);
        strikes_.push_back(0.1);

        termV_ = Matrix(optionTenors_.size(), strikes_.size());
        termV_[0][0]=0.287;  termV_[0][1]=0.274;  termV_[0][2]=0.256;  termV_[0][3]=0.245;  termV_[0][4]=0.227;  termV_[0][5]=0.148;  termV_[0][6]=0.096;  termV_[0][7]=0.09;   termV_[0][8]=0.11;   termV_[0][9]=0.139;  termV_[0][10]=0.166;  termV_[0][11]=0.19;   termV_[0][12]=0.214;
        termV_[1][0]=0.303;  termV_[1][1]=0.258;  termV_[1][2]=0.22;   termV_[1][3]=0.203;  termV_[1][4]=0.19;   termV_[1][5]=0.153;  termV_[1][6]=0.126;  termV_[1][7]=0.118;  termV_[1][8]=0.147;  termV_[1][9]=0.165;  termV_[1][10]=0.18;   termV_[1][11]=0.192;  termV_[1][12]=0.212;
        termV_[2][0]=0.303;  termV_[2][1]=0.257;  termV_[2][2]=0.216;  termV_[2][3]=0.196;  termV_[2][4]=0.182;  termV_[2][5]=0.154;  termV_[2][6]=0.134;  termV_[2][7]=0.127;  termV_[2][8]=0.149;  termV_[2][9]=0.166;  termV_[2][10]=0.18;   termV_[2][11]=0.192;  termV_[2][12]=0.212;
        termV_[3][0]=0.305;  termV_[3][1]=0.266;  termV_[3][2]=0.226;  termV_[3][3]=0.203;  termV_[3][4]=0.19;   termV_[3][5]=0.167;  termV_[3][6]=0.151;  termV_[3][7]=0.144;  termV_[3][8]=0.16;   termV_[3][9]=0.172;  termV_[3][10]=0.183;  termV_[3][11]=0.193;  termV_[3][12]=0.209;
        termV_[4][0]=0.294;  termV_[4][1]=0.261;  termV_[4][2]=0.216;  termV_[4][3]=0.201;  termV_[4][4]=0.19;   termV_[4][5]=0.171;  termV_[4][6]=0.158;  termV_[4][7]=0.151;  termV_[4][8]=0.163;  termV_[4][9]=0.172;  termV_[4][10]=0.181;  termV_[4][11]=0.188;  termV_[4][12]=0.201;
        termV_[5][0]=0.276;  termV_[5][1]=0.248;  termV_[5][2]=0.212;  termV_[5][3]=0.199;  termV_[5][4]=0.189;  termV_[5][5]=0.172;  termV_[5][6]=0.16;   termV_[5][7]=0.155;  termV_[5][8]=0.162;  termV_[5][9]=0.17;   termV_[5][10]=0.177;  termV_[5][11]=0.183;  termV_[5][12]=0.195;
        termV_[6][0]=0.26;   termV_[6][1]=0.237;  termV_[6][2]=0.21;   termV_[6][3]=0.198;  termV_[6][4]=0.188;  termV_[6][5]=0.172;  termV_[6][6]=0.161;  termV_[6][7]=0.156;  termV_[6][8]=0.161;  termV_[6][9]=0.167;  termV_[6][10]=0.173;  termV_[6][11]=0.179;  termV_[6][12]=0.19;
        termV_[7][0]=0.25;   termV_[7][1]=0.231;  termV_[7][2]=0.208;  termV_[7][3]=0.196;  termV_[7][4]=0.187;  termV_[7][5]=0.172;  termV_[7][6]=0.162;  termV_[7][7]=0.156;  termV_[7][8]=0.16;   termV_[7][9]=0.165;  termV_[7][10]=0.17;   termV_[7][11]=0.175;  termV_[7][12]=0.185;
        termV_[8][0]=0.244;  termV_[8][1]=0.226;  termV_[8][2]=0.206;  termV_[8][3]=0.195;  termV_[8][4]=0.186;  termV_[8][5]=0.171;  termV_[8][6]=0.161;  termV_[8][7]=0.156;  termV_[8][8]=0.158;  termV_[8][9]=0.162;  termV_[8][10]=0.166;  termV_[8][11]=0.171;  termV_[8][12]=0.18;
        termV_[9][0]=0.239;  termV_[9][1]=0.222;  termV_[9][2]=0.204;  termV_[9][3]=0.193;  termV_[9][4]=0.185;  termV_[9][5]=0.17;   termV_[9][6]=0.16;   termV_[9][7]=0.155;  termV_[9][8]=0.156;  termV_[9][9]=0.159;  termV_[9][10]=0.163;  termV_[9][11]=0.168;  termV_[9][12]=0.177;
        termV_[10][0]=0.235; termV_[10][1]=0.219; termV_[10][2]=0.202; termV_[10][3]=0.192; termV_[10][4]=0.183; termV_[10][5]=0.169; termV_[10][6]=0.159; termV_[10][7]=0.154; termV_[10][8]=0.154; termV_[10][9]=0.156; termV_[10][10]=0.16;  termV_[10][11]=0.164; termV_[10][12]=0.173;
        termV_[11][0]=0.227; termV_[11][1]=0.212; termV_[11][2]=0.197; termV_[11][3]=0.187; termV_[11][4]=0.179; termV_[11][5]=0.166; termV_[11][6]=0.156; termV_[11][7]=0.151; termV_[11][8]=0.149; termV_[11][9]=0.15;  termV_[11][10]=0.153; termV_[11][11]=0.157; termV_[11][12]=0.165;
        termV_[12][0]=0.22;  termV_[12][1]=0.206; termV_[12][2]=0.192; termV_[12][3]=0.183; termV_[12][4]=0.175; termV_[12][5]=0.162; termV_[12][6]=0.153; termV_[12][7]=0.147; termV_[12][8]=0.144; termV_[12][9]=0.144; termV_[12][10]=0.147; termV_[12][11]=0.151; termV_[12][12]=0.158;
        termV_[13][0]=0.211; termV_[13][1]=0.197; termV_[13][2]=0.185; termV_[13][3]=0.176; termV_[13][4]=0.168; termV_[13][5]=0.156; termV_[13][6]=0.147; termV_[13][7]=0.142; termV_[13][8]=0.138; termV_[13][9]=0.138; termV_[13][10]=0.14;  termV_[13][11]=0.144; termV_[13][12]=0.151;
        termV_[14][0]=0.204; termV_[14][1]=0.192; termV_[14][2]=0.18;  termV_[14][3]=0.171; termV_[14][4]=0.164; termV_[14][5]=0.152; termV_[14][6]=0.143; termV_[14][7]=0.138; termV_[14][8]=0.134; termV_[14][9]=0.134; termV_[14][10]=0.137; termV_[14][11]=0.14;  termV_[14][12]=0.148;
        termV_[15][0]=0.2;   termV_[15][1]=0.187; termV_[15][2]=0.176; termV_[15][3]=0.167; termV_[15][4]=0.16;  termV_[15][5]=0.148; termV_[15][6]=0.14;  termV_[15][7]=0.135; termV_[15][8]=0.131; termV_[15][9]=0.132; termV_[15][10]=0.135; termV_[15][11]=0.139; termV_[15][12]=0.146;

        capFloorVolSurface_ = boost::shared_ptr<CapFloorTermVolSurface>(new
            CapFloorTermVolSurface(0, calendar_, Following,
                                   optionTenors_, strikes_,
                                   termV_, dayCounter_));
    }

}


void OptionletStripperTest::testFlatTermVolatilityStripping() {

  BOOST_MESSAGE("Test forward/forward vol stripping from flat term vol surface");

    SavedSettings backup;
    setFlatTermVolSurface();

    boost::shared_ptr<IborIndex> iborIndex(new Euribor6M(yieldTermStructure_));

    boost::shared_ptr<OptionletStripper> optionletStripper1(new
        OptionletStripper1(flatTermVolSurface_,
                           iborIndex,
                           Null<Rate>(),
                           accuracy_));

    boost::shared_ptr<StrippedOptionletAdapter> strippedOptionletAdapter(new
        StrippedOptionletAdapter(optionletStripper1));

    Handle<OptionletVolatilityStructure> vol(strippedOptionletAdapter);

    vol->enableExtrapolation();

    boost::shared_ptr<BlackCapFloorEngine> strippedVolEngine(new
        BlackCapFloorEngine(yieldTermStructure_,
                            vol));

    boost::shared_ptr<CapFloor> cap;
    for (Size tenorIndex=0; tenorIndex<optionTenors_.size(); ++tenorIndex) {
        for (Size strikeIndex=0; strikeIndex<strikes_.size(); ++strikeIndex) {
            cap = MakeCapFloor(CapFloor::Cap,
                               optionTenors_[tenorIndex],
                               iborIndex,
                               strikes_[strikeIndex],
                               0*Days)
                  .withPricingEngine(strippedVolEngine);

            Real priceFromStrippedVolatility = cap->NPV();

            boost::shared_ptr<PricingEngine> blackCapFloorEngineConstantVolatility(new
                BlackCapFloorEngine(yieldTermStructure_,
                                    termV_[tenorIndex][strikeIndex]));

            cap->setPricingEngine(blackCapFloorEngineConstantVolatility);
            Real priceFromConstantVolatility = cap->NPV();

            Real error = std::fabs(priceFromStrippedVolatility - priceFromConstantVolatility);
            if (error>tolerance_)
                BOOST_FAIL("\noption tenor:       " << optionTenors_[tenorIndex] <<
                           "\nstrike:             " << io::rate(strikes_[strikeIndex]) <<
                           "\nstripped vol price: " << io::rate(priceFromStrippedVolatility) <<
                           "\nconstant vol price: " << io::rate(priceFromConstantVolatility) <<
                           "\nerror:              " << io::rate(error) <<
                           "\ntolerance:          " << io::rate(tolerance_));
            }
    }

    BOOST_MESSAGE("Done");
}

void OptionletStripperTest::testTermVolatilityStripping() {

  BOOST_MESSAGE("Test forward/forward vol stripping from non-flat term vol surface");

    SavedSettings backup;
    setCapFloorTermVolSurface();

    boost::shared_ptr<IborIndex> iborIndex(new Euribor6M(yieldTermStructure_));

    boost::shared_ptr<OptionletStripper> optionletStripper1(new
        OptionletStripper1(capFloorVolSurface_,
                           iborIndex,
                           Null<Rate>(),
                           accuracy_));

    boost::shared_ptr<StrippedOptionletAdapter> strippedOptionletAdapter =
        boost::shared_ptr<StrippedOptionletAdapter>(new
            StrippedOptionletAdapter(optionletStripper1));

    Handle<OptionletVolatilityStructure> vol(strippedOptionletAdapter);

    vol->enableExtrapolation();

    boost::shared_ptr<BlackCapFloorEngine> strippedVolEngine(new
        BlackCapFloorEngine(yieldTermStructure_,
                            vol));

    boost::shared_ptr<CapFloor> cap;
    for (Size tenorIndex=0; tenorIndex<optionTenors_.size(); ++tenorIndex) {
        for (Size strikeIndex=0; strikeIndex<strikes_.size(); ++strikeIndex) {
            cap = MakeCapFloor(CapFloor::Cap,
                               optionTenors_[tenorIndex],
                               iborIndex,
                               strikes_[strikeIndex],
                               0*Days)
                  .withPricingEngine(strippedVolEngine);

            Real priceFromStrippedVolatility = cap->NPV();

            boost::shared_ptr<PricingEngine> blackCapFloorEngineConstantVolatility(new
                BlackCapFloorEngine(yieldTermStructure_,
                                    termV_[tenorIndex][strikeIndex]));

            cap->setPricingEngine(blackCapFloorEngineConstantVolatility);
            Real priceFromConstantVolatility = cap->NPV();

            Real error = std::fabs(priceFromStrippedVolatility - priceFromConstantVolatility);
            if (error>tolerance_)
                BOOST_FAIL("\noption tenor:       " << optionTenors_[tenorIndex] <<
                           "\nstrike:             " << io::rate(strikes_[strikeIndex]) <<
                           "\nstripped vol price: " << io::rate(priceFromStrippedVolatility) <<
                           "\nconstant vol price: " << io::rate(priceFromConstantVolatility) <<
                           "\nerror:              " << io::rate(error) <<
                           "\ntolerance:          " << io::rate(tolerance_));
            }
    }
    BOOST_MESSAGE("Done");
}

test_suite* OptionletStripperTest::suite() {
    test_suite* suite = BOOST_TEST_SUITE("OptionletStripper Tests");
    suite->add(BOOST_TEST_CASE(&OptionletStripperTest::testFlatTermVolatilityStripping));
    suite->add(BOOST_TEST_CASE(&OptionletStripperTest::testTermVolatilityStripping));
    return suite;
}
