Frame *FrameFactory::createFrame(const ByteVector &origData, Header *tagHeader) const
{
  ByteVector data = origData;
  unsigned int version = tagHeader->majorVersion();
  Frame::Header *header = new Frame::Header(data, version);
  ByteVector frameID = header->frameID();

  // A quick sanity check -- make sure that the frameID is 4 uppercase Latin1
  // characters.  Also make sure that there is data in the frame.

  if(frameID.size() != (version < 3 ? 3 : 4) ||
     header->frameSize() <= static_cast<unsigned int>(header->dataLengthIndicator() ? 4 : 0) ||
     header->frameSize() > data.size())
  {
    delete header;
    return 0;
  }

#ifndef NO_ITUNES_HACKS
  if(version == 3 && frameID.size() == 4 && frameID[3] == '\0') {
    // iTunes v2.3 tags store v2.2 frames - convert now
    frameID = frameID.mid(0, 3);
    header->setFrameID(frameID);
    header->setVersion(2);
    updateFrame(header);
    header->setVersion(3);
  }
#endif

  for(ByteVector::ConstIterator it = frameID.begin(); it != frameID.end(); it++) {
    if( (*it < 'A' || *it > 'Z') && (*it < '0' || *it > '9') ) {
      delete header;
      return 0;
    }
  }

  if(version > 3 && (tagHeader->unsynchronisation() || header->unsynchronisation())) {
    // Data lengths are not part of the encoded data, but since they are synch-safe
    // integers they will be never actually encoded.
    ByteVector frameData = data.mid(Frame::Header::size(version), header->frameSize());
    frameData = SynchData::decode(frameData);
    data = data.mid(0, Frame::Header::size(version)) + frameData;
  }

  // TagLib doesn't mess with encrypted frames, so just treat them
  // as unknown frames.

  if(!zlib::isAvailable() && header->compression()) {
    debug("Compressed frames are currently not supported.");
    return new UnknownFrame(data, header);
  }

  if(header->encryption()) {
    debug("Encrypted frames are currently not supported.");
    return new UnknownFrame(data, header);
  }

  if(!updateFrame(header)) {
    header->setTagAlterPreservation(true);
    return new UnknownFrame(data, header);
  }

  // updateFrame() might have updated the frame ID.

  frameID = header->frameID();

  // This is where things get necissarily nasty.  Here we determine which
  // Frame subclass (or if none is found simply an Frame) based
  // on the frame ID.  Since there are a lot of possibilities, that means
  // a lot of if blocks.

  // Text Identification (frames 4.2)

  // Apple proprietary WFED (Podcast URL), MVNM (Movement Name), MVIN (Movement Number) are in fact text frames.
  if(frameID.startsWith("T") || frameID == "WFED" || frameID == "MVNM" || frameID == "MVIN") {

    TextIdentificationFrame *f = frameID != "TXXX"
      ? new TextIdentificationFrame(data, header)
      : new UserTextIdentificationFrame(data, header);

    d->setTextEncoding(f);

    if(frameID == "TCON")
      updateGenre(f);

    return f;
  }

  // Comments (frames 4.10)

  if(frameID == "COMM") {
    CommentsFrame *f = new CommentsFrame(data, header);
    d->setTextEncoding(f);
    return f;
  }

  // Attached Picture (frames 4.14)

  if(frameID == "APIC") {
    AttachedPictureFrame *f = new AttachedPictureFrame(data, header);
    d->setTextEncoding(f);
    return f;
  }

  // ID3v2.2 Attached Picture

  if(frameID == "PIC") {
    AttachedPictureFrame *f = new AttachedPictureFrameV22(data, header);
    d->setTextEncoding(f);
    return f;
  }

  // Relative Volume Adjustment (frames 4.11)

  if(frameID == "RVA2")
    return new RelativeVolumeFrame(data, header);

  // Unique File Identifier (frames 4.1)

  if(frameID == "UFID")
    return new UniqueFileIdentifierFrame(data, header);

  // General Encapsulated Object (frames 4.15)

  if(frameID == "GEOB") {
    GeneralEncapsulatedObjectFrame *f = new GeneralEncapsulatedObjectFrame(data, header);
    d->setTextEncoding(f);
    return f;
  }

  // URL link (frames 4.3)

  if(frameID.startsWith("W")) {
    if(frameID != "WXXX") {
      return new UrlLinkFrame(data, header);
    }
    else {
      UserUrlLinkFrame *f = new UserUrlLinkFrame(data, header);
      d->setTextEncoding(f);
      return f;
    }
  }

  // Unsynchronized lyric/text transcription (frames 4.8)

  if(frameID == "USLT") {
    UnsynchronizedLyricsFrame *f = new UnsynchronizedLyricsFrame(data, header);
    if(d->useDefaultEncoding)
      f->setTextEncoding(d->defaultEncoding);
    return f;
  }

  // Synchronised lyrics/text (frames 4.9)

  if(frameID == "SYLT") {
    SynchronizedLyricsFrame *f = new SynchronizedLyricsFrame(data, header);
    if(d->useDefaultEncoding)
      f->setTextEncoding(d->defaultEncoding);
    return f;
  }

  // Event timing codes (frames 4.5)

  if(frameID == "ETCO")
    return new EventTimingCodesFrame(data, header);

  // Popularimeter (frames 4.17)

  if(frameID == "POPM")
    return new PopularimeterFrame(data, header);

  // Private (frames 4.27)

  if(frameID == "PRIV")
    return new PrivateFrame(data, header);

  // Ownership (frames 4.22)

  if(frameID == "OWNE") {
    OwnershipFrame *f = new OwnershipFrame(data, header);
    d->setTextEncoding(f);
    return f;
  }

  // Chapter (ID3v2 chapters 1.0)

  if(frameID == "CHAP")
    return new ChapterFrame(tagHeader, data, header);

  // Table of contents (ID3v2 chapters 1.0)

  if(frameID == "CTOC")
    return new TableOfContentsFrame(tagHeader, data, header);

  // Apple proprietary PCST (Podcast)

  if(frameID == "PCST")
    return new PodcastFrame(data, header);

  return new UnknownFrame(data, header);
}