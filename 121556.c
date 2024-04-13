bool is_needed_for_second_block(const std::vector<unsigned char>&segment) {
    if (segment.size() <= 2) {
        return true; // don't understand this type of header
    }
    if (segment[0] != 0xff) {
        return true; // don't understand this type of header
    }
    switch (segment[1]) {
      case 0xC4: // DHT segment
      case 0xDB: // DQT segment
      case 0xDD: // DRI segment
      case 0xDA: // Start of scan
      case 0xC0:
      case 0xC1:
      case 0xC2:
        return true;
      case 0xD8:
      case 0xD9:
        dev_assert(false && "This should be filtered out by the previous loop");
        return true;
      default:
        return false;
    }
}