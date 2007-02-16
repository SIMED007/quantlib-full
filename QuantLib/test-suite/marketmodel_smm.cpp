/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Marco Bianchetti
 Copyright (C) 2007 Cristina Duminuco

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

#include "marketmodel_smm.hpp"
#include "utilities.hpp"
#include <ql/MarketModels/Products/MultiStep/multistepcoterminalswaps.hpp>
#include <ql/MarketModels/Products/MultiStep/multistepcoterminalswaptions.hpp>
#include <ql/MarketModels/Products/MultiStep/multistepswap.hpp>
#include <ql/MarketModels/Products/multiproductcomposite.hpp>
#include <ql/MarketModels/accountingengine.hpp>
#include <ql/MarketModels/utilities.hpp>
#include <ql/MarketModels/Evolvers/coterminalswapratepcevolver.hpp>
#include <ql/MarketModels/Evolvers/forwardratepcevolver.hpp>
#include <ql/MarketModels/Models/expcorrflatvol.hpp>
#include <ql/MarketModels/Models/expcorrabcdvol.hpp>
#include <ql/MarketModels/BrownianGenerators/mtbrowniangenerator.hpp>
#include <ql/MarketModels/BrownianGenerators/sobolbrowniangenerator.hpp>
#include <ql/MarketModels/swapforwardmappings.hpp>
#include <ql/MarketModels/CurveStates/coterminalswapcurvestate.hpp>
#include <ql/MonteCarlo/genericlsregression.hpp>
#include <ql/ShortRateModels/LiborMarketModels/lmlinexpcorrmodel.hpp>
#include <ql/ShortRateModels/LiborMarketModels/lmextlinexpvolmodel.hpp>
#include <ql/schedule.hpp>
#include <ql/Calendars/nullcalendar.hpp>
#include <ql/DayCounters/simpledaycounter.hpp>
#include <ql/PricingEngines/blackformula.hpp>
#include <ql/PricingEngines/blackcalculator.hpp>
#include <ql/Utilities/dataformatters.hpp>
#include <ql/Math/segmentintegral.hpp>
#include <ql/Math/convergencestatistics.hpp>
#include <ql/Math/functional.hpp>
#include <ql/Optimization/simplex.hpp>
#include <sstream>
#include <ql/Math/sequencestatistics.hpp>

#if defined(BOOST_MSVC)
#include <float.h>
//namespace { unsigned int u = _controlfp(_EM_INEXACT, _MCW_EM); }
#endif

using namespace QuantLib;
using namespace boost::unit_test_framework;

QL_BEGIN_TEST_LOCALS(MarketModelTest)

#define BEGIN(x) (x+0)
#define END(x) (x+LENGTH(x))

Date todaysDate, startDate, endDate;
std::vector<Time> rateTimes;
std::vector<Real> accruals;
Calendar calendar;
DayCounter dayCounter;
std::vector<Rate> todaysForwards;
std::vector<Real> coterminalAnnuity;
Spread displacement;
std::vector<DiscountFactor> todaysDiscounts;
std::vector<Volatility> volatilities, blackVols;
std::vector<Volatility> swaptionsVolatilities, swaptionsBlackVols;
Real a, b, c, d;
Real longTermCorrelation, beta;
Size measureOffset_;
unsigned long seed_;
Size paths_, trainingPaths_;
bool printReport_ = true;

