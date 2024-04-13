static void string_cleanup(char** out)
{
  free(*out);
  *out = NULL;
}