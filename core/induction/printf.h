#include <stdarg.h>

void i_debug (char *format, ...);
void i_printf (int level, char *format, ...);
void i_vaprintf (int level, char *format, va_list ap);						/* Level 0 = Always Logged, 1 = Higher level debug, 2 = Crazy */
void i_printf_set_wrapper (void (*func) ());

