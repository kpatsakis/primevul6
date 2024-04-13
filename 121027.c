unsigned lodepng_chunk_length(const unsigned char* chunk)
{
  return lodepng_read32bitInt(&chunk[0]);
}