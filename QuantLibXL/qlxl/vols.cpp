/*
 Copyright (C) 2002, 2003 Ferdinando Ametrano

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
/*! \file vols.cpp
    \brief QuantLib Excel volatility functions

    \fullpath
    qlxl/%vols.cpp
*/

// $Id$

#include <qlxl/qlxlfoper.hpp>

extern "C"
{


    using namespace QuantLib;


    LPXLOPER EXCEL_EXPORT xlBlackVol(XlfOper xlrefDate,
                                     XlfOper xlblackVolSurface,
                                     XlfOper xldate1,
                                     XlfOper xldate2,
                                     XlfOper xlstrike,
                                     XlfOper xlallowExtrapolation) {
        EXCEL_BEGIN;
        Date refDate = QlXlfOper(xlrefDate).AsDate();
        QuantLib::RelinkableHandle<QuantLib::BlackVolTermStructure>
            volSurface =
            QlXlfOper(xlblackVolSurface).AsBlackVolTermStructure(refDate);
        Date date1 = QlXlfOper(xldate1).AsDate();
        Date date2 = QlXlfOper(xldate2).AsDate();
        double strike = xlstrike.AsDouble();

        double result = volSurface->blackForwardVol(date1, date2, strike,
            xlallowExtrapolation.AsBool());
        return XlfOper(result);
        EXCEL_END;
    }


}
