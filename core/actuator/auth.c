#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/entity.h>
#include <induction/form.h>
#include <induction/message.h>
#include <induction/socket.h>
#include <induction/data.h>
#include <induction/msgproc.h>

#include "customer.h"
#include "shutdown.h"

int a_auth_required_msgcb (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  if (!msg || !msg->data || msg->datasize < 1)
  {
    /* No data received */
    exit(1);  // Error
  }

  int offset = 0;
  char *dataptr = msg->data;
  int result = i_data_get_int(msg->data, dataptr, msg->datasize, &offset);
  if (offset > 0)
  {
    /* Received result code */
    printf("%i", result);
    exit(result);
  }
  else
  {
    /* Error in result code */
    exit (1);
  }
}

int a_auth_required (i_resource *self, int argc, char *argv[], int optind)
{
  /* Ask LithiumCore if Authentication is required for this customer.
   *
   * The only option supplied is the customer resaddr
   */

  /* Send request to customer */
  i_resource_address *resaddr = i_resource_address_string_to_struct(argv[optind+2]);
  long msgid = i_message_send (self, MSG_AUTH_REQUIRED, NULL, 0, resaddr, MSG_FLAG_REQ, 0);
  i_resource_address_free(resaddr);

  /* Add callback for response */
  i_msgproc_callback_add (self, self->core_socket, msgid, MSG_TIMEOUT_SEC, 0, a_auth_required_msgcb, NULL);

  return 0;
}


