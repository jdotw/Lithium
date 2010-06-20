#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/path.h>
#include <induction/metric.h>
#include <induction/str.h>

#include "resource_address.h"
#include "strutil.h"
#include "form_render.h"

void a_form_render_spacer ()
{ printf ("<BR>"); }

void a_form_render_hrline ()
{ printf ("<HR>"); }

int a_form_render_link (i_form_item *item, int i, int x, int y)   /* Returns 0 on link rendering, -1 on no link */
{
  i_form_item_option *link_resaddr_opt;
  i_form_item_option *link_entaddr_opt;
  i_form_item_option *link_name_opt;
  i_form_item_option *link_refsec_opt;
  i_form_item_option *link_passdata_opt;
  char *resaddr_url = NULL;
  char *entaddr_url = NULL;
  char *form_url;
  char *refsec_url = NULL;

  link_resaddr_opt = i_form_item_find_option (item, ITEM_OPTION_LINK_RESADDR, i, x, y);
  link_entaddr_opt = i_form_item_find_option (item, ITEM_OPTION_LINK_ENTADDR, i, x, y);
  link_name_opt = i_form_item_find_option (item, ITEM_OPTION_LINK_NAME, i, x, y);
  link_refsec_opt = i_form_item_find_option (item, ITEM_OPTION_LINK_REFSEC, i, x, y);
  link_passdata_opt = i_form_item_find_option (item, ITEM_OPTION_LINK_PASSDATA, i, x, y);

  if (!link_name_opt) return -1;

  if (link_resaddr_opt) resaddr_url = i_string_web ((char *) link_resaddr_opt->data);
  if (link_entaddr_opt) entaddr_url = i_string_web ((char *) link_entaddr_opt->data);
  form_url = i_string_web ((char *) link_name_opt->data);
  if (link_refsec_opt) refsec_url = i_string_web ((char *) link_refsec_opt->data);

  printf ("<A HREF=\"index.php?action=form_get");
  if (resaddr_url) printf ("&resaddr=%s", resaddr_url);
  if (entaddr_url) printf ("&entaddr=%s", entaddr_url);
  printf ("&formname=%s", form_url);
  if (refsec_url) printf ("&refsec=%s", refsec_url);

  if (resaddr_url) free (resaddr_url);
  if (entaddr_url) free (entaddr_url);
  free (form_url);
  if (refsec_url) free (refsec_url);

  if (link_passdata_opt)    /* Check if we need to pass data to the form */
  {
    char *passdata_str;
    char *passdata_url;
    char *passdata_ptr;

    passdata_ptr = link_passdata_opt->data + link_passdata_opt->datasize - 1;

    /* First make sure the passdata is a null terminated string */
    if (*passdata_ptr != '\0')
    {
      /* The pass data isnt a null term string, so make it one */
      passdata_str = (char *) malloc (link_passdata_opt->datasize + 1);
      if (!passdata_str)
      { printf ("Unable to malloc passdata_str (%i bytes!)<BR>\n", link_passdata_opt->datasize); }
      memcpy (passdata_str, link_passdata_opt->data, link_passdata_opt->datasize);
      passdata_ptr = passdata_str + link_passdata_opt->datasize;
      *passdata_ptr = '\0';
    }
    else
    {
      /* The pass data is already null terminated */

      passdata_str = strdup (link_passdata_opt->data);
    }

    /* Form the URL string and add it to the URL */

    passdata_url = i_string_web (passdata_str);
    free (passdata_str);

    printf ("&passdata=%s", passdata_url);
    free (passdata_url);

  }

  printf ("\">");

  return 0; 
}

