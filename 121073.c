void lodepng_compress_settings_init(LodePNGCompressSettings* settings)
{
  /*compress with dynamic huffman tree (not in the mathematical sense, just not the predefined one)*/
  settings->btype = 2;
  settings->use_lz77 = 1;
  settings->windowsize = DEFAULT_WINDOWSIZE;
  settings->minmatch = 3;
  settings->nicematch = 128;
  settings->lazymatching = 1;

  settings->custom_zlib = 0;
  settings->custom_deflate = 0;
  settings->custom_context = 0;
}