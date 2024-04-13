gimp_channel_get_memsize (GimpObject *object,
                          gint64     *gui_size)
{
  GimpChannel *channel = GIMP_CHANNEL (object);

  *gui_size += channel->num_segs_in  * sizeof (GimpBoundSeg);
  *gui_size += channel->num_segs_out * sizeof (GimpBoundSeg);

  return GIMP_OBJECT_CLASS (parent_class)->get_memsize (object, gui_size);
}