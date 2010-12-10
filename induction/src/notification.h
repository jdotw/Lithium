typedef struct i_notification_s
{
  i_resource_address *owner;      /* The owner of the notification, this controls who will get it */

  char *subject;
  char *message;

  struct timeval timestamp;
  
} i_notification;

void i_notification_free (void *noteptr);
i_notification* i_notification_create (char *subject, char *message, i_resource_address *owner);
long i_notification_send (i_resource *self, i_notification *note);
char* i_notification_struct_to_data (i_notification *note, int *datasizeptr);
i_notification* i_notification_data_to_struct (char *data, int datasize);
