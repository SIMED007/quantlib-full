
/*!
 Copyright (C) 2004, 2005 Eric Ehlers

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

#include <interface.hpp>

using namespace ObjHandler;

Properties WIDGET_MAKE(
        const std::string &handle,
        const std::string &s,
        const int &i) {
    obj_ptr object(new ObjectWidget(s, i));
    ObjectHandler::instance().storeObject(handle, object);
    return object->getProperties();
}

Properties WIDGET_UPDATE(
        const std::string &handle,
        const std::string &s,
        const int &i) {
    boost::shared_ptr<ObjectWidget> object =
        boost::dynamic_pointer_cast<ObjectWidget>
        (ObjectHandler::instance().retrieveObject(handle));
    if (!object)
        throw Exception("WIDGET_UPDATE: unable to retrieve object " + handle);
    object->update(s, i);
    return object->getProperties();
}

void QL_LOGFILE(
        const std::string &logFileName) {
    setLogFile(logFileName);
}

void QL_LOGMESSAGE(
        const std::string &msg) {
    logMessage(msg);
}

string QL_ANY2STRING(
        const any_ptr &a) {
    return AnyToString(a);
}

