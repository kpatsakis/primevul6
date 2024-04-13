unsigned lodepng_zlib_compress(unsigned char** out, size_t* outsize, const unsigned char* in,
                               size_t insize, const LodePNGCompressSettings* settings)
{
  /*initially, *out must be NULL and outsize 0, if you just give some random *out
  that's pointing to a non allocated buffer, this'll crash*/
  ucvector outv;
  size_t i;
  unsigned error;
  unsigned char* deflatedata = 0;
  size_t deflatesize = 0;

  unsigned ADLER32;
  /*zlib data: 1 byte CMF (CM+CINFO), 1 byte FLG, deflate data, 4 byte ADLER32 checksum of the Decompressed data*/
  unsigned CMF = 120; /*0b01111000: CM 8, CINFO 7. With CINFO 7, any window size up to 32768 can be used.*/
  unsigned FLEVEL = 0;
  unsigned FDICT = 0;
  unsigned CMFFLG = 256 * CMF + FDICT * 32 + FLEVEL * 64;
  unsigned FCHECK = 31 - CMFFLG % 31;
  CMFFLG += FCHECK;

  /*ucvector-controlled version of the output buffer, for dynamic array*/
  ucvector_init_buffer(&outv, *out, *outsize);

  if (!ucvector_push_back(&outv, (unsigned char)(CMFFLG / 256))) return 83;
  if (!ucvector_push_back(&outv, (unsigned char)(CMFFLG % 256))) return 83;

  error = deflate(&deflatedata, &deflatesize, in, insize, settings);

  if(!error)
  {
    ADLER32 = adler32(in, (unsigned)insize);
    for(i = 0; i < deflatesize; i++)
    {
      if (!ucvector_push_back(&outv, deflatedata[i])) return 83;
    }
    free(deflatedata);
    error = !lodepng_add32bitInt(&outv, ADLER32);
  }

  if (!error)
  {
    *out = outv.data;
    *outsize = outv.size;
  }
  else
  {
    *out = NULL;
    *outsize = 0;
    ucvector_cleanup(&outv);
  }

  return error;
}