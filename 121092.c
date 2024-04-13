static void LodePNGIText_cleanup(LodePNGInfo* info)
{
  size_t i;
  for(i = 0; i < info->itext_num; i++)
  {
    string_cleanup(&info->itext_keys[i]);
    string_cleanup(&info->itext_langtags[i]);
    string_cleanup(&info->itext_transkeys[i]);
    string_cleanup(&info->itext_strings[i]);
  }
  free(info->itext_keys);
  free(info->itext_langtags);
  free(info->itext_transkeys);
  free(info->itext_strings);
}