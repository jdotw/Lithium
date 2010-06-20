/* Default RRA defs */

/* 8928 x 5 Minute Samples = 1 Month of samples */
#define RRD_RRA_MONTH "RRA:MIN:0.5:1:8928 RRA:AVERAGE:0.5:1:8928 RRA:MAX:0.5:1:8928"

/* 1464 x 6 Hour Samples    = 1 Year of samples   */
#define RRD_RRA_YEAR "RRA:MIN:0.5:72:1464  RRA:AVERAGE:0.5:72:1464 RRA:MAX:0.5:72:1464"

/* rrd.c */

int i_rrd_check_exists (i_resource *self, char *filename);
struct i_rrdtool_cmd_s *i_rrd_create (i_resource *self, char *filename, time_t start_time, time_t step_seconds, char *arg_str, int (*cbfunc) (), void *passdata);
struct i_rrdtool_cmd_s* i_rrd_update (i_resource *self, int priority, char *filename, char *arg_str, int (*cbfunc) (), void *passdata);
struct i_rrdtool_cmd_s* i_rrd_graph (i_resource *self, char *filename, time_t start_sec, time_t end_sec, char *title, char *y_label, unsigned int kbase, char *arg_str, int (*cbfunc) (), void *passdata);
struct i_rrdtool_cmd_s* i_rrd_pdfgraph (i_resource *self, char *imagepath, time_t start_sec, time_t end_sec, char *y_label, unsigned int kbase, char *args, int (*cbfunc) (), void *passdata);
struct i_rrdtool_cmd_s* i_rrd_xmlgraph (i_resource *self, char *imagepath, time_t start_sec, time_t end_sec, char *y_label, unsigned int kbase, char *args, int (*cbfunc) (), void *passdata);
struct i_rrdtool_cmd_s* i_rrd_xport (i_resource *self, time_t start_sec, time_t end_sec, char *rrdfullpath, int (*cbfunc) (), void *passdata);
struct i_rrdtool_cmd_s* i_rrd_tune (i_resource *self, char *filename, char *arg_str, int (*cbfunc) (), void *passdata);
char* i_rrd_comment_escape (char *in);
