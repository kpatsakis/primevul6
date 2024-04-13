gimp_channel_new (GimpImage     *image,
                  gint           width,
                  gint           height,
                  const gchar   *name,
                  const GimpRGB *color)
{
  GimpChannel *channel;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);

  channel =
    GIMP_CHANNEL (gimp_drawable_new (GIMP_TYPE_CHANNEL,
                                     image, name,
                                     0, 0, width, height,
                                     gimp_image_get_channel_format (image)));

  if (color)
    channel->color = *color;

  channel->show_masked = TRUE;

  /*  selection mask variables  */
  channel->x2          = width;
  channel->y2          = height;

  return channel;
}