
/*
 Copyright (C) 2003 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

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
/*! \file mctypedefs.hpp
    \brief Default choices for template instantiations

    \fullpath
    ql/MonteCarlo/%mctypedefs.hpp
*/

// $Id$

#ifndef quantlib_mc_typedefs_h
#define quantlib_mc_typedefs_h

#include <ql/RandomNumbers/rngtypedefs.hpp>
#include <ql/MonteCarlo/pathgenerator.hpp>
#include <ql/MonteCarlo/multipathgenerator.hpp>
#include <ql/MonteCarlo/pathpricer.hpp>
#include <ql/MonteCarlo/montecarlomodel.hpp>
#include <ql/Math/statistics.hpp>

namespace QuantLib {

    namespace MonteCarlo {

        //! default choice for Gaussian path generator.
        typedef PathGenerator_old<RandomNumbers::GaussianRandomGenerator>
            GaussianPathGenerator_old;

        typedef PathGenerator<RandomNumbers::GaussianRandomSequenceGenerator>
            GaussianPathGenerator;

        //! default choice for Gaussian multi-path generator.
        typedef
        MultiPathGenerator<RandomNumbers::RandomArrayGenerator<
            RandomNumbers::GaussianRandomGenerator> >
            GaussianMultiPathGenerator;

        //! default choice for one-factor Monte Carlo model.
        typedef MonteCarloModel<Math::Statistics,
                                GaussianPathGenerator_old,
                                PathPricer_old<Path> >
                                    OneFactorMonteCarloOption_old;

        //! default choice for multi-factor Monte Carlo model.
        typedef MonteCarloModel<Math::Statistics,
                                GaussianMultiPathGenerator,
                                PathPricer_old<MultiPath> >
                                    MultiFactorMonteCarloOption;

    }

}


#endif
