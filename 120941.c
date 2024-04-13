static unsigned addChunk_iTXt(ucvector* out, unsigned compressed, const char* keyword, const char* langtag,
                              const char* transkey, const char* textstring, LodePNGCompressSettings* zlibsettings)
{
  unsigned error = 0;
  ucvector data;
  size_t i, textsize = strlen(textstring);

  ucvector_init(&data);

  for(i = 0; keyword[i] != 0; i++) ucvector_push_back(&data, (unsigned char)keyword[i]);
  if(i < 1 || i > 79) return 89; /*error: invalid keyword size*/
  ucvector_push_back(&data, 0); /*null termination char*/
  ucvector_push_back(&data, compressed ? 1 : 0); /*compression flag*/
  ucvector_push_back(&data, 0); /*compression method*/
  for(i = 0; langtag[i] != 0; i++) ucvector_push_back(&data, (unsigned char)langtag[i]);
  ucvector_push_back(&data, 0); /*null termination char*/
  for(i = 0; transkey[i] != 0; i++) ucvector_push_back(&data, (unsigned char)transkey[i]);
  ucvector_push_back(&data, 0); /*null termination char*/

  if(compressed)
  {
    ucvector compressed_data;
    ucvector_init(&compressed_data);
    error = zlib_compress(&compressed_data.data, &compressed_data.size,
                          (unsigned char*)textstring, textsize, zlibsettings);
    if(!error)
    {
      for(i = 0; i < compressed_data.size; i++) ucvector_push_back(&data, compressed_data.data[i]);
    }
    ucvector_cleanup(&compressed_data);
  }
  else /*not compressed*/
  {
    for(i = 0; textstring[i] != 0; i++) ucvector_push_back(&data, (unsigned char)textstring[i]);
  }

  if(!error) error = addChunk(out, "iTXt", data.data, data.size);
  ucvector_cleanup(&data);
  return error;
}