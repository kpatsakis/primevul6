gimp_channel_invalidate_boundary (GimpDrawable *drawable)
{
  GimpChannel *channel = GIMP_CHANNEL (drawable);

  channel->boundary_known = FALSE;
  channel->bounds_known   = FALSE;
}