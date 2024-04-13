String string_convert_cyrillic_string(const String& input, char from, char to) {
  const unsigned char *from_table, *to_table;
  unsigned char tmp;
  auto uinput = (unsigned char*)input.slice().data();
  String retString(input.size(), ReserveString);
  unsigned char *str = (unsigned char *)retString.mutableData();

  from_table = nullptr;
  to_table   = nullptr;

  switch (toupper((int)(unsigned char)from)) {
  case 'W': from_table = _cyr_win1251;  break;
  case 'A':
  case 'D': from_table = _cyr_cp866;    break;
  case 'I': from_table = _cyr_iso88595; break;
  case 'M': from_table = _cyr_mac;      break;
  case 'K':
    break;
  default:
    throw_invalid_argument("Unknown source charset: %c", from);
    break;
  }

  switch (toupper((int)(unsigned char)to)) {
  case 'W': to_table = _cyr_win1251;    break;
  case 'A':
  case 'D': to_table = _cyr_cp866;      break;
  case 'I': to_table = _cyr_iso88595;   break;
  case 'M': to_table = _cyr_mac;        break;
  case 'K':
    break;
  default:
    throw_invalid_argument("Unknown destination charset: %c", to);
    break;
  }

  for (int i = 0; i < input.size(); i++) {
    tmp = from_table == nullptr ? uinput[i] : from_table[uinput[i]];
    str[i] = to_table == nullptr ? tmp : to_table[tmp + 256];
  }
  retString.setSize(input.size());
  return retString;
}