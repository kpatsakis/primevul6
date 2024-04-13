bool HHVM_FUNCTION(imagesettile, const Resource& image, const Resource& tile) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  gdImagePtr til = get_valid_image_resource(tile);
  if (!til) return false;
  gdImageSetTile(im, til);
  return true;
}