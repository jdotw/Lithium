int i_configfile_check_exists (i_resource *self, char *file);
int i_configfile_open (i_resource *self, char *file, int flags);
char* i_configfile_read_line (int fd);
void i_configfile_clean_comments (char *dump);
char* i_configfile_get_section (i_resource *self, char *file, char *section);
char* i_configfile_block_get_element (char *block, char *elementname, int instance);
char* i_configfile_get (i_resource *self, char *file, char *section, char *elementname, int instance);
int i_configfile_instance_count (i_resource *self, char *file, char *section, char *elementname);

