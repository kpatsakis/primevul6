gimp_channel_boundary (GimpChannel         *channel,
                       const GimpBoundSeg **segs_in,
                       const GimpBoundSeg **segs_out,
                       gint                *num_segs_in,
                       gint                *num_segs_out,
                       gint                 x1,
                       gint                 y1,
                       gint                 x2,
                       gint                 y2)
{
  g_return_val_if_fail (GIMP_IS_CHANNEL (channel), FALSE);
  g_return_val_if_fail (segs_in != NULL, FALSE);
  g_return_val_if_fail (segs_out != NULL, FALSE);
  g_return_val_if_fail (num_segs_in != NULL, FALSE);
  g_return_val_if_fail (num_segs_out != NULL, FALSE);

  return GIMP_CHANNEL_GET_CLASS (channel)->boundary (channel,
                                                     segs_in, segs_out,
                                                     num_segs_in, num_segs_out,
                                                     x1, y1,
                                                     x2, y2);
}