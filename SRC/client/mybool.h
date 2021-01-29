#ifndef MYBOOL_H
#define MYBOOL_H

typedef enum {
	FALSE,
	TRUE
} mybool;

#define mybool_to_str(x) (x == FALSE ? "false" : "true")

#endif