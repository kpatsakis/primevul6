void lodepng_color_profile_init(LodePNGColorProfile* profile)
{
  profile->colored = 0;
  profile->key = 0;
  profile->alpha = 0;
  profile->key_r = profile->key_g = profile->key_b = 0;
  profile->numcolors = 0;
  profile->bits = 1;
}