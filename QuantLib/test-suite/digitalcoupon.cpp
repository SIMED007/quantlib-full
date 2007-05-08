/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Cristina Duminuco
 Copyright (C) 2007 Giorgio Facchinetti

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include "digitalcoupon.hpp"
#include "utilities.hpp"
#include <ql/indexes/ibor/euribor.hpp>
#include <ql/utilities/dataformatters.hpp>
#include <ql/cashflows/digitalcoupon.hpp>
#include <ql/cashflows/capflooredcoupon.hpp>
#include <ql/settings.hpp>
#include <ql/termstructures/volatilities/capletconstantvol.hpp>
#include <ql/pricingengines/blackformula.hpp>
#include <ql/time/daycounters/actual360.hpp>

using namespace QuantLib;
using namespace boost::unit_test_framework;

QL_BEGIN_TEST_LOCALS(DigitalCouponTest)

// global data

Date today_, settlement_;
Real nominal_;
Calendar calendar_;
boost::shared_ptr<IborIndex> index_;
Natural fixingDays_;
RelinkableHandle<YieldTermStructure> termStructure_;

void setup() {
    fixingDays_ = 2;
    nominal_ = 1000000.0;
    index_ = boost::shared_ptr<IborIndex>(new Euribor6M(termStructure_));
    calendar_ = index_->fixingCalendar();
    today_ = calendar_.adjust(Date::todaysDate());
    Settings::instance().evaluationDate() = today_;
    settlement_ = calendar_.advance(today_,fixingDays_,Days);
    termStructure_.linkTo(flatRate(settlement_,0.05,Actual365Fixed()));  // by default: Continuous and annual
}

void teardown() {
    Settings::instance().evaluationDate() = Date();
}

QL_END_TEST_LOCALS(DigitalCouponTest)

