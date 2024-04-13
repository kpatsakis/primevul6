gimp_channel_scale (GimpItem              *item,
                    gint                   new_width,
                    gint                   new_height,
                    gint                   new_offset_x,
                    gint                   new_offset_y,
                    GimpInterpolationType  interpolation_type,
                    GimpProgress          *progress)
{
  GimpChannel *channel = GIMP_CHANNEL (item);

  if (G_TYPE_FROM_INSTANCE (item) == GIMP_TYPE_CHANNEL)
    {
      new_offset_x = 0;
      new_offset_y = 0;
    }

  /*  don't waste CPU cycles scaling an empty channel  */
  if (channel->bounds_known && channel->empty)
    {
      GimpDrawable *drawable = GIMP_DRAWABLE (item);
      GeglBuffer   *new_buffer;

      new_buffer =
        gegl_buffer_new (GEGL_RECTANGLE (0, 0, new_width, new_height),
                         gimp_drawable_get_format (drawable));

      gimp_drawable_set_buffer_full (drawable,
                                     gimp_item_is_attached (item), NULL,
                                     new_buffer,
                                     GEGL_RECTANGLE (new_offset_x, new_offset_y,
                                                     0,            0),
                                     TRUE);
      g_object_unref (new_buffer);

      gimp_channel_clear (GIMP_CHANNEL (item), NULL, FALSE);
    }
  else
    {
      GIMP_ITEM_CLASS (parent_class)->scale (item, new_width, new_height,
                                             new_offset_x, new_offset_y,
                                             interpolation_type, progress);
    }
}