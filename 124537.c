gimp_channel_get_node (GimpFilter *filter)
{
  GimpDrawable *drawable = GIMP_DRAWABLE (filter);
  GimpChannel  *channel  = GIMP_CHANNEL (filter);
  GeglNode     *node;
  GeglNode     *source;
  GeglNode     *mode_node;
  const Babl   *color_format;

  node = GIMP_FILTER_CLASS (parent_class)->get_node (filter);

  source = gimp_drawable_get_source_node (drawable);
  gegl_node_add_child (node, source);

  g_warn_if_fail (channel->color_node == NULL);

  if (gimp_drawable_get_linear (drawable))
    color_format = babl_format ("RGBA float");
  else
    color_format = babl_format ("R'G'B'A float");

  channel->color_node = gegl_node_new_child (node,
                                             "operation", "gegl:color",
                                             "format",    color_format,
                                             NULL);
  gimp_gegl_node_set_color (channel->color_node,
                            &channel->color);

  g_warn_if_fail (channel->mask_node == NULL);

  channel->mask_node = gegl_node_new_child (node,
                                            "operation", "gegl:opacity",
                                            NULL);
  gegl_node_connect_to (channel->color_node, "output",
                        channel->mask_node,  "input");

  g_warn_if_fail (channel->invert_node == NULL);

  channel->invert_node = gegl_node_new_child (node,
                                              "operation", "gegl:invert-linear",
                                              NULL);

  if (channel->show_masked)
    {
      gegl_node_connect_to (source,               "output",
                            channel->invert_node, "input");
      gegl_node_connect_to (channel->invert_node, "output",
                            channel->mask_node,   "aux");
    }
  else
    {
      gegl_node_connect_to (source,             "output",
                            channel->mask_node, "aux");
    }

  mode_node = gimp_drawable_get_mode_node (drawable);

  gegl_node_connect_to (channel->mask_node, "output",
                        mode_node,          "aux");

  return node;
}