# This file was generated by Rcpp::compileAttributes
# Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#' This function uses the Boost Date_Time library to parse 
#' datetimes (and dates) from strings. It returns a 
#' \code{DatetimeVector},  i.e. a vector of \sQuote{POSIXct} 
#' objects. These represent dates and time as (possibly fractional) 
#' seconds since the \sQuote{epoch} of January 1, 1970. The default
#' timezone of \sQuote{UTC} is set.
#'
#' A numer of fixed formats are tried in succession. These include
#' the standard ISO format \sQuote{YYYY-MM-DD HH:MM:SS} as well as
#' different local variants including several forms popular in the 
#' United States.  Two-digits years and clearly ambigous formats such
#' as \sQuote{03/04/05} are ignored.  In the case of parsing failure
#' a \code{NA} value is returned.
#'
#' Fractional seconds are supported as well.  As R itself only supports 
#' microseconds, the Boost compile-time option for nano-second resolution 
#' has not been enabled.
#'
#' @title Parse POSIXct objects from character variables
#' @param x A vector of type character, integer or numeric with 
#' date(time) expressions to be parsed and converted. 
#' @return A vector of \code{Datetime} objects with \sQuote{POSIXct} elements.
#' @author Dirk Eddelbuettel
#' @examples
#' ## See the source code (hah!) for a full list of formats
#' times <- c("2004-03-21 12:45:33.123456",
#'           "2004/03/21 12:45:33.123456",
#'           "20040321 124533.123456",
#'           "21.03.2004 12:45:33.123456", 
#'           "03/21/2004 12:45:33.123456",
#'           "03-21-2004 12:45:33.123456", 
#'           "2004-03-21",
#'           "20040321",
#'           "03/21/2004",
#'           "03-21-2004",
#'           "20010101")   
#' toPOSIXct(times)
#' format(toPOSIXct(times), tz="UTC")
toPOSIXct <- function(x) {
    .Call('RcppBDT_toPOSIXct', PACKAGE = 'RcppBDT', x)
}

charToPOSIXct <- function(sv) {
    .Call('RcppBDT_charToPOSIXct', PACKAGE = 'RcppBDT', sv)
}

cToPOSIXct <- function(sv) {
    .Call('RcppBDT_cToPOSIXct', PACKAGE = 'RcppBDT', sv)
}

