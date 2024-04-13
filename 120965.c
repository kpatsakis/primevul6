static unsigned addChunk_pHYs(ucvector* out, const LodePNGInfo* info)
{
  unsigned error = 0;
  ucvector data;
  ucvector_init(&data);

  if (!lodepng_add32bitInt(&data, info->phys_x))
    return 1;
  if (!lodepng_add32bitInt(&data, info->phys_y))
    return 1;
  if (!ucvector_push_back(&data, info->phys_unit))
    return 1;

  error = addChunk(out, "pHYs", data.data, data.size);
  ucvector_cleanup(&data);

  return error;
}