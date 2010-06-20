typedef struct i_navtree_node_s
{
  /* Node Identification */
  char *name_str;
  char *desc_str;
  short opstate;
  short adminstate;
  struct i_navtree_node_s *parent;

  /* Navigation Variables */
  struct i_resource_address_s *resaddr; /* Resource addresss */
  struct i_entity_address_s *entaddr;   /* Entity address */
  char *form_str;                       /* Form Name (NULL for main) */
  char *passdata;                       /* Form passdata */
  int passdata_size;                    /* Form passdata size */

  /* Child Nodes */
  struct i_list_s *child_list;          /* Child nodes */

} i_navtree_node;

#define NAVTREE_EXP_ALWAYS 0
#define NAVTREE_EXP_RESTRICT 1
#define NAVTREE_EXP_NEVER 2

/* navtree.c */
i_navtree_node* i_navtree_generate (i_resource *self, struct i_entity_s *ent, i_navtree_node *root);
int i_navtree_defaultfunc_set (int (*defaultfunc) ());

/* navtree_node.c */
i_navtree_node* i_navtree_node_create (char *name_str, char *desc_str, i_resource_address *resaddr, struct i_entity_address_s *entaddr, char *form_str, char *passdata, int passdata_size, i_navtree_node *parent);
void i_navtree_node_free (void *nodeptr);
