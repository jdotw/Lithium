char* i_string_to_lower (char *str);
char *i_string_web (char *str);
char* i_string_extract_word (char *str, int *offsetptr);
char* i_string_glue (char *str1, char *str2);
char* i_string_volume (double units, char *unit_str);
char* i_string_volume_metric (struct i_metric_s *met, struct i_metric_value_s *val);
char* i_string_rate (double rate, char *unit_str);
char* i_string_rate_metric (struct i_metric_s *met, struct i_metric_value_s *val);
char* i_string_divby_metric (struct i_metric_s *met, struct i_metric_value_s *val);
