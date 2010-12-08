typedef struct v_chassis_item_s
{
  struct i_object_s *obj;

  /* Power */
  struct i_metric_s *systempower;
  struct i_metric_s *powercontrolfault;
  struct i_metric_s *mainspowerfault;
  struct i_metric_s *poweroverload;
  struct i_metric_s *poweroncause;
  struct i_metric_s *poweroffcause;

  /* Chassis */
//  struct i_metric_s *coolingfault;
//  struct i_metric_s *drivefault;
  struct i_metric_s *keylock;
  struct i_metric_s *idlight;

  /* ID */
  struct i_metric_s *serial;
  struct i_metric_s *model;
  

} v_chassis_item;

#define CH_SYSTEMPOWER 1
#define CH_POWEROVERLOAD 2
#define CH_POWERFAULT 8
#define CH_POWERCONTROLFAULT 16

#define CH_PWR_ACFAILED 1
#define CH_PWR_POWERFAULT 2
#define CH_PWR_POWEROVERLOAD 8
#define CH_PWR_IPMION 16

#define CH_KEYLOCK 2
#define CH_DRIVEFAULT 4
#define CH_COOLINGFAULT 8
#define CH_IDLIGHT 48

/* chassis.c */
struct i_container_s* v_chassis_cnt ();
struct v_chassis_item_s* v_chassis_static_item ();
int v_chassis_enable (i_resource *self);
int v_chassis_disable (i_resource *self);

/* chassis_item.c */
v_chassis_item* v_chassis_item_create ();
void v_chassis_item_free (void *itemptr);

/* chassis_cntform.c */
int v_chassis_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* chassis_objform.c */
int v_chassis_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* Bit locations */

