/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2008 Andreas Gaida
 Copyright (C) 2008 Ralph Schreyer
 Copyright (C) 2008 Klaus Spanderen

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

/*! \file fdhestonbarrierengine.cpp
    \brief Finite-Differences Heston barrier option engine
*/

#include <ql/math/distributions/normaldistribution.hpp>
#include <ql/methods/finitedifferences/multidim/fdmamericanstepcondition.hpp>
#include <ql/methods/finitedifferences/multidim/fdmdividendhandler.hpp>
#include <ql/methods/finitedifferences/multidim/fdmhestonsolver.hpp>
#include <ql/methods/finitedifferences/multidim/fdmhestonvariancemesher.hpp>
#include <ql/methods/finitedifferences/multidim/fdminnervaluecalculator.hpp>
#include <ql/methods/finitedifferences/multidim/fdmlinearoplayout.hpp>
#include <ql/methods/finitedifferences/multidim/fdmmeshercomposite.hpp>
#include <ql/methods/finitedifferences/multidim/uniform1dmesher.hpp>
#include <ql/pricingengines/barrier/fdhestonbarrierengine.hpp>
#include <ql/pricingengines/barrier/fdhestonrebateengine.hpp>
#include <ql/pricingengines/vanilla/fdhestonvanillaengine.hpp>
#include <ql/time/daycounters/actualactual.hpp>

namespace QuantLib {

	FdHestonBarrierEngine::FdHestonBarrierEngine(
			const boost::shared_ptr<HestonModel>& model, 
			Size tGrid, Size xGrid, Size vGrid)
    : GenericModelEngine<HestonModel,
    					DividendBarrierOption::arguments,
    					DividendBarrierOption::results>(model),
      tGrid_(tGrid), xGrid_(xGrid), vGrid_(vGrid) {
    }
    
