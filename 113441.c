Variant HHVM_FUNCTION(exif_read_data,
    const String& filename, const String& sections /*="" */,
    bool arrays /*=false */, bool thumbnail /*=false */) {
  int i, ret, sections_needed=0;
  image_info_type ImageInfo;
  char tmp[64], *sections_str, *s;

  memset(&ImageInfo, 0, sizeof(ImageInfo));
  if (!sections.empty()) {
    php_vspprintf(&sections_str, 0, ",%s,", sections.c_str());

    /* sections_str DOES start with , and SPACES are NOT allowed in names */
    s = sections_str;
    while(*++s) {
      if (*s==' ') {
        *s = ',';
      }
    }
    for (i=0; i<SECTION_COUNT; i++) {
      snprintf(tmp, sizeof(tmp), ",%s,", exif_get_sectionname(i).c_str());
      if (strstr(sections_str, tmp)) {
        sections_needed |= 1<<i;
      }
    }
    if (sections_str) IM_FREE(sections_str);
  }
  ret = exif_read_file(&ImageInfo, filename, thumbnail, 0);
  sections_str = exif_get_sectionlist(ImageInfo.sections_found);
  /* do not inform about in debug*/
  ImageInfo.sections_found |= FOUND_COMPUTED|FOUND_FILE;
  if (ret==0|| (sections_needed &&
                !(sections_needed&ImageInfo.sections_found))) {
    exif_discard_imageinfo(&ImageInfo);
    if (sections_str) IM_FREE(sections_str);
    return false;
  }
  /* now we can add our information */
  exif_iif_add_str(&ImageInfo, SECTION_FILE, "FileName",
                   (char *)ImageInfo.FileName.c_str());
  exif_iif_add_int(&ImageInfo, SECTION_FILE, "FileDateTime",
                   ImageInfo.FileDateTime);
  exif_iif_add_int(&ImageInfo, SECTION_FILE, "FileSize",
                   ImageInfo.FileSize);
  exif_iif_add_int(&ImageInfo, SECTION_FILE, "FileType",
                   ImageInfo.FileType);
  exif_iif_add_str(&ImageInfo, SECTION_FILE, "MimeType",
                   (char*)php_image_type_to_mime_type(ImageInfo.FileType));
  exif_iif_add_str(&ImageInfo, SECTION_FILE, "SectionsFound",
                   sections_str ? sections_str : (char *)"NONE");

  if (ImageInfo.Width>0 &&  ImageInfo.Height>0) {
    exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "html",
                     "width=\"%d\" height=\"%d\"",
                     ImageInfo.Width, ImageInfo.Height);
    exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Height",
                     ImageInfo.Height);
    exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Width",
                     ImageInfo.Width);
  }
  exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "IsColor",
                   ImageInfo.IsColor);
  if (ImageInfo.motorola_intel != -1) {
    exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "ByteOrderMotorola",
                     ImageInfo.motorola_intel);
  }
  if (ImageInfo.FocalLength) {
    exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "FocalLength",
                     "%4.1Fmm", ImageInfo.FocalLength);
    if (ImageInfo.CCDWidth) {
      exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "35mmFocalLength",
                       "%dmm",
                       (int)(ImageInfo.FocalLength/ImageInfo.CCDWidth*35+0.5));
    }
  }
  if (ImageInfo.CCDWidth) {
    exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "CCDWidth",
                     "%dmm", (int)ImageInfo.CCDWidth);
  }
  if (ImageInfo.ExposureTime>0) {
    if (ImageInfo.ExposureTime <= 0.5) {
      exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "ExposureTime",
                       "%0.3F s (1/%d)", ImageInfo.ExposureTime,
                       (int)(0.5 + 1/ImageInfo.ExposureTime));
    } else {
      exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "ExposureTime",
                       "%0.3F s", ImageInfo.ExposureTime);
    }
  }
  if (ImageInfo.ApertureFNumber) {
    exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "ApertureFNumber",
                     "f/%.1F", ImageInfo.ApertureFNumber);
  }
  if (ImageInfo.Distance) {
    if (ImageInfo.Distance<0) {
      exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "FocusDistance",
                       "Infinite");
    } else {
      exif_iif_add_fmt(&ImageInfo, SECTION_COMPUTED, "FocusDistance",
                       "%0.2Fm", ImageInfo.Distance);
    }
  }
  if (ImageInfo.UserComment) {
    exif_iif_add_buffer(&ImageInfo, SECTION_COMPUTED, "UserComment",
                        ImageInfo.UserCommentLength, ImageInfo.UserComment);
    if (ImageInfo.UserCommentEncoding &&
        strlen(ImageInfo.UserCommentEncoding)) {
      exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "UserCommentEncoding",
                       ImageInfo.UserCommentEncoding);
    }
  }

  exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "Copyright",
                   ImageInfo.Copyright);
  exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "Copyright.Photographer",
                   ImageInfo.CopyrightPhotographer);
  exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "Copyright.Editor",
                   ImageInfo.CopyrightEditor);

  for (i=0; i<ImageInfo.xp_fields.count; i++) {
    exif_iif_add_str(&ImageInfo, SECTION_WINXP,
                     exif_get_tagname(ImageInfo.xp_fields.list[i].tag,
                     nullptr, 0, exif_get_tag_table(SECTION_WINXP)),
                     ImageInfo.xp_fields.list[i].value);
  }
  if (ImageInfo.Thumbnail.size) {
    if (thumbnail) {
      /* not exif_iif_add_str : this is a buffer */
      exif_iif_add_tag(&ImageInfo, SECTION_THUMBNAIL, "THUMBNAIL",
                       TAG_NONE, TAG_FMT_UNDEFINED, ImageInfo.Thumbnail.size,
                       ImageInfo.Thumbnail.data);
    }
    if (!ImageInfo.Thumbnail.width || !ImageInfo.Thumbnail.height) {
      /* try to evaluate if thumbnail data is present */
      exif_scan_thumbnail(&ImageInfo);
    }
    exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Thumbnail.FileType",
                     ImageInfo.Thumbnail.filetype);
    exif_iif_add_str(&ImageInfo, SECTION_COMPUTED, "Thumbnail.MimeType",
      (char*)php_image_type_to_mime_type(ImageInfo.Thumbnail.filetype));
  }
  if (ImageInfo.Thumbnail.width && ImageInfo.Thumbnail.height) {
    exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Thumbnail.Height",
                     ImageInfo.Thumbnail.height);
    exif_iif_add_int(&ImageInfo, SECTION_COMPUTED, "Thumbnail.Width",
                     ImageInfo.Thumbnail.width);
  }
  if (sections_str) IM_FREE(sections_str);

  Array retarr;
  add_assoc_image_info(retarr, arrays, &ImageInfo,
                       SECTION_FILE);
  add_assoc_image_info(retarr, true, &ImageInfo,
                       SECTION_COMPUTED);
  add_assoc_image_info(retarr, arrays, &ImageInfo,
                       SECTION_ANY_TAG);
  add_assoc_image_info(retarr, arrays, &ImageInfo,
                       SECTION_IFD0);
  add_assoc_image_info(retarr, true, &ImageInfo,
                       SECTION_THUMBNAIL);
  add_assoc_image_info(retarr, true, &ImageInfo,
                       SECTION_COMMENT);
  add_assoc_image_info(retarr, arrays, &ImageInfo,
                       SECTION_EXIF);
  add_assoc_image_info(retarr, arrays, &ImageInfo,
                       SECTION_GPS);
  add_assoc_image_info(retarr, arrays, &ImageInfo,
                       SECTION_INTEROP);
  add_assoc_image_info(retarr, arrays, &ImageInfo,
                       SECTION_FPIX);
  add_assoc_image_info(retarr, arrays, &ImageInfo,
                       SECTION_APP12);
  add_assoc_image_info(retarr, arrays, &ImageInfo,
                       SECTION_WINXP);
  add_assoc_image_info(retarr, arrays, &ImageInfo,
                       SECTION_MAKERNOTE);

  exif_discard_imageinfo(&ImageInfo);
  return retarr;
}