////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012-2014 Jun Wu <quark@zju.edu.cn>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include "strconv.h"
#include <cstdio>

namespace conv = lrun::strconv;
using std::string;


double conv::to_double(const string& str) {
    double v = 0;
    sscanf(str.c_str(), "%lg", &v);
    return v;
}

long conv::to_long(const string& str) {
    long v = 0;
    sscanf(str.c_str(), "%ld", &v);
    return v;
}

unsigned long conv::to_ulong(const string& str) {
    unsigned long v = 0;
    sscanf(str.c_str(), "%lu", &v);
    return v;
}

long long conv::to_longlong(const string& str) {
    long long v = 0;
    sscanf(str.c_str(), "%lld", &v);
    return v;
}

bool conv::to_bool(const string& str) {
    if (str.empty()) return false;
    switch (str.c_str()[0]) {
        case '1': case 't': case 'T': case 'e': case 'E':
            return true;
        default:
            return false;
    }
}

long long conv::to_bytes(const string& str) {
    long long result = 1;
    // accept str which ends with 'k', 'kb', 'm', 'M', etc.
    int pos = str.length() - 1;
    if (pos > 0 && (str[pos] == 'b' || str[pos] == 'B')) --pos;
    if (pos > 0) {
        switch (str[pos]) {
            case 'g': case 'G':
                result *= 1024;
            case 'm': case 'M':
                result *= 1024;
            case 'k': case 'K':
                result *= 1024;
        }
    }
    if (result == 1) {
        // read as long long
        result = conv::to_longlong(str);
    } else {
        // read as double so that the user can use things like 0.5mb
        result *= conv::to_double(str);
    }
    return result;
}

string conv::from_double(double value, int precision) {
    char buf[1024];
    char format[16];
    snprintf(format, sizeof format, "%%.%df", precision);
    snprintf(buf, sizeof buf, format, value);
    return buf;
}

string conv::from_long(long value) {
    char buf[sizeof(long) * 3 + 1];
    snprintf(buf, sizeof buf, "%ld", value);
    return buf;
}

string conv::from_ulong(unsigned long value) {
    char buf[sizeof(unsigned long) * 3 + 1];
    snprintf(buf, sizeof buf, "%lu", value);
    return buf;
}

string conv::from_longlong(long long value) {
    char buf[sizeof(long long) * 3 + 1];
    snprintf(buf, sizeof buf, "%lld", value);
    return buf;
}
