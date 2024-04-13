gimp_channel_set_color (GimpChannel   *channel,
                        const GimpRGB *color,
                        gboolean       push_undo)
{
  g_return_if_fail (GIMP_IS_CHANNEL (channel));
  g_return_if_fail (color != NULL);

  if (gimp_rgba_distance (&channel->color, color) > RGBA_EPSILON)
    {
      if (push_undo && gimp_item_is_attached (GIMP_ITEM (channel)))
        {
          GimpImage *image = gimp_item_get_image (GIMP_ITEM (channel));

          gimp_image_undo_push_channel_color (image, C_("undo-type", "Set Channel Color"),
                                              channel);
        }

      channel->color = *color;

      if (gimp_filter_peek_node (GIMP_FILTER (channel)))
        {
          gimp_gegl_node_set_color (channel->color_node,
                                    &channel->color);
        }

      gimp_drawable_update (GIMP_DRAWABLE (channel), 0, 0, -1, -1);

      g_signal_emit (channel, channel_signals[COLOR_CHANGED], 0);
    }
}