static int exif_process_IFD_in_MAKERNOTE(image_info_type *ImageInfo,
                                         char * value_ptr, int value_len,
                                         char *offset_base, size_t IFDlength,
                                         size_t displacement) {
  int de, section_index = SECTION_MAKERNOTE;
  int NumDirEntries, old_motorola_intel, offset_diff;
  const maker_note_type *maker_note;
  char *dir_start;
  char *value_end = value_ptr + value_len;

  for (unsigned int i=0;
       i<=sizeof(maker_note_array)/sizeof(maker_note_type); i++) {
    if (i==sizeof(maker_note_array)/sizeof(maker_note_type))
      return 0;
    maker_note = maker_note_array+i;

    if (maker_note->make &&
        (!ImageInfo->make || strcmp(maker_note->make, ImageInfo->make))) {
      continue;
    }
    if (maker_note->model &&
        (!ImageInfo->model || strcmp(maker_note->model, ImageInfo->model))) {
      continue;
    }
    if (maker_note->id_string &&
        strncmp(maker_note->id_string, value_ptr,
                (maker_note->id_string_len < value_len ?
                 maker_note->id_string_len : value_len))) {
      continue;
    }
    break;
  }

  if (maker_note->offset >= value_len) return 0;

  dir_start = value_ptr + maker_note->offset;
  ImageInfo->sections_found |= FOUND_MAKERNOTE;

  old_motorola_intel = ImageInfo->motorola_intel;
  switch (maker_note->byte_order) {
    case MN_ORDER_INTEL:
      ImageInfo->motorola_intel = 0;
      break;
    case MN_ORDER_MOTOROLA:
      ImageInfo->motorola_intel = 1;
      break;
    default:
    case MN_ORDER_NORMAL:
      break;
  }
  if (value_end - dir_start < 2) return 0;
  NumDirEntries = php_ifd_get16u(dir_start, ImageInfo->motorola_intel);

  switch (maker_note->offset_mode) {
    case MN_OFFSET_MAKER:
      offset_base = value_ptr;
      break;
    case MN_OFFSET_GUESS:
      if (value_end - (dir_start+10) < 4) return 0;
      offset_diff = 2 + NumDirEntries*12 + 4 -
                    php_ifd_get32u(dir_start+10, ImageInfo->motorola_intel);
      if (offset_diff < 0 || offset_diff >= value_len) return 0;
      offset_base = value_ptr + offset_diff;
      break;
    default:
    case MN_OFFSET_NORMAL:
      break;
  }

  if ((2+NumDirEntries*12) > value_len) {
    raise_warning("Illegal IFD size: 2 + x%04X*12 = x%04X > x%04X",
                    NumDirEntries, 2+NumDirEntries*12, value_len);
    return 0;
  }

  for (de=0;de<NumDirEntries;de++) {
    if (!exif_process_IFD_TAG(ImageInfo, dir_start + 2 + 12 * de,
                              offset_base, value_end, IFDlength, displacement,
                              section_index, 0, maker_note->tag_table)) {
      return 0;
    }
  }
  ImageInfo->motorola_intel = old_motorola_intel;
  return 0;
}