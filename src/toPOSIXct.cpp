// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*-
//
// toPOSIXct: Use Boost Date_Time to convert date(time) data to POSIXt
//
// Copyright (C) 2015  Dirk Eddelbuettel
//
// This file is part of RcppBDT.
//
// RcppBDT is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.  
//                           
// RcppBDT is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//                 
// You should have received a copy of the GNU General Public License
// along with RcppBDT.  If not, see <http://www.gnu.org/licenses/>.

#include <boost/date_time.hpp>
#include <boost/lexical_cast.hpp>
#include <Rcpp.h>

namespace bt = boost::posix_time;

const std::locale formats[] = {
    std::locale(std::locale::classic(), new bt::time_input_facet("%Y-%m-%d %H:%M:%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%Y/%m/%d %H:%M:%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%Y%m%d %H%M%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%m/%d/%Y %H:%M:%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%m-%d-%Y %H:%M:%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%d.%m.%Y %H:%M:%S%f")),

    std::locale(std::locale::classic(), new bt::time_input_facet("%Y-%b-%d %H:%M:%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%Y/%b/%d %H:%M:%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%Y%b%d %H%M%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%b/%d/%Y %H:%M:%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%b-%d-%Y %H:%M:%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%d.%b.%Y %H:%M:%S%f")),

    std::locale(std::locale::classic(), new bt::time_input_facet("%Y-%B-%d %H:%M:%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%Y/%B/%d %H:%M:%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%Y%B%d %H%M%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%B/%d/%Y %H:%M:%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%B-%d-%Y %H:%M:%S%f")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%d.%B.%Y %H:%M:%S%f")),

    std::locale(std::locale::classic(), new bt::time_input_facet("%Y-%m-%d")),
    // will not work std::locale(std::locale::classic(), new bt::time_input_facet("%Y%m%d")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%m/%d/%Y")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%m-%d-%Y")),

    std::locale(std::locale::classic(), new bt::time_input_facet("%Y-%b-%d")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%Y%b%d")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%b/%d/%Y")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%b-%d-%Y")),

    std::locale(std::locale::classic(), new bt::time_input_facet("%Y-%B-%d")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%Y%B%d")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%B/%d/%Y")),
    std::locale(std::locale::classic(), new bt::time_input_facet("%B-%d-%Y"))

};
const size_t nformats = sizeof(formats)/sizeof(formats[0]);

double stringToTime(const std::string s) {

    bt::ptime pt, ptbase;
    const bt::ptime timet_start(boost::gregorian::date(1970,1,1));

    // loop over formats and try them til one fits
    for (size_t i=0; pt == ptbase && i < nformats; ++i) {
        std::istringstream is(s);
        is.imbue(formats[i]);
        is >> pt;
    }
    
    if (pt == ptbase) {
        return NAN;
    } else { 
        bt::time_duration diff = pt - timet_start;

        // Define BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG to use nanoseconds
        // (and then use diff.total_nanoseconds()/1.0e9;  instead)
        return diff.total_microseconds()/1.0e6;
    }
}

template <int RTYPE>
Rcpp::DatetimeVector toPOSIXct_impl(const Rcpp::Vector<RTYPE>& sv) {

    int n = sv.size();
    Rcpp::DatetimeVector pv(n);
    
    for (int i=0; i<n; i++) {
        std::string s = boost::lexical_cast<std::string>(sv[i]);
        //Rcpp::Rcout << sv[i] << " -- " << s << std::endl;

        // Boost Date_Time gets the 'YYYYMMDD' format wrong, even
        // when given as an explicit argument. So we need to test here.
        // While we're at it, may as well test for obviously wrong data.
        int l = s.size();
        if ((l < 8) ||          // impossibly short
            (l == 9)) {         // 8 or 10 works, 9 cannot
            Rcpp::stop("Inadmissable input: %s", s);
        } else if (l == 8) {    // turn YYYYMMDD into YYYY/MM/DD
            s = s.substr(0, 4) + "/" + s.substr(4, 2) + "/" + s.substr(6,2);
        }
        pv[i] = stringToTime(s);
    }
    return pv;
}

