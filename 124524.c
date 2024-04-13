gimp_channel_feather (GimpChannel *channel,
                      gdouble      radius_x,
                      gdouble      radius_y,
                      gboolean     edge_lock,
                      gboolean     push_undo)
{
  g_return_if_fail (GIMP_IS_CHANNEL (channel));

  if (! gimp_item_is_attached (GIMP_ITEM (channel)))
    push_undo = FALSE;

  GIMP_CHANNEL_GET_CLASS (channel)->feather (channel, radius_x, radius_y,
                                             edge_lock, push_undo);
}