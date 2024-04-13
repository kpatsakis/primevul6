static unsigned hash_init(Hash* hash, unsigned windowsize)
{
  unsigned i;
  hash->head = (int*)calloc(sizeof(int), HASH_NUM_VALUES);
  hash->val = (int*)calloc(sizeof(int), windowsize);
  hash->chain = (unsigned short*)calloc(sizeof(unsigned short), windowsize);

  hash->zeros = (unsigned short*)calloc(sizeof(unsigned short), windowsize);
  hash->headz = (int*)calloc(sizeof(int), (MAX_SUPPORTED_DEFLATE_LENGTH + 1));
  hash->chainz = (unsigned short*)calloc(sizeof(unsigned short), windowsize);

  if(!hash->head || !hash->chain || !hash->val  || !hash->headz|| !hash->chainz || !hash->zeros)
  {
    return 83; /*alloc fail*/
  }

  /*initialize hash table*/
  for(i = 0; i < HASH_NUM_VALUES; i++) hash->head[i] = -1;
  for(i = 0; i < windowsize; i++) hash->val[i] = -1;
  for(i = 0; i < windowsize; i++) hash->chain[i] = i; /*same value as index indicates uninitialized*/

  for(i = 0; i <= MAX_SUPPORTED_DEFLATE_LENGTH; i++) hash->headz[i] = -1;
  for(i = 0; i < windowsize; i++) hash->chainz[i] = i; /*same value as index indicates uninitialized*/

  return 0;
}