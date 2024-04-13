static void string_set(char** out, const char* in)
{
  size_t insize = strlen(in), i = 0;
  if(string_resize(out, insize))
  {
    for(i = 0; i < insize; i++)
    {
      (*out)[i] = in[i];
    }
  }
}