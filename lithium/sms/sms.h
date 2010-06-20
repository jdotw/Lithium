typedef struct
{
  char *number;
  char *message;
  l_gsmdevice *device;
} l_sms_send_cbdata;

/* sms.c */

l_sms_send_cbdata* l_sms_send_cbdata_create ();
void l_sms_send_cbdata_free (void *cbdataptr);

int l_sms_send (i_resource *self, l_gsmdevice *device, char *number, char *message);
int l_sms_send_next (i_resource *self, l_gsmdevice *device);
int l_sms_send_msgmode_callback (i_resource *self, char *line, void *passdata);
int l_sms_send_cmgs_callback (i_resource *self, char *line, void *passdata);
int l_sms_send_sendok_callback (i_resource *self, char *line, void *passdata);
int l_sms_send_failed (i_resource *self, l_sms_send_cbdata *cbdata);

