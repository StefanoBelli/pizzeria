#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse_dbms_conn_config.h"
#include "jsmn.h"

#define BUFF_SIZE 4096

static char config[BUFF_SIZE];

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
		strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
			return 0;
	}
	
	return -1;
}

static size_t load_file(const char *filename) {
	FILE *f = fopen(filename, "rb");
	if (f == NULL) {
		fprintf(stderr, "Unable to open file %s\n", filename);
		exit(1);
	}
	
	fseek(f, 0, SEEK_END);
	size_t fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  // same as rewind(f);
	
	if (fsize >= BUFF_SIZE) {
		fprintf(stderr, "Configuration file too large\n");
		abort();
	}
	
	fread(config, fsize, 1, f);
	fclose(f);
	
	config[fsize] = 0;
	return fsize;
}

mybool parse_dbms_conn_config(const char *path, dbms_conn_config *conf) {
	int i;
	int r;
	
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */
	
	load_file(path);
	
	jsmn_init(&p);
	r = jsmn_parse(&p, config, strlen(config), t, sizeof(t) / sizeof(t[0]));
	if (r < 0) {
		printf("(parse_dbms_conn_config) failed to parse JSON: %d\n", r);
		return FALSE;
	}
	
	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("(parse_dbms_conn_config) object expected\n");
		return FALSE;
	}
	
	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(config, &t[i], "host") == 0) {
			/* We may use strndup() to fetch string value */
			conf->db_hostname =
				strndup(config + t[i + 1].start, t[i + 1].end - t[i + 1].start);
			i++;
		} else if (jsoneq(config, &t[i], "username") == 0) {
			conf->db_username =
				strndup(config + t[i + 1].start, t[i + 1].end - t[i + 1].start);
			i++;
		} else if (jsoneq(config, &t[i], "password") == 0) {
			conf->db_password =
				strndup(config + t[i + 1].start, t[i + 1].end - t[i + 1].start);
			i++;
		} else if (jsoneq(config, &t[i], "port") == 0) {
			conf->db_port = strtol(config + t[i + 1].start, NULL, 10);
			i++;
		} else if (jsoneq(config, &t[i], "database") == 0) {
			conf->db_name =
				strndup(config + t[i + 1].start, t[i + 1].end - t[i + 1].start);
			i++;
		} else {
			printf("(parse_dbms_conn_config) unexpected key: %.*s\n", t[i].end - t[i].start,
				config + t[i].start);
		}
	}

	return TRUE;
}
