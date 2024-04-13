Frame *FrameFactory::createFrame(const ByteVector &data, unsigned int version) const
{
  Header tagHeader;
  tagHeader.setMajorVersion(version);
  return createFrame(data, &tagHeader);
}