static unsigned string_resize(char** out, size_t size)
{
  char* data = (char*)realloc(*out, size + 1);
  if(data)
  {
    data[size] = 0; /*null termination char*/
    *out = data;
  }
  else
    string_cleanup(out);
 
  return data != 0;
}