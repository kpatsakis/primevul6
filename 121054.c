static unsigned readChunk_zTXt(LodePNGInfo* info, const LodePNGDecompressSettings* zlibsettings,
                               const unsigned char* data, size_t chunkLength)
{
  unsigned error = 0;
  unsigned i;

  unsigned length, string2_begin;
  char *key = 0;
  ucvector decoded;

  ucvector_init(&decoded);

  while(!error) /*not really a while loop, only used to break on error*/
  {
    for(length = 0; length < chunkLength && data[length] != 0; length++) ;
    if(length + 2 >= chunkLength) CERROR_BREAK(error, 75); /*no null termination, corrupt?*/
    if(length < 1 || length > 79) CERROR_BREAK(error, 89); /*keyword too short or long*/

    key = (char*)malloc(length + 1);
    if(!key) CERROR_BREAK(error, 83); /*alloc fail*/

    key[length] = 0;
    for(i = 0; i < length; i++) key[i] = (char)data[i];

    if(data[length + 1] != 0) CERROR_BREAK(error, 72); /*the 0 byte indicating compression must be 0*/

    string2_begin = length + 2;
    if(string2_begin > chunkLength) CERROR_BREAK(error, 75); /*no null termination, corrupt?*/

    length = chunkLength - string2_begin;
    /*will fail if zlib error, e.g. if length is too small*/
    error = zlib_decompress(&decoded.data, &decoded.size,
                            (unsigned char*)(&data[string2_begin]),
                            length, zlibsettings);
    if(error) break;
    if (!ucvector_push_back(&decoded, 0)) ERROR_BREAK(83);

    error = lodepng_add_text(info, key, (char*)decoded.data);

    break;
  }

  free(key);
  ucvector_cleanup(&decoded);

  return error;
}