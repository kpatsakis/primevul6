gimp_channel_translate (GimpItem *item,
                        gdouble   off_x,
                        gdouble   off_y,
                        gboolean  push_undo)
{
  GimpChannel *channel = GIMP_CHANNEL (item);
  gint         x, y, width, height;

  gimp_item_bounds (GIMP_ITEM (channel), &x, &y, &width, &height);

  /*  update the old area  */
  gimp_drawable_update (GIMP_DRAWABLE (item), x, y, width, height);

  if (push_undo)
    gimp_channel_push_undo (channel, NULL);

  if (gimp_rectangle_intersect (x + SIGNED_ROUND (off_x),
                                y + SIGNED_ROUND (off_y),
                                width, height,
                                0, 0,
                                gimp_item_get_width  (GIMP_ITEM (channel)),
                                gimp_item_get_height (GIMP_ITEM (channel)),
                                &x, &y, &width, &height))
    {
      /*  copy the portion of the mask we will keep to a temporary
       *  buffer
       */
      GeglBuffer *tmp_buffer =
        gegl_buffer_new (GEGL_RECTANGLE (0, 0, width, height),
                         gimp_drawable_get_format (GIMP_DRAWABLE (channel)));

      gimp_gegl_buffer_copy (gimp_drawable_get_buffer (GIMP_DRAWABLE (channel)),
                             GEGL_RECTANGLE (x - SIGNED_ROUND (off_x),
                                             y - SIGNED_ROUND (off_y),
                                             width, height),
                             GEGL_ABYSS_NONE,
                             tmp_buffer,
                             GEGL_RECTANGLE (0, 0, 0, 0));

      /*  clear the mask  */
      gegl_buffer_clear (gimp_drawable_get_buffer (GIMP_DRAWABLE (channel)),
                         NULL);

      /*  copy the temp mask back to the mask  */
      gimp_gegl_buffer_copy (tmp_buffer, NULL, GEGL_ABYSS_NONE,
                             gimp_drawable_get_buffer (GIMP_DRAWABLE (channel)),
                             GEGL_RECTANGLE (x, y, 0, 0));

      /*  free the temporary mask  */
      g_object_unref (tmp_buffer);

      channel->x1 = x;
      channel->y1 = y;
      channel->x2 = x + width;
      channel->y2 = y + height;
    }
  else
    {
      /*  clear the mask  */
      gegl_buffer_clear (gimp_drawable_get_buffer (GIMP_DRAWABLE (channel)),
                         NULL);

      channel->empty = TRUE;
      channel->x1    = 0;
      channel->y1    = 0;
      channel->x2    = gimp_item_get_width  (GIMP_ITEM (channel));
      channel->y2    = gimp_item_get_height (GIMP_ITEM (channel));
    }

  /*  update the new area  */
  gimp_drawable_update (GIMP_DRAWABLE (item),
                        channel->x1, channel->y1,
                        channel->x2 - channel->x1,
                        channel->y2 - channel->y1);
}