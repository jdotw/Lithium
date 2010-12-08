void a_form_render_spacer ();
int a_form_render_link (i_form_item *item, int i, int x, int y);
void a_form_render_image (i_form_item *item);
void a_form_render_string (i_form_item *item);
void a_form_render_table (i_form_item *item);
void a_form_render_entry (i_form_item *item);
void a_form_render_password (i_form_item *item);
void a_form_render_textarea (i_form_item *item);
void a_form_render_hidden (i_form_item *item);
void a_form_render_dropdown (i_form_item *item);
void a_form_render_frame_start (i_form_item *item);
void a_form_render_frame_end (i_form_item *item);
void a_form_render (i_resource *self, i_form *form, char *resaddr_str, char *entaddr_str, char *form_name, time_t ref_sec, char *pass_data);

#define TABLE_ROW_BG_SHADED "#F5F5F5"