void setup() {

    // Times
    calendar = NullCalendar();
    todaysDate = Settings::instance().evaluationDate();
    //startDate = todaysDate + 5*Years;
    endDate = todaysDate + 10*Years;
    Schedule dates(todaysDate, endDate, Period(Semiannual),
                   calendar, Following, Following, true, false);
    rateTimes = std::vector<Time>(dates.size()-1);
    accruals = std::vector<Real>(rateTimes.size()-1);
    dayCounter = SimpleDayCounter();
    for (Size i=1; i<dates.size(); ++i)
        rateTimes[i-1] = dayCounter.yearFraction(todaysDate, dates[i]);
    for (Size i=1; i<rateTimes.size(); ++i)
        accruals[i-1] = rateTimes[i] - rateTimes[i-1];

    // Rates & displacement
    todaysForwards = std::vector<Rate>(accruals.size());
    displacement = 0.02;
    for (Size i=0; i<todaysForwards.size(); ++i) {
        todaysForwards[i] = 0.05 + 0.0010*i;
        //todaysForwards[i] = 0.04;
    }

    // Discounts
    todaysDiscounts = std::vector<DiscountFactor>(rateTimes.size());
    todaysDiscounts[0] = 0.95;
    for (Size i=1; i<rateTimes.size(); ++i)
        todaysDiscounts[i] = todaysDiscounts[i-1] /
            (1.0+todaysForwards[i-1]*accruals[i-1]);

    //// Coterminal swap rates & annuities
    //Size N = todaysForwards.size();
    //todaysCoterminalSwapRates = std::vector<Rate>(N);
    //coterminalAnnuity = std::vector<Real>(N);
    //Real floatingLeg = 0.0;
    //for (Size i=1; i<=N; ++i) {
    //    if (i==1) {
    //        coterminalAnnuity[N-1] = accruals[N-1]*todaysDiscounts[N];
    //    } else {
    //        coterminalAnnuity[N-i] = coterminalAnnuity[N-i+1] +
    //                                 accruals[N-i]*todaysDiscounts[N-i+1];
    //    }
    //    floatingLeg = todaysDiscounts[N-i]-todaysDiscounts[N];
    //    todaysCoterminalSwapRates[N-i] = floatingLeg/coterminalAnnuity[N-i];
    //}

    // Cap/Floor Volatilities
    Volatility mktVols[] = {0.15541283,
                            0.18719678,
                            0.20890740,
                            0.22318179,
                            0.23212717,
                            0.23731450,
                            0.23988649,
                            0.24066384,
                            0.24023111,
                            0.23900189,
                            0.23726699,
                            0.23522952,
                            0.23303022,
                            0.23076564,
                            0.22850101,
                            0.22627951,
                            0.22412881,
                            0.22206569,
                            0.22009939
    };
    a = -0.0597;
    b =  0.1677;
    c =  0.5403;
    d =  0.1710;
    volatilities = std::vector<Volatility>(todaysForwards.size());
    blackVols = std::vector<Volatility>(todaysForwards.size());
    for (Size i=0; i<todaysForwards.size(); i++) {
        volatilities[i] = todaysForwards[i]*mktVols[i]/
                         (todaysForwards[i]+displacement);
        blackVols[i]= mktVols[i];
    }

    // Swaption volatility quick fix
    swaptionsVolatilities = volatilities;

    // Cap/Floor Correlation
    longTermCorrelation = 0.5;
    beta = 0.2;
    measureOffset_ = 5;

    // Monte Carlo
    seed_ = 42;

#ifdef _DEBUG
    paths_ = 127;
    trainingPaths_ = 31;
#else
    paths_ = 32767; //262144-1; //; // 2^15-1
    trainingPaths_ = 8191; // 2^13-1
#endif
}

const boost::shared_ptr<SequenceStatistics> simulate(
        const boost::shared_ptr<MarketModelEvolver>& evolver,
        const MarketModelMultiProduct& product)
{
    Size initialNumeraire = evolver->numeraires().front();
    Real initialNumeraireValue = todaysDiscounts[initialNumeraire];

    AccountingEngine engine(evolver, product, initialNumeraireValue);
    boost::shared_ptr<SequenceStatistics> stats(new
        SequenceStatistics(product.numberOfProducts()));
    engine.multiplePathValues(*stats, paths_);
    return stats;
}


enum MarketModelType { ExponentialCorrelationFlatVolatility,
                       ExponentialCorrelationAbcdVolatility/*,
                       CalibratedMM*/
};

std::string marketModelTypeToString(MarketModelType type) {
    switch (type) {
      case ExponentialCorrelationFlatVolatility:
          return "Exp. Corr. Flat Vol.";
      case ExponentialCorrelationAbcdVolatility:
          return "Exp. Corr. Abcd Vol.";
      //case CalibratedMM:
      //    return "CalibratedMarketModel";
      default:
        QL_FAIL("unknown MarketModelEvolver type");
    }
}


