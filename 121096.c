void lodepng_chunk_type(char type[5], const unsigned char* chunk)
{
  unsigned i;
  for(i = 0; i < 4; i++) type[i] = (char)chunk[4 + i];
  type[4] = 0; /*null termination char*/
}