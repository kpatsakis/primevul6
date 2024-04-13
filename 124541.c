gimp_channel_set_opacity (GimpChannel *channel,
                          gdouble      opacity,
                          gboolean     push_undo)
{
  g_return_if_fail (GIMP_IS_CHANNEL (channel));

  opacity = CLAMP (opacity, GIMP_OPACITY_TRANSPARENT, GIMP_OPACITY_OPAQUE);

  if (channel->color.a != opacity)
    {
      if (push_undo && gimp_item_is_attached (GIMP_ITEM (channel)))
        {
          GimpImage *image = gimp_item_get_image (GIMP_ITEM (channel));

          gimp_image_undo_push_channel_color (image, C_("undo-type", "Set Channel Opacity"),
                                              channel);
        }

      channel->color.a = opacity;

      if (gimp_filter_peek_node (GIMP_FILTER (channel)))
        {
          gimp_gegl_node_set_color (channel->color_node,
                                    &channel->color);
        }

      gimp_drawable_update (GIMP_DRAWABLE (channel), 0, 0, -1, -1);

      g_signal_emit (channel, channel_signals[COLOR_CHANGED], 0);
    }
}