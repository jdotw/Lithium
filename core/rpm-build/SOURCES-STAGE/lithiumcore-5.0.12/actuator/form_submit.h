typedef struct
{
  char *src_resaddr;
  char *src_entaddr;
  char *src_form;
  time_t src_ref_sec;
  char *src_passdata;
  int src_passdatasize;
  char *src_module;

  char *dst_resaddr;
  char *dst_entaddr;
  char *dst_form;
  time_t dst_ref_sec;
  char *dst_module;
  char *dst_passdata;
  int dst_passdatasize;

  int argc;
  char **argv;
  int optind;

  i_form_get_cbdata *formget_op;
  i_form_send_cbdata *formsend_op;
} a_form_submit_cbdata;

a_form_submit_cbdata* a_form_submit_cbdata_create ();
void a_form_submit_cbdata_free (void *cbdataptr);
int a_form_submit (i_resource *self, int argc, char *argv[], int optind);
int a_form_submit_formget_callback (i_resource *self, i_form *form, i_resource_address *srcresaddr, i_entity_address *srcentaddr, char *form_name, int result, void *passdata);
int a_form_submit_formsend_callback (i_resource *self, i_form *form, void *passdata);

