int l_heartbeat_enable (i_resource *self, i_resource *res);
int l_heartbeat_timer (i_resource *self, struct i_timer_s *timer, void *passdata);
int l_heartbeat_handler (i_resource *self, struct i_socket_s *sock, struct i_message_s *msg, void *passdata);
  
