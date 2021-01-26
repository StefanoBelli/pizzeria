#define MAX_LINE 1024

#include <stdio.h>
#include <string.h>

#include "global_config.h"
#include "goodmalloc.h"
#include "myutils.h"
#include "myezcvt.h"

typedef struct {
  char* buf;
  int buf_len;
} line;

static line read_line() {
  line ln;
  good_malloc(ln.buf, char, MAX_LINE + 2);

  fgets(ln.buf, MAX_LINE, stdin);

  int efflen = strlen(ln.buf) - 1;
  ln.buf[efflen] = 0;

  ln.buf_len = efflen;

  return ln;
}

static void any_key() {
  printf("\nPress any key to continue...");
  getchar();
}

mybool show_menu() {
  clear();
  printf("---Menu principale (%s)---\n", cfg.username);

  for (int i = 0; i < cfg.menu_entries_len; ++i) {
    printf("(%d) %s\n", i + 1, cfg.menu_entries[i].entry);
  }

  int entries_len_plus_one = cfg.menu_entries_len + 1;
  printf("(%d) Exit application\n", entries_len_plus_one);

  int choice;

  printf("> ");
  line ln = read_line();
  if (cvt_str_to_int(ln.buf, &choice) == FALSE) {
    good_free(ln.buf);
    return TRUE;
  }

  good_free(ln.buf);

  if (choice < 1 || choice > entries_len_plus_one) {
    return TRUE;
  }

  if (choice == entries_len_plus_one) {
    return FALSE;
  }

  clear();

  int choice_minus_one = choice - 1;
  printf(">>> %s\n", cfg.menu_entries[choice_minus_one].entry);

  mybool res = cfg.menu_entries[choice_minus_one].handler();
  if (res == TRUE) {
    puts(">>> OK");
  } else {
    puts(">>> FAIL");
  }

  any_key();

  return TRUE;
}

#define INVALID(x)                         \
  if (x == FALSE) {                        \
    puts("il dato inserito non è valido"); \
    good_free(curln.buf);                  \
    return FALSE;                          \
  }

mybool show_form(const char* message, const form_field* fields,
                 int fields_len) {
  printf("---%s---\n", message);

  for (int i = 0; i < fields_len; ++i) {
    form_field field = fields[i];

    printf("--> %s: ", field.field_name);
    line curln = read_line();

    if (curln.buf_len > field.expected_max_len ||
        curln.buf_len < field.expected_min_len) {
      printf("field \"%s\" expects %d as max length and %d as min length.\n",
             field.field_name, field.expected_max_len, field.expected_min_len);

      good_free(curln.buf);
      return FALSE;
    }

    if (field.output_type == STRING) {
      memcpy(field.output, curln.buf, field.output_size);
    } else if (field.output_type == INTEGER) {
      INVALID(cvt_str_to_int(curln.buf, (int*)field.output));
    } else if (field.output_type == DOUBLE) {
      INVALID(cvt_str_to_double(curln.buf, (double*)field.output));
    } else if (field.output_type == MYBOOL) {
      INVALID(cvt_str_yesno_to_mybool(curln.buf, (mybool*)field.output));
    }

    good_free(curln.buf);
  }

  mybool is_ok = FALSE;

  printf("Va bene? [y/n]: ");
  line ln = read_line();
  cvt_str_yesno_to_mybool(ln.buf, &is_ok);
  good_free(ln.buf);

  return is_ok;
}

#undef INVALID

mybool date_check(int day, int month, int year) {
	if (year >= 1900 && year <= 9999) {
		if (month >= 1 && month <= 12) {
      		if ((day >= 1 && day <= 31) &&
          		(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 ||
           		month == 10 || month == 12))
        		return TRUE;
      		else if ((day >= 1 && day <= 30) &&
            	(month == 4 || month == 6 || month == 9 || month == 11))
        		return TRUE;
      		else if ((day >= 1 && day <= 28) && (month == 2))
        		return TRUE;
      		else if (day == 29 && month == 2 &&
        	    	(year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)))
        		return TRUE;
      		else
        		return FALSE;
    	} else {
			return FALSE;
		}
	}

  return FALSE;
}