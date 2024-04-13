gimp_channel_real_boundary (GimpChannel         *channel,
                            const GimpBoundSeg **segs_in,
                            const GimpBoundSeg **segs_out,
                            gint                *num_segs_in,
                            gint                *num_segs_out,
                            gint                 x1,
                            gint                 y1,
                            gint                 x2,
                            gint                 y2)
{
  if (! channel->boundary_known)
    {
      gint x3, y3, x4, y4;

      /* free the out of date boundary segments */
      g_free (channel->segs_in);
      g_free (channel->segs_out);

      if (gimp_item_bounds (GIMP_ITEM (channel), &x3, &y3, &x4, &y4))
        {
          GeglBuffer    *buffer;
          GeglRectangle  rect = { x3, y3, x4, y4 };

          x4 += x3;
          y4 += y3;

          buffer = gimp_drawable_get_buffer (GIMP_DRAWABLE (channel));

          channel->segs_out = gimp_boundary_find (buffer, &rect,
                                                  babl_format ("Y float"),
                                                  GIMP_BOUNDARY_IGNORE_BOUNDS,
                                                  x1, y1, x2, y2,
                                                  GIMP_BOUNDARY_HALF_WAY,
                                                  &channel->num_segs_out);
          x1 = MAX (x1, x3);
          y1 = MAX (y1, y3);
          x2 = MIN (x2, x4);
          y2 = MIN (y2, y4);

          if (x2 > x1 && y2 > y1)
            {
              channel->segs_in = gimp_boundary_find (buffer, NULL,
                                                     babl_format ("Y float"),
                                                     GIMP_BOUNDARY_WITHIN_BOUNDS,
                                                     x1, y1, x2, y2,
                                                     GIMP_BOUNDARY_HALF_WAY,
                                                     &channel->num_segs_in);
            }
          else
            {
              channel->segs_in     = NULL;
              channel->num_segs_in = 0;
            }
        }
      else
        {
          channel->segs_in      = NULL;
          channel->segs_out     = NULL;
          channel->num_segs_in  = 0;
          channel->num_segs_out = 0;
        }

      channel->boundary_known = TRUE;
    }

  *segs_in      = channel->segs_in;
  *segs_out     = channel->segs_out;
  *num_segs_in  = channel->num_segs_in;
  *num_segs_out = channel->num_segs_out;

  return (! channel->empty);
}