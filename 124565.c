gimp_channel_get_parent (GimpChannel *channel)
{
  g_return_val_if_fail (GIMP_IS_CHANNEL (channel), NULL);

  return GIMP_CHANNEL (gimp_viewable_get_parent (GIMP_VIEWABLE (channel)));
}