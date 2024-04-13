static void LodePNGUnknownChunks_cleanup(LodePNGInfo* info)
{
  unsigned i;
  for(i = 0; i < 3; i++) free(info->unknown_chunks_data[i]);
}