gimp_channel_real_all (GimpChannel *channel,
                       gboolean     push_undo)
{
  GeglColor *color;

  if (push_undo)
    gimp_channel_push_undo (channel,
                            GIMP_CHANNEL_GET_CLASS (channel)->all_desc);

  /*  clear the channel  */
  color = gegl_color_new ("#fff");
  gegl_buffer_set_color (gimp_drawable_get_buffer (GIMP_DRAWABLE (channel)),
                         NULL, color);
  g_object_unref (color);

  /*  we know the bounds  */
  channel->bounds_known = TRUE;
  channel->empty        = FALSE;
  channel->x1           = 0;
  channel->y1           = 0;
  channel->x2           = gimp_item_get_width  (GIMP_ITEM (channel));
  channel->y2           = gimp_item_get_height (GIMP_ITEM (channel));

  gimp_drawable_update (GIMP_DRAWABLE (channel), 0, 0, -1, -1);
}