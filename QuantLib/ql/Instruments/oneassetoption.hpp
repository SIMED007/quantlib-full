
/*
 Copyright (C) 2003 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file oneassetoption.hpp
    \brief Option on a single asset
*/

#ifndef quantlib_oneasset_option_h
#define quantlib_oneasset_option_h

#include <ql/option.hpp>
#include <ql/stochasticprocess.hpp>

namespace QuantLib {

    //! Base class for options on a single asset
    class OneAssetOption : public Option {
      public:
        OneAssetOption(const Handle<BlackScholesStochasticProcess>& stochProc,
                       const Handle<Payoff>& payoff,
                       const Handle<Exercise>& exercise,
                       const Handle<PricingEngine>& engine =
                                            Handle<PricingEngine>());
        //! \name Instrument interface
        //@{
        class arguments;
        class results;
        bool isExpired() const;
        //@}
        //! \name greeks
        //@{
        double delta() const;
        double deltaForward() const;
        double elasticity() const;
        double gamma() const;
        double theta() const;
        double thetaPerDay() const;
        double vega() const;
        double rho() const;
        double dividendRho() const;
        double itmCashProbability() const;
        //@}
        /*! \warning Options with a gamma that changes sign have
                     values that are <b>not</b> monotonic in the
                     volatility, e.g binary options. In these cases
                     impliedVolatility can fail and in any case it is
                     almost meaningless.  Another possible source of
                     failure is to have a targetValue that is not
                     attainable with any volatility, e.g.  a
                     targetValue lower than the intrinsic value in the
                     case of American options.

            \bug run-time crashes are possible with the Borland compiler
        */
        double impliedVolatility(double price,
                                 double accuracy = 1.0e-4,
                                 Size maxEvaluations = 100,
                                 double minVol = QL_MIN_VOLATILITY,
                                 double maxVol = QL_MAX_VOLATILITY) const;
        void setupArguments(Arguments*) const;
      protected:
        void setupExpired() const;
        void performCalculations() const;
        // results
        mutable double delta_, deltaForward_, elasticity_, gamma_, theta_,
            thetaPerDay_, vega_, rho_, dividendRho_, itmCashProbability_;
        // arguments
        Handle<BlackScholesStochasticProcess> blackScholesProcess_;
      private:
        // helper class for implied volatility calculation
        class ImpliedVolHelper {
          public:
            ImpliedVolHelper(const Handle<PricingEngine>& engine,
                             double targetValue);
            double operator()(double x) const;
          private:
            Handle<PricingEngine> engine_;
            double targetValue_;
            Handle<SimpleQuote> vol_;
            const Value* results_;
        };
    };

    //! %Arguments for single-asset option calculation
    class OneAssetOption::arguments : public Option::arguments {
      public:
        arguments() {}
        void validate() const;
        Handle<BlackScholesStochasticProcess> blackScholesProcess;
    };

    //! %Results from single-asset option calculation
    class OneAssetOption::results : public Value,
                                    public Greeks,
                                    public MoreGreeks {
      public:
        void reset() {
            Value::reset();
            Greeks::reset();
            MoreGreeks::reset();
        }
    };

}


#endif

