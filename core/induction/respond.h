void i_respond_ok (i_resource *self, i_message *original_msg);
void i_respond_failed (i_resource *self, i_message *original_msg, int errcode);
void i_respond_denied (i_resource *self, i_message *original_msg);
void i_respond_nodata (i_resource *self, i_message *original_msg);
void i_respond_route_failed (i_resource *self, i_message *original_msg);
