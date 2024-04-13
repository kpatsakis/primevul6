unsigned lodepng_deflate(unsigned char** out, size_t* outsize,
                         const unsigned char* in, size_t insize,
                         const LodePNGCompressSettings* settings)
{
  unsigned error;
  ucvector v;
  ucvector_init_buffer(&v, *out, *outsize);
  error = lodepng_deflatev(&v, in, insize, settings);
  *out = v.data;
  *outsize = v.size;
  return error;
}