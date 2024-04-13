unsigned char lodepng_chunk_ancillary(const unsigned char* chunk)
{
  return((chunk[4] & 32) != 0);
}