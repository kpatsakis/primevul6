gimp_channel_bounds (GimpItem *item,
                     gdouble  *x,
                     gdouble  *y,
                     gdouble  *width,
                     gdouble  *height)
{
  GimpChannel *channel = GIMP_CHANNEL (item);

  if (! channel->bounds_known)
    {
      GeglBuffer *buffer = gimp_drawable_get_buffer (GIMP_DRAWABLE (channel));

      channel->empty = ! gimp_gegl_mask_bounds (buffer,
                                                &channel->x1,
                                                &channel->y1,
                                                &channel->x2,
                                                &channel->y2);

      channel->bounds_known = TRUE;
    }

  *x      = channel->x1;
  *y      = channel->y1;
  *width  = channel->x2 - channel->x1;
  *height = channel->y2 - channel->y1;

  return ! channel->empty;
}