void a_form_render_image (i_form_item *item)
{
  /* Renders an image found in the form
   *
   * This is a bit tricky for a web interface. First the 
   * image will need to be dumped somewhere, then a URL string made for it
   *
   */

  int num;
  int fd;
  int datasize = 0;
  i_form_item_option *opt;
  char *label = NULL;
  char *desc = NULL;
  char *data = NULL;
  char *filename = NULL;
  char *filename_unique;
  char *cache_file;
  char *cache_path;
  char *cache_file_unique;
  struct stat stat_buf;
  struct timeval now;

  /* Retrieve variables from the item */

  opt = i_form_item_find_option (item, ITEM_OPTION_FILENAME, 0, 0, 0);
  if (opt) { filename = opt->data; }

  opt = i_form_item_find_option (item, ITEM_OPTION_DATA, 0, 0, 0);
  if (opt) { data = opt->data; datasize = opt->datasize; }

  opt = i_form_item_find_option (item, ITEM_OPTION_LABEL, 0, 0, 0);
  if (opt) label = opt->data;

  opt = i_form_item_find_option (item, ITEM_OPTION_VALUE, 0, 0, 0);
  if (opt) desc = opt->data;

  /* If a filename and data are present, write 
   * the data to a cache/temp file
   */
  if (filename && data)
  {
    cache_path = i_path_glue (getenv("PWD"), "cache");
    if (!cache_path) { printf ("Unable to create cache_path string<BR>\n"); return; }
    num = stat (cache_path, &stat_buf);
    if (num != 0)
    { /* Attempt to create the cache path */
      num = mkdir (cache_path, 0777);
      if (num != 0)
      {
        printf ("Unable to create cache path<BR>\n");
        free (cache_path);
        return;
      }
    }

    gettimeofday (&now, NULL);

    asprintf (&cache_file, "%li-%i-%i-%s", now.tv_sec, now.tv_usec, getpid(), filename);
    
    cache_file_unique = i_path_glue (cache_path, cache_file);
    free (cache_path);
    free (cache_file);
    if (!cache_path) { printf ("Unable to create cache_file string<BR>\n"); return; }

    unlink (cache_file_unique);    /* Clear out any old cache */
    fd = open (cache_file_unique, O_RDWR|O_CREAT, 0644);
    if (fd < 1)
    { printf ("Unable to create cache_file %s<BR>\n", cache_file_unique);  free (cache_file_unique); return; }
    free (cache_file_unique);

    num = write (fd, data, datasize);
    fsync (fd);
    close (fd);
    if (num != datasize)
    { printf ("Unable to write file out (%i bytes of %i written)<BR>\n", num, datasize); return; }
  }

  /* Begin DIV */
  printf ("<DIV ID=form_image>\n");

  /* Render image label */
  if (label)
  { printf ("<TABLE><TR><TD CLASS=label><B>%s</B></TD></TR></TABLE>", label); }

  /* Render image table */
  printf ("<TABLE>\n");
  printf ("<TR><TD>\n");
  num = a_form_render_link (item, 0, 0, 0);
  if (filename)
  {
    asprintf (&filename_unique, "%li-%i-%i-%s", now.tv_sec, now.tv_usec, getpid(), filename);
    printf ("<IMG SRC=cache/%s>", filename_unique);
    free (filename_unique);
  }
  else
  { printf ("No image available."); }
  if (num == 0) printf ("</A>");
  printf ("</TR></TR>\n");
  if (desc) printf ("<TR><TD CLASS=desc>%s</TR></TD>\n", desc);
  printf ("</TABLE>\n");
  printf ("</DIV>\n");
}

