gimp_channel_get_opacity (GimpChannel *channel)
{
  g_return_val_if_fail (GIMP_IS_CHANNEL (channel), GIMP_OPACITY_TRANSPARENT);

  return channel->color.a;
}