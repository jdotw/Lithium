/* value.c */

typedef struct i_value_s
{
  /* Value Variables */
  long integer;                         /* Integer value (signed 32bit) */
  unsigned long count;                  /* Counter value (wrapping unsigned 32bit) */
  unsigned long gauge;                  /* Gauge value (non-wrapping unsigned 32bit) */
  unsigned long count64;                /* Counter value (wrapping unsigned 32bit) */
  unsigned long gauge64;                /* Gauge value (non-wrapping unsigned 32bit) */
  double flt;                           /* Floating point value */
  char *str;                            /* String value */
  void *oid;                            /* OID value */
  int oid_len;                          /* Length of OID */
  struct i_ip_s *ip;                    /* IP Address */
  struct timeval tv;                    /* Time value */
  char *data;                           /* Opaque data */
  int datasize;                         /* Opaque datasize */

  /* Value Info */
  struct timeval tstamp;                /* Timestamp of value */
  unsigned short origin;                /* Origin of value */
} i_value;    /* Must be compatible with i_metric_value */

/* Value types KEEP IN SYNC WITH metric.h */
#define VALTYPE_INTEGER 1                   /* Signed 32bit integer value */
#define VALTYPE_COUNT 2                     /* Unsigned 32bit integer wrapping value */
#define VALTYPE_GAUGE 3                     /* Unsigned 32bit integer non-wrapping value */
#define VALTYPE_FLOAT 4                     /* Floating point value */
#define VALTYPE_STRING 5                    /* NULL terminated string value */
#define VALTYPE_OID 6                       /* OID value */
#define VALTYPE_IP 7                        /* IP Address */
#define VALTYPE_INTERVAL 8                  /* A time interval in seconds */
#define VALTYPE_HEXSTRING 9                 /* A time interval in seconds */
#define VALTYPE_COUNT_HEX64 10              /* Unsigned 64bit integer wrapping value derived from hex string */
#define VALTYPE_GAUGE_HEX64 11              /* Unsigned 64bit integer non-wrapping value derived from hex string */
#define VALTYPE_COUNT64 12                  /* Unsigned 64bit integer non-wrapping value */
#define VALTYPE_DATA 100                    /* Opaque data value */

/* Value Origins */
#define VALORIG_INSTANT 0                   /* Instantaneous Value */
#define VALORIG_RECORD 1                    /* Recorded Value */

i_value* i_value_create ();
void i_value_free (void *valptr);
i_value* i_value_duplicate (i_value *val);

/* value_enumstr.c */

typedef struct
{
  long val_int;
  char *str;
} i_value_enumstr;

i_value_enumstr* i_value_enumstr_create ();
void i_value_enumstr_free (void *estrptr);
int i_value_enumstr_add (i_list *enumstr_list, long val_int, char *str);
char* i_value_enumstr_eval (i_list *enumstr_list, int val_int);

/* value_str.c */

char* i_value_valstr (unsigned short val_type, i_value *val, char *unit_str, i_list *enumstr_list);
char* i_value_valstr_raw (unsigned short val_type, i_value *val);
i_value* i_value_valstr_set (unsigned short val_type, i_value *orig_val, char *valstr);

/* value_flt.c */

double i_value_valflt (unsigned short val_type, i_value *val);
i_value* i_value_valflt_set (unsigned short val_type, i_value *orig_val, double valflt);

/* value_type.c */

char* i_value_typestr (unsigned short val_type);
unsigned short i_value_isnum (unsigned short val_type);
