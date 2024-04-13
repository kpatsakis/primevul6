static unsigned readChunk_pHYs(LodePNGInfo* info, const unsigned char* data, size_t chunkLength)
{
  if(chunkLength != 9) return 74; /*invalid pHYs chunk size*/

  info->phys_defined = 1;
  info->phys_x = 16777216u * data[0] + 65536u * data[1] + 256u * data[2] + data[3];
  info->phys_y = 16777216u * data[4] + 65536u * data[5] + 256u * data[6] + data[7];
  info->phys_unit = data[8];

  return 0; /* OK */
}