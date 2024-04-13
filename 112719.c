Variant StringUtil::Explode(const String& input, const String& delimiter,
                            int limit /* = 0x7FFFFFFF */) {
  if (delimiter.empty()) {
    throw_invalid_argument("delimiter: (empty)");
    return false;
  }

  Array ret(Array::Create());

  if (input.empty()) {
    if (limit >= 0) {
      ret.append("");
    }
    return ret;
  }

  if (limit > 1) {
    int pos = input.find(delimiter);
    if (pos < 0) {
      ret.append(input);
    } else {
      int len = delimiter.size();
      int pos0 = 0;
      do {
        ret.append(input.substr(pos0, pos - pos0));
        pos += len;
        pos0 = pos;
      } while ((pos = input.find(delimiter, pos)) >= 0 && --limit > 1);

      if (pos0 <= input.size()) {
        ret.append(input.substr(pos0));
      }
    }
  } else if (limit < 0) {
    int pos = input.find(delimiter);
    if (pos >= 0) {
      std::vector<int> positions;
      int len = delimiter.size();
      int pos0 = 0;
      int found = 0;
      do {
        positions.push_back(pos0);
        positions.push_back(pos - pos0);
        pos += len;
        pos0 = pos;
        found++;
      } while ((pos = input.find(delimiter, pos)) >= 0);

      if (pos0 <= input.size()) {
        positions.push_back(pos0);
        positions.push_back(input.size() - pos0);
        found++;
      }
      int iMax = (found + limit) << 1;
      for (int i = 0; i < iMax; i += 2) {
        ret.append(input.substr(positions[i], positions[i+1]));
      }
    } // else we have negative limit and delimiter not found
  } else {
    ret.append(input);
  }

  return ret;
}