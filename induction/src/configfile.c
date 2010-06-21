#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "induction.h"
#include "configfile.h"
#include "path.h"
#include "construct.h"
#include "files.h"

static char *static_nodeconf_cache = NULL;

int i_configfile_check_exists (i_resource *self, char *file) /* 1=Exists 0=Doesnt */
{
  int fd;
  char *fullpath;
  char *config_path = NULL;

  if (self && self->construct)    /* Keep things safe for when self==NULL */
    config_path = self->construct->config_path;

  fullpath = i_path_glue (config_path, file);
  if (!fullpath) return 0;
	
  fd = open (fullpath, O_RDONLY);
  free (fullpath);
  if (fd < 1) return 0;
  close (fd);

  return 1;
}

int i_configfile_open (i_resource *self, char *file, int flags)
{
  int fd;
  char *fullpath;
  char *config_path = NULL;

  if (self && self->construct)  /* Safe for when self=NULL */
  { config_path = self->construct->config_path; }
  
  fullpath = i_path_glue (config_path, file);
  if (!fullpath) { i_printf (1, "i_configfile_open failed glue full path for %s", file); return -1; }

  fd = open (fullpath, flags);
  if (fd < 0) { i_printf (2, "i_configfile_open failed to open config file %s", fullpath); free (fullpath); return -1; }
  free (fullpath);

  return fd;
}

char* i_configfile_read_line (int fd)
{
  int num;
  int startseek;
  int charcount=0;
  char chr=' ';
  char *string;

  startseek = lseek (fd, 0, SEEK_CUR);		/* Record where the FD is */

  while (chr != '\n')
  {
    num = read (fd, &chr, 1);		/* Read in one character */
    if (num != 1) { i_printf (1, "i_configfile_read_line failed to read in character in config file"); return NULL; }
    charcount++;
  }

  /* At this point FD is at the first character after the \n */

  lseek (fd, startseek, SEEK_SET);		/* Set the FD back to where we started */
  string = malloc (charcount + 1);
  if (!string) { i_printf (1, "i_configfile_read_line failed to malloc string"); return NULL; }
  num = read (fd, string, charcount);		/* Actually read in the line we want */
  if (num < 1) { i_printf (1, "i_configfile_read_line failed to read line"); return NULL; }
  string[charcount] = '\0';

  return string;					/* Hoik! */
}

void i_configfile_clean_comments (char *dump)
{
  /* Comments begin with '#' and go until the end of the line */
	
  int i;
  int num;
  char *startptr = dump;
  char *endptr;

  while (1)
  {
    startptr = strchr (startptr, '#');	/* Go to the first # */
    if (!startptr) break;			/* No more #'s */
    *startptr = '\n';			/* Insert a dummy \n where the comment originally began */
    startptr++;				/* Move along */
    endptr = strchr (startptr, '\n');	/* Go to the end of the line */
    if (!endptr)				/* Assume we found a coment but no end of line. End of file? */
      endptr = dump + strlen (dump);	/* Set to end of dump */

    num = endptr - startptr;
    for (i=0; i < num; i++)
    {
      *startptr = ' ';		/* WHiteout ! :D */
      startptr++;
    }
  }
}

