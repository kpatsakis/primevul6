gimp_channel_get_color (GimpChannel *channel,
                        GimpRGB     *color)
{
  g_return_if_fail (GIMP_IS_CHANNEL (channel));
  g_return_if_fail (color != NULL);

  *color = channel->color;
}