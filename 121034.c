unsigned lodepng_chunk_check_crc(const unsigned char* chunk)
{
  unsigned length = lodepng_chunk_length(chunk);
  unsigned CRC = lodepng_read32bitInt(&chunk[length + 8]);
  /*the CRC is taken of the data and the 4 chunk type letters, not the length*/
  unsigned checksum = lodepng_crc32(&chunk[4], length + 4);
  if(CRC != checksum) return 1;
  else return 0;
}