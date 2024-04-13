bool HHVM_FUNCTION(imagesetbrush,
    const Resource& image, const Resource& brush) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  gdImagePtr tile = get_valid_image_resource(brush);
  if (!tile) return false;
  gdImageSetBrush(im, tile);
  return true;
}