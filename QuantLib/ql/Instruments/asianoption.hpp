
/*
 Copyright (C) 2003, 2004 Ferdinando Ametrano
 Copyright (C) 2004 StatPro Italia srl

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

/*! \file asianoption.hpp
    \brief Asian option on a single asset
*/

#ifndef quantlib_asian_option_hpp
#define quantlib_asian_option_hpp

#include <ql/Instruments/oneassetstrikedoption.hpp>
#include <vector>

namespace QuantLib {

    //! placeholder for enumerated averaging types
    struct Average {
        enum Type { Arithmetic, Geometric };
    };

    //! Asian option
    /*! \ingroup instruments */
    class DiscreteAveragingAsianOption : public OneAssetStrikedOption {
      public:
        class arguments;
        class engine;
        DiscreteAveragingAsianOption(
                Average::Type averageType,
                double runningProduct,
                Size pastFixings,
                std::vector<Date> fixingDates,
                const boost::shared_ptr<BlackScholesProcess>&,
                const boost::shared_ptr<StrikedTypePayoff>& payoff,
                const boost::shared_ptr<Exercise>& exercise,
                const boost::shared_ptr<PricingEngine>& engine = 
                                           boost::shared_ptr<PricingEngine>());
        void setupArguments(Arguments*) const;
      protected:
        // arguments
        Average::Type averageType_;
        double runningProduct_;
        Size pastFixings_;
        std::vector<Date> fixingDates_;
    };

    //! extra arguments for single asset asian option calculation
    class DiscreteAveragingAsianOption::arguments 
        : public OneAssetStrikedOption::arguments {
      public:
        void validate() const;
        Average::Type averageType;
        double runningProduct;
        Size pastFixings;
        std::vector<Date> fixingDates;
    };

    //! Discrete averaging asian engine base class
    class DiscreteAveragingAsianOption::engine 
        : public GenericEngine<DiscreteAveragingAsianOption::arguments, 
                               DiscreteAveragingAsianOption::results> {};


}


#endif

