gimp_channel_duplicate (GimpItem *item,
                        GType     new_type)
{
  GimpItem *new_item;

  g_return_val_if_fail (g_type_is_a (new_type, GIMP_TYPE_DRAWABLE), NULL);

  new_item = GIMP_ITEM_CLASS (parent_class)->duplicate (item, new_type);

  if (GIMP_IS_CHANNEL (new_item))
    {
      GimpChannel *channel     = GIMP_CHANNEL (item);
      GimpChannel *new_channel = GIMP_CHANNEL (new_item);

      new_channel->color        = channel->color;
      new_channel->show_masked  = channel->show_masked;

      /*  selection mask variables  */
      new_channel->bounds_known = channel->bounds_known;
      new_channel->empty        = channel->empty;
      new_channel->x1           = channel->x1;
      new_channel->y1           = channel->y1;
      new_channel->x2           = channel->x2;
      new_channel->y2           = channel->y2;

      if (new_type == GIMP_TYPE_CHANNEL)
        {
          /*  8-bit channel hack: make sure pixels between all sorts
           *  of channels of an image is always copied without any
           *  gamma conversion
           */
          GimpDrawable *new_drawable = GIMP_DRAWABLE (new_item);
          GimpImage    *image        = gimp_item_get_image (item);
          const Babl   *format       = gimp_image_get_channel_format (image);

          if (format != gimp_drawable_get_format (new_drawable))
            {
              GeglBuffer *new_buffer;

              new_buffer =
                gegl_buffer_new (GEGL_RECTANGLE (0, 0,
                                                 gimp_item_get_width  (new_item),
                                                 gimp_item_get_height (new_item)),
                                 format);

              gegl_buffer_set_format (new_buffer,
                                      gimp_drawable_get_format (new_drawable));
              gimp_gegl_buffer_copy (gimp_drawable_get_buffer (new_drawable),
                                     NULL, GEGL_ABYSS_NONE,
                                     new_buffer, NULL);
              gegl_buffer_set_format (new_buffer, NULL);

              gimp_drawable_set_buffer (new_drawable, FALSE, NULL, new_buffer);
              g_object_unref (new_buffer);
            }
        }
    }

  return new_item;
}