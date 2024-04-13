gimp_channel_pickable_iface_init (GimpPickableInterface *iface)
{
  iface->get_opacity_at = gimp_channel_get_opacity_at;
}