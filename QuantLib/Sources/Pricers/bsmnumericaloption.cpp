
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
 * The license is also available at http://quantlib.sourceforge.net/LICENSE.TXT
 *
 * The members of the QuantLib Group are listed in the Authors.txt file, also
 * available at http://quantlib.sourceforge.net/Authors.txt
*/

/*! \file bsmnumericaloption.cpp
    \brief common code for numerical option evaluation

    $Source$
    $Name$
    $Log$
    Revision 1.42  2001/05/24 12:52:02  nando
    smoothing #include xx.hpp

    Revision 1.41  2001/05/22 13:25:28  marmar
    setGridLimits interface changed

    Revision 1.40  2001/04/26 16:05:42  marmar
    underlying_ not mutable anymore, setGridLimits accepts the value for center

    Revision 1.39  2001/04/23 14:20:34  marmar
    Cosmetic changes

    Revision 1.38  2001/04/09 14:13:34  nando
    all the *.hpp moved below the Include/ql level

    Revision 1.37  2001/04/06 18:46:22  nando
    changed Authors, Contributors, Licence and copyright header

*/

#include "ql/Pricers/bsmnumericaloption.hpp"
#include "ql/FiniteDifferences/valueatcenter.hpp"

namespace QuantLib {

    namespace Pricers {

        using FiniteDifferences::BoundaryCondition;
        using FiniteDifferences::BSMOperator;
        using FiniteDifferences::valueAtCenter;
        using FiniteDifferences::firstDerivativeAtCenter;
        using FiniteDifferences::secondDerivativeAtCenter;

        BSMNumericalOption::BSMNumericalOption(BSMNumericalOption::Type type,
            double underlying, double strike, Rate dividendYield,
            Rate riskFreeRate, Time residualTime, double volatility,
            int gridPoints)
        : BSMOption(type, underlying, strike, dividendYield, riskFreeRate,
            residualTime, volatility),
            gridPoints_(safeGridPoints(gridPoints, residualTime)),
            grid_(gridPoints_), initialPrices_(gridPoints_){
                hasBeenCalculated_ = false;
        }

        double BSMNumericalOption::value() const {
            if (!hasBeenCalculated_)
                calculate();
            return value_;
        }

        double BSMNumericalOption::delta() const {
            if (!hasBeenCalculated_)
                calculate();
            return delta_;
        }

        double BSMNumericalOption::gamma() const {
            if(!hasBeenCalculated_)
                calculate();
            return gamma_;
        }

        double BSMNumericalOption::theta() const {
            if(!hasBeenCalculated_)
                calculate();
            return theta_;
        }

        void BSMNumericalOption::setGridLimits(double center, double timeDelay) const {

            center_ = center;
            double volSqrtTime = volatility_*QL_SQRT(timeDelay);
            // the prefactor fine tunes performance at small volatilities
            double prefactor = 1.0 + 0.02/volSqrtTime;
            double minMaxFactor = QL_EXP(4.0 * prefactor * volSqrtTime);
            sMin_ = center_/minMaxFactor;  // underlying grid min value
            sMax_ = center_*minMaxFactor;  // underlying grid max value
            // insure strike is included in the grid
            double safetyZoneFactor = 1.1;
            if(sMin_ > strike_/safetyZoneFactor){
                sMin_ = strike_/safetyZoneFactor;
                // enforce central placement of the underlying
                sMax_ = center_/(sMin_/center_);
            }
            if(sMax_ < strike_*safetyZoneFactor){
                sMax_ = strike_*safetyZoneFactor;
                // enforce central placement of the underlying
                sMin_ = center_/(sMax_/center_);
            }
        }

        void BSMNumericalOption::initializeGrid() const {
            gridLogSpacing_ = (QL_LOG(sMax_)-QL_LOG(sMin_))/(gridPoints_-1);
            double edx = QL_EXP(gridLogSpacing_);
            grid_[0] = sMin_;
            int j;
            for (j=1; j<gridPoints_; j++)
                grid_[j] = grid_[j-1]*edx;
        }

        void BSMNumericalOption::initializeInitialCondition() const {
            int j;
            switch (type_) {
              case Call:
                for(j = 0; j < gridPoints_; j++)
                    initialPrices_[j] = QL_MAX(grid_[j]-strike_,0.0);
                break;
              case Put:
                for(j = 0; j < gridPoints_; j++)
                    initialPrices_[j] = QL_MAX(strike_-grid_[j],0.0);
                break;
              case Straddle:
                for(j = 0; j < gridPoints_; j++)
                    initialPrices_[j] = QL_FABS(strike_-grid_[j]);
                break;
              default:
                QL_REQUIRE(1 == 0,
                           "BSMNumericalOption: invalid option type");
            }
        }

        void BSMNumericalOption::initializeOperator() const {
            finiteDifferenceOperator_ = BSMOperator(gridPoints_,
                gridLogSpacing_, riskFreeRate_, dividendYield_, volatility_);

            finiteDifferenceOperator_.setLowerBC(
                BoundaryCondition(BoundaryCondition::Neumann,
                    initialPrices_[1]-initialPrices_[0]));

            finiteDifferenceOperator_.setHigherBC(
                BoundaryCondition(BoundaryCondition::Neumann,
                    initialPrices_[gridPoints_-1] -
                        initialPrices_[gridPoints_-2]));
        }

    }

}


