static unsigned addChunk_IHDR(ucvector* out, unsigned w, unsigned h,
                              LodePNGColorType colortype, unsigned bitdepth, unsigned interlace_method)
{
  unsigned error = 0;
  ucvector header;
  ucvector_init(&header);

  if (!lodepng_add32bitInt(&header, w)) /*width*/
    return 1;
  if (!lodepng_add32bitInt(&header, h)) /*height*/
    return 1;
  ucvector_push_back(&header, (unsigned char)bitdepth); /*bit depth*/
  ucvector_push_back(&header, (unsigned char)colortype); /*color type*/
  ucvector_push_back(&header, 0); /*compression method*/
  ucvector_push_back(&header, 0); /*filter method*/
  ucvector_push_back(&header, interlace_method); /*interlace method*/

  error = addChunk(out, "IHDR", header.data, header.size);
  ucvector_cleanup(&header);

  return error;
}