void DigitalCouponTest::testAssetOrNothing() {

    BOOST_MESSAGE("Testing European asset-or-nothing digital coupon ...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    Volatility vols[] = { 0.05, 0.15, 0.30 };
    Rate strikes[] = { 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07 };

    Real gearing = 0.8;
    Real spread = 0.005;

    for (Size i = 0; i< LENGTH(vols); i++) {
            Volatility capletVolatility = vols[i];
            RelinkableHandle<CapletVolatilityStructure> volatility;
            volatility.linkTo(boost::shared_ptr<CapletVolatilityStructure>(new
                                CapletConstantVolatility(today_, capletVolatility,
                                                         Actual360())));
        for (Size j = 0; j< LENGTH(strikes); j++) {
            Rate strike = strikes[j];
            for (Size k = 0; k<10; k++) {
                Date startDate = calendar_.advance(settlement_,(k+1)*Years);
                Date endDate = calendar_.advance(settlement_,(k+2)*Years);
                Rate nullstrike = Null<Rate>();
                Real gap = 1e-08; /* very low, in order to compare digital option value 
                                     with black formula result */
                Date paymentDate = endDate;

                boost::shared_ptr<FloatingRateCoupon> underlying(
                                            new IborCoupon(paymentDate,
                                                           nominal_,
                                                           startDate,
                                                           endDate,
                                                           fixingDays_,
                                                           index_,
                                                           gearing,
                                                           spread));
                // Floating Rate Coupon - Call Digital option
                DigitalCoupon digitalCappedCoupon(underlying, strike, nullstrike,
                                                  nullstrike, false, false, gap);    
                boost::shared_ptr<IborCouponPricer> pricer(
                    new BlackIborCouponPricer(volatility));
                digitalCappedCoupon.setPricer(pricer);
                
                // Check vs decomposition used in digital coupon class implementation
                CappedFlooredIborCoupon cappedIborCoupon_d(paymentDate,
                                                           nominal_,
                                                           startDate,
                                                           endDate,
                                                           fixingDays_,
                                                           index_,
                                                           gearing,
                                                           spread,
                                                           strike - gap);
                cappedIborCoupon_d.setPricer(pricer);
                CappedFlooredIborCoupon cappedIborCoupon_u(paymentDate,
                                                           nominal_,
                                                           startDate,
                                                           endDate,
                                                           fixingDays_,
                                                           index_,
                                                           gearing,
                                                           spread,
                                                           strike + gap);
                cappedIborCoupon_u.setPricer(pricer);
                
                Time accrualPeriod = underlying->accrualPeriod();
                Real discount = termStructure_->discount(endDate);
                Real underlyingPrice = underlying->price(termStructure_);
                Real optionPrice = underlyingPrice * (cappedIborCoupon_u.rate() -
                                                      cappedIborCoupon_d.rate() )
                                                   / (2.0 * gap);
                Real decompositionPrice = underlyingPrice - optionPrice;
                Real digitalPrice = digitalCappedCoupon.price(termStructure_);
                Real error = std::fabs(decompositionPrice - digitalPrice);
                Real tolerance = 1e-10;
                if (error>tolerance) {
                    BOOST_ERROR("\nFloating Coupon - Digital Call Option:" << 
                                "\nVolatility = " << io::rate(capletVolatility) <<
                                "\nStrike = " << io::rate(strike) <<
                                "\nExercise = " << k+1 << " years" <<
                                "\nCoupon Price = "  << digitalPrice <<
                                "\nDecomposition price = " << decompositionPrice <<
                                "\nError = " << error );
                }
                
                // Check digital option price vs N(d2) price                
                Rate forward = underlying->rate();
                Date exerciseDate = underlying->fixingDate();
                Real stdDeviation = std::sqrt(
                                    volatility->blackVariance(exerciseDate,
                                                             (strike-spread)/gearing));
                Real ITM = blackFormulaCashItmProbability(Option::Call,
                                                         (strike-spread)/gearing,
                                                         (forward-spread)/gearing,
                                                          stdDeviation);
                Real nd2Price = ITM * nominal_ * accrualPeriod * discount * forward;
                optionPrice = digitalCappedCoupon.optionRate() *
                              nominal_ * accrualPeriod * discount;
                error = std::abs(nd2Price - optionPrice);
                Real optionTolerance = 1e-03;
                if (error>optionTolerance) {
                    BOOST_ERROR("\nDigital Call Option:" << 
                                "\nVolatility = " << io::rate(capletVolatility) <<
                                "\nStrike = " << io::rate(strike) <<
                                "\nExercise = " << k+1 << " years" <<
                                "\nOption price by replication "  << optionPrice <<
                                "\nOption price by black formula " << nd2Price <<
                                "\nError " << error );
                }
                
                // Floating Rate Coupon + Put Digital option
                DigitalCoupon digitalFlooredCoupon(underlying, nullstrike, strike,
                                                   nullstrike, true, true, gap);
                digitalFlooredCoupon.setPricer(pricer);

                // Check vs decomposition used in digital coupon class implementation
                CappedFlooredIborCoupon flooredIborCoupon_d(paymentDate,
                                                            nominal_,
                                                            startDate,
                                                            endDate,
                                                            fixingDays_,
                                                            index_,
                                                            gearing,
                                                            spread,
                                                            Null<Rate>(),
                                                            strike - gap);
                flooredIborCoupon_d.setPricer(pricer);
                CappedFlooredIborCoupon flooredIborCoupon_u(paymentDate,
                                                            nominal_,
                                                            startDate,
                                                            endDate,
                                                            fixingDays_,
                                                            index_,
                                                            gearing,
                                                            spread,
                                                            Null<Rate>(),
                                                            strike + gap);
                flooredIborCoupon_u.setPricer(pricer);

                optionPrice = underlyingPrice * (flooredIborCoupon_u.rate() -
                                                 flooredIborCoupon_d.rate() )
                                              / (2.0*gap);
                decompositionPrice = underlyingPrice + optionPrice;
                digitalPrice = digitalFlooredCoupon.price(termStructure_);
                error = std::fabs(decompositionPrice - digitalPrice);
                tolerance = 1.e-04; /* lower tolerance than in call case: 
                                       here the replication is slightly different
                                       from the one used in class DigitalCoupon */
                if (error>tolerance) {
                    BOOST_ERROR("\nFloating Rate Coupon + Digital Put Option:" << 
                                "\nVolatility = " << io::rate(capletVolatility) <<
                                "\nStrike = " << io::rate(strike) <<
                                "\nExercise = " << k+1 << " years" <<
                                "\nDigital coupon price "  << digitalPrice <<
                                "\nDecomposition price  " << decompositionPrice <<
                                "\nError " << error );
                }

                // Check digital option price vs N(d2) price
                ITM = blackFormulaCashItmProbability(Option::Put,
                                                    (strike-spread)/gearing,
                                                    (forward-spread)/gearing,
                                                     stdDeviation);
                nd2Price = ITM * nominal_ * accrualPeriod * discount * forward;
                optionPrice = digitalFlooredCoupon.optionRate() *
                              nominal_ * accrualPeriod * discount;
                error = std::abs(nd2Price - optionPrice);
                optionTolerance = 1.e-03;
                if (error>optionTolerance) {
                    BOOST_ERROR("\nDigital Put Option:" << 
                                "\nVolatility = " << io::rate(capletVolatility) <<
                                "\nStrike = " << io::rate(strike) <<
                                "\nExercise = " << k+1 << " years" <<
                                "\nOption price by replication "  << optionPrice <<
                                "\nOption price by black formula " << nd2Price <<
                                "\nError " << error );
                }
            }
        }
    }
    QL_TEST_TEARDOWN
}


void DigitalCouponTest::testAssetOrNothingDeepInTheMoney() {

    BOOST_MESSAGE("Testing European deep in-the-money asset-or-nothing digital coupon ...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    Real gearing = 1.0;
    Real spread = 0.0;

    Volatility capletVolatility = 0.0001;
    RelinkableHandle<CapletVolatilityStructure> volatility;
    volatility.linkTo(boost::shared_ptr<CapletVolatilityStructure>(new
            CapletConstantVolatility(today_, capletVolatility, Actual360())));
            
    for (Size k = 0; k<10; k++) {   // Loop on start and end dates
        Date startDate = calendar_.advance(settlement_,(k+1)*Years);
        Date endDate = calendar_.advance(settlement_,(k+2)*Years);
        Rate nullstrike = Null<Rate>();
        Rate cashRate = 0.01;
        Real gap = 1e-4;
        Date paymentDate = endDate;

        boost::shared_ptr<FloatingRateCoupon> underlying(
                                    new IborCoupon(paymentDate,
                                                   nominal_,
                                                   startDate,
                                                   endDate,
                                                   fixingDays_,
                                                   index_,
                                                   gearing,
                                                   spread));

        // Floating Rate Coupon - Deep-in-the-money Call Digital option
        Rate strike = 0.001;
        DigitalCoupon digitalCappedCoupon(underlying, strike, nullstrike, nullstrike,
                                          false, false, gap);    
        boost::shared_ptr<IborCouponPricer> pricer(
            new BlackIborCouponPricer(volatility));
        digitalCappedCoupon.setPricer(pricer);
        
        // Check price vs its target price            
        Time accrualPeriod = underlying->accrualPeriod();
        Real discount = termStructure_->discount(endDate);

        Real targetOptionPrice = underlying->price(termStructure_);
        Real targetPrice = 0.0;
        Real digitalPrice = digitalCappedCoupon.price(termStructure_);     
        Real error = std::fabs(targetPrice - digitalPrice);
        Real tolerance = 1e-08;
        if (error>tolerance) {
            BOOST_ERROR("\nFloating Coupon - Digital Call Option:" << 
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nCoupon Price = "  << digitalPrice <<
                        "\nTarget price = " << targetPrice <<
                        "\nError = " << error );
        }
            
        // Check digital option price
        Real replicationOptionPrice = digitalCappedCoupon.optionRate() *
                                      nominal_ * accrualPeriod * discount;
        error = std::abs(targetOptionPrice - replicationOptionPrice);
        Real optionTolerance = 1e-08;
        if (error>optionTolerance) {
            BOOST_ERROR("\nDigital Call Option:" << 
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nPrice by replication = " << replicationOptionPrice <<
                        "\nTarget price = " << targetOptionPrice <<
                        "\nError = " << error);
        }

        // Floating Rate Coupon + Deep-in-the-money Put Digital option          
        strike = 0.99;
        DigitalCoupon digitalFlooredCoupon(underlying, nullstrike, strike, nullstrike,
                                           true, true, gap);   
        digitalFlooredCoupon.setPricer(pricer);

        // Check price vs its target price
        targetOptionPrice = underlying->price(termStructure_);
        targetPrice = underlying->price(termStructure_) + targetOptionPrice ;
        digitalPrice = digitalFlooredCoupon.price(termStructure_);       
        error = std::fabs(targetPrice - digitalPrice);
        if (error>tolerance) {
            BOOST_ERROR("\nFloating Coupon + Digital Put Option:" << 
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nDigital coupon price = "  << digitalPrice <<
                        "\nTarget price = " << targetPrice <<
                        "\nError " << error);
        }

        // Check digital option
        replicationOptionPrice = digitalFlooredCoupon.optionRate() *
                                 nominal_ * accrualPeriod * discount;
        error = std::abs(targetOptionPrice - replicationOptionPrice);
        if (error>optionTolerance) {
            BOOST_ERROR("\nDigital Put Option:" << 
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nPrice by replication = " << replicationOptionPrice <<
                        "\nTarget price = " << targetOptionPrice <<
                        "\nError " << error);
        }
    }        
    QL_TEST_TEARDOWN
}

void DigitalCouponTest::testAssetOrNothingDeepOutTheMoney() {

    BOOST_MESSAGE("Testing European deep out-the-money asset-or-nothing digital coupon ...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    Real gearing = 1.0;
    Real spread = 0.0;

    Volatility capletVolatility = 0.0001;
    RelinkableHandle<CapletVolatilityStructure> volatility;
    volatility.linkTo(boost::shared_ptr<CapletVolatilityStructure>(new
                        CapletConstantVolatility(today_, capletVolatility, Actual360())));
            
    for (Size k = 0; k<10; k++) { // loop on start and end dates
        Date startDate = calendar_.advance(settlement_,(k+1)*Years);
        Date endDate = calendar_.advance(settlement_,(k+2)*Years);
        Rate nullstrike = Null<Rate>();
        Rate cashRate = 0.01;
        Real gap = 1e-4;
        Date paymentDate = endDate;

        boost::shared_ptr<FloatingRateCoupon> underlying(new IborCoupon(paymentDate,
                                                                        nominal_,
                                                                        startDate,
                                                                        endDate,
                                                                        fixingDays_,
                                                                        index_,
                                                                        gearing,
                                                                        spread));
 
        // Floating Rate Coupon - Deep-out-of-the-money Call Digital option
        Rate strike = 0.99;
        DigitalCoupon digitalCappedCoupon(underlying, strike, nullstrike, nullstrike,
                                          false, false, gap);    
        boost::shared_ptr<IborCouponPricer> pricer(new BlackIborCouponPricer(volatility));
        digitalCappedCoupon.setPricer(pricer);
        
        // Check price vs its target
        Time accrualPeriod = underlying->accrualPeriod();
        Real discount = termStructure_->discount(endDate);

        Real targetPrice = underlying->price(termStructure_);
        Real digitalPrice = digitalCappedCoupon.price(termStructure_);      
        Real error = std::fabs(targetPrice - digitalPrice);
        Real tolerance = 1e-12;
        if (error>tolerance) {
            BOOST_ERROR("\nFloating Coupon - Digital Call Option :" << 
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nCoupon price = "  << digitalPrice <<
                        "\nTarget price = " << targetPrice <<
                        "\nError = " << error );
        }
            
        // Check digital option price
        Real targetOptionPrice = 0.;
        Real replicationOptionPrice = digitalCappedCoupon.optionRate() *
                                      nominal_ * accrualPeriod * discount;
        error = std::abs(targetOptionPrice - replicationOptionPrice);
        Real optionTolerance = 1e-08;
        if (error>optionTolerance) {
            BOOST_ERROR("\nDigital Call Option:" << 
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nPrice by replication = "  << replicationOptionPrice <<
                        "\nTarget price = " << targetOptionPrice <<
                        "\nError = " << error );
        }
            
        // Floating Rate Coupon - Deep-out-of-the-money Put Digital option
        strike = 0.01;
        DigitalCoupon digitalFlooredCoupon(underlying, nullstrike, strike, nullstrike,
                                           true, true, gap);   
        digitalFlooredCoupon.setPricer(pricer);

        // Check price vs its target
        targetPrice = underlying->price(termStructure_);
        digitalPrice = digitalFlooredCoupon.price(termStructure_);
        tolerance = 1e-08;       
        error = std::fabs(targetPrice - digitalPrice);
        if (error>tolerance) {
            BOOST_ERROR("\nFloating Coupon + Digital Put Coupon:" << 
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nCoupon price = "  << digitalPrice <<
                        "\nTarget price = " << targetPrice <<
                        "\nError = " << error );
        }

        // Check digital option
        targetOptionPrice = 0.0;
        replicationOptionPrice = digitalFlooredCoupon.optionRate() *
                                 nominal_ * accrualPeriod * discount;
        error = std::abs(targetOptionPrice - replicationOptionPrice);
        if (error>optionTolerance) {
            BOOST_ERROR("\nDigital Put Coupon:" << 
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nPrice by replication = " << replicationOptionPrice <<
                        "\nTarget price = " << targetOptionPrice <<
                        "\nError = " << error );
        }
    }        
    QL_TEST_TEARDOWN
}

void DigitalCouponTest::testCashOrNothingDeepInTheMoney() {

    BOOST_MESSAGE("Testing European deep in-the-money cash-or-nothing digital coupon ...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    Real gearing = 1.0;
    Real spread = 0.0;

    Volatility capletVolatility = 0.0001;
    RelinkableHandle<CapletVolatilityStructure> volatility;
    volatility.linkTo(boost::shared_ptr<CapletVolatilityStructure>(new
            CapletConstantVolatility(today_, capletVolatility, Actual360())));

    for (Size k = 0; k<10; k++) {   // Loop on start and end dates
        Date startDate = calendar_.advance(settlement_,(k+1)*Years);
        Date endDate = calendar_.advance(settlement_,(k+2)*Years);
        Rate nullstrike = Null<Rate>();
        Rate cashRate = 0.01;
        Real gap = 1e-4;
        Date paymentDate = endDate;

        boost::shared_ptr<FloatingRateCoupon> underlying(new IborCoupon(paymentDate,
                                                                        nominal_,
                                                                        startDate,
                                                                        endDate,
                                                                        fixingDays_,
                                                                        index_,
                                                                        gearing,
                                                                        spread));
        // Floating Rate Coupon - Deep-in-the-money Call Digital option
        Rate strike = 0.001;
        DigitalCoupon digitalCappedCoupon(underlying, strike, nullstrike, cashRate,
                                          false, false, gap);    
        boost::shared_ptr<IborCouponPricer> pricer(
            new BlackIborCouponPricer(volatility));
        digitalCappedCoupon.setPricer(pricer);


        // Check price vs its target      
        Time accrualPeriod = underlying->accrualPeriod();
        Real discount = termStructure_->discount(endDate);

        Real targetOptionPrice = cashRate * nominal_ * accrualPeriod * discount;
        Real targetPrice = underlying->price(termStructure_) - targetOptionPrice;
        Real digitalPrice = digitalCappedCoupon.price(termStructure_);

        Real error = std::fabs(targetPrice - digitalPrice);
        Real tolerance = 1e-09;
        if (error>tolerance) {
            BOOST_ERROR("\nFloating Coupon - Digital Call Coupon:" <<
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nCoupon price = "  << digitalPrice <<
                        "\nTarget price = " << targetPrice <<
                        "\nError " << error );
        }

        // Check digital option price
        Real replicationOptionPrice = digitalCappedCoupon.optionRate() *
                                      nominal_ * accrualPeriod * discount;
        error = std::abs(targetOptionPrice - replicationOptionPrice);
        Real optionTolerance = 1e-09;
        if (error>optionTolerance) {
            BOOST_ERROR("\nDigital Call Option:" <<
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nPrice by replication = " << replicationOptionPrice <<
                        "\nTarget price = " << targetOptionPrice <<
                        "\nError = " << error);
        }

        // Floating Rate Coupon + Deep-in-the-money Put Digital option
        strike = 0.99;
        DigitalCoupon digitalFlooredCoupon(underlying, nullstrike, strike, cashRate,
                                           true, true, gap);   
        digitalFlooredCoupon.setPricer(pricer);

        // Check price vs its target
        targetPrice = underlying->price(termStructure_) + targetOptionPrice;
        digitalPrice = digitalFlooredCoupon.price(termStructure_);       
        error = std::fabs(targetPrice - digitalPrice);
        if (error>tolerance) {
            BOOST_ERROR("\nFloating Coupon + Digital Put Option:" <<
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nCoupon price = "  << digitalPrice <<
                        "\nTarget price  = " << targetPrice <<
                        "\nError = " << error );
        }

        // Check digital option
        replicationOptionPrice = digitalFlooredCoupon.optionRate() *
                                 nominal_ * accrualPeriod * discount;
        error = std::abs(targetOptionPrice - replicationOptionPrice);
        if (error>optionTolerance) {
            BOOST_ERROR("\nDigital Put Coupon:" <<
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nPrice by replication = " << replicationOptionPrice <<
                        "\nTarget price = " << targetOptionPrice <<
                        "\nError = " << error );
        }
    }
    QL_TEST_TEARDOWN
}

void DigitalCouponTest::testCashOrNothingDeepOutTheMoney() {

    BOOST_MESSAGE("Testing European deep out-the-money cash-or-nothing digital coupon ...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    Real gearing = 1.0;
    Real spread = 0.0;

    Volatility capletVolatility = 0.0001;
    RelinkableHandle<CapletVolatilityStructure> volatility;
    volatility.linkTo(boost::shared_ptr<CapletVolatilityStructure>(new
                        CapletConstantVolatility(today_,capletVolatility,Actual360())));

    for (Size k = 0; k<10; k++) { // loop on start and end dates
        Date startDate = calendar_.advance(settlement_,(k+1)*Years);
        Date endDate = calendar_.advance(settlement_,(k+2)*Years);
        Rate nullstrike = Null<Rate>();
        Rate cashRate = 0.01;
        Real gap = 1e-4;
        Date paymentDate = endDate;

        boost::shared_ptr<FloatingRateCoupon> underlying(new IborCoupon(paymentDate,
                                                                        nominal_,
                                                                        startDate,
                                                                        endDate,
                                                                        fixingDays_,
                                                                        index_,
                                                                        gearing,
                                                                        spread));
        // Deep out-of-the-money Capped Digital Coupon
        Rate strike = 0.99;
        DigitalCoupon digitalCappedCoupon(underlying, strike, nullstrike, cashRate,
                                          false, false, gap);    
        boost::shared_ptr<IborCouponPricer> pricer(new BlackIborCouponPricer(volatility));
        digitalCappedCoupon.setPricer(pricer);

        // Check price vs its target
        Time accrualPeriod = underlying->accrualPeriod();
        Real discount = termStructure_->discount(endDate);

        Real targetPrice = underlying->price(termStructure_);
        Real digitalPrice = digitalCappedCoupon.price(termStructure_);      
        Real error = std::fabs(targetPrice - digitalPrice);
        Real tolerance = 1e-12;
        if (error>tolerance) {
            BOOST_ERROR("\nFloating Coupon + Digital Call Option:" <<
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nCoupon price = "  << digitalPrice <<
                        "\nTarget price  = " << targetPrice <<
                        "\nError = " << error );
        }

        // Check digital option price
        Real targetOptionPrice = 0.;
        Real replicationOptionPrice = digitalCappedCoupon.optionRate() *
                                      nominal_ * accrualPeriod * discount;
        error = std::abs(targetOptionPrice - replicationOptionPrice);
        Real optionTolerance = 1e-09;
        if (error>optionTolerance) {
            BOOST_ERROR("\nDigital Call Option:" <<
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nPrice by replication = "  << replicationOptionPrice <<
                        "\nTarget price = " << targetOptionPrice <<
                        "\nError = " << error );
        }

        // Deep out-of-the-money Floored Digital Coupon
        strike = 0.01;
        DigitalCoupon digitalFlooredCoupon(underlying, nullstrike, strike, cashRate,
                                           true, true, gap);   
        digitalFlooredCoupon.setPricer(pricer);

        // Check price vs its target
        targetPrice = underlying->price(termStructure_);
        digitalPrice = digitalFlooredCoupon.price(termStructure_);
        tolerance = 1e-09;
        error = std::fabs(targetPrice - digitalPrice);
        if (error>tolerance) {
            BOOST_ERROR("\nDigital Floored Coupon:" <<
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nCoupon price = "  << digitalPrice <<
                        "\nTarget price  = " << targetPrice <<
                        "\nError = " << error );
        }

        // Check digital option
        targetOptionPrice = 0.0;
        replicationOptionPrice = digitalFlooredCoupon.optionRate() *
                                 nominal_ * accrualPeriod * discount;
        error = std::abs(targetOptionPrice - replicationOptionPrice);
        if (error>optionTolerance) {
            BOOST_ERROR("\nDigital Put Option:" <<
                        "\nVolatility = " << io::rate(capletVolatility) <<
                        "\nStrike = " << io::rate(strike) <<
                        "\nExercise = " << k+1 << " years" <<
                        "\nPrice by replication " << replicationOptionPrice <<
                        "\nTarget price " << targetOptionPrice <<
                        "\nError " << error );
        }
    }
    QL_TEST_TEARDOWN
}

void DigitalCouponTest::testCashOrNothing() {

    BOOST_MESSAGE("Testing European cash-or-nothing digital coupon ...");

    QL_TEST_BEGIN
    QL_TEST_SETUP

    Volatility vols[] = { 0.05, 0.15, 0.30 };
    Rate strikes[] = { 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07 };

    Real gearing = 3.0;
    Real spread = -0.0002;

    for (Size i = 0; i< LENGTH(vols); i++) {
            Volatility capletVolatility = vols[i];
            RelinkableHandle<CapletVolatilityStructure> volatility;
            volatility.linkTo(boost::shared_ptr<CapletVolatilityStructure>(new
                        CapletConstantVolatility(today_,capletVolatility,Actual360())));
        for (Size j = 0; j< LENGTH(strikes); j++) {
            Rate strike = strikes[j];
            for (Size k = 0; k<10; k++) {
                Date startDate = calendar_.advance(settlement_,(k+1)*Years);
                Date endDate = calendar_.advance(settlement_,(k+2)*Years);
                Rate nullstrike = Null<Rate>();
                Rate cashRate = 0.01;
                Real gap = 1e-08; /* very low, in order to compare digital option value 
                                     with black formula result */
                Date paymentDate = endDate;

                boost::shared_ptr<FloatingRateCoupon> underlying(
                                            new IborCoupon(paymentDate,
                                                           nominal_,
                                                           startDate,
                                                           endDate,
                                                           fixingDays_,
                                                           index_,
                                                           gearing,
                                                           spread));
                // Floating Rate Coupon - Call Digital option
                DigitalCoupon digitalCappedCoupon(underlying, strike, nullstrike,
                                                  cashRate, false, false, gap);    
                boost::shared_ptr<IborCouponPricer> pricer(
                    new BlackIborCouponPricer(volatility));
                digitalCappedCoupon.setPricer(pricer);

                // Check vs decomposition used in digital coupon class implementation
                CappedFlooredIborCoupon cappedIborCoupon_d(paymentDate,
                                                           nominal_,
                                                           startDate,
                                                           endDate,
                                                           fixingDays_,
                                                           index_,
                                                           gearing,
                                                           spread,
                                                           strike - gap);
                cappedIborCoupon_d.setPricer(pricer);
                CappedFlooredIborCoupon cappedIborCoupon_u(paymentDate,
                                                           nominal_,
                                                           startDate,
                                                           endDate,
                                                           fixingDays_,
                                                           index_,
                                                           gearing,
                                                           spread,
                                                           strike + gap);
                cappedIborCoupon_u.setPricer(pricer);

                Time accrualPeriod = underlying->accrualPeriod();
                Real discount = termStructure_->discount(endDate);

                Real optionPrice = cashRate * (cappedIborCoupon_u.rate() -
                                               cappedIborCoupon_d.rate() )
                                            / (2.0*gap) *
                                   nominal_ *  accrualPeriod *  discount;
                Real decompositionPrice = underlying->price(termStructure_) -
                                          optionPrice;
                Real digitalPrice = digitalCappedCoupon.price(termStructure_);
                Real error = std::fabs(decompositionPrice - digitalPrice);
                Real tolerance = 1.e-10;
                if (error>tolerance) {
                    BOOST_ERROR("\nDigital Coupon - Digital Call Option:" <<
                                "\nVolatility = " << io::rate(capletVolatility) <<
                                "\nStrike = " << io::rate(strike) <<
                                "\nExercise = " << k+1 << " years" <<
                                "\nCoupon price = "  << digitalPrice <<
                                "\nDecomposition price  = " << decompositionPrice <<
                                "\nError = " << error );
                }

                // Check digital option price vs N(d2) price
                Date exerciseDate = underlying->fixingDate();
                Real forward = underlying->rate();
                Real stdDeviation = std::sqrt(
                                    volatility->blackVariance(exerciseDate,
                                                             (strike-spread)/gearing));
                Real ITM = blackFormulaCashItmProbability(Option::Call,
                                                         (strike-spread)/gearing,
                                                         (forward-spread)/gearing,
                                                          stdDeviation);
                Real nd2Price = ITM * nominal_ * accrualPeriod * discount * cashRate;
                optionPrice = digitalCappedCoupon.optionRate() *
                              nominal_ * accrualPeriod * discount;
                error = std::abs(nd2Price - optionPrice);
                Real optionTolerance = 1e-04;
                if (error>optionTolerance) {
                    BOOST_ERROR("\nDigital Put Option:" <<
                                "\nVolatility = " << io::rate(capletVolatility) <<
                                "\nStrike = " << io::rate(strike) <<
                                "\nExercise = " << k+1 << " years" <<
                                "\nPrice by replication = "  << optionPrice <<
                                "\nPrice by black formula = " << nd2Price <<
                                "\nError = " << error );
                }

                // Floating Rate Coupon + Put Digital option
                DigitalCoupon digitalFlooredCoupon(underlying, nullstrike, strike,
                                                   cashRate, true, true, gap);   
                digitalFlooredCoupon.setPricer(pricer);

                // Check vs decomposition used in digital coupon class implementation
                CappedFlooredIborCoupon flooredIborCoupon_d(paymentDate,
                                                            nominal_,
                                                            startDate,
                                                            endDate,
                                                            fixingDays_,
                                                            index_,
                                                            gearing,
                                                            spread,
                                                            nullstrike,
                                                            strike - gap);
                flooredIborCoupon_d.setPricer(pricer);
                CappedFlooredIborCoupon flooredIborCoupon_u(paymentDate,
                                                            nominal_,
                                                            startDate,
                                                            endDate,
                                                            fixingDays_,
                                                            index_,
                                                            gearing,
                                                            spread,
                                                            nullstrike,
                                                            strike + gap);
                flooredIborCoupon_u.setPricer(pricer);
                optionPrice = cashRate * (flooredIborCoupon_u.rate() -
                                          flooredIborCoupon_d.rate() )
                                       / (2.0*gap) *
                              nominal_ * accrualPeriod * discount;
                decompositionPrice = underlying->price(termStructure_) + optionPrice;
                digitalPrice = digitalFlooredCoupon.price(termStructure_);
                error = std::fabs(decompositionPrice - digitalPrice);
                tolerance = 1.e-04; /* lower tolerance than in call case: 
                                       here the replication is slightly different
                                       from the one used in class DigitalCoupon */
                if (error>tolerance) {
                    BOOST_ERROR("\nDigital Coupon + Put Digital Option:" <<
                                "\nVolatility = " << io::rate(capletVolatility) <<
                                "\nStrike = " << io::rate(strike) <<
                                "\nExercise = " << k+1 << " years" <<
                                "\nCoupon price = "  << digitalPrice <<
                                "\nDecomposition price  = " << decompositionPrice <<
                                "\nError = " << error );
                }

                // Check digital option price vs N(d2) price
                ITM = blackFormulaCashItmProbability(Option::Put,
                                                     (strike-spread)/gearing,
                                                     (forward-spread)/gearing,
                                                      stdDeviation);
                nd2Price = ITM * nominal_ * accrualPeriod * discount * cashRate;
                optionPrice = digitalFlooredCoupon.optionRate() *
                              nominal_ * accrualPeriod * discount;
                error = std::abs(nd2Price - optionPrice);
                if (error>optionTolerance) {
                    BOOST_ERROR("\nPut Digital Option:" <<
                                "\nVolatility = " << io::rate(capletVolatility) <<
                                "\nStrike = " << io::rate(strike) <<
                                "\nExercise = " << k+1 << " years" <<
                                "\nPrice by replication = "  << optionPrice <<
                                "\nPrice by black formula = " << nd2Price <<
                                "\nError = " << error );
                }
            }
        }
    }
    QL_TEST_TEARDOWN

}

test_suite* DigitalCouponTest::suite() {
    test_suite* suite = BOOST_TEST_SUITE("Digital coupon tests");
    suite->add(BOOST_TEST_CASE(&DigitalCouponTest::testAssetOrNothing));
    suite->add(BOOST_TEST_CASE(&DigitalCouponTest::testAssetOrNothingDeepInTheMoney));
    suite->add(BOOST_TEST_CASE(&DigitalCouponTest::testAssetOrNothingDeepOutTheMoney));
    suite->add(BOOST_TEST_CASE(&DigitalCouponTest::testCashOrNothing));
    suite->add(BOOST_TEST_CASE(&DigitalCouponTest::testCashOrNothingDeepInTheMoney));
    suite->add(BOOST_TEST_CASE(&DigitalCouponTest::testCashOrNothingDeepOutTheMoney));
    return suite;
}
