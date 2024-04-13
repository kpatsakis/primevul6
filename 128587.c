static int xml_elem_writefunc(int (*fptr)(void *data, const char *text, int size), const char *text, void *data, int len)
{
   return fptr && text ? fptr(data, text, len ? len : strlen(text)) : 0;
}