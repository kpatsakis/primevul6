gimp_channel_finalize (GObject *object)
{
  GimpChannel *channel = GIMP_CHANNEL (object);

  g_clear_pointer (&channel->segs_in,  g_free);
  g_clear_pointer (&channel->segs_out, g_free);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}