void a_form_render_string (i_form_item *item)
{
  int num;
  int link_render_result = -1;
  char *label_width = LABEL_COL_WIDTH;
  char *label_str;
  char *value_str;
  i_form_item_option *opt;
  i_form_item_option *color_opt;

  if (!item || item->type != FORM_ITEM_STRING) return;

  opt = i_form_item_find_option (item, ITEM_OPTION_LABEL, 0, 0, 0);
  if (opt)
  { label_str = a_string_nl_br ((char *)opt->data); }
  else
  { label_str = NULL; }

  opt = i_form_item_find_option (item, ITEM_OPTION_VALUE, 0, 0, 0);
  if (opt) 
  { 
    value_str = a_string_nl_br ((char *)opt->data); 
  }
  else
  { 
    /* No value, set value_str to NULL
     * and set the width of the label 
     * column (label_width) to 100%
     */
    value_str = NULL;
    label_width = "100%";
  }

  printf ("<DIV ID=form_string><TABLE CELLSPACING=0><TR>");

  if (label_str) 
  {
    /* TD setup */
    printf ("<TD CLASS=label");
    color_opt = i_form_item_find_option (item, ITEM_OPTION_BGCOLOR, 0, 0, 0);
    if (color_opt)
    { printf (" BGCOLOR=%s", (char *)color_opt->data); }
    printf (">");

    /* Font setup */
    printf ("<B>");
    color_opt = i_form_item_find_option (item, ITEM_OPTION_FGCOLOR, 0, 0, 0);
    if (color_opt)
    { printf ("<FONT COLOR=%s>", (char *)color_opt->data); }

    /* Render link if there is no value for this string */
    if (!value_str)
    {
      /* No value, make the link off the label */
      link_render_result = a_form_render_link (item, 0, 0, 0);
    }

    /* String render */
    printf ("%s", label_str);
    free (label_str);

    /* Check if a link was rendered */
    if (link_render_result == 0) printf ("</A>");

    /* End */
    if (color_opt) printf ("</FONT>");
    printf ("</B></TD>\n");
  }

  if (value_str)
  {
    /* TD setup */
    printf ("<TD CLASS=desc");
    color_opt = i_form_item_find_option (item, ITEM_OPTION_BGCOLOR, 0, 1, 0);
    if (color_opt)
    { printf (" BGCOLOR=%s", (char *)color_opt->data); }
    printf (">");

    /* Font setup */
    color_opt = i_form_item_find_option (item, ITEM_OPTION_FGCOLOR, 0, 1, 0);
    if (color_opt)
    { printf ("<FONT COLOR=%s>", (char *)color_opt->data); }

    /* String render */
    num = a_form_render_link (item, 0, 0, 0);
    printf ("%s", value_str);
    free (value_str); 
    if (num == 0) printf ("</A>");

    /* End */
    if (color_opt) printf ("</FONT>");
    printf ("</FONT></TD>");
  }

  printf ("</TR></TABLE></DIV>\n");

}

