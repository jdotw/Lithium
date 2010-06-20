#define NO_FORK 1

typedef struct i_construct_s
{
	int ppid;
	int pid;
    long id;
    int quiet;
	char *config_path;
	char *module_path;
	char *log_path;
	char *module_name;
	struct i_module_s *module;
} i_construct;

i_construct* i_construct_create (i_construct *clone, char *module_name);
void i_construct_free (i_construct *construct);
int i_construct_spawn ();
int i_construct_spawn_post_registration ();
