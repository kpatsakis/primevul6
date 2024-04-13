static void ucvector_cleanup(void* p)
{
  ((ucvector*)p)->size = ((ucvector*)p)->allocsize = 0;
  free(((ucvector*)p)->data);
  ((ucvector*)p)->data = NULL;
}