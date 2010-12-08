typedef struct i_interface_s
{
  struct i_object_s *obj;
  unsigned long index;

  struct i_metric_s *sysuptime;
  struct i_metric_s *adminstate;
  struct i_metric_s *opstate;
  struct i_metric_s *alias;
  struct i_metric_s *mtu;
  struct i_metric_s *speed;
  struct i_metric_s *lastchange_uptime;
  struct i_metric_s *lastchange;
  struct i_metric_s *discont_uptime;
  struct i_metric_s *discont;
  struct i_list_s *ip_list;
  struct i_list_s *route_list;

  struct i_metric_s *octets_in;
  struct i_metric_s *octets_out;
  struct i_metric_s *bps_in;
  struct i_metric_s *bps_out;
  struct i_metric_s *utilpc_in;
  struct i_metric_s *utilpc_out;
  struct i_metric_s *mb_day_in;
  struct i_metric_s *mb_day_out;
  struct i_metric_s *mb_month_in;
  struct i_metric_s *mb_month_out;

  struct i_metric_s *ucastpkts_in;
  struct i_metric_s *ucastpkts_out;
  struct i_metric_s *upps_in;
  struct i_metric_s *upps_out;
  struct i_metric_s *nucastpkts_in;
  struct i_metric_s *nucastpkts_out;
  struct i_metric_s *nupps_in;
  struct i_metric_s *nupps_out;
  struct i_metric_s *pkts_in;
  struct i_metric_s *pkts_out;
  struct i_metric_s *pps_in;
  struct i_metric_s *pps_out;

  struct i_metric_s *errors_in;
  struct i_metric_s *errors_out;
  struct i_metric_s *eps_in;
  struct i_metric_s *eps_out;
  struct i_metric_s *discards_in;
  struct i_metric_s *discards_out;
  struct i_metric_s *dps_in;
  struct i_metric_s *dps_out;

  /*
   * Combined/Custom Graphs 
   */
  struct i_metric_cgraph_s *tput_cg;

  /* Flags */
  unsigned short disabled_metrics;

} i_interface;

typedef struct i_interface_bytecount_s
{
  char *site_id;
  char *device_id;
  char *ifdescr;
  int mday;
  int month;
  int year;

  unsigned long in_mb;        /* Aggregated value in megabytes for time period */
  unsigned long in_remain;    /* The portion of the last diff which was < 1Mb */
  unsigned long in_actual;    /* Actual value at last update */
  struct timeval in_time;     /* Time stamp of last update */

  unsigned long out_mb;       /* Aggregated value in megabytes for time period */
  unsigned long out_remain;   /* The portion of the last diff which was < 1Mb */
  unsigned long out_actual;   /* Actual value at last update */
  struct timeval out_time;    /* Time stamp of last update */
} i_interface_bytecount;

/* interface.c */

i_interface* i_interface_create ();
void i_interface_free (void *ifaceptr);
i_interface_bytecount* i_interface_bytecount_create ();
void i_interface_bytecount_free (void *bcptr);
char* i_interface_state_str (int state);
