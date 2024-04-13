Frame *FrameFactory::createFrame(const ByteVector &data, bool synchSafeInts) const
{
  return createFrame(data, static_cast<unsigned int>(synchSafeInts ? 4 : 3));
}