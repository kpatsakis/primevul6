gimp_channel_get_show_masked (GimpChannel *channel)
{
  g_return_val_if_fail (GIMP_IS_CHANNEL (channel), FALSE);

  return channel->show_masked;
}