gimp_channel_real_shrink (GimpChannel *channel,
                          gint         radius_x,
                          gint         radius_y,
                          gboolean     edge_lock,
                          gboolean     push_undo)
{
  gint x1, y1, x2, y2;

  if (radius_x == 0 && radius_y == 0)
    return;

  if (radius_x <= 0 && radius_y <= 0)
    {
      gimp_channel_grow (channel, -radius_x, -radius_y, push_undo);
      return;
    }

  if (radius_x < 0 || radius_y < 0)
    return;

  if (! gimp_item_bounds (GIMP_ITEM (channel), &x1, &y1, &x2, &y2))
    return;

  x2 += x1;
  y2 += y1;

  if (gimp_channel_is_empty (channel))
    return;

  if (x1 > 0)
    x1--;
  if (y1 > 0)
    y1--;
  if (x2 < gimp_item_get_width (GIMP_ITEM (channel)))
    x2++;
  if (y2 < gimp_item_get_height (GIMP_ITEM (channel)))
    y2++;

  if (push_undo)
    gimp_channel_push_undo (channel,
                            GIMP_CHANNEL_GET_CLASS (channel)->shrink_desc);

  gimp_gegl_apply_shrink (gimp_drawable_get_buffer (GIMP_DRAWABLE (channel)),
                          NULL, NULL,
                          gimp_drawable_get_buffer (GIMP_DRAWABLE (channel)),
                          GEGL_RECTANGLE (x1, y1, x2 - x1, y2 - y1),
                          radius_x, radius_y, edge_lock);

  gimp_drawable_update (GIMP_DRAWABLE (channel), 0, 0, -1, -1);
}