//' This function uses the Boost Date_Time library to parse 
//' datetimes (and dates) from strings. It returns a 
//' \code{DatetimeVector},  i.e. a vector of \sQuote{POSIXct} 
//' objects. These represent dates and time as (possibly fractional) 
//' seconds since the \sQuote{epoch} of January 1, 1970. The default
//' timezone of \sQuote{UTC} is set.
//'
//' A numer of fixed formats are tried in succession. These include
//' the standard ISO format \sQuote{YYYY-MM-DD HH:MM:SS} as well as
//' different local variants including several forms popular in the 
//' United States.  Two-digits years and clearly ambigous formats such
//' as \sQuote{03/04/05} are ignored.  In the case of parsing failure
//' a \code{NA} value is returned.
//'
//' Fractional seconds are supported as well.  As R itself only supports 
//' microseconds, the Boost compile-time option for nano-second resolution 
//' has not been enabled.
//'
//' @title Parse POSIXct objects from character variables
//' @param x A vector of type character, integer or numeric with 
//' date(time) expressions to be parsed and converted. 
//' @return A vector of \code{Datetime} objects with \sQuote{POSIXct} elements.
//' @author Dirk Eddelbuettel
//' @examples
//' ## See the source code (hah!) for a full list of formats
//' times <- c("2004-03-21 12:45:33.123456",
//'           "2004/03/21 12:45:33.123456",
//'           "20040321 124533.123456",
//'           "21.03.2004 12:45:33.123456", 
//'           "03/21/2004 12:45:33.123456",
//'           "03-21-2004 12:45:33.123456", 
//'           "2004-03-21",
//'           "20040321",
//'           "03/21/2004",
//'           "03-21-2004",
//'           "20010101")   
//' toPOSIXct(times)
//' format(toPOSIXct(times), tz="UTC")
// [[Rcpp::export]]
Rcpp::DatetimeVector toPOSIXct(SEXP x) {
    if (Rcpp::is<Rcpp::CharacterVector>(x)) {
        return toPOSIXct_impl<STRSXP>(x);
    } else if (Rcpp::is<Rcpp::IntegerVector>(x)) {
        return toPOSIXct_impl<INTSXP>(x); 
    } else if (Rcpp::is<Rcpp::NumericVector>(x)) {
        // here we have two cases: either we are an int like
        // 200150315 'mistakenly' cast to numeric, or we actually
        // are a proper large numeric (ie as.numeric(Sys.time())
        Rcpp::NumericVector v(x);
        if (v[0] < 21990101) {  // somewhat arbitrary cuttoff
            // actual integer date notation: convert to string and parse
            return toPOSIXct_impl<REALSXP>(x);
        } else {
            // we think it is a numeric time, so treat it as one
            return Rcpp::DatetimeVector(x);
        }
    } else {
        Rcpp::stop("Unsupported Type");
        return R_NilValue;//not reached
    }
}


// [[Rcpp::export]]
Rcpp::DatetimeVector charToPOSIXct(Rcpp::CharacterVector sv) {
    int n = sv.size();
    Rcpp::DatetimeVector pv(n);

    const bt::ptime timet_start(boost::gregorian::date(1970,1,1));
    bt::ptime pt;
    std::locale fmt = std::locale(std::locale::classic(), 
                                  new bt::time_input_facet("%Y-%m-%d %H:%M:%S%f"));

    for (int i=0; i<n; i++) {
        std::istringstream is(std::string(sv[i]));
        is.imbue(fmt);
        is >> pt;
        bt::time_duration diff = pt - timet_start;

        pv[i] = diff.total_microseconds()/1.0e6;
    }
    return pv;
}

// The next function uses the non-stream-based parsing in Boost Date_Time
// and requires _linking_ with -lboost_date_time which makes the (otherwise
// header-only) build more complicate
// // [ [ Rcpp::export ] ]
// Rcpp::DatetimeVector charToPOSIXctNS(Rcpp::CharacterVector sv) {
//     int n = sv.size();
//     Rcpp::DatetimeVector pv(n);

//     const bt::ptime timet_start(boost::gregorian::date(1970,1,1));
//     for (int i=0; i<n; i++) {
//         bt::ptime pt = bt::time_from_string(std::string(sv[i]));
//         bt::time_duration diff = pt - timet_start;
//         pv[i] = diff.total_microseconds()/1.0e6;
//     }
//     return pv;
// }


// The next version if for comparison only and uses the C library strptime
// Not that this does NOT work for sub-second entries
// TODO: make the R-internal strptime accessible
// [[Rcpp::export]]
Rcpp::DatetimeVector cToPOSIXct(Rcpp::CharacterVector sv) {
    int n = sv.size();
    Rcpp::DatetimeVector pv(n);

    for (int i=0; i<n; i++) {
        const char *s = sv[i];
        struct tm tm;
        strptime(s, "%Y-%d-%m %H:%M:%S", &tm);
        time_t t = mktime(&tm);
        pv[i] = t;
    }
    return pv;
}