boost::shared_ptr<MarketModel> makeMarketModel(
                                        const EvolutionDescription& evolution,
                                        Size numberOfFactors,
                                        MarketModelType marketModelType,
                                        Spread forwardBump = 0.0,
                                        Volatility volBump = 0.0) {

    std::vector<Time> fixingTimes(evolution.rateTimes());
    fixingTimes.pop_back();
    boost::shared_ptr<LmVolatilityModel> volModel(new
        LmExtLinearExponentialVolModel(fixingTimes, 0.5, 0.6, 0.1, 0.1));
    boost::shared_ptr<LmCorrelationModel> corrModel(new
        LmLinearExponentialCorrelationModel(evolution.numberOfRates(),
                                            longTermCorrelation, beta));
    std::vector<Rate> bumpedRates(todaysForwards.size());
    LMMCurveState curveState_lmm(rateTimes);
    curveState_lmm.setOnForwardRates(todaysForwards);
    std::vector<Rate> usedRates = curveState_lmm.coterminalSwapRates();
    std::transform(usedRates.begin(), usedRates.end(),
                   bumpedRates.begin(),
                   std::bind1st(std::plus<Rate>(), forwardBump));
    std::vector<Volatility> bumpedVols(volatilities.size());
    std::transform(volatilities.begin(), volatilities.end(),
                   bumpedVols.begin(),
                   std::bind1st(std::plus<Rate>(), volBump));

    switch (marketModelType) {
        case ExponentialCorrelationFlatVolatility:
            return boost::shared_ptr<MarketModel>(new
                ExpCorrFlatVol(longTermCorrelation, beta,
                               bumpedVols,
                               evolution,
                               numberOfFactors,
                               bumpedRates,
                               std::vector<Spread>(bumpedRates.size(), displacement)));
        case ExponentialCorrelationAbcdVolatility:
            return boost::shared_ptr<MarketModel>(new
                ExpCorrAbcdVol(0.0,0.0,1.0,1.0,
                               bumpedVols,
                               longTermCorrelation, beta,
                               evolution,
                               numberOfFactors,
                               bumpedRates,
                               std::vector<Spread>(bumpedRates.size(), displacement)));
        //case CalibratedMM:
        //    return boost::shared_ptr<MarketModel>(new
        //        CalibratedMarketModel(volModel, corrModel,
        //                              evolution,
        //                              numberOfFactors,
        //                              bumpedForwards,
        //                              displacement));
        default:
            QL_FAIL("unknown MarketModel type");
    }
}

enum MeasureType { ProductSuggested, Terminal, MoneyMarket, MoneyMarketPlus };

std::string measureTypeToString(MeasureType type) {
    switch (type) {
      case ProductSuggested:
          return "ProductSuggested measure";
      case Terminal:
          return "Terminal measure";
      case MoneyMarket:
          return "Money Market measure";
      case MoneyMarketPlus:
          return "Money Market Plus measure";
      default:
        QL_FAIL("unknown measure type");
    }
}

std::vector<Size> makeMeasure(const MarketModelMultiProduct& product,
                              MeasureType measureType)
{
    std::vector<Size> result;
    EvolutionDescription evolution(product.evolution());
    switch (measureType) {
      case ProductSuggested:
        result = product.suggestedNumeraires();
        break;
      case Terminal:
        result = terminalMeasure(evolution);
        if (!isInTerminalMeasure(evolution, result)) {
            Array a(result.size());
            std::copy(result.begin(), result.end(), a.begin());
            BOOST_ERROR("\nfailure in verifying Terminal measure:\n" << a);
        }
        break;
      case MoneyMarket:
        result = moneyMarketMeasure(evolution);
        if (!isInMoneyMarketMeasure(evolution, result)) {
            Array a(result.size());
            std::copy(result.begin(), result.end(), a.begin());
            BOOST_ERROR("\nfailure in verifying MoneyMarket measure:\n" << a);
        }
        break;
      case MoneyMarketPlus:
        result = moneyMarketPlusMeasure(evolution, measureOffset_);
        if (!isInMoneyMarketPlusMeasure(evolution, result, measureOffset_)) {
            Array a(result.size());
            std::copy(result.begin(), result.end(), a.begin());
            BOOST_ERROR("\nfailure in verifying MoneyMarketPlus(" <<
                        measureOffset_ << ") measure:\n" << a);
        }
        break;
      default:
        QL_FAIL("unknown measure type");
    }
    checkCompatibility(evolution, result);
    if (printReport_) {
        Array num(result.size());
        std::copy(result.begin(), result.end(), num.begin());
        BOOST_MESSAGE("    " << measureTypeToString(measureType) << ": " << num);
    }
    return result;
}

enum EvolverType { /*Ipc,*/ Pc /*, NormalPc*/};

