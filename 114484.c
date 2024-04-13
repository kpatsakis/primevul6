bool FrameFactory::updateFrame(Frame::Header *header) const
{
  const ByteVector frameID = header->frameID();

  switch(header->version()) {

  case 2: // ID3v2.2
  {
    if(frameID == "CRM" ||
       frameID == "EQU" ||
       frameID == "LNK" ||
       frameID == "RVA" ||
       frameID == "TIM" ||
       frameID == "TSI" ||
       frameID == "TDA")
    {
      debug("ID3v2.4 no longer supports the frame type " + String(frameID) +
            ".  It will be discarded from the tag.");
      return false;
    }

    // ID3v2.2 only used 3 bytes for the frame ID, so we need to convert all of
    // the frames to their 4 byte ID3v2.4 equivalent.

    for(size_t i = 0; i < frameConversion2Size; ++i) {
      if(frameID == frameConversion2[i][0]) {
        header->setFrameID(frameConversion2[i][1]);
        break;
      }
    }

    break;
  }

  case 3: // ID3v2.3
  {
    if(frameID == "EQUA" ||
       frameID == "RVAD" ||
       frameID == "TIME" ||
       frameID == "TRDA" ||
       frameID == "TSIZ" ||
       frameID == "TDAT")
    {
      debug("ID3v2.4 no longer supports the frame type " + String(frameID) +
            ".  It will be discarded from the tag.");
      return false;
    }

    for(size_t i = 0; i < frameConversion3Size; ++i) {
      if(frameID == frameConversion3[i][0]) {
        header->setFrameID(frameConversion3[i][1]);
        break;
      }
    }

    break;
  }

  default:

    // This should catch a typo that existed in TagLib up to and including
    // version 1.1 where TRDC was used for the year rather than TDRC.

    if(frameID == "TRDC")
      header->setFrameID("TDRC");

    break;
  }

  return true;
}