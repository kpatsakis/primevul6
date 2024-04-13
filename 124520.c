gimp_channel_convert_type (GimpDrawable     *drawable,
                           GimpImage        *dest_image,
                           const Babl       *new_format,
                           GimpColorProfile *dest_profile,
                           GeglDitherMethod  layer_dither_type,
                           GeglDitherMethod  mask_dither_type,
                           gboolean          push_undo,
                           GimpProgress     *progress)
{
  GeglBuffer *dest_buffer;

  dest_buffer =
    gegl_buffer_new (GEGL_RECTANGLE (0, 0,
                                     gimp_item_get_width  (GIMP_ITEM (drawable)),
                                     gimp_item_get_height (GIMP_ITEM (drawable))),
                     new_format);

  if (mask_dither_type == GEGL_DITHER_NONE)
    {
      gimp_gegl_buffer_copy (gimp_drawable_get_buffer (drawable), NULL,
                             GEGL_ABYSS_NONE,
                             dest_buffer, NULL);
    }
  else
    {
      gint bits;

      bits = (babl_format_get_bytes_per_pixel (new_format) * 8 /
              babl_format_get_n_components (new_format));

      gimp_gegl_apply_dither (gimp_drawable_get_buffer (drawable),
                              NULL, NULL,
                              dest_buffer, 1 << bits, mask_dither_type);
    }

  gimp_drawable_set_buffer (drawable, push_undo, NULL, dest_buffer);
  g_object_unref (dest_buffer);
}