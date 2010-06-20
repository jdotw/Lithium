#define GSM_STATE_INIT 1      /* Device initialising */
#define GSM_STATE_READY 2     /* Device ready/idle */
#define GSM_STATE_BUSY 3      /* Device busy */

#define DEFAULT_RETRY_SECONDS 60

typedef struct l_gsmdevice_s
{
  int state;                      /* State of device */
  i_socket *socket;               /* Socket */
  i_list *smsq;                  /* Notification queue */
  i_timer *sigstrength_timer;     /* Sigstrength check timer */
} l_gsmdevice;

/* gsmdevice.c */

l_gsmdevice *l_gsmdevice_create ();
void l_gsmdevice_free (void *deviceptr);
l_gsmdevice* l_gsmdevice_device ();
int l_gsmdevice_init (i_resource *self, l_gsmdevice *device);
int l_gsmdevice_init_reset_callback (i_resource *self, char *line, void *passdata);
int l_gsmdevice_init_pin_callback (i_resource *self, char *line, void *passdata);
int l_gsmdevice_init_registration_callback (i_resource *self, char *line, void *passdata);
int l_gsmdevice_init_failed (i_resource *self, l_gsmdevice *device);
int l_gsmdevice_init_retry_callback (i_resource *self, i_timer *timer, void *passdata);
