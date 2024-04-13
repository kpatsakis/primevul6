gimp_channel_real_is_empty (GimpChannel *channel)
{
  GeglBuffer *buffer;

  if (channel->bounds_known)
    return channel->empty;

  buffer = gimp_drawable_get_buffer (GIMP_DRAWABLE (channel));

  if (! gimp_gegl_mask_is_empty (buffer))
    return FALSE;

  /*  The mask is empty, meaning we can set the bounds as known  */
  g_clear_pointer (&channel->segs_in,  g_free);
  g_clear_pointer (&channel->segs_out, g_free);

  channel->empty          = TRUE;
  channel->num_segs_in    = 0;
  channel->num_segs_out   = 0;
  channel->bounds_known   = TRUE;
  channel->boundary_known = TRUE;
  channel->x1             = 0;
  channel->y1             = 0;
  channel->x2             = gimp_item_get_width  (GIMP_ITEM (channel));
  channel->y2             = gimp_item_get_height (GIMP_ITEM (channel));

  return TRUE;
}