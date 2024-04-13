gimp_channel_fill (GimpItem         *item,
                   GimpDrawable     *drawable,
                   GimpFillOptions  *fill_options,
                   gboolean          push_undo,
                   GimpProgress     *progress,
                   GError          **error)
{
  GimpChannel        *channel = GIMP_CHANNEL (item);
  const GimpBoundSeg *segs_in;
  const GimpBoundSeg *segs_out;
  gint                n_segs_in;
  gint                n_segs_out;
  gint                offset_x, offset_y;

  if (! gimp_channel_boundary (channel, &segs_in, &segs_out,
                               &n_segs_in, &n_segs_out,
                               0, 0, 0, 0))
    {
      g_set_error_literal (error, GIMP_ERROR, GIMP_FAILED,
                           _("Cannot fill empty channel."));
      return FALSE;
    }

  gimp_item_get_offset (item, &offset_x, &offset_y);

  gimp_drawable_fill_boundary (drawable,
                               fill_options,
                               segs_in, n_segs_in,
                               offset_x, offset_y,
                               push_undo);

  return TRUE;
}