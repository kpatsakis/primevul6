unsigned lodepng_info_copy(LodePNGInfo* dest, const LodePNGInfo* source)
{
  lodepng_info_cleanup(dest);
  *dest = *source;
  lodepng_color_mode_init(&dest->color);
  CERROR_TRY_RETURN(lodepng_color_mode_copy(&dest->color, &source->color));

#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
  CERROR_TRY_RETURN(LodePNGText_copy(dest, source));
  CERROR_TRY_RETURN(LodePNGIText_copy(dest, source));

  LodePNGUnknownChunks_init(dest);
  CERROR_TRY_RETURN(LodePNGUnknownChunks_copy(dest, source));
#endif /*LODEPNG_COMPILE_ANCILLARY_CHUNKS*/
  return 0;
}