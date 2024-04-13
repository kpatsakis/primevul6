unsigned lodepng_get_channels(const LodePNGColorMode* info)
{
  return getNumColorChannels(info->colortype);
}