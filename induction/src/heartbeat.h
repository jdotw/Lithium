#define HEARTBEAT_SEND_SEC 30
#define HEARTBEAT_CHECK_SEC 250

int i_heartbeat_enable (i_resource *self);
int i_heartbeat_timer (i_resource *self, struct i_timer_s *timer, void *passdata);
