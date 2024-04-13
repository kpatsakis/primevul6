Variant HHVM_FUNCTION(exif_thumbnail, const String& filename,
                         VRefParam width /* = null */,
                         VRefParam height /* = null */,
                         VRefParam imagetype /* = null */) {
  image_info_type ImageInfo;

  memset(&ImageInfo, 0, sizeof(ImageInfo));

  int ret = exif_read_file(&ImageInfo, filename.c_str(), 1, 0);
  if (ret==0) {
    exif_discard_imageinfo(&ImageInfo);
    return false;
  }

  if (!ImageInfo.Thumbnail.data || !ImageInfo.Thumbnail.size) {
    exif_discard_imageinfo(&ImageInfo);
    return false;
  }

  if (!ImageInfo.Thumbnail.width || !ImageInfo.Thumbnail.height) {
    exif_scan_thumbnail(&ImageInfo);
  }
  width.assignIfRef((int64_t)ImageInfo.Thumbnail.width);
  height.assignIfRef((int64_t)ImageInfo.Thumbnail.height);
  imagetype.assignIfRef(ImageInfo.Thumbnail.filetype);
  String str(ImageInfo.Thumbnail.data, ImageInfo.Thumbnail.size, CopyString);
  exif_discard_imageinfo(&ImageInfo);
  return str;
}