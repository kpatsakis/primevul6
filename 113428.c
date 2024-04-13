bool HHVM_FUNCTION(imagesetstyle, const Resource& image, const Array& style) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  int *stylearr;
  int index;
  size_t malloc_size = sizeof(int) * style.size();
  stylearr = (int *)IM_MALLOC(malloc_size);
  CHECK_ALLOC_R(stylearr, malloc_size, false);
  index = 0;
  for (ArrayIter iter(style); iter; ++iter) {
    stylearr[index++] = cellToInt(tvToCell(iter.secondVal()));
  }
  gdImageSetStyle(im, stylearr, index);
  IM_FREE(stylearr);
  return true;
}