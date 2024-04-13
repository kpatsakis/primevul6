string_blank (GString *string)
{
  string->str[0] = '\0';
  string->len = 0;
}