char* i_configfile_get_section (i_resource *self, char *file, char *section)		/* Return the section block for a given section */
{
  int num;
  int filesize;
  int fd;
  char *block;
  char *filedump;
  char *sectionstart;
  char *sectionend;
  char *sectionstr;

  /* Check if this is a request from the NODECONF_FILE */
  if (!strcmp(file, NODECONF_FILE) && static_nodeconf_cache)
  {
    /* File is NODECONF and it is cached */
    filedump = strdup (static_nodeconf_cache);
  }
  else
  {
    /* File is either not NODECONF or is not cached */

    /* Open file */
    fd = i_configfile_open (self, file, O_RDONLY);
    if (fd < 0) { i_printf (2, "i_configfile_get_section failed to open file %s", file); return NULL; }

    /* Get file size */
    filesize = lseek (fd, 0, SEEK_END);		/* How big is this file? */
    if (filesize < 1) { close (fd); return NULL; }	/* Empty config file, no sections */

    /* Alloc memory */
    filedump = malloc (filesize + 1);
    if (!filedump) { i_printf (1, "i_configfile_get_section failed to malloc filedump for section %s in %s", section, file); close (fd); return NULL; }

    /* Read file */
    num = lseek (fd, 0, SEEK_SET);			/* Back to the start */
    num = read (fd, filedump, filesize);		/* Read the whole config file in */
    close (fd);
    if (num < 1) { i_printf (1, "i_configfile_get_section failed read file section %s in %s", section, file); free (filedump); return NULL; }
    filedump[filesize] = '\0';

    /* Clean comments */
//    i_configfile_clean_comments (filedump);			/* GO through the dump line by line and remove the comments */
    
    /* Check if we should cache the file */
    if (!strcmp(file, NODECONF_FILE) && self->type != 4)
    { static_nodeconf_cache = strdup (filedump); }
  }

  /* Ok, the whole config now resides in filedump */

  asprintf (&sectionstr, "<section %s>", section);	/* Make up the string we're lookin for (section start) */
  sectionstart = strstr (filedump, sectionstr);		/* Find our sections start */
  free (sectionstr);					/* Recycle */
  if (!sectionstart) {free (filedump); return NULL;}		/* Section not found */

  asprintf (&sectionstr, "</%s>", section);		/* Make up the string we're looking for (section end) */
  sectionend = strstr (sectionstart, sectionstr);		/* Find the end */
  num = strlen (sectionstr);				/* We'll need this in a sec */
  free (sectionstr);					/* Dont need this now */
  if (!sectionend) 
  { 
    i_printf (1, "i_configfile_get_section found section %s found but it was not terminated", section); 
    free (filedump); 
    return NULL; 
  }
  sectionend = sectionend + num;				/* Now we are really at the end */
  *sectionend = '\0';					/* Terminate it so we can copy it */

  /* *sectionstart and *sectionend are now set right */

  block = malloc (sectionend - sectionstart + 1);
  if (!block) { i_printf (1, "i_configfile_get_section failed to malloc block for section %s in %s", section, file); free (filedump); return NULL; }
  strcpy (block, sectionstart);

  free (filedump);

  return block;
}

char* i_configfile_block_get_element (char *block, char *elementname, int instance) /* Return the element in a given block */
{
  int i;
  char *blockptr = NULL;
  char *startptr;
  char *endptr;
  char *element;

  /* FIX : A non-null terminated block sent to this could cause BAD-STUFF */
  
  /* OK, block is given to use which is the entire config block starting at <Section> and ending with </blah> */
	
  blockptr = strchr (block, '>');			/* Move to the end of the section identifier */
  if (!blockptr) return NULL;
  blockptr++;

  /* Now skip the instances we dont want */
  
  for (i=0; i < instance; i++)
  {
    blockptr = strstr (blockptr, elementname);	/* Then loop to the required instance */
    if (!blockptr) { return NULL; }			/* Didnt find the instance of the element */

    blockptr = strchr (blockptr, '"');  /* Move to start of data */
    if (!blockptr) { return NULL; } 		/* Didnt find the instance of the element */
    blockptr++;

    blockptr = strchr (blockptr, '"');  /* Move to start of data */
    if (!blockptr) { return NULL; }         /* Didnt find the instance of the element */
    blockptr++;
  }

  blockptr = strstr (blockptr, elementname);        /* Go to the first instance of element name */
  if (!blockptr) return NULL;           /* Didnt find the instance of the element */
          
  startptr = strchr (blockptr, '"');		/* Find the first " */
  if (!startptr) return NULL;			/* No value found */
  startptr++;					/* Move past the " */
  endptr = strchr (startptr, '"');		/* Find the last " */
  if (!endptr) return NULL;			/* Didnt terminate the string, you deserve it */

  *endptr = '\0';                                 /* Temporarily pre-terminate the string for copying */
  element = strdup (startptr);
  *endptr = '"';                                  /* Return it to normal */

  return element;
}

char* i_configfile_get (i_resource *self, char *file, char *section, char *elementname, int instance)
{
  char *element;
  char *sectionblock;

  /* First Step, Find our section */
  sectionblock = i_configfile_get_section (self, file, section);
  if (!sectionblock) { i_printf (2, "i_configfile_get failed to find section %s in %s", section, file); return NULL; }

  /* Second Step Find the Element, by instance of the element */
  element = i_configfile_block_get_element (sectionblock, elementname, instance);
  free (sectionblock);
  if (!element) { i_printf (2, "i_configfile_get failed to find instance %i of element %s, in section %s (%s)", instance, elementname, section, file); return NULL; }

  return element;
}

int i_configfile_instance_count (i_resource *self, char *file, char *section, char *elementname)
{
  int count = 0;
  char *block;

  /* First Step, Find our section */
  block = i_configfile_get_section (self, file, section);
  if (!block) return 0;

  while (i_configfile_block_get_element (block, elementname, count) != NULL)		/* Loop for each instance we find */
  {
    count++;
  }

  return count;
}

