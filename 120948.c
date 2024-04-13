static void string_init(char** out)
{
  *out = NULL;
  string_resize(out, 0);
}