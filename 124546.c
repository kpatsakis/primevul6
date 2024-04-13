gimp_channel_set_show_masked (GimpChannel *channel,
                              gboolean     show_masked)
{
  g_return_if_fail (GIMP_IS_CHANNEL (channel));

  if (show_masked != channel->show_masked)
    {
      channel->show_masked = show_masked ? TRUE : FALSE;

      if (channel->invert_node)
        {
          GeglNode *source;

          source = gimp_drawable_get_source_node (GIMP_DRAWABLE (channel));

          if (channel->show_masked)
            {
              gegl_node_connect_to (source,               "output",
                                    channel->invert_node, "input");
              gegl_node_connect_to (channel->invert_node, "output",
                                    channel->mask_node,   "aux");
            }
          else
            {
              gegl_node_disconnect (channel->invert_node, "input");

              gegl_node_connect_to (source,             "output",
                                    channel->mask_node, "aux");
            }
        }

      gimp_drawable_update (GIMP_DRAWABLE (channel), 0, 0, -1, -1);
    }
}