Variant HHVM_FUNCTION(iptcparse, const String& iptcblock) {
  unsigned int inx = 0, len, tagsfound = 0;
  unsigned char *buffer, recnum, dataset, key[16];
  unsigned int str_len = iptcblock.length();
  Array ret;

  buffer = (unsigned char *)iptcblock.c_str();
  while (inx < str_len) { /* find 1st tag */
    if ((buffer[inx] == 0x1c) &&
        ((buffer[inx+1] == 0x01) || (buffer[inx+1] == 0x02))) {
      break;
    } else {
      inx++;
    }
  }

  while (inx < str_len) {
    if (buffer[ inx++ ] != 0x1c) {
     /* we ran against some data which does not conform to IPTC -
        stop parsing! */
      break;
    }

    if ((inx + 4) >= str_len)
      break;

    dataset = buffer[inx++];
    recnum = buffer[inx++];

    if (buffer[inx] & (unsigned char) 0x80) { /* long tag */
      if (inx + 6 >= str_len) break;

      len = (((long)buffer[inx + 2]) << 24) +
            (((long)buffer[inx + 3]) << 16) +
            (((long)buffer[inx + 4]) <<  8) +
            (((long)buffer[inx + 5]));
      inx += 6;
    } else { /* short tag */
      len = (((unsigned short)buffer[inx])<<8) |
            (unsigned short)buffer[inx+1];
      inx += 2;
    }

    snprintf((char *)key, sizeof(key), "%d#%03d",
             (unsigned int)dataset, (unsigned int)recnum);

    if ((len > str_len) || (inx + len) > str_len) {
      break;
    }

    String skey((const char *)key, CopyString);
    if (!ret.exists(skey)) {
      ret.set(skey, Array::CreateVArray());
    }
    auto const lval = ret.lvalAt(skey);
    forceToArray(lval).append(
      String((const char *)(buffer+inx), len, CopyString));
    inx += len;
    tagsfound++;
  }

  if (!tagsfound) {
    return false;
  }
  return ret;
}