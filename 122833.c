    bool isWebPType(BasicIo& iIo, bool /*advance*/)
    {
        if (iIo.size() < 12) {
          return false;
        }
        const int32_t len = 4;
        const unsigned char RiffImageId[4] = { 'R', 'I', 'F' ,'F'};
        const unsigned char WebPImageId[4] = { 'W', 'E', 'B' ,'P'};
        byte webp[len];
        byte data[len];
        byte riff[len];
        iIo.read(riff, len);
        iIo.read(data, len);
        iIo.read(webp, len);
        bool matched_riff = (memcmp(riff, RiffImageId, len) == 0);
        bool matched_webp = (memcmp(webp, WebPImageId, len) == 0);
        iIo.seek(-12, BasicIo::cur);
        return matched_riff && matched_webp;
    }