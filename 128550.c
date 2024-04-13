static int simplestring_out_fptr(void *f, const char *text, int size)
{
   simplestring* buf = (simplestring*)f;
   if(buf) {
      simplestring_addn(buf, text, size);
   }
   return 0;
}