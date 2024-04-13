gimp_channel_new_from_alpha (GimpImage     *image,
                             GimpDrawable  *drawable,
                             const gchar   *name,
                             const GimpRGB *color)
{
  GimpChannel *channel;
  GeglBuffer  *dest_buffer;
  gint         width;
  gint         height;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_DRAWABLE (drawable), NULL);
  g_return_val_if_fail (gimp_drawable_has_alpha (drawable), NULL);

  width  = gimp_item_get_width  (GIMP_ITEM (drawable));
  height = gimp_item_get_height (GIMP_ITEM (drawable));

  channel = gimp_channel_new (image, width, height, name, color);

  gimp_channel_clear (channel, NULL, FALSE);

  dest_buffer = gimp_drawable_get_buffer (GIMP_DRAWABLE (channel));

  gegl_buffer_set_format (dest_buffer,
                          gimp_drawable_get_component_format (drawable,
                                                              GIMP_CHANNEL_ALPHA));
  gimp_gegl_buffer_copy (gimp_drawable_get_buffer (drawable), NULL,
                         GEGL_ABYSS_NONE,
                         dest_buffer, NULL);
  gegl_buffer_set_format (dest_buffer, NULL);

  return channel;
}