gimp_channel_push_undo (GimpChannel *channel,
                        const gchar *undo_desc)
{
  g_return_if_fail (GIMP_IS_CHANNEL (channel));
  g_return_if_fail (gimp_item_is_attached (GIMP_ITEM (channel)));

  gimp_image_undo_push_mask (gimp_item_get_image (GIMP_ITEM (channel)),
                             undo_desc, channel);
}