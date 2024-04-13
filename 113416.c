static void add_assoc_image_info(Array &value, bool sub_array,
                                 image_info_type *image_info,
                                 int section_index) {
  char buffer[64], *val, *name, uname[64];
  int i, ap, l, b, idx=0, unknown=0;
  image_info_value *info_value;
  image_info_data  *info_data;
  Array tmp;
  Array *tmpi = &tmp;
  Array array;

  if (image_info->info_list[section_index].count) {
    if (!sub_array) {
      tmpi = &value;
    }

    for(i=0; i<image_info->info_list[section_index].count; i++) {
      info_data  = &image_info->info_list[section_index].list[i];
      info_value = &info_data->value;
      if (!(name = info_data->name)) {
        snprintf(uname, sizeof(uname), "%d", unknown++);
        name = uname;
      }
      if (info_data->length==0) {
        tmpi->set(String(name, CopyString), uninit_null());
      } else {
        switch (info_data->format) {
        default:
          /* Standard says more types possible but skip them...
           * but allow users to handle data if they know how to
           * So not return but use type UNDEFINED
           * return;
           */
        case TAG_FMT_BYTE:
        case TAG_FMT_SBYTE:
        case TAG_FMT_UNDEFINED:
          if (!info_value->s) {
            tmpi->set(String(name, CopyString), "");
          } else {
            tmpi->set(String(name, CopyString),
                      String(info_value->s, info_data->length, CopyString));
          }
          break;

        case TAG_FMT_STRING:
          if (!(val = info_value->s)) {
            val = "";
          }
          if (section_index==SECTION_COMMENT) {
            tmpi->set(idx++, String(val, CopyString));
          } else {
            tmpi->set(String(name, CopyString), String(val, CopyString));
          }
          break;

        case TAG_FMT_URATIONAL:
        case TAG_FMT_SRATIONAL:
        /*case TAG_FMT_BYTE:
        case TAG_FMT_SBYTE:*/
        case TAG_FMT_USHORT:
        case TAG_FMT_SSHORT:
        case TAG_FMT_SINGLE:
        case TAG_FMT_DOUBLE:
        case TAG_FMT_ULONG:
        case TAG_FMT_SLONG:
          /* now the rest, first see if it becomes an array */
          if ((l = info_data->length) > 1) {
            array.clear();
          }
          for(ap=0; ap<l; ap++) {
            if (l>1) {
              info_value = &info_data->value.list[ap];
            }
            switch (info_data->format) {
            case TAG_FMT_BYTE:
              if (l>1) {
                info_value = &info_data->value;
                for (b=0;b<l;b++) {
                  array.set(b, (int)(info_value->s[b]));
                }
                break;
              }
            case TAG_FMT_USHORT:
            case TAG_FMT_ULONG:
              if (l==1) {
                tmpi->set(String(name, CopyString), (int)info_value->u);
              } else {
                array.set(ap, (int)info_value->u);
              }
              break;

            case TAG_FMT_URATIONAL:
              snprintf(buffer, sizeof(buffer), "%i/%i",
                       info_value->ur.num, info_value->ur.den);
              if (l==1) {
                tmpi->set(String(name, CopyString),
                          String(buffer, CopyString));
              } else {
                array.set(ap, String(buffer, CopyString));
              }
              break;

            case TAG_FMT_SBYTE:
              if (l>1) {
                info_value = &info_data->value;
                for (b=0;b<l;b++) {
                  array.set(ap, (int)info_value->s[b]);
                }
                break;
              }
            case TAG_FMT_SSHORT:
            case TAG_FMT_SLONG:
              if (l==1) {
                tmpi->set(String(name, CopyString), info_value->i);
              } else {
                array.set(ap, info_value->i);
              }
              break;

            case TAG_FMT_SRATIONAL:
              snprintf(buffer, sizeof(buffer), "%i/%i",
                       info_value->sr.num, info_value->sr.den);
              if (l==1) {
                tmpi->set(String(name, CopyString),
                          String(buffer, CopyString));
              } else {
                array.set(ap, String(buffer, CopyString));
              }
              break;

            case TAG_FMT_SINGLE:
              if (l==1) {
                tmpi->set(String(name, CopyString), info_value->f);
              } else {
                array.set(ap, info_value->f);
              }
              break;

            case TAG_FMT_DOUBLE:
              if (l==1) {
                tmpi->set(String(name, CopyString), info_value->d);
              } else {
                array.set(ap, info_value->d);
              }
              break;
            }
            info_value = &info_data->value.list[ap];
          }
          if (l>1) {
            tmpi->set(String(name, CopyString), array);
          }
          break;
        }
      }
    }
    if (sub_array) {
      value.set(exif_get_sectionname(section_index), tmp);
    }
  }
}