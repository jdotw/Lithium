#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "induction.h"
#include "path.h"

extern i_resource *global_self;

char* i_path_extract_filename (char *path)
{
  char *filename;
  char *pathptr;

  if (!path) return NULL;

  pathptr = strrchr (path, '/');
  if (!pathptr) return strdup (path);

  pathptr++;
  filename = strdup (pathptr);

  return filename;
}

char* i_path_glue (char *path, char *file)
{
	int pathsize;
	char *fullpath;

    if (!file) return NULL;
	if (!path) return (strdup (file));

	pathsize = strlen(path) + strlen(file) + 2;
    fullpath = (char *) malloc (pathsize);
    memset (fullpath, 0, pathsize);

	snprintf (fullpath, pathsize, "%s/%s", path, file);

	return fullpath;
}

char* i_path_socket (char *ident, char *name)
{
	int pathsize;
	char *path;
	char *tmppath = "/tmp/";
	struct stat statbuf;

	if (!ident || !name) return NULL;

	pathsize = strlen (ident) + strlen (tmppath) + 1;
	path = (char *) malloc (pathsize);
	if (!path) return NULL;
	
	sprintf (path, "%s%s", tmppath, ident);

	if ((stat(path, &statbuf)) == -1)
	{
		mkdir (path, 0777);
	}

	free (path);

	pathsize += strlen (name) + 1;

	path = (char *) malloc (pathsize);
	if (!path) return NULL;

	sprintf (path, "%s%s-%s", tmppath, ident, name);

	return path;
}

int i_path_mkdir (char *path, mode_t mode)
{
  int num;

  num = mkdir (path, mode);
  if (num != 0 && errno != EEXIST)
  { return -1; }

  return 0;
}

char* i_path_escape_spaces (char *path)
{
  char *escape;

  escape = (char *)malloc((strlen(path)*2)+1);    // Double is overkill but safe 
  unsigned int i;
  unsigned int escape_i = 0;
  for (i=0; i < strlen(path); i++)
  {
    if (path[i] == ' ')
    {
      /* Escape the space */
      escape[escape_i] = '\\';
      escape_i++;
    }

    /* Perform normal copy */
    escape[escape_i] = path[i];
    escape_i++;
  }

  return escape;
}
  
