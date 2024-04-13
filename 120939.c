unsigned lodepng_add_text(LodePNGInfo* info, const char* key, const char* str)
{
  char** new_keys = (char**)(realloc(info->text_keys, sizeof(char*) * (info->text_num + 1)));
  char** new_strings = (char**)(realloc(info->text_strings, sizeof(char*) * (info->text_num + 1)));
  if(!new_keys || !new_strings)
  {
    free(new_keys);
    free(new_strings);
    return 83; /*alloc fail*/
  }

  info->text_num++;
  info->text_keys = new_keys;
  info->text_strings = new_strings;

  string_init(&info->text_keys[info->text_num - 1]);
  string_set(&info->text_keys[info->text_num - 1], key);

  string_init(&info->text_strings[info->text_num - 1]);
  string_set(&info->text_strings[info->text_num - 1], str);

  return 0;
}