void a_form_render_table (i_form_item *item)
{
  int y;
  int *col_count;
  int *row_count;
  i_form_item_option *opt;

  if (!item) return;

  printf ("<DIV ID=form_table>\n");
  
  opt = i_form_item_find_option (item, ITEM_OPTION_LABEL, 0, 0, 0);
  if (opt) 
  {
    printf ("<TABLE><TR><TD ID=label><B>%s</B></TD><TD WIDTH=70%%>&nbsp;</TD></TR></TABLE>", (char *) opt->data);
  }

  opt = i_form_item_find_option (item, ITEM_OPTION_COL_COUNT, 0, 0, 0);
  if (!opt) { printf ("Table item present but col count not found<BR>"); return; }
  col_count = (int *) opt->data;

  opt = i_form_item_find_option (item, ITEM_OPTION_ROW_COUNT, 0, 0, 0);
  if (!opt) { printf ("Table empty<BR>\n"); return; }  /* Empty table */
  row_count = (int *) opt->data;

  /* Start the table */

  printf ("<TABLE ID=nospace_table");

  opt = i_form_item_find_option (item, ITEM_OPTION_WIDTH, 0, 0, 0);
  if (opt) printf (" WIDTH=%s", (char *) opt->data);
  if (!opt)
  {
    /* Check for prop spacing */
    opt = i_form_item_find_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0);
    if (opt) printf (" WIDTH=100%%"); 
  } 

  printf ("><TR><TD>\n");

  printf ("<TABLE");

  opt = i_form_item_find_option (item, ITEM_OPTION_WIDTH, 0, 0, 0);
  if (opt) printf (" WIDTH=%s", (char *) opt->data);
  if (!opt)
  {
    /* Check for prop spacing */
    opt = i_form_item_find_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0);
    if (opt) printf (" WIDTH=100%%");
  }

  printf (">");

  for (y=0; y < *row_count; y++)    /* Loop for each row */
  {
    int x;
    int mod;
    i_form_item_option *color_opt;

    /* Begin Row Def */
    printf ("<TR ALIGN=LEFT");

    /* Background colour */
    color_opt = i_form_item_find_option (item, ITEM_OPTION_BGCOLOR, 1, -1, y);
    if (color_opt)
    {
      /* Use specified BGCOLOR */ 
      printf (" BGCOLOR=%s", (char *) color_opt->data); 
    }
    else
    {
      /* Use alternating row colours */
      if (i_form_item_find_option(item, ITEM_OPTION_NOHEADERS, 0, 0, 0))
      { mod = 1; }
      else
      { mod = 0; }
      if ((y != 0 || i_form_item_find_option(item, ITEM_OPTION_NOHEADERS, 0, 0, 0)) && y % 2 == mod)
      {
        /* Even/Shaded row */
        printf (" BGCOLOR=%s", TABLE_ROW_BG_SHADED);
      }
    }

    /* End Row Def */
    printf (">\n");
    
    for (x=0; x < *col_count; x++)  /* Loop for each col */
    {
      int num;
      i_form_item_option *spacing_opt;

      /* Setup the HTML table */
      printf ("<TD VALIGN=TOP");

      /* Spacing */     
      spacing_opt = i_form_item_find_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0);
      if (spacing_opt) printf (" WIDTH=*");
      else 
      {
        /* See if column specific spacing is applied */
        spacing_opt = i_form_item_find_option (item, ITEM_OPTION_WIDTH, 1, x, 0); 
        if (spacing_opt) printf (" WIDTH=%s", (char *) spacing_opt->data);
      }

      /* Alignment */
      opt = i_form_item_find_option (item, ITEM_OPTION_ALIGN, 1, x, y);
      if (!opt)
      { opt = i_form_item_find_option (item, ITEM_OPTION_ALIGN, 0, x, 0); }
      if (opt)
      { printf (" ALIGN=%s", (char *) opt->data); }

      /* Background colour */
      color_opt = i_form_item_find_option (item, ITEM_OPTION_BGCOLOR, 1, x, y);
      if (color_opt)
      { printf (" BGCOLOR=%s", (char *) color_opt->data); }

      /* Forground colour */
      color_opt = i_form_item_find_option (item, ITEM_OPTION_FGCOLOR, 1, x, y);
      if (color_opt)
      { printf (" FGCOLOR=%s", (char *) color_opt->data); }

      /* Finish table definition */
      printf (">");

      /* Check for any links */

      num = a_form_render_link (item, 0, x, y);

      /* Printf the cell */

      opt = i_form_item_find_option (item, ITEM_OPTION_LABEL, 1, x, y);                         /* Find the cells label */
      if (opt) 
      {
        char *label_str;

        /* Setup column headers */
        if (y == 0 && !i_form_item_find_option(item, ITEM_OPTION_NOHEADERS, 0, 0, 0)) 
        { printf ("<B><U>"); }

        /* Font Setup */
        color_opt = i_form_item_find_option (item, ITEM_OPTION_FGCOLOR, 1, x, y);
        if (color_opt)
        { printf ("<FONT COLOR=%s>", (char *) color_opt->data); }

        /* Render string */
        label_str = a_string_nl_br ((char *)opt->data);
        printf ("%s", label_str);
        free (label_str);

        /* End font setup */
        if (color_opt) printf ("</FONT>");

        /* End column headers */
        if (y == 0 && !i_form_item_find_option(item, ITEM_OPTION_NOHEADERS, 0, 0, 0)) 
        { printf ("</U></B>"); }
      }
      else printf ("&nbsp;");

      /* Tidy up the link if present */
      if (num == 0) printf ("</A>");

      /* Tidy up HTML table */
      printf ("</TD>");
    }

    printf ("</TR>\n");
  }
  printf ("</TABLE>\n");

  printf ("</TD></TR></TABLE>\n");

  printf ("</DIV>\n");
}

void a_form_render_entry (i_form_item *item)
{
  i_form_item_option *label_opt;
  i_form_item_option *value_opt;

  if (!item || item->type != FORM_ITEM_ENTRY) return;

  printf ("<DIV ID=form_entry><TABLE CELLSPACING=0 WIDTH=100%%><TR><TD WIDTH=%s ALIGN=LEFT><B>", LABEL_COL_WIDTH);
  label_opt = i_form_item_find_option (item, ITEM_OPTION_LABEL, 0, 0, 0);
  if (label_opt) printf ("%s", (char *) label_opt->data);
  else printf ("&nbsp;");
  printf ("</B></TD>\n");

  value_opt = i_form_item_find_option (item, ITEM_OPTION_VALUE, 0, 0, 0);
  if (value_opt)                                           /* If there is data, print it */
  {
    printf ("<TD ALIGN=LEFT>\n");
    printf ("<input type=\"text\" size=45 name=\"%s\" value=\"%s\">\n", item->name, (char *) value_opt->data);
    printf ("</TD>\n");
  }

  printf ("</TR></TABLE></DIV>\n");

}

