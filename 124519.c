gimp_channel_real_clear (GimpChannel *channel,
                         const gchar *undo_desc,
                         gboolean     push_undo)
{
  GeglBuffer    *buffer;
  GeglRectangle  rect;
  GeglRectangle  aligned_rect;

  if (channel->bounds_known && channel->empty)
    return;

  if (push_undo)
    {
      if (! undo_desc)
        undo_desc = GIMP_CHANNEL_GET_CLASS (channel)->clear_desc;

      gimp_channel_push_undo (channel, undo_desc);
    }

  buffer = gimp_drawable_get_buffer (GIMP_DRAWABLE (channel));

  if (channel->bounds_known)
    {
      rect.x      = channel->x1;
      rect.y      = channel->y1;
      rect.width  = channel->x2 - channel->x1;
      rect.height = channel->y2 - channel->y1;
    }
  else
    {
      rect.x      = 0;
      rect.y      = 0;
      rect.width  = gimp_item_get_width  (GIMP_ITEM (channel));
      rect.height = gimp_item_get_height (GIMP_ITEM (channel));
    }

  gegl_rectangle_align_to_buffer (&aligned_rect, &rect, buffer,
                                  GEGL_RECTANGLE_ALIGNMENT_SUPERSET);

  gegl_buffer_clear (buffer, &aligned_rect);

  /*  we know the bounds  */
  channel->bounds_known = TRUE;
  channel->empty        = TRUE;
  channel->x1           = 0;
  channel->y1           = 0;
  channel->x2           = gimp_item_get_width  (GIMP_ITEM (channel));
  channel->y2           = gimp_item_get_height (GIMP_ITEM (channel));

  gimp_drawable_update (GIMP_DRAWABLE (channel),
                        rect.x, rect.y, rect.width, rect.height);
}