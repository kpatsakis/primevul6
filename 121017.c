unsigned get_color_profile(LodePNGColorProfile* profile,
                           const unsigned char* in, unsigned w, unsigned h,
                           const LodePNGColorMode* mode)
{
  unsigned error = 0;
  size_t i;
  ColorTree tree;
  size_t numpixels = w * h;

  unsigned colored_done = lodepng_is_greyscale_type(mode) ? 1 : 0;
  unsigned alpha_done = lodepng_can_have_alpha(mode) ? 0 : 1;
  unsigned numcolors_done = 0;
  unsigned bpp = lodepng_get_bpp(mode);
  unsigned bits_done = bpp == 1 ? 1 : 0;
  unsigned maxnumcolors = 257;
  unsigned sixteen = 0;
  if(bpp <= 8) maxnumcolors = bpp == 1 ? 2 : (bpp == 2 ? 4 : (bpp == 4 ? 16 : 256));

  color_tree_init(&tree);

  /*Check if the 16-bit input is truly 16-bit*/
  if(mode->bitdepth == 16)
  {
    unsigned short r, g, b, a;
    for(i = 0; i < numpixels; i++)
    {
      getPixelColorRGBA16(&r, &g, &b, &a, in, i, mode);
      if(r % 257u != 0 || g % 257u != 0 || b % 257u != 0 || a % 257u != 0) /*first and second byte differ*/
      {
        sixteen = 1;
        break;
      }
    }
  }

  if(sixteen)
  {
    unsigned short r = 0, g = 0, b = 0, a = 0;
    profile->bits = 16;
    bits_done = numcolors_done = 1; /*counting colors no longer useful, palette doesn't support 16-bit*/

    for(i = 0; i < numpixels; i++)
    {
      getPixelColorRGBA16(&r, &g, &b, &a, in, i, mode);
      
      if(!colored_done && (r != g || r != b))
      {
        profile->colored = 1;
        colored_done = 1;
      }

      if(!alpha_done)
      {
        unsigned matchkey = (r == profile->key_r && g == profile->key_g && b == profile->key_b);
        if(a != 65535 && (a != 0 || (profile->key && !matchkey)))
        {
          profile->alpha = 1;
          alpha_done = 1;
          if(profile->bits < 8) profile->bits = 8; /*PNG has no alphachannel modes with less than 8-bit per channel*/
        }
        else if(a == 0 && !profile->alpha && !profile->key)
        {
          profile->key = 1;
          profile->key_r = r;
          profile->key_g = g;
          profile->key_b = b;
        }
        else if(a == 65535 && profile->key && matchkey)
        {
          /* Color key cannot be used if an opaque pixel also has that RGB color. */
          profile->alpha = 1;
          alpha_done = 1;
        }
      }

      if(alpha_done && numcolors_done && colored_done && bits_done) break;
    }
  }
  else /* < 16-bit */
  {
    for(i = 0; i < numpixels; i++)
    {
      unsigned char r = 0, g = 0, b = 0, a = 0;
      getPixelColorRGBA8(&r, &g, &b, &a, in, i, mode);

      if(!bits_done && profile->bits < 8)
      {
        /*only r is checked, < 8 bits is only relevant for greyscale*/
        unsigned bits = getValueRequiredBits(r);
        if(bits > profile->bits) profile->bits = bits;
      }
      bits_done = (profile->bits >= bpp);

      if(!colored_done && (r != g || r != b))
      {
        profile->colored = 1;
        colored_done = 1;
        if(profile->bits < 8) profile->bits = 8; /*PNG has no colored modes with less than 8-bit per channel*/
      }

      if(!alpha_done)
      {
        unsigned matchkey = (r == profile->key_r && g == profile->key_g && b == profile->key_b);
        if(a != 255 && (a != 0 || (profile->key && !matchkey)))
        {
          profile->alpha = 1;
          alpha_done = 1;
          if(profile->bits < 8) profile->bits = 8; /*PNG has no alphachannel modes with less than 8-bit per channel*/
        }
        else if(a == 0 && !profile->alpha && !profile->key)
        {
          profile->key = 1;
          profile->key_r = r;
          profile->key_g = g;
          profile->key_b = b;
        }
        else if(a == 255 && profile->key && matchkey)
        {
          /* Color key cannot be used if an opaque pixel also has that RGB color. */
          profile->alpha = 1;
          alpha_done = 1;
          if(profile->bits < 8) profile->bits = 8; /*PNG has no alphachannel modes with less than 8-bit per channel*/
        }
      }

      if(!numcolors_done)
      {
        if(!color_tree_has(&tree, r, g, b, a))
        {
          color_tree_add(&tree, r, g, b, a, profile->numcolors);
          if(profile->numcolors < 256)
          {
            unsigned char* p = profile->palette;
            unsigned n = profile->numcolors;
            p[n * 4 + 0] = r;
            p[n * 4 + 1] = g;
            p[n * 4 + 2] = b;
            p[n * 4 + 3] = a;
          }
          profile->numcolors++;
          numcolors_done = profile->numcolors >= maxnumcolors;
        }
      }

      if(alpha_done && numcolors_done && colored_done && bits_done) break;
    }

    /*make the profile's key always 16-bit for consistency - repeat each byte twice*/
    profile->key_r *= 257;
    profile->key_g *= 257;
    profile->key_b *= 257;
  }

  color_tree_cleanup(&tree);
  return error;
}