void a_form_render_password (i_form_item *item)
{
  i_form_item_option *label_opt;
  i_form_item_option *value_opt;

  if (!item || item->type != FORM_ITEM_PASSWORD) return;

  printf ("<DIV ID=form_entry><TABLE CELLSPACING=0 WIDTH=100%%><TR><TD WIDTH=%s ALIGN=LEFT><B>", LABEL_COL_WIDTH);
  label_opt = i_form_item_find_option (item, ITEM_OPTION_LABEL, 0, 0, 0);
  if (label_opt) printf ("%s", (char *) label_opt->data);
  else printf ("&nbsp;");
  printf ("</B></TD>\n");

  value_opt = i_form_item_find_option (item, ITEM_OPTION_VALUE, 0, 0, 0);
  if (value_opt)                                           /* If there is data, print it */
  {
    printf ("<TD ALIGN=LEFT>\n");
    printf ("<input type=\"password\" size=45 name=\"%s\" value=\"%s\">\n", item->name, (char *) value_opt->data);
    printf ("</TD>\n");
  }

  printf ("</TR></TABLE></DIV>\n");

}

void a_form_render_textarea (i_form_item *item)
{
  i_form_item_option *label_opt;
  i_form_item_option *value_opt;

  printf ("<DIV ID=form_textarea><TABLE CELLSPACING=0 WIDTH=100%%><TR><TD CLASS=label ALIGN=LEFT><B>");
  label_opt = i_form_item_find_option (item, ITEM_OPTION_LABEL, 0, 0, 0);  
  if (label_opt) printf ("%s", (char *) label_opt->data);  
  else printf ("&nbsp;");
  printf ("</B></TD>\n");

  printf ("<TD ALIGN=LEFT>\n");
  printf ("<TEXTAREA WRAP=VIRTUAL NAME=\"%s\"", item->name);
  printf (" ROWS=10 COLS=48>");
  value_opt = i_form_item_find_option (item, ITEM_OPTION_VALUE, 0, 0, 0);
  if (value_opt && value_opt->data)                        /* If there is data, print it */
  { printf ("%s", (char *) value_opt->data); }
  printf ("</TEXTAREA>\n");
  printf ("</TD>\n");

  printf ("</TR></TABLE></DIV>\n");
}


void a_form_render_hidden (i_form_item *item)
{
  i_form_item_option *value_opt;

  if (!item || item->type != FORM_ITEM_HIDDEN) return;

  value_opt = i_form_item_find_option (item, ITEM_OPTION_VALUE, 0, 0, 0);
  if (value_opt)                                           /* If there is data, print it */
  {
    printf ("<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", item->name, (char *) value_opt->data);
  }

}

void a_form_render_dropdown (i_form_item *item)
{
  int i;
  i_form_item_option *label_opt;
  i_form_item_option *value_opt;
  i_form_item_option *main_label_opt = NULL;
  i_form_item_option *selected_value_opt = NULL;

  if (!item || item->type != FORM_ITEM_DROPDOWN) return;

  printf ("<DIV ID=form_dropdown>");
  printf ("<TABLE><TR><TD CLASS=label><B>");
  main_label_opt = i_form_item_find_option (item, ITEM_OPTION_LABEL, 0, 0, 0);
  if (main_label_opt) printf ("%s", (char *) main_label_opt->data);
  else printf ("&nbsp;");
  printf ("</B></TD>\n");

  printf ("<TD ALIGN=LEFT>\n");
  printf ("<select name=\"%s\">", item->name);
  for (i=1; (label_opt = i_form_item_find_option (item, ITEM_OPTION_LABEL, i, 0, 0)) != NULL; i++)
  {
    value_opt = i_form_item_find_option (item, ITEM_OPTION_VALUE, i, 0, 0);
    if (value_opt) 
    { 
      i_form_item_option *selected;

      printf ("<option value=");
      printf ("\"%s\"", (char *) value_opt->data);

      selected = i_form_item_find_option (item, ITEM_OPTION_SELECTED, i, 0, 0);
      if (selected)
      {
        selected_value_opt = label_opt;
        printf (" selected=true");
      }

      printf (">");
      printf ("%s", (char *) label_opt->data);
      printf ("</option>");
    }
  }

  printf ("</select></TD></TR></TABLE></DIV>");    

}