    void FdHestonBarrierEngine::calculate() const {
    	
        // 1. Layout
		std::vector<Size> dim;
		dim.push_back(xGrid_);
		dim.push_back(vGrid_);
		const boost::shared_ptr<FdmLinearOpLayout> layout(
											  new FdmLinearOpLayout(dim));

		// 2. Mesher
        const boost::shared_ptr<HestonProcess>& process = model_->process();
	    const Time maturity = process->time(arguments_.exercise->lastDate());

	    // 2.1 The variance mesher
	    const boost::shared_ptr<FdmHestonVarianceMesher> varianceMesher(
			new FdmHestonVarianceMesher(layout->dim()[1], process, maturity));

	    // 2.2 The equity mesher	    
	    // Calculate the forward
        const Real spot = process->s0()->value();
        QL_REQUIRE(spot > 0.0, "negative or null underlying given");
        Real F = spot*process->dividendYield()->discount(maturity)
		             /process->riskFreeRate()->discount(maturity);
        std::vector<Date> dividendDates;
    	std::vector<Time> dividendTimes;
    	std::vector<Real> dividends;
        if(!arguments_.cashFlow.empty()) {
            Date todaysDate = Date::todaysDate();
            DayCounter dayCounter = ActualActual();
        	for (DividendSchedule::const_iterator iter 
        						= arguments_.cashFlow.begin();
        	     				iter != arguments_.cashFlow.end(); ++iter) {
        		dividendDates.push_back((*iter)->date());
        		dividendTimes.push_back(dayCounter.yearFraction(
        				Settings::instance().evaluationDate(),(*iter)->date()));
        		dividends.push_back((*iter)->amount());        		
        		F -= dividends.back()*process->riskFreeRate()->discount(
        												dividendTimes.back());        		
        	}
        }
        QL_REQUIRE(F > 0.0, "negative forward given");
        
        // Set the grid boundaries
        const Real normInvEps = fabs(InverseCumulativeNormal()(0.0001));
        const Real sigmaSqrtT =  varianceMesher->volaEstimate()
                                *std::sqrt(maturity);       
        Real xMin = std::log(F) - sigmaSqrtT*normInvEps*2.0
                                - sigmaSqrtT*sigmaSqrtT/2.0;
        Real xMax = std::log(F) + sigmaSqrtT*normInvEps*2.0 
                                - sigmaSqrtT*sigmaSqrtT/2.0;
        if (   arguments_.barrierType == Barrier::DownIn 
        	|| arguments_.barrierType == Barrier::DownOut) {
        	xMin = std::log(arguments_.barrier);
        }
        if (   arguments_.barrierType == Barrier::UpIn 
        	|| arguments_.barrierType == Barrier::UpOut) {
        	xMax = std::log(arguments_.barrier);
        }

		const boost::shared_ptr<Fdm1dMesher> equityMesher(
							new Uniform1dMesher(xMin, xMax, layout->dim()[0]));

		std::vector<boost::shared_ptr<Fdm1dMesher> > meshers;
		meshers.push_back(equityMesher);
		meshers.push_back(varianceMesher);
		boost::shared_ptr<FdmMesher> mesher (new FdmMesherComposite(
															layout, meshers));

		// 3. Step conditions
        std::list<boost::shared_ptr<StepCondition<Array> > > stepConditions;
		std::list<std::vector<Time> > stoppingTimes;

		// 3.1 Step condition if discrete dividends
        if(!arguments_.cashFlow.empty()) {
    		boost::shared_ptr<StepCondition<Array> > dividendCondition(
    			new FdmDividendHandler(dividendTimes, dividends, mesher, 0));
    		stepConditions.push_back(dividendCondition);
    		stoppingTimes.push_back(dividendTimes);
        }

		// 3.2 Step condition if american exercise
		if (arguments_.exercise->type() == Exercise::American) {
			boost::shared_ptr<FdmInnerValueCalculator> calculator(
									new FdmLogInnerValue(arguments_.payoff, 0));
			stepConditions.push_back(boost::shared_ptr<StepCondition<Array> >(
							new FdmAmericanStepCondition(mesher,calculator)));
		}

        boost::shared_ptr<FdmStepConditionComposite> conditions( 
				new FdmStepConditionComposite(stoppingTimes, stepConditions));

		// 4. Boundary conditions 
		std::vector<boost::shared_ptr<FdmDirichletBoundary> > boundaries; 
        if (   arguments_.barrierType == Barrier::DownIn 
        	|| arguments_.barrierType == Barrier::DownOut) {
			boundaries.push_back(boost::shared_ptr<FdmDirichletBoundary>(
				new FdmDirichletBoundary(layout, arguments_.rebate, 0, 
										 FdmDirichletBoundary::Lower)));

		}
        if (   arguments_.barrierType == Barrier::UpIn 
        	|| arguments_.barrierType == Barrier::UpOut) {
			boundaries.push_back(boost::shared_ptr<FdmDirichletBoundary>(
				new FdmDirichletBoundary(layout, arguments_.rebate, 0, 
										 FdmDirichletBoundary::Upper)));
		}

		// 5. Solver		
		boost::shared_ptr<FdmHestonSolver> solver(new FdmHestonSolver(
										Handle<HestonProcess>(process), 
										mesher, boundaries, conditions,
										arguments_.payoff, maturity, tGrid_));
		
		results_.value = solver->valueAt(spot, process->v0());
		results_.delta = solver->deltaAt(spot, process->v0(), spot*0.01);
		results_.gamma = solver->gammaAt(spot, process->v0(), spot*0.01);
		results_.theta = solver->thetaAt(spot, process->v0());
		
		// 6. Calculate vanilla option and rebate for in-barriers 
        if (   arguments_.barrierType == Barrier::DownIn 
        	|| arguments_.barrierType == Barrier::UpIn) {
        	// Cast the payoff
            boost::shared_ptr<StrikedTypePayoff> payoff =
                	boost::dynamic_pointer_cast<StrikedTypePayoff>(
                											arguments_.payoff);
            // Calculate the vanilla option
            boost::shared_ptr<DividendVanillaOption> vanillaOption(
        			new DividendVanillaOption(payoff,arguments_.exercise,
        									  dividendDates, dividends));
            vanillaOption->setPricingEngine(boost::shared_ptr<PricingEngine>(
            		new FdHestonVanillaEngine(model_, tGrid_, xGrid_, vGrid_)));
            // Calculate the rebate value
            boost::shared_ptr<DividendBarrierOption> rebateOption(
        			new DividendBarrierOption(arguments_.barrierType,
        					                  arguments_.barrier,
        					                  arguments_.rebate,
        					                  payoff, arguments_.exercise,
        									  dividendDates, dividends));
            rebateOption->setPricingEngine(boost::shared_ptr<PricingEngine>(
            		new FdHestonRebateEngine(model_, tGrid_, 20, 10)));

			results_.value = vanillaOption->NPV()   + rebateOption->NPV() 
			                                        - results_.value;
			results_.delta = vanillaOption->delta() + rebateOption->delta()
			                                        - results_.delta;
			results_.gamma = vanillaOption->gamma() + rebateOption->gamma()
			                                        - results_.gamma;
			results_.theta = vanillaOption->theta() + rebateOption->theta()
			                                        - results_.theta;
        }
    }
}
