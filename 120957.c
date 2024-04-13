static unsigned addChunk_IDAT(ucvector* out, const unsigned char* data, size_t datasize,
                              LodePNGCompressSettings* zlibsettings)
{
  ucvector zlibdata;
  unsigned error = 0;

  /*compress with the Zlib compressor*/
  ucvector_init(&zlibdata);
  error = zlib_compress(&zlibdata.data, &zlibdata.size, data, datasize, zlibsettings);
  if(!error) error = addChunk(out, "IDAT", zlibdata.data, zlibdata.size);
  ucvector_cleanup(&zlibdata);

  return error;
}