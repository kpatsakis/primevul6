static int multipart_buffer_headers(multipart_buffer *self,
                                    header_list &header) {
  char *line;
  std::string key;
  std::string buf_value;
  std::pair<std::string, std::string> entry;

  /* didn't find boundary, abort */
  if (!find_boundary(self, self->boundary)) {
    return 0;
  }

  /* get lines of text, or CRLF_CRLF */

  while( (line = get_line(self)) && strlen(line) > 0 )
  {
    /* add header to table */
    char *value = nullptr;

    /* space in the beginning means same header */
    if (!isspace(line[0])) {
      value = strchr(line, ':');
    }

    if (value) {
      if (!buf_value.empty() && !key.empty() ) {
        entry = std::make_pair(key, buf_value);
        header.push_back(entry);
        buf_value.erase();
        key.erase();
      }
      *value = '\0';
      do { value++; } while(isspace(*value));
      key.assign(line);
      buf_value.append(value);
    } else if (!buf_value.empty() ) {
      /* If no ':' on the line, add to previous line */
      buf_value.append(line);
    } else {
      continue;
    }
  }

  if (!buf_value.empty() && !key.empty()) {
    entry = std::make_pair(key, buf_value);
    header.push_back(entry);
  }

  return 1;
}