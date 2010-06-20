int i_send_code (i_resource *self, i_resource_address *src, i_resource_address *dst, int code);
int i_send_ok (i_resource *self, i_resource_address *src, i_resource_address *dst);
int i_send_fail (i_resource *self, i_resource_address *src, i_resource_address *dst, int error);
int i_send_unknown (i_resource *self, i_resource_address *src, i_resource_address *dst);
int i_send_no_data (i_resource *self, i_resource_address *src, i_resource_address *dst);
int i_send_code_to_sockfd (int sockfd, i_resource_address *src, i_resource_address *dst, int code);
