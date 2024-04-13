gimp_channel_get_opacity_at (GimpPickable *pickable,
                             gint          x,
                             gint          y)
{
  GimpChannel *channel = GIMP_CHANNEL (pickable);
  gdouble      value   = GIMP_OPACITY_TRANSPARENT;

  if (x >= 0 && x < gimp_item_get_width  (GIMP_ITEM (channel)) &&
      y >= 0 && y < gimp_item_get_height (GIMP_ITEM (channel)))
    {
      if (! channel->bounds_known ||
          (! channel->empty &&
           x >= channel->x1 &&
           x <  channel->x2 &&
           y >= channel->y1 &&
           y <  channel->y2))
        {
          gegl_buffer_sample (gimp_drawable_get_buffer (GIMP_DRAWABLE (channel)),
                              x, y, NULL, &value, babl_format ("Y double"),
                              GEGL_SAMPLER_NEAREST, GEGL_ABYSS_NONE);
        }
    }

  return value;
}