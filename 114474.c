  void updateGenre(TextIdentificationFrame *frame)
  {
    StringList fields = frame->fieldList();
    StringList newfields;

    for(StringList::ConstIterator it = fields.begin(); it != fields.end(); ++it) {
      String s = *it;
      int end = s.find(")");

      if(s.startsWith("(") && end > 0) {
        // "(12)Genre"
        String text = s.substr(end + 1);
        bool ok;
        int number = s.substr(1, end - 1).toInt(&ok);
        if(ok && number >= 0 && number <= 255 && !(ID3v1::genre(number) == text))
          newfields.append(s.substr(1, end - 1));
        if(!text.isEmpty())
          newfields.append(text);
      }
      else {
        // "Genre" or "12"
        newfields.append(s);
      }
    }

    if(newfields.isEmpty())
      fields.append(String());

    frame->setText(newfields);
  }