int l_msgproc_child_default_from_client (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_msgproc_child_default_from_client_auth_cb (i_resource *self, i_authentication *auth, int result, void *passdata);
int l_msgproc_child_default_from_core (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_msgproc_child_fatal_from_core (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_msgproc_child_fatal_from_client (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
