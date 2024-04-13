bool string_substr_check(int len, int &f, int &l) {
  if (l < 0 && -l > len) {
    return false;
  } else if (l > len) {
    l = len;
  }

  if (f > len) {
    return false;
  } else if (f < 0 && -f > len) {
    f = 0;
  }

  if (l < 0 && (l + len - f) < 0) {
    return false;
  }

  // if "from" position is negative, count start position from the end
  if (f < 0) {
    f += len;
    if (f < 0) {
      f = 0;
    }
  }
  if (f >= len) {
    return false;
  }

  // if "length" position is negative, set it to the length
  // needed to stop that many chars from the end of the string
  if (l < 0) {
    l += len - f;
    if (l < 0) {
      l = 0;
    }
  }
  if ((unsigned int)f + (unsigned int)l > (unsigned int)len) {
    l = len - f;
  }
  return true;
}