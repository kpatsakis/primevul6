gimp_channel_all (GimpChannel *channel,
                  gboolean     push_undo)
{
  g_return_if_fail (GIMP_IS_CHANNEL (channel));

  if (! gimp_item_is_attached (GIMP_ITEM (channel)))
    push_undo = FALSE;

  GIMP_CHANNEL_GET_CLASS (channel)->all (channel, push_undo);
}