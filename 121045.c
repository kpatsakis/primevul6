unsigned char lodepng_chunk_private(const unsigned char* chunk)
{
  return((chunk[6] & 32) != 0);
}