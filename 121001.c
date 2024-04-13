static void coin_cleanup(void* c)
{
  uivector_cleanup(&((Coin*)c)->symbols);
}