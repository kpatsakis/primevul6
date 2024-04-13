gimp_channel_resize (GimpItem     *item,
                     GimpContext  *context,
                     GimpFillType  fill_type,
                     gint          new_width,
                     gint          new_height,
                     gint          offset_x,
                     gint          offset_y)
{
  GIMP_ITEM_CLASS (parent_class)->resize (item, context, GIMP_FILL_TRANSPARENT,
                                          new_width, new_height,
                                          offset_x, offset_y);

  if (G_TYPE_FROM_INSTANCE (item) == GIMP_TYPE_CHANNEL)
    {
      gimp_item_set_offset (item, 0, 0);
    }
}