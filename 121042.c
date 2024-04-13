unsigned char lodepng_chunk_safetocopy(const unsigned char* chunk)
{
  return((chunk[7] & 32) != 0);
}