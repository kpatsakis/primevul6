static inline void ReversePSDString(Image *image,char *p,size_t length)
{
  char
    *q;

  if (image->endian == MSBEndian)
    return;

  q=p+length;
  for(--q; p < q; ++p, --q)
  {
    *p = *p ^ *q,
    *q = *p ^ *q,
    *p = *p ^ *q;
  }
}