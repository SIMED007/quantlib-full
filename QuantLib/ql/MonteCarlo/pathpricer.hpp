
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

/*! \file pathpricer.hpp
    \brief base class for single-path pricers
*/

#ifndef quantlib_montecarlo_path_pricer_h
#define quantlib_montecarlo_path_pricer_h

#include <ql/option.hpp>
#include <ql/types.hpp>
#include <ql/termstructure.hpp>
#include <functional>

namespace QuantLib {

    //! base class for path pricers
    /*! Given a path the value of an option is returned on that path. */
    template<class PathType, class ValueType=double>
    class PathPricer : public std::unary_function<PathType, ValueType> {
      public:
        explicit PathPricer(const RelinkableHandle<TermStructure>& discountTS =
                                           RelinkableHandle<TermStructure>());
        virtual ~PathPricer() {}
        virtual ValueType operator()(const PathType& path) const=0;
      protected:
        RelinkableHandle<TermStructure> discountTS_;
    };

    template<class P,class V>
    PathPricer<P,V>::PathPricer(
                            const RelinkableHandle<TermStructure>& discountTS)
    : discountTS_(discountTS) {}

}


#endif
