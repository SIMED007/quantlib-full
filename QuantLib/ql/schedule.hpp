/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006 StatPro Italia srl

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

/*! \file schedule.hpp
    \brief date schedule
*/

#ifndef quantlib_schedule_hpp
#define quantlib_schedule_hpp

#include <ql/calendar.hpp>
#include <ql/Calendars/nullcalendar.hpp>
#include <ql/Utilities/null.hpp>
#include <vector>

namespace QuantLib {

    //! Payment schedule
    class Schedule {
      public:
        #ifndef QL_DISABLE_DEPRECATED
        //! \deprecated use other constructors instead
        Schedule(const Calendar& calendar,
                 const Date& startDate,
                 const Date& endDate,
                 Frequency frequency,
                 BusinessDayConvention convention,
                 const Date& stubDate = Date(),
                 bool startFromEnd = false,
                 bool longFinal = false);
        //! \deprecated use other constructors instead
        Schedule(const Calendar& calendar,
                 const Date& startDate,
                 const Date& endDate,
                 const Period& tenor,
                 BusinessDayConvention convention,
                 const Date& stubDate = Date(),
                 bool startFromEnd = false,
                 bool longFinal = false);
        #endif
        Schedule(const std::vector<Date>&,
                 const Calendar& calendar = NullCalendar(),
                 BusinessDayConvention convention = Unadjusted);
        Schedule(const Date& effectiveDate,
                 const Date& terminationDate,
                 const Period& tenor,
                 const Calendar& calendar,
                 BusinessDayConvention convention,
                 BusinessDayConvention terminationDateConvention,
                 bool backward,
                 bool endOfMonth,
                 const Date& firstDate = Date(),
                 const Date& nextToLastDate = Date());
        //! \name Date access
        //@{
        Size size() const { return dates_.size(); }
        const Date& operator[](Size i) const;
        const Date& at(Size i) const;
        const Date& date(Size i) const;
        const std::vector<Date>& dates() const { return dates_; }
        bool isRegular(Size i) const;
        //@}
        //! \name Other inspectors
        //@{
        const Calendar& calendar() const;
        const Date& startDate() const;
        const Date& endDate() const;
        Frequency frequency() const;
        const Period& tenor() const;
        BusinessDayConvention businessDayConvention() const;
        //@}
        //! \name Iterators
        //@{
        typedef std::vector<Date>::const_iterator const_iterator;
        const_iterator begin() const { return dates_.begin(); }
        const_iterator end() const { return dates_.end(); }
        //@}
      private:
        bool fullInterface_;
        Calendar calendar_;
        Frequency frequency_;
        Period tenor_;
        BusinessDayConvention convention_;
        Date firstDate_, nextToLastDate_;
        bool startFromEnd_;
        bool longFinal_;
        bool endOfMonth_, finalIsRegular_;
        std::vector<Date> dates_;
        std::vector<bool> isRegular_;
    };


    //! helper class
    /*! This class provides a more comfortable interface to the
        argument list of Schedule's constructor.
    */
    class MakeSchedule {
      public:
        MakeSchedule(const Date& effectiveDate,
                     const Date& terminationDate,
                     const Period& tenor,
                     const Calendar& calendar,
                     BusinessDayConvention convention);
        #ifndef QL_DISABLE_DEPRECATED
        MakeSchedule(const Calendar& calendar,
                     const Date& startDate,
                     const Date& endDate,
                     Frequency frequency,
                     BusinessDayConvention convention);
        MakeSchedule& withStubDate(const Date& d);
        MakeSchedule& longFinalPeriod(bool flag=true);
        MakeSchedule& shortFinalPeriod(bool flag=true);
        #endif
        MakeSchedule& terminationDateConvention(BusinessDayConvention conv);
        MakeSchedule& backwards(bool flag=true);
        MakeSchedule& forwards(bool flag=true);
        MakeSchedule& endOfMonth(bool flag=true);
        MakeSchedule& withFirstDate(const Date& d);
        MakeSchedule& withNextToLastDate(const Date& d);
        operator Schedule() const {
            Date firstDate, nextToLastDate;
            if (stubDate_!=Date()) {
                if (backward_) {
                    firstDate = firstDate_;
                    nextToLastDate = stubDate_;
                } else {
                    firstDate = stubDate_;
                    nextToLastDate = nextToLastDate_;
                }
            }
            return Schedule(effectiveDate_, terminationDate_, tenor_,
                calendar_, convention_, terminationDateConvention_,
                backward_, endOfMonth_, firstDate, nextToLastDate);
            }
      private:
        Calendar calendar_;
        Date effectiveDate_, terminationDate_;
        Period tenor_;
        BusinessDayConvention convention_, terminationDateConvention_;
        bool backward_, endOfMonth_;
        Date stubDate_, firstDate_, nextToLastDate_;
    };



    // inline definitions

    inline Schedule::Schedule(const std::vector<Date>& dates,
                              const Calendar& calendar,
                              BusinessDayConvention convention)
    : fullInterface_(false),
      calendar_(calendar), frequency_(Frequency(-1)),
      tenor_(Period()), convention_(convention),
      startFromEnd_(false), longFinal_(false), finalIsRegular_(true),
      dates_(dates) {}

    inline const Date& Schedule::date(Size i) const {
        QL_REQUIRE(i <= dates_.size(),
                   "date index out of bounds");
        return dates_[i];
    }

    inline const Date& Schedule::operator[](Size i) const {
        #if defined(QL_EXTRA_SAFETY_CHECKS)
        QL_REQUIRE(i <= dates_.size(),
                   "date index out of bounds");
        #endif
        return dates_[i];
    }

    inline const Date& Schedule::at(Size i) const {
        QL_REQUIRE(i <= dates_.size(),
                   "date index out of bounds");
        return dates_[i];
    }

    inline const Calendar& Schedule::calendar() const {
        return calendar_;
    }

    inline const Date& Schedule::startDate() const {
        return dates_.front();
    }

    inline const Date& Schedule::endDate() const {
        return dates_.back();
    }

    inline const Period& Schedule::tenor() const {
        QL_REQUIRE(fullInterface_, "full interface not available");
        return tenor_;
    }

    inline Frequency Schedule::frequency() const {
        QL_REQUIRE(fullInterface_, "full interface not available");
        QL_REQUIRE(Integer(frequency_) != -1,
                   "frequency not available");
        return frequency_;
    }

    inline BusinessDayConvention Schedule::businessDayConvention() const {
        return convention_;
    }

}

#endif
