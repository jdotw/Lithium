#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "induction.h"
#include "form.h"
#include "path.h"

/** \addtogroup form_item Form Items
 * @ingroup form
 * @{
 */

i_form_item* i_form_image_add (i_form *form, char *name, char *label, char *desc, char *imagefullpath)    /* Ad an image to the form */
{
  int num;
  i_form_item *item;

  /* Create item */
  item = i_form_create_item (FORM_ITEM_IMAGE, name);
  if (!item) { i_printf (1, "i_form_image_add unable to create item"); return NULL; }
  if (label) i_form_item_add_option (item, ITEM_OPTION_LABEL, 0, 0, 0, label, (strlen(label)+1));
  if (desc) i_form_item_add_option (item, ITEM_OPTION_VALUE, 0, 0, 0, desc, (strlen(desc)+1));

  /* Set image if one is specified */
  if (imagefullpath)
  {
    num = i_form_image_setimage (item, imagefullpath);
    if (num != 0)
    { i_printf (1, "i_form_image_add failed to set image"); i_form_free_item (item); return NULL; }
  }

  /* Add item to form */
  i_form_add_item (form, item);

  return item;
}

int i_form_image_setimage (i_form_item *item, char *fullpath)
{
  int fd;
  size_t datasize;
  size_t read_count;
  char *filename;
  char *data;

  fd = open (fullpath, O_RDONLY);
  if (fd < 1)
  { i_printf (1, "i_form_image_setimage failed to open file %s (%s)", fullpath, strerror(errno)); return -1; }
  datasize = lseek (fd, 0, SEEK_END);
  lseek (fd, 0, SEEK_SET);

  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_form_image_setimage failed to malloc data (%lu bytes)", datasize); close (fd); return -1; }

  read_count = read (fd, data, datasize);
  close (fd);
  if (read_count != datasize)
  { i_printf (1, "i_form_image_setimage failed to read file in (read %i of %i bytes", read_count, datasize); free (data); return -1; }

  filename = i_path_extract_filename (fullpath);      /* Extract the filename */
  if (!filename)
  { i_printf (1, "i_form_image_setimage failed to extract filename from path"); free (data); return -1; }

  i_form_item_add_option (item, ITEM_OPTION_FILENAME, 0, 0, 0, filename, (strlen(filename)+1));
  free (filename);
  i_form_item_add_option (item, ITEM_OPTION_DATA, 0, 0, 0, data, datasize);
  free (data);

  return 0;
}

/* @} */
