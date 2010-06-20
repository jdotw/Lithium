typedef struct v_pci_item_s
{
  struct i_object_s *obj;

  /* Xserve Intel */
  struct i_metric_s *temp;
  struct i_metric_s *power;

} v_pci_item;

/* pci.c */
struct i_container_s* v_pci_cnt ();
v_pci_item* v_pci_get (char *desc_str);
int v_pci_enable (i_resource *self);
v_pci_item* v_pci_create (i_resource *self, char *name_str);
int v_pci_disable (i_resource *self);

/* pci_item.c */
v_pci_item* v_pci_item_create ();
void v_pci_item_free (void *itemptr);

/* pci_cntform.c */
int v_pci_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* pci_objform.c */
int v_pci_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_pci_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);


