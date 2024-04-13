Variant HHVM_FUNCTION(imagecolorsforindex, const Resource& image,
                                           int64_t index) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  if (index >= 0 &&
      (gdImageTrueColor(im) || index < gdImageColorsTotal(im))) {
    return make_map_array(
      s_red,  gdImageRed(im,index),
      s_green, gdImageGreen(im,index),
      s_blue, gdImageBlue(im,index),
      s_alpha, gdImageAlpha(im,index)
    );
  }
  raise_warning("Color index %" PRId64 " out of range", index);
  return false;
}