#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "entity.h"
#include "form.h"

/** \addtogroup form_item Form Items
 * @ingroup form
 * @{
 */

i_form_item* i_form_table_create (i_form *form, char *name, char *label, int cols)
{
  int rows = 0;
  i_form_item *item;

  if (!form || !name || cols < 1) return NULL;

  item = i_form_create_item (FORM_ITEM_TABLE, name);
  if (!item) { i_printf (1, "i_form_table_create unable to create item"); return NULL; }
  
  if (label) i_form_item_add_option (item, ITEM_OPTION_LABEL, 0, 0, 0, label, (strlen(label)+1));
  i_form_item_add_option (item, ITEM_OPTION_COL_COUNT, 0, 0, 0, &cols, sizeof(int));
  i_form_item_add_option (item, ITEM_OPTION_ROW_COUNT, 0, 0, 0, &rows, sizeof(int));  /* Initial value */

  i_form_add_item (form, item);

  return item;  
}

int i_form_table_add_row (i_form_item *item, char *labels[])
{
  int row_count;
  int col_count;
  int i;
  int num;
  i_form_item_option *opt;

  /* Set up the row_count int so that it reflect the current number of rows in
   * the table 
   */
  
  opt = i_form_item_find_option (item, ITEM_OPTION_ROW_COUNT, 0, 0, 0);
  if (opt) { memcpy (&row_count, opt->data, sizeof(int)); }
  else { i_printf (1, "i_form_table_add_row unable to get ITEM_OPTION_ROW_COUNT"); return -1; }

  opt = i_form_item_find_option (item, ITEM_OPTION_COL_COUNT, 0, 0, 0);
  if (opt) { memcpy (&col_count, opt->data, sizeof(int)); }
  else { i_printf (1, "i_form_table_add_row unable to get ITEM_OPTION_COL_COUNT"); return -1; }
  
  for (i=0; i < col_count; i++)    /* Loop for each col */
  {
    if (labels[i]) i_form_item_add_option (item, ITEM_OPTION_LABEL, 1, i, row_count, labels[i], strlen(labels[i])+1);  
  }

  row_count++;
  num = i_form_item_delete_option (item, ITEM_OPTION_ROW_COUNT, 0, 0, 0);
  if (num != 0) { i_printf (1, "i_form_table_add_row failed to delete old ITEM_OPTION_ROW_COUNT option. continuing"); }
  i_form_item_add_option (item, ITEM_OPTION_ROW_COUNT, 0, 0, 0, &row_count, sizeof(int));
  
  return (row_count-1);
}

void i_form_table_add_link (i_form_item *item, int col, int row, i_resource_address *resaddr, i_entity_address *entaddr, char *form_name, time_t ref_sec, char *passdata, int passdata_size)
{ i_form_item_add_link (item, 0, col, row, resaddr, entaddr, form_name, ref_sec, passdata, passdata_size); }

int i_form_table_rowcount (i_form_item *item)
{
  int rowcount;
  i_form_item_option *opt;

  opt = i_form_item_find_option (item, ITEM_OPTION_ROW_COUNT, 0, 0, 0);
  if (opt) 
  { memcpy (&rowcount, opt->data, sizeof(int)); }
  else 
  { i_printf (1, "i_form_table_rowcount failed to get ITEM_OPTION_ROW_COUNT item option"); return 0; }

  return rowcount;
}

int i_form_table_colcount (i_form_item *item)
{ 
  int colcount;
  i_form_item_option *opt;

  opt = i_form_item_find_option (item, ITEM_OPTION_COL_COUNT, 0, 0, 0);
  if (opt) 
  { memcpy (&colcount, opt->data, sizeof(int)); }  
  else   
  { i_printf (1, "i_form_table_colcount failed to get ITEM_OPTION_COL_COUNT item option"); return 0; }
  
  return colcount;
} 

/* @} */
