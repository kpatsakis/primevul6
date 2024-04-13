unsigned lodepng_save_file(const unsigned char* buffer, size_t buffersize, const char* filename)
{
  FILE* file;
  int ret = 0;
  file = fopen(filename, "wb" );
  if(!file) return 79;
  if (fwrite((char*)buffer , 1 , buffersize, file) != buffersize)
    ret = 91;
  fclose(file);
  return ret;
}