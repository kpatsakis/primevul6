gimp_channel_convert (GimpItem  *item,
                      GimpImage *dest_image,
                      GType      old_type)
{
  GimpChannel  *channel  = GIMP_CHANNEL (item);
  GimpDrawable *drawable = GIMP_DRAWABLE (item);

  if (! gimp_drawable_is_gray (drawable))
    {
      gimp_drawable_convert_type (drawable, dest_image,
                                  GIMP_GRAY,
                                  gimp_image_get_precision (dest_image),
                                  gimp_drawable_has_alpha (drawable),
                                  NULL,
                                  GEGL_DITHER_NONE, GEGL_DITHER_NONE,
                                  FALSE, NULL);
    }

  if (gimp_drawable_has_alpha (drawable))
    {
      GeglBuffer *new_buffer;
      const Babl *format;
      GimpRGB     background;

      format = gimp_drawable_get_format_without_alpha (drawable);

      new_buffer =
        gegl_buffer_new (GEGL_RECTANGLE (0, 0,
                                         gimp_item_get_width (item),
                                         gimp_item_get_height (item)),
                         format);

      gimp_rgba_set (&background, 0.0, 0.0, 0.0, 0.0);

      gimp_gegl_apply_flatten (gimp_drawable_get_buffer (drawable),
                               NULL, NULL,
                               new_buffer, &background,
                               GIMP_LAYER_COLOR_SPACE_RGB_LINEAR);

      gimp_drawable_set_buffer_full (drawable, FALSE, NULL,
                                     new_buffer,
                                     GEGL_RECTANGLE (
                                       gimp_item_get_offset_x (item),
                                       gimp_item_get_offset_y (item),
                                       0, 0),
                                     TRUE);
      g_object_unref (new_buffer);
    }

  if (G_TYPE_FROM_INSTANCE (channel) == GIMP_TYPE_CHANNEL)
    {
      gint width  = gimp_image_get_width  (dest_image);
      gint height = gimp_image_get_height (dest_image);

      gimp_item_set_offset (item, 0, 0);

      if (gimp_item_get_width  (item) != width ||
          gimp_item_get_height (item) != height)
        {
          gimp_item_resize (item, gimp_get_user_context (dest_image->gimp),
                            GIMP_FILL_TRANSPARENT,
                            width, height, 0, 0);
        }
    }

  GIMP_ITEM_CLASS (parent_class)->convert (item, dest_image, old_type);
}