void a_form_render_frame_start (i_form_item *item)
{
  i_form_item_option *opt;

  /* Start Division */
  printf ("<DIV ID=box>");

  /* Frame Title */
  opt = i_form_item_find_option (item, ITEM_OPTION_LABEL, 0, 0, 0);
  if (opt) 
  { printf ("<TABLE ID=title><TR><TD ID=title>%s</TD></TR></TABLE>", (char *) opt->data); } 

  return;
}

void a_form_render_frame_end (i_form_item *item)
{
  /* End Division */
  printf ("</DIV>");

  return;
}

void a_form_render (i_resource *self, i_form *form, char *resaddr_str, char *entaddr_str, char *form_name, time_t ref_sec, char *pass_data)
{
  i_form_item *item;
  i_form_option *opt;

  /* Check if for is submitable */
  if (form->submit == 1) 
  {
    char *src_resaddr_url;
    char *src_entaddr_url;
    char *src_form_url;
    char *src_refsec_url;
    char *src_passdata_url = NULL;
    char *dst_resaddr_url;
    char *dst_entaddr_url;
    char *dst_form_url; 
    char *dst_refsec_url; 
    char *dst_passdata_url = NULL;

    /* Src values */

    opt = i_form_option_find (form, FORM_OPTION_SOURCE_RESADDR);          /* Check if a return resaddr is specified */
    if (opt) src_resaddr_url = i_string_web ((char *) opt->data);
    else src_resaddr_url = i_string_web (resaddr_str);                    /* If not return to same resaddr */
    
    opt = i_form_option_find (form, FORM_OPTION_SOURCE_ENTADDR);          /* Check if a return entaddr is specified */
    if (opt) src_entaddr_url = i_string_web ((char *) opt->data);
    else src_entaddr_url = i_string_web (entaddr_str);                    /* If not return to same entaddr */

    opt = i_form_option_find (form, FORM_OPTION_SOURCE_FORM);             /* Check if a return form is specified */
    if (opt) src_form_url = i_string_web ((char *) opt->data);
    else src_form_url = i_string_web (form_name);                         /* If not, return it to the default form (<form_name>-submit)*/

    opt = i_form_option_find (form, FORM_OPTION_SOURCE_REFSEC);           /* Check if a return refsec is specified */
    if (opt) src_refsec_url = i_string_web ((char *) opt->data);
    else 
    {
      char *str;
      asprintf (&str, "%li", ref_sec);
      src_refsec_url = i_string_web (str);
      free (str);
    }

    opt = i_form_option_find (form, FORM_OPTION_SOURCE_PASSDATA);
    if (opt) src_passdata_url = i_string_web ((char *) opt->data);
    else if (pass_data) src_passdata_url = i_string_web (pass_data);

    /* Dst values */

    opt = i_form_option_find (form, FORM_OPTION_SUBMIT_RESADDR);          /* Check if a return resaddr is specified */
    if (opt) dst_resaddr_url = i_string_web ((char *) opt->data);
    else dst_resaddr_url = i_string_web (resaddr_str);                    /* If not return to same resaddr */

    opt = i_form_option_find (form, FORM_OPTION_SUBMIT_ENTADDR);          /* Check if a return entaddr is specified */
    if (opt) dst_entaddr_url = i_string_web ((char *) opt->data);
    else dst_entaddr_url = i_string_web (entaddr_str);                    /* If not return to same entaddr */

    opt = i_form_option_find (form, FORM_OPTION_SUBMIT_NAME);             /* Check if a return form is specified */
    if (opt) dst_form_url = i_string_web ((char *) opt->data);
    else dst_form_url = i_string_web (form_name);                         /* If not, return it to the default form (<form_name>-submit)*/

    opt = i_form_option_find (form, FORM_OPTION_SUBMIT_REFSEC);           /* Check if a return refsec is specified */
    if (opt) dst_refsec_url = i_string_web ((char *) opt->data);
    else 
    {
      char *str;
      asprintf (&str, "%li", ref_sec);
      dst_refsec_url = i_string_web (str);
      free (str);
    } 

    opt = i_form_option_find (form, FORM_OPTION_SUBMIT_PASSDATA);
    if (opt) dst_passdata_url = i_string_web ((char *) opt->data);
    else if (pass_data) dst_passdata_url = i_string_web (pass_data);

    /* Print the form string */
    printf ("<FORM action=\"index.php?action=form_submit&srcresaddr=%s&srcentaddr=%s&srcform=%s&srcrefsec=%s", 
      src_resaddr_url, src_entaddr_url, src_form_url, src_refsec_url);
    if (src_passdata_url) printf ("&srcpassdata=%s", src_passdata_url);                                    /* Optional src pass data */
    printf ("&dstresaddr=%s&dstentaddr=%s&dstform=%s&dstrefsec=%s", 
      dst_resaddr_url, dst_entaddr_url, dst_form_url, dst_refsec_url);                                      /* dst requirements */
    if (dst_passdata_url) printf ("&dstpassdata=%s", dst_passdata_url);                                    /* Optional src pass data */
    printf ("\" method=post>\n");                                                                           /* Require end */

    /* Clean up */
    free (src_resaddr_url);
    free (src_entaddr_url);
    free (src_form_url);
    free (src_refsec_url);
    if (src_passdata_url) free (src_passdata_url);
    free (dst_resaddr_url);
    free (dst_entaddr_url);
    free (dst_form_url);
    free (dst_refsec_url);
    if (dst_passdata_url) free (dst_passdata_url);
  }

  /* Render items */

  for (i_list_move_head(form->items); (item = i_list_restore(form->items)) != NULL; i_list_move_next(form->items))
  {

    switch (item->type)
    {
      case FORM_ITEM_FRAMESTART: a_form_render_frame_start (item);
                                 break;
      case FORM_ITEM_FRAMEEND: a_form_render_frame_end (item);
                                 break;
      case FORM_ITEM_STRING:  a_form_render_string (item);
                              break;
      case FORM_ITEM_ENTRY:   a_form_render_entry (item);
                              break;
      case FORM_ITEM_PASSWORD:   a_form_render_password (item);
                              break;
      case FORM_ITEM_TEXTAREA:   a_form_render_textarea (item);
                                 break;
      case FORM_ITEM_DROPDOWN: a_form_render_dropdown (item);
                               break;
      case FORM_ITEM_HIDDEN: a_form_render_hidden (item);
                             break;
      case FORM_ITEM_TABLE: a_form_render_table (item);
                            break;
      case FORM_ITEM_SPACER: a_form_render_spacer ();
                             break;
      case FORM_ITEM_HRLINE: a_form_render_hrline ();
                             break;
      case FORM_ITEM_IMAGE: a_form_render_image (item);
                            break;
      default:        printf ("Unknown/Unsupported item type (%i)<BR>\n", item->type);
    }

  }

  /* Completre Submittable form data */
  if (form->submit == 1)
  {
    i_form_option *submit_label_opt;

    /* Start Division */
    printf ("<DIV ID=box>");
  
    /* Frame Title */
    printf ("<TABLE ID=title><TR><TD ID=title>Submit Form</TD></TR></TABLE>");

    /* Begin Def */
    printf ("<input type=\"submit\" name=\"submit\" value=");

    /* Check for a specified submit label */
    submit_label_opt = i_form_option_find (form, FORM_OPTION_SUBMIT_LABEL);
    if (submit_label_opt) printf ("\"%s\"", (char *) submit_label_opt->data);
    else printf ("\"Submit\"");

    /* End Def */
    printf (" size=80></FORM>\n");

    /* End Division */
    printf ("</DIV>");
  }

  return;   
}

