gimp_channel_is_empty (GimpChannel *channel)
{
  g_return_val_if_fail (GIMP_IS_CHANNEL (channel), TRUE);

  return GIMP_CHANNEL_GET_CLASS (channel)->is_empty (channel);
}