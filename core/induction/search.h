int i_search_cache_init (i_resource *self);
int i_search_cache_insert (i_resource *self, i_entity *entity);
int i_search_cache_delete (i_resource *self, i_entity *entity);
struct i_list_s* i_search_cache_query (i_resource *self, struct i_list_s *keywords, int max_type, int flags);

#define SEARCH_ANY 0    /* Match any of the keywords -- OR */
#define SEARCH_ALL 2    /* Match all of the keywords -- AND */
#define SEARCH_REGEX 4  /* Use REGEX */  

