static int exif_process_IFD_in_TIFF(image_info_type *ImageInfo,
                                    size_t dir_offset, int section_index) {
  int i, sn, num_entries, sub_section_index = 0;
  unsigned char *dir_entry;
  char tagname[64];
  size_t ifd_size, dir_size, entry_offset, next_offset,
         entry_length, entry_value=0, fgot;
  int entry_tag , entry_type;
  tag_table_type tag_table = exif_get_tag_table(section_index);

  if (ImageInfo->ifd_nesting_level > MAX_IFD_NESTING_LEVEL) {
    return 0;
  }

  if (ImageInfo->FileSize >= dir_offset+2) {
    sn = exif_file_sections_add(ImageInfo, M_PSEUDO, 2, nullptr);
    if (sn == -1) return 0;
    /* we do not know the order of sections */
    ImageInfo->infile->seek(dir_offset, SEEK_SET);
    String snData = ImageInfo->infile->read(2);
    memcpy(ImageInfo->file.list[sn].data, snData.c_str(), 2);
    num_entries = php_ifd_get16u(ImageInfo->file.list[sn].data,
                                 ImageInfo->motorola_intel);
    dir_size = 2/*num dir entries*/ +
               12/*length of entry*/*num_entries +
               4/* offset to next ifd (points to thumbnail or NULL)*/;
    if (ImageInfo->FileSize >= dir_offset+dir_size) {
      if (exif_file_sections_realloc(ImageInfo, sn, dir_size)) {
        return 0;
      }
      snData = ImageInfo->infile->read(dir_size-2);
      memcpy(ImageInfo->file.list[sn].data+2, snData.c_str(), dir_size-2);
      next_offset =
        php_ifd_get32u(ImageInfo->file.list[sn].data + dir_size - 4,
                       ImageInfo->motorola_intel);
      /* now we have the directory we can look how long it should be */
      ifd_size = dir_size;
      char *end = (char*)ImageInfo->file.list[sn].data + dir_size;
      for(i=0;i<num_entries;i++) {
        dir_entry = ImageInfo->file.list[sn].data+2+i*12;
        CHECK_BUFFER_R(dir_entry+4, end, 4, 0);
        entry_tag = php_ifd_get16u(dir_entry+0, ImageInfo->motorola_intel);
        entry_type = php_ifd_get16u(dir_entry+2, ImageInfo->motorola_intel);
        if (entry_type > NUM_FORMATS) {
          raise_notice("Read from TIFF: tag(0x%04X,%12s): "
                       "Illegal format code 0x%04X, switching to BYTE",
                       entry_tag,
                       exif_get_tagname(entry_tag, tagname, -12, tag_table),
                       entry_type);
          /* Since this is repeated in exif_process_IFD_TAG make it a
             notice here and make it a warning in the exif_process_IFD_TAG
             which is called elsewhere. */
          entry_type = TAG_FMT_BYTE;
        }
        entry_length =
          php_ifd_get32u(dir_entry+4, ImageInfo->motorola_intel) *
          get_php_tiff_bytes_per_format(entry_type);
        if (entry_length <= 4) {
          switch(entry_type) {
          case TAG_FMT_USHORT:
            CHECK_BUFFER_R(dir_entry+8, end, 2, 0);
            entry_value  = php_ifd_get16u(dir_entry+8,
                                          ImageInfo->motorola_intel);
            break;
          case TAG_FMT_SSHORT:
            CHECK_BUFFER_R(dir_entry+8, end, 2, 0);
            entry_value  = php_ifd_get16s(dir_entry+8,
                                          ImageInfo->motorola_intel);
            break;
          case TAG_FMT_ULONG:
            CHECK_BUFFER_R(dir_entry+8, end, 4, 0);
            entry_value  = php_ifd_get32u(dir_entry+8,
                                          ImageInfo->motorola_intel);
            break;
          case TAG_FMT_SLONG:
            CHECK_BUFFER_R(dir_entry+8, end, 4, 0);
            entry_value  = php_ifd_get32s(dir_entry+8,
                                          ImageInfo->motorola_intel);
            break;
          }
          switch(entry_tag) {
          case TAG_IMAGEWIDTH:
          case TAG_COMP_IMAGE_WIDTH:
            ImageInfo->Width  = entry_value;
            break;
          case TAG_IMAGEHEIGHT:
          case TAG_COMP_IMAGE_HEIGHT:
            ImageInfo->Height = entry_value;
            break;
          case TAG_PHOTOMETRIC_INTERPRETATION:
            switch (entry_value) {
            case PMI_BLACK_IS_ZERO:
            case PMI_WHITE_IS_ZERO:
            case PMI_TRANSPARENCY_MASK:
              ImageInfo->IsColor = 0;
              break;
            case PMI_RGB:
            case PMI_PALETTE_COLOR:
            case PMI_SEPARATED:
            case PMI_YCBCR:
            case PMI_CIELAB:
              ImageInfo->IsColor = 1;
              break;
            }
            break;
          }
        } else {
          CHECK_BUFFER_R(dir_entry+8, end, 4, 0);
          entry_offset =
            php_ifd_get32u(dir_entry+8, ImageInfo->motorola_intel);
          /* if entry needs expading ifd cache and entry is at end of
             current ifd cache. */
          /* otherwise there may be huge holes between two entries */
          if (entry_offset + entry_length > dir_offset + ifd_size &&
              entry_offset == dir_offset + ifd_size) {
            ifd_size = entry_offset + entry_length - dir_offset;
          }
        }
      }
      if (ImageInfo->FileSize >=
          dir_offset + ImageInfo->file.list[sn].size) {
        if (ifd_size > dir_size) {
          if (dir_offset + ifd_size > ImageInfo->FileSize) {
            raise_warning("Error in TIFF: filesize(x%04lX) less than "
                            "size of IFD(x%04lX + x%04lX)",
                            ImageInfo->FileSize, dir_offset, ifd_size);
            return 0;
          }
          if (exif_file_sections_realloc(ImageInfo, sn, ifd_size)) {
            return 0;
          } else {
            end = (char*)ImageInfo->file.list[sn].data + dir_size;
          }
          /* read values not stored in directory itself */
          snData = ImageInfo->infile->read(ifd_size-dir_size);
          memcpy(ImageInfo->file.list[sn].data+dir_size, snData.c_str(),
                 ifd_size-dir_size);
        }
        /* now process the tags */
        for(i=0;i<num_entries;i++) {
          dir_entry = ImageInfo->file.list[sn].data+2+i*12;
          CHECK_BUFFER_R(dir_entry+2, end, 2, 0);
          entry_tag = php_ifd_get16u(dir_entry+0, ImageInfo->motorola_intel);
          entry_type = php_ifd_get16u(dir_entry+2, ImageInfo->motorola_intel);
          if (entry_tag == TAG_EXIF_IFD_POINTER ||
              entry_tag == TAG_INTEROP_IFD_POINTER ||
              entry_tag == TAG_GPS_IFD_POINTER ||
              entry_tag == TAG_SUB_IFD) {
            switch(entry_tag) {
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
            case TAG_SUB_IFD:
              ImageInfo->sections_found |= FOUND_THUMBNAIL;
              sub_section_index = SECTION_THUMBNAIL;
              break;
            }
            CHECK_BUFFER_R(dir_entry+8, end, 4, 0);
            entry_offset =
              php_ifd_get32u(dir_entry+8, ImageInfo->motorola_intel);
            ImageInfo->ifd_nesting_level++;
            exif_process_IFD_in_TIFF(ImageInfo, entry_offset,
                                     sub_section_index);
            if (section_index!=SECTION_THUMBNAIL && entry_tag==TAG_SUB_IFD) {
              if (ImageInfo->Thumbnail.filetype != IMAGE_FILETYPE_UNKNOWN &&
                  ImageInfo->Thumbnail.size &&
                  ImageInfo->Thumbnail.offset &&
                  ImageInfo->read_thumbnail) {
                if (!ImageInfo->Thumbnail.data) {
                  ImageInfo->Thumbnail.data =
                    (char *)IM_MALLOC(ImageInfo->Thumbnail.size);
                  ImageInfo->infile->seek(ImageInfo->Thumbnail.offset,
                                          SEEK_SET);
                  String str =
                    ImageInfo->infile->read(ImageInfo->Thumbnail.size);
                  fgot = str.length();
                  if (fgot < ImageInfo->Thumbnail.size) {
                    raise_warning("Thumbnail goes IFD boundary or "
                                    "end of file reached");
                    IM_FREE(ImageInfo->Thumbnail.data);
                    ImageInfo->Thumbnail.data = nullptr;
                  } else {
                    memcpy(ImageInfo->Thumbnail.data, str.c_str(), fgot);
                    exif_thumbnail_build(ImageInfo);
                  }
                }
              }
            }
          } else {
            if (!exif_process_IFD_TAG(ImageInfo, (char*)dir_entry,
                          (char*)(ImageInfo->file.list[sn].data-dir_offset),
                          (char*)(ImageInfo->file.list[sn].data + ifd_size),
                          ifd_size, 0, section_index, 0, tag_table)) {
              return 0;
            }
          }
        }
        /* If we had a thumbnail in a SUB_IFD we have ANOTHER image in
           NEXT IFD */
        if (next_offset && section_index != SECTION_THUMBNAIL) {
          /* this should be a thumbnail IFD */
          /* the thumbnail itself is stored at Tag=StripOffsets */
          ImageInfo->ifd_nesting_level++;
          exif_process_IFD_in_TIFF(ImageInfo, next_offset,
                                   SECTION_THUMBNAIL);
          if (!ImageInfo->Thumbnail.data && ImageInfo->Thumbnail.offset &&
              ImageInfo->Thumbnail.size && ImageInfo->read_thumbnail) {
            ImageInfo->Thumbnail.data =
              (char *)IM_MALLOC(ImageInfo->Thumbnail.size);
            CHECK_ALLOC_R(ImageInfo->Thumbnail.data,
                          ImageInfo->Thumbnail.size, 0);
            ImageInfo->infile->seek(ImageInfo->Thumbnail.offset, SEEK_SET);
            String str = ImageInfo->infile->read(ImageInfo->Thumbnail.size);
            fgot = str.length();
            if (fgot < ImageInfo->Thumbnail.size) {
              raise_warning("Thumbnail goes IFD boundary or "
                              "end of file reached");
              IM_FREE(ImageInfo->Thumbnail.data);
              ImageInfo->Thumbnail.data = nullptr;
            } else {
              memcpy(ImageInfo->Thumbnail.data, str.c_str(), fgot);
              exif_thumbnail_build(ImageInfo);
            }
          }
        }
        return 1;
      } else {
        raise_warning("Error in TIFF: filesize(x%04lX) less than "
                        "size of IFD(x%04lX)",
                        ImageInfo->FileSize,
                        dir_offset+ImageInfo->file.list[sn].size);
        return 0;
      }
    } else {
      raise_warning("Error in TIFF: filesize(x%04lX) less than size "
                      "of IFD dir(x%04lX)",
                      ImageInfo->FileSize, dir_offset+dir_size);
      return 0;
    }
  } else {
    raise_warning("Error in TIFF: filesize(x%04lX) less than "
                    "start of IFD dir(x%04lX)",
                    ImageInfo->FileSize, dir_offset+2);
    return 0;
  }
}