std::string evolverTypeToString(EvolverType type) {
    switch (type) {
      //case Ipc:
      //    return "iterative predictor corrector";
      case Pc:
          return "predictor corrector";
      //case NormalPc:
      //    return "predictor corrector for normal case";
      default:
        QL_FAIL("unknown MarketModelEvolver type");
    }
}
boost::shared_ptr<MarketModelEvolver> makeMarketModelEvolver(
    const boost::shared_ptr<MarketModel>& marketModel,
    const std::vector<Size>& numeraires,
    const BrownianGeneratorFactory& generatorFactory,
    EvolverType evolverType,
    Size initialStep = 0)
{
    switch (evolverType) {
        case Pc:
            return boost::shared_ptr<MarketModelEvolver>(new
                CoterminalSwapRatePcEvolver(marketModel, generatorFactory,
                                            numeraires, initialStep));
        default:
            QL_FAIL("unknown CoterminalSwapMarketModelEvolver type");
    }
}

void checkCoterminalSwapsAndSwaptions(const SequenceStatistics& stats,
                                      const Rate fixedRate,
                                      const std::vector<boost::shared_ptr<StrikedTypePayoff> >& payoffs,
                                      const boost::shared_ptr<MarketModel> marketModel,
                                      const std::string& config) {
    
    std::vector<Real> results = stats.mean();
    std::vector<Real> errors = stats.errorEstimate();
    std::vector<Real> discrepancies(todaysForwards.size());
    
    Size N = todaysForwards.size();

    // check Swaps
    Real maxError = QL_MIN_REAL;
    LMMCurveState curveState_lmm(rateTimes);                                // set up curve state in LMM
    curveState_lmm.setOnForwardRates(todaysForwards);                       // set up fwd rates in LMM
    std::vector<Rate> atmSwapRates = curveState_lmm.coterminalSwapRates();  // calculate ct swap rates in LMM
    CoterminalSwapCurveState curveState(rateTimes);                         // set up curve state in SMM
    curveState.setOnCoterminalSwapRates(atmSwapRates);                      // set up ct swap rates in SMM coherently with fwd rates in LMM
    // alternatively
    //CoterminalSwapCurveState curveState(rateTimes);                       // set up curve state in SMM
    //curveState.setOnCoterminalSwapRates(todaysCoterminalSwapRates);       // set up ct swap rates in SMM

    std::vector<Real> expectedNPVs(atmSwapRates.size());
    Real errorThreshold = 0.5;
    for (Size i=0; i<N; ++i) {
        Real expectedNPV = curveState.coterminalSwapAnnuity(i, i) 
            * (atmSwapRates[i]-fixedRate) * todaysDiscounts[i];
        expectedNPVs[i] = expectedNPV;
        discrepancies[i] = (results[i]-expectedNPVs[i])/errors[i];
        maxError = std::max(std::fabs(discrepancies[i]), maxError);
    }
    if (maxError > errorThreshold) {
        BOOST_MESSAGE(config);
        for (Size i=0; i<N; ++i) {
            BOOST_MESSAGE(io::ordinal(i+1) << " coterminal swap NPV: "
                          << io::rate(results[i])
                          << " +- " << io::rate(errors[i])
                          << "; expected: " << io::rate(expectedNPVs[i])
                          << "; discrepancy/error = "
                          << discrepancies[N-1-i]
                          << " standard errors");
        }
        BOOST_ERROR("test failed");
    }
    
    // check Swaptions
    maxError = 0;
    //const Spread displacement = 0;
    Matrix jacobian =
        SwapForwardMappings::coterminalSwapZedMatrix(curveState, displacement);    
    std::vector<Rate> expectedSwaptions(N);
    std::vector<Real> stdDevSwaptions(N);
        for (Size i=0; i<N; ++i) {
        const Matrix& forwardsCovariance = marketModel->totalCovariance(i);
        Matrix cotSwapsCovariance= jacobian * forwardsCovariance * transpose(jacobian);
        //Time expiry = rateTimes[i];
        const std::vector<Time>&  taus = curveState.rateTaus();
        Real expectedSwaption = BlackCalculator(
                        payoffs[i],
                        atmSwapRates[i]+displacement,
                        //std::sqrt(cotSwapsCovariance[i][i]),
                        volatilities[i]*std::sqrt(rateTimes[i]),
                        curveState.coterminalSwapAnnuity(i,i) * todaysDiscounts[i]).value();
        expectedSwaptions[i] = expectedSwaption;
        discrepancies[i] = (results[N+i]-expectedSwaptions[i])/errors[N+i];
        maxError = std::max(std::fabs(discrepancies[i]), maxError);
    }
    errorThreshold = 2.0;

   if (maxError > errorThreshold) {
        BOOST_MESSAGE(config);
        for (Size i=1; i<=N; ++i) {
            BOOST_MESSAGE(
                    io::ordinal(i) << " Swaption: "
                    << io::rate(results[2*N-i])
                    << " +- " << io::rate(errors[2*N-i])
                    << "; expected: " << io::rate(expectedSwaptions[N-i])
                    << "; discrepancy/error = "
                    << io::percent(discrepancies[N-i])
                    << " standard errors");
        }
        BOOST_ERROR("test failed");
    }
}

