static void JPEGSetImageSamplingFactor(struct jpeg_decompress_struct *jpeg_info,  Image *image,ExceptionInfo *exception)
{
  char
    sampling_factor[MagickPathExtent];

  switch (jpeg_info->out_color_space)
  {
    case JCS_CMYK:
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Colorspace: CMYK");
      (void) FormatLocaleString(sampling_factor,MagickPathExtent,
        "%dx%d,%dx%d,%dx%d,%dx%d",jpeg_info->comp_info[0].h_samp_factor,
        jpeg_info->comp_info[0].v_samp_factor,
        jpeg_info->comp_info[1].h_samp_factor,
        jpeg_info->comp_info[1].v_samp_factor,
        jpeg_info->comp_info[2].h_samp_factor,
        jpeg_info->comp_info[2].v_samp_factor,
        jpeg_info->comp_info[3].h_samp_factor,
        jpeg_info->comp_info[3].v_samp_factor);
      break;
    }
    case JCS_GRAYSCALE:
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Colorspace: GRAYSCALE");
      (void) FormatLocaleString(sampling_factor,MagickPathExtent,"%dx%d",
        jpeg_info->comp_info[0].h_samp_factor,
        jpeg_info->comp_info[0].v_samp_factor);
      break;
    }
    case JCS_RGB:
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Colorspace: RGB");
      (void) FormatLocaleString(sampling_factor,MagickPathExtent,
        "%dx%d,%dx%d,%dx%d",jpeg_info->comp_info[0].h_samp_factor,
        jpeg_info->comp_info[0].v_samp_factor,
        jpeg_info->comp_info[1].h_samp_factor,
        jpeg_info->comp_info[1].v_samp_factor,
        jpeg_info->comp_info[2].h_samp_factor,
        jpeg_info->comp_info[2].v_samp_factor);
      break;
    }
    default:
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Colorspace: %d",
        jpeg_info->out_color_space);
      (void) FormatLocaleString(sampling_factor,MagickPathExtent,
        "%dx%d,%dx%d,%dx%d,%dx%d",jpeg_info->comp_info[0].h_samp_factor,
        jpeg_info->comp_info[0].v_samp_factor,
        jpeg_info->comp_info[1].h_samp_factor,
        jpeg_info->comp_info[1].v_samp_factor,
        jpeg_info->comp_info[2].h_samp_factor,
        jpeg_info->comp_info[2].v_samp_factor,
        jpeg_info->comp_info[3].h_samp_factor,
        jpeg_info->comp_info[3].v_samp_factor);
      break;
    }
  }
  (void) SetImageProperty(image,"jpeg:sampling-factor",sampling_factor,
    exception);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Sampling Factors: %s",
    sampling_factor);
}