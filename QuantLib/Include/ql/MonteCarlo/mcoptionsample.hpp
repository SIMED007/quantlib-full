
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
/*! \file mcoptionsample.hpp
    \brief Create a sample generator from a path generator and a path pricer

    $Id$
*/

// $Source$
// $Log$
// Revision 1.9  2001/07/20 13:06:57  marmar
// Monte Carlo interfaces imporved
//
// Revision 1.8  2001/07/19 16:40:10  lballabio
// Improved docs a bit
//
// Revision 1.7  2001/07/13 14:29:08  sigmud
// removed a few gcc compile warnings
//
// Revision 1.6  2001/06/22 16:38:15  lballabio
// Improved documentation
//
// Revision 1.5  2001/05/24 15:38:08  nando
// smoothing #include xx.hpp and cutting old Log messages
//

#ifndef quantlib_montecarlo_option_sample_h
#define quantlib_montecarlo_option_sample_h

#include "ql/handle.hpp"

namespace QuantLib {

    namespace MonteCarlo {

        //! %Option value sampler
        /*! Given a path generator class PG, together with an instance 
            "samplePath", and a path pricer PP, again with an instance 
            "pathpricer", a sample generator OptionSample<PG, PP> returns, 
            at each next(), a value for the option price.
    
            Minimal interfaces for PG and PP:
    
            \code
            class PG{
                PATH_TYPE next() const;
                double weight() const;
            };
        
            class PP{
                ValueType value(PATH_TYPE &) const;
            };
            \endcode
        */
        template<class PG, class PP>
        class OptionSample {
          public:
            typedef typename PP::ValueType SampleType;

            OptionSample(){}
            OptionSample(Handle<PG> samplePath, Handle<PP> pathPricer);
            SampleType next() const; 
            double weight() const;
          private:
            mutable Handle<PG> samplePath_;
            Handle<PP> pathPricer_;
            mutable double weight_;
        };

        // inline definitions
        
        template<class PG, class PP>
        inline OptionSample<PG, PP>::OptionSample(Handle<PG> samplePath,
               Handle<PP> pathPricer): samplePath_(samplePath),
               pathPricer_(pathPricer), weight_(0){}

        template<class PG, class PP>
            inline OptionSample<PG, PP>::SampleType OptionSample<PG, PP>::next() const{
            double price = pathPricer_ -> value(samplePath_ -> next());
            weight_ = samplePath_ -> weight();
            return price;
        }

        template<class PG, class PP>
        inline double OptionSample<PG, PP>::weight() const{
            return weight_;
        }

    }

}
#endif
