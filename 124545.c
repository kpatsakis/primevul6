gimp_channel_new_from_component (GimpImage       *image,
                                 GimpChannelType  type,
                                 const gchar     *name,
                                 const GimpRGB   *color)
{
  GimpChannel *channel;
  GeglBuffer  *src_buffer;
  GeglBuffer  *dest_buffer;
  gint         width;
  gint         height;
  const Babl  *format;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);

  format = gimp_image_get_component_format (image, type);

  g_return_val_if_fail (format != NULL, NULL);

  gimp_pickable_flush (GIMP_PICKABLE (image));

  src_buffer = gimp_pickable_get_buffer (GIMP_PICKABLE (image));
  width  = gegl_buffer_get_width  (src_buffer);
  height = gegl_buffer_get_height (src_buffer);

  channel = gimp_channel_new (image, width, height, name, color);

  dest_buffer = gimp_drawable_get_buffer (GIMP_DRAWABLE (channel));

  gegl_buffer_set_format (dest_buffer, format);
  gimp_gegl_buffer_copy (src_buffer, NULL, GEGL_ABYSS_NONE, dest_buffer, NULL);
  gegl_buffer_set_format (dest_buffer, NULL);

  return channel;
}