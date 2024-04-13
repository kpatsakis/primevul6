static int exif_process_IFD_TAG(image_info_type *ImageInfo, char *dir_entry,
                                char *offset_base, char *end, size_t IFDlength,
                                size_t displacement, int section_index,
                                int ReadNextIFD, tag_table_type tag_table) {
  size_t length;
  int tag, format, components;
  char *value_ptr, tagname[64], cbuf[32], *outside=nullptr;
  size_t byte_count, offset_val, fpos, fgot;
  int64_t byte_count_signed;
  xp_field_type *tmp_xp;

  /* Protect against corrupt headers */
  if (ImageInfo->ifd_nesting_level > MAX_IFD_NESTING_LEVEL) {
    raise_warning("corrupt EXIF header: maximum directory "
                    "nesting level reached");
    return 0;
  }
  ImageInfo->ifd_nesting_level++;

  CHECK_BUFFER_R(dir_entry+4, end, 4, 0);
  tag = php_ifd_get16u(dir_entry, ImageInfo->motorola_intel);
  format = php_ifd_get16u(dir_entry+2, ImageInfo->motorola_intel);
  components = php_ifd_get32u(dir_entry+4, ImageInfo->motorola_intel);

  if (!format || format > NUM_FORMATS) {
    /* (-1) catches illegal zero case as unsigned underflows to
       positive large. */
    raise_warning("Process tag(x%04X=%s): Illegal format code 0x%04X, "
                    "suppose BYTE", tag,
                    exif_get_tagname(tag, tagname, -12, tag_table), format);
    format = TAG_FMT_BYTE;
    /*return TRUE;*/
  }

  if (components < 0) {
    raise_warning("Process tag(x%04X=%s): Illegal components(%d)",
                    tag, exif_get_tagname(tag, tagname, -12, tag_table),
                    components);
    return 1;
  }

  byte_count_signed = (int64_t)components *
                      get_php_tiff_bytes_per_format(format);

  if (byte_count_signed < 0 || (byte_count_signed > 2147483648)) {
    raise_warning("Process tag(x%04X=%s): Illegal byte_count(%ld)",
                    tag, exif_get_tagname(tag, tagname, -12, tag_table),
                    byte_count_signed);
    return 1; // ignore that field, but don't abort parsing
  }
  byte_count = (size_t)byte_count_signed;

  if (byte_count > 4) {
    CHECK_BUFFER_R(dir_entry+8, end, 4, 0);
    offset_val = php_ifd_get32u(dir_entry+8, ImageInfo->motorola_intel);
    /* If its bigger than 4 bytes, the dir entry contains an offset. */
    value_ptr = offset_base+offset_val;
    if (byte_count > IFDlength ||
        offset_val > IFDlength-byte_count ||
        value_ptr < dir_entry ||
        offset_val < (size_t)(dir_entry-offset_base)) {
      /*
      // It is important to check for IMAGE_FILETYPE_TIFF
      // JPEG does not use absolute pointers instead
      // its pointers are relative to the start
      // of the TIFF header in APP1 section.
      */
      if (byte_count > ImageInfo->FileSize ||
          offset_val>ImageInfo->FileSize-byte_count ||
          (ImageInfo->FileType!=IMAGE_FILETYPE_TIFF_II &&
           ImageInfo->FileType!=IMAGE_FILETYPE_TIFF_MM &&
           ImageInfo->FileType!=IMAGE_FILETYPE_JPEG)) {
        if (value_ptr < dir_entry) {
          /* we can read this if offset_val > 0 */
          /* some files have their values in other parts of the file */
          raise_warning("Process tag(x%04X=%s): Illegal pointer offset"
                          "(x%04lX < %04lX)", tag,
                          exif_get_tagname(tag, tagname, -12, tag_table),
                          offset_val, dir_entry-offset_base);
        } else {
          /* this is for sure not allowed */
          /* exception are IFD pointers */
          raise_warning("Process tag(x%04X=%s): Illegal pointer offset"
                          "(x%04lX + x%04lX = x%04lX > x%04lX)", tag,
                          exif_get_tagname(tag, tagname, -12, tag_table),
                          offset_val, byte_count, offset_val+byte_count,
                          IFDlength);
        }
        return 0;
      }
      if (byte_count>sizeof(cbuf)) {
        /* mark as outside range and get buffer */
        value_ptr = (char *)IM_MALLOC(byte_count);
        CHECK_ALLOC_R(value_ptr, byte_count, 0);
        outside = value_ptr;
      } else {
        /*
        // in most cases we only access a small range so
        // it is faster to use a static buffer there
        // BUT it offers also the possibility to have
        // pointers read without the need to free them
        // explicitley before returning.
        */
        memset(&cbuf, 0, sizeof(cbuf));
        value_ptr = cbuf;
      }

      fpos = ImageInfo->infile->tell();
      ImageInfo->infile->seek(displacement+offset_val, SEEK_SET);
      fgot = ImageInfo->infile->tell();
      if (fgot!=displacement+offset_val) {
        if (outside) IM_FREE(outside);
        raise_warning("Wrong file pointer: 0x%08lX != 0x%08lX",
                        fgot, displacement+offset_val);
        return 0;
      }
      String str = ImageInfo->infile->read(byte_count);
      fgot = str.length();
      memcpy(value_ptr, str.c_str(), fgot);
      ImageInfo->infile->seek(fpos, SEEK_SET);
      if (fgot<byte_count) {
        if (outside) IM_FREE(outside);
        raise_warning("Unexpected end of file reached");
        return 0;
      }
    }
  } else {
    /* 4 bytes or less and value is in the dir entry itself */
    value_ptr = dir_entry+8;
    offset_val= value_ptr-offset_base;
  }

  ImageInfo->sections_found |= FOUND_ANY_TAG;
  if (section_index==SECTION_THUMBNAIL) {
    if (!ImageInfo->Thumbnail.data) {
      switch(tag) {
        case TAG_IMAGEWIDTH:
        case TAG_COMP_IMAGE_WIDTH:
          ImageInfo->Thumbnail.width =
            exif_convert_any_to_int(value_ptr, format,
                                    ImageInfo->motorola_intel);
          break;

        case TAG_IMAGEHEIGHT:
        case TAG_COMP_IMAGE_HEIGHT:
          ImageInfo->Thumbnail.height =
            exif_convert_any_to_int(value_ptr, format,
                                    ImageInfo->motorola_intel);
          break;

        case TAG_STRIP_OFFSETS:
        case TAG_JPEG_INTERCHANGE_FORMAT:
          /* accept both formats */
          ImageInfo->Thumbnail.offset =
            exif_convert_any_to_int(value_ptr, format,
                                    ImageInfo->motorola_intel);
          break;

        case TAG_STRIP_BYTE_COUNTS:
          if (ImageInfo->FileType == IMAGE_FILETYPE_TIFF_II ||
              ImageInfo->FileType == IMAGE_FILETYPE_TIFF_MM) {
            ImageInfo->Thumbnail.filetype = ImageInfo->FileType;
          } else {
            /* motorola is easier to read */
            ImageInfo->Thumbnail.filetype = IMAGE_FILETYPE_TIFF_MM;
          }
          ImageInfo->Thumbnail.size =
            exif_convert_any_to_int(value_ptr, format,
                                    ImageInfo->motorola_intel);
          break;

        case TAG_JPEG_INTERCHANGE_FORMAT_LEN:
          if (ImageInfo->Thumbnail.filetype == IMAGE_FILETYPE_UNKNOWN) {
            ImageInfo->Thumbnail.filetype = IMAGE_FILETYPE_JPEG;
            ImageInfo->Thumbnail.size =
              exif_convert_any_to_int(value_ptr, format,
                                      ImageInfo->motorola_intel);
          }
          break;
      }
    }
  } else {
    if (section_index==SECTION_IFD0 || section_index==SECTION_EXIF)
    switch(tag) {
      case TAG_COPYRIGHT:
        /* check for "<photographer> NUL <editor> NUL" */
        if (byte_count>1 && (length=php_strnlen(value_ptr, byte_count)) > 0) {
          if (length<byte_count-1) {
            /* When there are any characters after the first NUL */
            PHP_STRDUP(ImageInfo->CopyrightPhotographer, value_ptr);
            PHP_STRNDUP(
              ImageInfo->CopyrightEditor,
              value_ptr + length + 1,
              byte_count - length - 1
            );
            if (ImageInfo->Copyright) IM_FREE(ImageInfo->Copyright);
            php_vspprintf(&ImageInfo->Copyright, 0, "%s, %s",
                          value_ptr, ImageInfo->CopyrightEditor);
            /* format = TAG_FMT_UNDEFINED; this musn't be ASCII         */
            /* but we are not supposed to change this                   */
            /* keep in mind that image_info does not store editor value */
          } else {
            PHP_STRNDUP(ImageInfo->Copyright, value_ptr, byte_count);
          }
        }
        break;

      case TAG_USERCOMMENT:
        ImageInfo->UserCommentLength =
          exif_process_user_comment(ImageInfo, &(ImageInfo->UserComment),
                                    &(ImageInfo->UserCommentEncoding),
                                    value_ptr, byte_count);
        break;

      case TAG_XP_TITLE:
      case TAG_XP_COMMENTS:
      case TAG_XP_AUTHOR:
      case TAG_XP_KEYWORDS:
      case TAG_XP_SUBJECT: {
        size_t realloc_size =
          (ImageInfo->xp_fields.count+1) * sizeof(xp_field_type);
        tmp_xp = (xp_field_type*)
          IM_REALLOC(ImageInfo->xp_fields.list, realloc_size);
        if (!tmp_xp) {
          if (outside) IM_FREE(outside);
        }
        CHECK_ALLOC_R(tmp_xp, realloc_size, 0);
        ImageInfo->sections_found |= FOUND_WINXP;
        ImageInfo->xp_fields.list = tmp_xp;
        ImageInfo->xp_fields.count++;
        exif_process_unicode(ImageInfo,
          &(ImageInfo->xp_fields.list[ImageInfo->xp_fields.count-1]),
          tag, value_ptr, byte_count);
        break;
      }
      case TAG_FNUMBER:
        /* Simplest way of expressing aperture, so I trust it the most.
           (overwrite previously computed value if there is one) */
        ImageInfo->ApertureFNumber =
          (float)exif_convert_any_format(value_ptr, format,
                                         ImageInfo->motorola_intel);
        break;

      case TAG_APERTURE:
      case TAG_MAX_APERTURE:
        /* More relevant info always comes earlier, so only use this
           field if we don't have appropriate aperture information yet. */
        if (ImageInfo->ApertureFNumber == 0) {
          ImageInfo->ApertureFNumber
            = (float)exp(exif_convert_any_format(value_ptr, format,
                           ImageInfo->motorola_intel)*log(2)*0.5);
        }
        break;

      case TAG_SHUTTERSPEED:
        /* More complicated way of expressing exposure time, so only use
           this value if we don't already have it from somewhere else.
           SHUTTERSPEED comes after EXPOSURE TIME
          */
        if (ImageInfo->ExposureTime == 0) {
          ImageInfo->ExposureTime
            = (float)(1/exp(exif_convert_any_format(value_ptr, format,
                              ImageInfo->motorola_intel)*log(2)));
        }
        break;
      case TAG_EXPOSURETIME:
        ImageInfo->ExposureTime = -1;
        break;

      case TAG_COMP_IMAGE_WIDTH:
        ImageInfo->ExifImageWidth =
          exif_convert_any_to_int(value_ptr, format,
                                  ImageInfo->motorola_intel);
        break;

      case TAG_FOCALPLANE_X_RES:
        ImageInfo->FocalplaneXRes =
          exif_convert_any_format(value_ptr, format,
                                  ImageInfo->motorola_intel);
        break;

      case TAG_SUBJECT_DISTANCE:
        /* Inidcates the distacne the autofocus camera is focused to.
           Tends to be less accurate as distance increases. */
        ImageInfo->Distance =
          (float)exif_convert_any_format(value_ptr, format,
                                         ImageInfo->motorola_intel);
        break;

      case TAG_FOCALPLANE_RESOLUTION_UNIT:
        switch((int)exif_convert_any_format(value_ptr, format,
                                            ImageInfo->motorola_intel)) {
          case 1: ImageInfo->FocalplaneUnits = 25.4; break; /* inch */
          case 2:
            /* According to the information I was using, 2 measn meters.
               But looking at the Cannon powershot's files, inches is the only
               sensible value. */
            ImageInfo->FocalplaneUnits = 25.4;
            break;

          case 3: ImageInfo->FocalplaneUnits = 10;   break;  /* centimeter */
          case 4: ImageInfo->FocalplaneUnits = 1;    break;  /* milimeter  */
          case 5: ImageInfo->FocalplaneUnits = .001; break;  /* micrometer */
        }
        break;

      case TAG_SUB_IFD:
        if (format==TAG_FMT_IFD) {
          /* If this is called we are either in a TIFFs thumbnail or
             a JPEG where we cannot handle it */
          /* TIFF thumbnail: our data structure cannot store a thumbnail
             of a thumbnail */
          /* JPEG do we have the data area and what to do with it */
          raise_notice("Skip SUB IFD");
        }
        break;

      case TAG_MAKE:
        PHP_STRNDUP(ImageInfo->make, value_ptr, byte_count);
        break;
      case TAG_MODEL:
        PHP_STRNDUP(ImageInfo->model, value_ptr, byte_count);
        break;

      case TAG_MAKER_NOTE:
        exif_process_IFD_in_MAKERNOTE(ImageInfo, value_ptr, byte_count,
                                      offset_base, IFDlength, displacement);
        break;

      case TAG_EXIF_IFD_POINTER:
      case TAG_GPS_IFD_POINTER:
      case TAG_INTEROP_IFD_POINTER:
        if (ReadNextIFD) {
          char *Subdir_start;
          int sub_section_index = 0;
          switch(tag) {
            case TAG_EXIF_IFD_POINTER:
              ImageInfo->sections_found |= FOUND_EXIF;
              sub_section_index = SECTION_EXIF;
              break;
            case TAG_GPS_IFD_POINTER:
              ImageInfo->sections_found |= FOUND_GPS;
              sub_section_index = SECTION_GPS;
              break;
            case TAG_INTEROP_IFD_POINTER:
              ImageInfo->sections_found |= FOUND_INTEROP;
              sub_section_index = SECTION_INTEROP;
              break;
          }
          CHECK_BUFFER_R(value_ptr, end, 4, 0);
          Subdir_start = offset_base +
                         php_ifd_get32u(value_ptr, ImageInfo->motorola_intel);
          if (Subdir_start < offset_base ||
              Subdir_start > offset_base+IFDlength) {
            raise_warning("Illegal IFD Pointer");
            return 0;
          }
          if (!exif_process_IFD_in_JPEG(ImageInfo, Subdir_start,
                                        offset_base, end, IFDlength,
                                        displacement, sub_section_index)) {
            return 0;
          }
        }
    }
  }
  exif_iif_add_tag(ImageInfo, section_index,
                   exif_get_tagname(tag, tagname, sizeof(tagname), tag_table),
                   tag, format, components, value_ptr);
  if (outside) IM_FREE(outside);
  return 1;
}