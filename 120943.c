static void hash_cleanup(Hash* hash)
{
  free(hash->head);
  free(hash->val);
  free(hash->chain);

  free(hash->zeros);
  free(hash->headz);
  free(hash->chainz);
}