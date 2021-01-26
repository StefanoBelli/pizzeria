#ifndef MYEZCVT_H
#define MYEZCVT_H

#include "mybool.h"

mybool cvt_str_to_int(const char* in, int* out);
mybool cvt_str_to_double(const char* in, double* out);
mybool cvt_str_yesno_to_mybool(const char* in, mybool* out);

#endif