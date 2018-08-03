// Copyright 2007 Google Inc. All Rights Reserved.
//
// A collection of methods to convert back and forth between a number
// and a human-readable string representing the number.

#ifndef STRINGS_HUMAN_READABLE_H__
#define STRINGS_HUMAN_READABLE_H__

#include <functional>
using std::binary_function;
#include <string>
using std::string;

#include "base/integral_types.h"
#include "base/macros.h"

//                                 WARNING
// HumanReadable{NumBytes, Int} don't give you the standard set of SI prefixes.
//
// HumanReadableNumBytes uses binary powers -- 1M = 1 << 20 -- but for numbers
// less than 1024, it adds the suffix "B" for "bytes." It is OK when you need
// to print a literal number of bytes, but can be awfully confusing for
// anything else.
//
// HumanReadableInt uses decimal powers -- 1M = 10^3 -- but prints
// 'B'-for-billion instead of 'G'-for-giga. It's good for representing
// true numbers, like how many documents are in a repository.
// HumanReadableNum is the same as HumanReadableInt but has additional
// support for DoubleToString(), where smaller numbers will print more
// (up to 3) decimal places.
//
// If you want SI prefixes, use the functions in si_prefix.h instead; for
// example, strings::si_prefix::ToDecimalString(1053.2) == "1.05k".

class HumanReadableNumBytes {
 public:
  // Converts between an int64 representing a number of bytes and a
  // human readable string representing the same number.
  // e.g. 1000000 -> "976.6K".
  //  Note that calling these two functions in succession isn't a
  //  noop, since ToString() may round.
  static bool ToInt64(const string &str, int64 *num_bytes);
  static string ToString(int64 num_bytes);
  // Like ToString but without rounding.  For example 1025 would return
  // "1025B" rather than "1.0K".  Uses the largest common denominator.
  static string ToStringWithoutRounding(int64 num_bytes);

  static bool ToDouble(const string &str, double *num_bytes);
  // Function overloading this with a function that takes an int64 is just
  // asking for trouble.
  static string DoubleToString(double num_bytes);

  // TODO(user): Maybe change this class to use SIPrefix?

  // ----------------------------------------------------------------------
  // LessThan
  // humanreadablebytes_less
  // humanreadablebytes_greater
  //    These numerically compare the values encoded in strings by
  //    ToString().  Strings which cannot be parsed are treated as
  //    if they represented the value 0.  The following byte sizes
  //    would be sorted as:
  //        3B
  //        .06K
  //        .03M
  //        10000G
  //        10T
  //        3.01P
  //        3.02P
  //        0.007E
  // ----------------------------------------------------------------------
  static bool LessThan(const string &a, const string &b);

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(HumanReadableNumBytes);
};


// See documentation at HumanReadableNumBytes::LessThan().
struct humanreadablebytes_less
    : public std::binary_function<const string&, const string&, bool> {
  bool operator()(const string& a, const string &b) const {
    return HumanReadableNumBytes::LessThan(a, b);
  }
};

// See documentation at HumanReadableNumBytes::LessThan().
struct humanreadablebytes_greater
    : public std::binary_function<const string&, const string&, bool> {
  bool operator()(const string& a, const string &b) const {
    return HumanReadableNumBytes::LessThan(b, a);
  }
};

class HumanReadableInt {
 public:
  // Similar to HumanReadableNumBytes::ToInt64(), but uses decimal
  // rather than binary expansions - so M = 1 million, B = 1 billion,
  // etc. Numbers beyond 1T are expressed as "3E14" etc.
  static string ToString(int64 value);

  // Reverses ToString(). Note that calling these two functions in
  // succession isn't a noop, since ToString() may round.
  static bool ToInt64(const string &str, int64 *value);

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(HumanReadableInt);
};

class HumanReadableNum {
 public:
  // Same as HumanReadableInt::ToString().
  static string ToString(int64 value);

  // Similar to HumanReadableInt::ToString(), but prints 2 decimal
  // places for numbers with absolute value < 10.0 and 1 decimal place
  // for numbers >= 10.0 and < 100.0.
  static string DoubleToString(double value);

  // Reverses DoubleToString(). Note that calling these two functions in
  // succession isn't a noop, since there may be rounding errors.
  static bool ToDouble(const string &str, double *value);

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(HumanReadableNum);
};

class HumanReadableElapsedTime {
 public:
  // Converts a time interval as double to a human readable
  // string. For example:
  //   0.001       -> "1 ms"
  //   10.0        -> "10 s"
  //   933120.0    -> "10.8 days"
  //   39420000.0  -> "1.25 years"
  //   -10         -> "-10 s"
  static string ToShortString(double seconds);


 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(HumanReadableElapsedTime);
};

#endif  // STRINGS_HUMAN_READABLE_H__
