#include <stdlib.h>

#include <induction.h>
#include <induction/socket.h>

#include "shutdown.h"

void a_shutdown (i_resource *self)
{
  i_socket_free (self->core_socket);
}
