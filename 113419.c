Variant HHVM_FUNCTION(getimagesize, const String& filename,
                      VRefParam imageinfo /*=null */) {
  if (auto stream = File::Open(filename, "rb")) {
    return getImageSize(stream, imageinfo);
  }
  return false;
}