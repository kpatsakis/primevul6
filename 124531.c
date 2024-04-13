gimp_channel_set_buffer (GimpDrawable        *drawable,
                         gboolean             push_undo,
                         const gchar         *undo_desc,
                         GeglBuffer          *buffer,
                         const GeglRectangle *bounds)
{
  GimpChannel *channel    = GIMP_CHANNEL (drawable);
  GeglBuffer  *old_buffer = gimp_drawable_get_buffer (drawable);

  if (old_buffer)
    {
      g_signal_handlers_disconnect_by_func (old_buffer,
                                            gimp_channel_buffer_changed,
                                            channel);
    }

  GIMP_DRAWABLE_CLASS (parent_class)->set_buffer (drawable,
                                                  push_undo, undo_desc,
                                                  buffer, bounds);

  gegl_buffer_signal_connect (buffer, "changed",
                              G_CALLBACK (gimp_channel_buffer_changed),
                              channel);

  if (gimp_filter_peek_node (GIMP_FILTER (channel)))
    {
      const Babl *color_format;

      if (gimp_drawable_get_linear (drawable))
        color_format = babl_format ("RGBA float");
      else
        color_format = babl_format ("R'G'B'A float");

      gegl_node_set (channel->color_node,
                     "format", color_format,
                     NULL);
    }
}