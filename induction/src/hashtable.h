typedef struct i_hashtable_key_s
{
	void *key;
	int size;
	unsigned long hash;
} i_hashtable_key;

typedef struct i_hashtable_cell_s
{
	i_hashtable_key *key;
	void *data;
	struct i_hashtable_cell_s *next;
} i_hashtable_cell;

typedef struct i_hashtable_s
{
	unsigned long size;
	unsigned long inuse;
    i_hashtable_cell **table;
    void (*destructor) (void *);
} i_hashtable;

unsigned long i_hash_string (unsigned char *str, unsigned long htsize);
unsigned long i_hash_int (int integer, unsigned long htsize);
unsigned long i_hash_long (long value, unsigned long htsize);
unsigned long i_hash_ip ( unsigned long int srcaddr, unsigned short int srcport, 
		                  unsigned long int dstaddr, unsigned short int dstport,
		                  unsigned long htsize);

i_hashtable_key* i_hashtable_create_key (void *data, int size);
void i_hashtable_free_key (i_hashtable_key *key);
void i_hashtable_key_free (i_hashtable_key *key);
void i_hashtable_free_cell (i_hashtable_cell *cell);
void i_hashtable_free (void *htptr);
void i_hashtable_clean (i_hashtable *ht);
i_hashtable* i_hashtable_create (unsigned int size);
int i_hashtable_put (i_hashtable *table, i_hashtable_key *key, void *data);
void* i_hashtable_get (i_hashtable *table, i_hashtable_key *key);
int i_hashtable_remove (i_hashtable *table, i_hashtable_key *key);

i_hashtable_key* i_hashtable_key_resource (void *res_addr_ptr, unsigned long htsize);
i_hashtable_key* i_hashtable_create_key_int (int integer, unsigned long htsize);
i_hashtable_key* i_hashtable_key_ip (unsigned long int srcaddr, unsigned 
				short int srcport,
				unsigned long int dstaddr, 
				unsigned short int dstport,
				unsigned long htsize);
i_hashtable_key* i_hashtable_create_key_string (char *str, unsigned long htsize);
i_hashtable_key* i_hashtable_create_key_long (long value, unsigned long htsize);

void i_hashtable_iterate ();
i_hashtable_key* i_hashtable_duplicate_key (i_hashtable_key *key);

void i_hashtable_set_destructor (i_hashtable *ht, void (*destructor) (void *data));
