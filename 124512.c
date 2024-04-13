gimp_channel_grow (GimpChannel *channel,
                   gint         radius_x,
                   gint         radius_y,
                   gboolean     push_undo)
{
  g_return_if_fail (GIMP_IS_CHANNEL (channel));

  if (! gimp_item_is_attached (GIMP_ITEM (channel)))
    push_undo = FALSE;

  GIMP_CHANNEL_GET_CLASS (channel)->grow (channel, radius_x, radius_y,
                                          push_undo);
}