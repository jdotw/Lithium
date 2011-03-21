typedef struct
{
  int pid;
  int readfd;
  int writefd;

  struct i_socket_s *sock;
  struct i_socket_callback_s *sockcb;
} m_rrd_proc;

#if (defined (__i386__) || defined( __x86_64__ ))
  #define RRD_PROC_COUNT 1
#else
  #define RRD_PROC_COUNT 5
#endif
#define MAX_P1_Q_LENGTH 10000
#define MAX_P2_Q_LENGTH 1000

/* rrd.c */
i_list* m_rrd_proclist ();
i_list* m_rrd_freeproclist ();
i_list* m_rrd_cmdqueue ();
int m_rrd_q_drops ();
void m_rrd_q_drops_reset ();
int m_rrd_init (i_resource *self);
int m_rrd_exec (i_resource *self, int priority, char *cmd_str);
int m_rrd_queuerunner (i_resource *self);

/* rrd_proc.c */
m_rrd_proc* m_rrd_proc_spawn (i_resource *self);
int m_rrd_proc_kill (i_resource *self, m_rrd_proc *proc);
int m_rrd_proc_sockcb (i_resource *self, struct i_socket_s *sock, void *passdata);


