gimp_channel_new_mask (GimpImage *image,
                       gint       width,
                       gint       height)
{
  GimpChannel *channel;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);

  channel =
    GIMP_CHANNEL (gimp_drawable_new (GIMP_TYPE_CHANNEL,
                                     image, _("Selection Mask"),
                                     0, 0, width, height,
                                     gimp_image_get_mask_format (image)));

  channel->show_masked = TRUE;
  channel->x2          = width;
  channel->y2          = height;

  gegl_buffer_clear (gimp_drawable_get_buffer (GIMP_DRAWABLE (channel)),
                     NULL);

  return channel;
}