#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "myezcvt.h"

mybool cvt_str_to_int(const char* in, int* out)  {
	char *endptr;
	*out = strtol(in, &endptr, 10);

	return *endptr == 0;
}

mybool cvt_str_to_double(const char* in, double* out) {
	char *endptr;
	*out = strtod(in, &endptr);

	return (in + strlen(in)) == endptr;
}

mybool cvt_str_yesno_to_mybool(const char* in, mybool* out) {
	size_t in_len = strlen(in);

	if(in_len > 3)
		return FALSE;

	char lowerin[3] = { 0 };
	for(size_t i = 0; i < in_len; ++i)
		lowerin[i] = tolower(in[i]);

	if(strcmp("yes", lowerin) == 0 || strcmp("y", lowerin) == 0) {
		*out = TRUE;
		return TRUE;
	} else if(strcmp("no", lowerin) == 0 || strcmp("n", lowerin) == 0) {
		*out = FALSE;
		return TRUE;
	}

	return FALSE;
}