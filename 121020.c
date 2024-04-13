static unsigned readChunk_bKGD(LodePNGInfo* info, const unsigned char* data, size_t chunkLength)
{
  if(info->color.colortype == LCT_PALETTE)
  {
    /*error: this chunk must be 1 byte for indexed color image*/
    if(chunkLength != 1) return 43;

    info->background_defined = 1;
    info->background_r = info->background_g = info->background_b = data[0];
  }
  else if(info->color.colortype == LCT_GREY || info->color.colortype == LCT_GREY_ALPHA)
  {
    /*error: this chunk must be 2 bytes for greyscale image*/
    if(chunkLength != 2) return 44;

    info->background_defined = 1;
    info->background_r = info->background_g = info->background_b = 256u * data[0] + data[1];
  }
  else if(info->color.colortype == LCT_RGB || info->color.colortype == LCT_RGBA)
  {
    /*error: this chunk must be 6 bytes for greyscale image*/
    if(chunkLength != 6) return 45;

    info->background_defined = 1;
    info->background_r = 256u * data[0] + data[1];
    info->background_g = 256u * data[2] + data[3];
    info->background_b = 256u * data[4] + data[5];
  }

  return 0; /* OK */
}