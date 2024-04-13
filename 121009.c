unsigned lodepng_auto_choose_color(LodePNGColorMode* mode_out,
                                   const unsigned char* image, unsigned w, unsigned h,
                                   const LodePNGColorMode* mode_in)
{
  LodePNGColorProfile prof;
  unsigned error = 0;
  unsigned i, n, palettebits, grey_ok, palette_ok;

  lodepng_color_profile_init(&prof);
  error = get_color_profile(&prof, image, w, h, mode_in);
  if(error) return error;
  mode_out->key_defined = 0;

  if(prof.key && w * h <= 16) prof.alpha = 1; /*too few pixels to justify tRNS chunk overhead*/
  grey_ok = !prof.colored && !prof.alpha; /*grey without alpha, with potentially low bits*/
  n = prof.numcolors;
  palettebits = n <= 2 ? 1 : (n <= 4 ? 2 : (n <= 16 ? 4 : 8));
  palette_ok = n <= 256 && (n * 2 < w * h) && prof.bits <= 8;
  if(w * h < n * 2) palette_ok = 0; /*don't add palette overhead if image has only a few pixels*/
  if(grey_ok && prof.bits <= palettebits) palette_ok = 0; /*grey is less overhead*/

  if(palette_ok)
  {
    unsigned char* p = prof.palette;
    lodepng_palette_clear(mode_out); /*remove potential earlier palette*/
    for(i = 0; i < prof.numcolors; i++)
    {
      error = lodepng_palette_add(mode_out, p[i * 4 + 0], p[i * 4 + 1], p[i * 4 + 2], p[i * 4 + 3]);
      if(error) break;
    }

    mode_out->colortype = LCT_PALETTE;
    mode_out->bitdepth = palettebits;

    if(mode_in->colortype == LCT_PALETTE && mode_in->palettesize >= mode_out->palettesize
        && mode_in->bitdepth == mode_out->bitdepth)
    {
      /*If input should have same palette colors, keep original to preserve its order and prevent conversion*/
      lodepng_color_mode_cleanup(mode_out);
      lodepng_color_mode_copy(mode_out, mode_in);
    }
  }
  else /*8-bit or 16-bit per channel*/
  {
    mode_out->bitdepth = prof.bits;
    mode_out->colortype = prof.alpha ? (prof.colored ? LCT_RGBA : LCT_GREY_ALPHA)
                                     : (prof.colored ? LCT_RGB : LCT_GREY);

    if(prof.key && !prof.alpha)
    {
      unsigned mask = (1u << mode_out->bitdepth) - 1u; /*profile always uses 16-bit, mask converts it*/
      mode_out->key_r = prof.key_r & mask;
      mode_out->key_g = prof.key_g & mask;
      mode_out->key_b = prof.key_b & mask;
      mode_out->key_defined = 1;
    }
  }

  return error;
}