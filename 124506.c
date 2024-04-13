gimp_channel_init (GimpChannel *channel)
{
  gimp_rgba_set (&channel->color, 0.0, 0.0, 0.0, GIMP_OPACITY_OPAQUE);

  channel->show_masked    = FALSE;

  /*  Selection mask variables  */
  channel->boundary_known = FALSE;
  channel->segs_in        = NULL;
  channel->segs_out       = NULL;
  channel->num_segs_in    = 0;
  channel->num_segs_out   = 0;
  channel->empty          = FALSE;
  channel->bounds_known   = FALSE;
  channel->x1             = 0;
  channel->y1             = 0;
  channel->x2             = 0;
  channel->y2             = 0;
}