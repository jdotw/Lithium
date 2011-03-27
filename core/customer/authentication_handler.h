int l_authentication_check_verification_callback (i_resource *self, i_authentication *auth, int result, void *passdata);
int l_authentication_check_handler (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_authentication_required_handler (i_resource *self, struct i_socket_s *sock, struct i_message_s *msg, void *passdata);