QL_END_TEST_LOCALS(MarketModelTest_smm)

void MarketModelSmmTest::testMultiStepCoterminalSwapsAndSwaptions() {

    BOOST_MESSAGE("Repricing multi-step coterminal swaps "
                  " and swaptions in a SWAP market model...");
    QL_TEST_BEGIN
    QL_TEST_SETUP
    Real fixedRate = 0.04;

    // swaps
    std::vector<Time> swapPaymentTimes(rateTimes.begin()+1, rateTimes.end());
    MultiStepCoterminalSwaps swaps(rateTimes, accruals, accruals,
                                   swapPaymentTimes,
                                   fixedRate);
    // swaptions (generic strike)
    std::vector<Time> swaptionPaymentTimes(rateTimes.begin(), rateTimes.end()-1);
    std::vector<boost::shared_ptr<StrikedTypePayoff> > payoffs(todaysForwards.size());
    for (Size i = 0; i < payoffs.size(); ++i)
        payoffs[i] = boost::shared_ptr<StrikedTypePayoff>(new 
            PlainVanillaPayoff(Option::Call, fixedRate+displacement));
    MultiStepCoterminalSwaptions swaptions(rateTimes,
                                           swaptionPaymentTimes, payoffs);
    MultiProductComposite product;
    product.add(swaps);
    product.add(swaptions);
    product.finalize();
    EvolutionDescription evolution = product.evolution();
    MarketModelType marketModels[] = {// CalibratedMM,
                                       ExponentialCorrelationFlatVolatility,
                                       ExponentialCorrelationAbcdVolatility };
    for (Size j=0; j<LENGTH(marketModels); j++) {
        Size testedFactors[] = { /*4, 8,*/ todaysForwards.size()};
        for (Size m=0; m<LENGTH(testedFactors); ++m) {        
            Size factors = testedFactors[m];
            // Composite's ProductSuggested is the Terminal one
            MeasureType measures[] = { // ProductSuggested,
                                       Terminal,
                                       //MoneyMarketPlus,
                                       MoneyMarket};
            for (Size k=0; k<LENGTH(measures); k++) {
                std::vector<Size> numeraires = makeMeasure(product, measures[k]);
                boost::shared_ptr<MarketModel> marketModel =
                    makeMarketModel(evolution, factors, marketModels[j]);
                EvolverType evolvers[] = { Pc /*, Ipc */};
                boost::shared_ptr<MarketModelEvolver> evolver;
                Size stop = isInTerminalMeasure(evolution, numeraires) ? 0 : 1;                
                for (Size i=0; i<LENGTH(evolvers)-stop; i++) {
                    for (Size n=0; n<1; n++) {
                        //MTBrownianGeneratorFactory generatorFactory(seed_);
                        SobolBrownianGeneratorFactory generatorFactory(
                            SobolBrownianGenerator::Diagonal);
                        evolver = makeMarketModelEvolver(marketModel,
                                                         numeraires,
                                                         generatorFactory,
                                                         evolvers[i]);
                        std::ostringstream config;
                        config <<
                            marketModelTypeToString(marketModels[j]) << ", " << 
                            factors << (factors>1 ? (factors==todaysForwards.size() ? " (full) factors, " : " factors, ") : " factor,") <<
                            measureTypeToString(measures[k]) << ", " <<
                            evolverTypeToString(evolvers[i]) << ", " <<
                            "MT BGF";
                        BOOST_MESSAGE("    " << config.str());
                        std::cout << config.str() << "\n";
                        boost::shared_ptr<SequenceStatistics> stats = simulate(evolver, product);
                        checkCoterminalSwapsAndSwaptions(*stats, fixedRate,
                                                         payoffs, marketModel,config.str());
                    }
                }
            }
        }
    }
    QL_TEST_END
}



// --- Call the desired tests
test_suite* MarketModelSmmTest::suite() {
    test_suite* suite = BOOST_TEST_SUITE("SMM Market-model tests");

    suite->add(BOOST_TEST_CASE(&MarketModelSmmTest::testMultiStepCoterminalSwapsAndSwaptions));
    return suite;
}
