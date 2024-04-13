gimp_channel_is_attached (GimpItem *item)
{
  GimpImage *image = gimp_item_get_image (item);

  return (GIMP_IS_IMAGE (image) &&
          gimp_container_have (gimp_image_get_channels (image),
                               GIMP_OBJECT (item)));
}