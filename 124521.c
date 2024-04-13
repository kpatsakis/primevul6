gimp_channel_new_from_buffer (GimpImage     *image,
                              GeglBuffer    *buffer,
                              const gchar   *name,
                              const GimpRGB *color)
{
  GimpChannel *channel;
  GeglBuffer  *dest;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GEGL_IS_BUFFER (buffer), NULL);

  channel = gimp_channel_new (image,
                              gegl_buffer_get_width  (buffer),
                              gegl_buffer_get_height (buffer),
                              name, color);

  dest = gimp_drawable_get_buffer (GIMP_DRAWABLE (channel));
  gimp_gegl_buffer_copy (buffer, NULL, GEGL_ABYSS_NONE, dest, NULL);

  return channel;
}