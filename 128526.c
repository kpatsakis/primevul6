static int file_out_fptr(void *f, const char *text, int size)
{
   fputs(text, (FILE *)f);
   return 0;
}