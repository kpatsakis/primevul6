static MagickBooleanType GetEXIFProperty(const Image *image,
  const char *property)
{
#define MaxDirectoryStack  16
#define EXIF_DELIMITER  "\n"
#define EXIF_NUM_FORMATS  12
#define EXIF_FMT_BYTE  1
#define EXIF_FMT_STRING  2
#define EXIF_FMT_USHORT  3
#define EXIF_FMT_ULONG  4
#define EXIF_FMT_URATIONAL  5
#define EXIF_FMT_SBYTE  6
#define EXIF_FMT_UNDEFINED  7
#define EXIF_FMT_SSHORT  8
#define EXIF_FMT_SLONG  9
#define EXIF_FMT_SRATIONAL  10
#define EXIF_FMT_SINGLE  11
#define EXIF_FMT_DOUBLE  12
#define TAG_EXIF_OFFSET  0x8769
#define TAG_GPS_OFFSET  0x8825
#define TAG_INTEROP_OFFSET  0xa005

#define EXIFMultipleValues(size,format,arg) \
{ \
   ssize_t \
     component; \
 \
   size_t \
     length; \
 \
   unsigned char \
     *p1; \
 \
   length=0; \
   p1=p; \
   for (component=0; component < components; component++) \
   { \
     length+=FormatLocaleString(buffer+length,MaxTextExtent-length, \
       format", ",arg); \
     if (length >= (MaxTextExtent-1)) \
       length=MaxTextExtent-1; \
     p1+=size; \
   } \
   if (length > 1) \
     buffer[length-2]='\0'; \
   value=AcquireString(buffer); \
}

#define EXIFMultipleFractions(size,format,arg1,arg2) \
{ \
   ssize_t \
     component; \
 \
   size_t \
     length; \
 \
   unsigned char \
     *p1; \
 \
   length=0; \
   p1=p; \
   for (component=0; component < components; component++) \
   { \
     length+=FormatLocaleString(buffer+length,MaxTextExtent-length, \
       format", ",(arg1),(arg2)); \
     if (length >= (MaxTextExtent-1)) \
       length=MaxTextExtent-1; \
     p1+=size; \
   } \
   if (length > 1) \
     buffer[length-2]='\0'; \
   value=AcquireString(buffer); \
}

  typedef struct _DirectoryInfo
  {
    const unsigned char
      *directory;

    size_t
      entry;

    ssize_t
      offset;
  } DirectoryInfo;

  typedef struct _TagInfo
  {
    size_t
      tag;

    const char
      description[36];
  } TagInfo;

  static const TagInfo
    EXIFTag[] =
    {
      {  0x001, "exif:InteroperabilityIndex" },
      {  0x002, "exif:InteroperabilityVersion" },
      {  0x100, "exif:ImageWidth" },
      {  0x101, "exif:ImageLength" },
      {  0x102, "exif:BitsPerSample" },
      {  0x103, "exif:Compression" },
      {  0x106, "exif:PhotometricInterpretation" },
      {  0x10a, "exif:FillOrder" },
      {  0x10d, "exif:DocumentName" },
      {  0x10e, "exif:ImageDescription" },
      {  0x10f, "exif:Make" },
      {  0x110, "exif:Model" },
      {  0x111, "exif:StripOffsets" },
      {  0x112, "exif:Orientation" },
      {  0x115, "exif:SamplesPerPixel" },
      {  0x116, "exif:RowsPerStrip" },
      {  0x117, "exif:StripByteCounts" },
      {  0x11a, "exif:XResolution" },
      {  0x11b, "exif:YResolution" },
      {  0x11c, "exif:PlanarConfiguration" },
      {  0x11d, "exif:PageName" },
      {  0x11e, "exif:XPosition" },
      {  0x11f, "exif:YPosition" },
      {  0x118, "exif:MinSampleValue" },
      {  0x119, "exif:MaxSampleValue" },
      {  0x120, "exif:FreeOffsets" },
      {  0x121, "exif:FreeByteCounts" },
      {  0x122, "exif:GrayResponseUnit" },
      {  0x123, "exif:GrayResponseCurve" },
      {  0x124, "exif:T4Options" },
      {  0x125, "exif:T6Options" },
      {  0x128, "exif:ResolutionUnit" },
      {  0x12d, "exif:TransferFunction" },
      {  0x131, "exif:Software" },
      {  0x132, "exif:DateTime" },
      {  0x13b, "exif:Artist" },
      {  0x13e, "exif:WhitePoint" },
      {  0x13f, "exif:PrimaryChromaticities" },
      {  0x140, "exif:ColorMap" },
      {  0x141, "exif:HalfToneHints" },
      {  0x142, "exif:TileWidth" },
      {  0x143, "exif:TileLength" },
      {  0x144, "exif:TileOffsets" },
      {  0x145, "exif:TileByteCounts" },
      {  0x14a, "exif:SubIFD" },
      {  0x14c, "exif:InkSet" },
      {  0x14d, "exif:InkNames" },
      {  0x14e, "exif:NumberOfInks" },
      {  0x150, "exif:DotRange" },
      {  0x151, "exif:TargetPrinter" },
      {  0x152, "exif:ExtraSample" },
      {  0x153, "exif:SampleFormat" },
      {  0x154, "exif:SMinSampleValue" },
      {  0x155, "exif:SMaxSampleValue" },
      {  0x156, "exif:TransferRange" },
      {  0x157, "exif:ClipPath" },
      {  0x158, "exif:XClipPathUnits" },
      {  0x159, "exif:YClipPathUnits" },
      {  0x15a, "exif:Indexed" },
      {  0x15b, "exif:JPEGTables" },
      {  0x15f, "exif:OPIProxy" },
      {  0x200, "exif:JPEGProc" },
      {  0x201, "exif:JPEGInterchangeFormat" },
      {  0x202, "exif:JPEGInterchangeFormatLength" },
      {  0x203, "exif:JPEGRestartInterval" },
      {  0x205, "exif:JPEGLosslessPredictors" },
      {  0x206, "exif:JPEGPointTransforms" },
      {  0x207, "exif:JPEGQTables" },
      {  0x208, "exif:JPEGDCTables" },
      {  0x209, "exif:JPEGACTables" },
      {  0x211, "exif:YCbCrCoefficients" },
      {  0x212, "exif:YCbCrSubSampling" },
      {  0x213, "exif:YCbCrPositioning" },
      {  0x214, "exif:ReferenceBlackWhite" },
      {  0x2bc, "exif:ExtensibleMetadataPlatform" },
      {  0x301, "exif:Gamma" },
      {  0x302, "exif:ICCProfileDescriptor" },
      {  0x303, "exif:SRGBRenderingIntent" },
      {  0x320, "exif:ImageTitle" },
      {  0x5001, "exif:ResolutionXUnit" },
      {  0x5002, "exif:ResolutionYUnit" },
      {  0x5003, "exif:ResolutionXLengthUnit" },
      {  0x5004, "exif:ResolutionYLengthUnit" },
      {  0x5005, "exif:PrintFlags" },
      {  0x5006, "exif:PrintFlagsVersion" },
      {  0x5007, "exif:PrintFlagsCrop" },
      {  0x5008, "exif:PrintFlagsBleedWidth" },
      {  0x5009, "exif:PrintFlagsBleedWidthScale" },
      {  0x500A, "exif:HalftoneLPI" },
      {  0x500B, "exif:HalftoneLPIUnit" },
      {  0x500C, "exif:HalftoneDegree" },
      {  0x500D, "exif:HalftoneShape" },
      {  0x500E, "exif:HalftoneMisc" },
      {  0x500F, "exif:HalftoneScreen" },
      {  0x5010, "exif:JPEGQuality" },
      {  0x5011, "exif:GridSize" },
      {  0x5012, "exif:ThumbnailFormat" },
      {  0x5013, "exif:ThumbnailWidth" },
      {  0x5014, "exif:ThumbnailHeight" },
      {  0x5015, "exif:ThumbnailColorDepth" },
      {  0x5016, "exif:ThumbnailPlanes" },
      {  0x5017, "exif:ThumbnailRawBytes" },
      {  0x5018, "exif:ThumbnailSize" },
      {  0x5019, "exif:ThumbnailCompressedSize" },
      {  0x501a, "exif:ColorTransferFunction" },
      {  0x501b, "exif:ThumbnailData" },
      {  0x5020, "exif:ThumbnailImageWidth" },
      {  0x5021, "exif:ThumbnailImageHeight" },
      {  0x5022, "exif:ThumbnailBitsPerSample" },
      {  0x5023, "exif:ThumbnailCompression" },
      {  0x5024, "exif:ThumbnailPhotometricInterp" },
      {  0x5025, "exif:ThumbnailImageDescription" },
      {  0x5026, "exif:ThumbnailEquipMake" },
      {  0x5027, "exif:ThumbnailEquipModel" },
      {  0x5028, "exif:ThumbnailStripOffsets" },
      {  0x5029, "exif:ThumbnailOrientation" },
      {  0x502a, "exif:ThumbnailSamplesPerPixel" },
      {  0x502b, "exif:ThumbnailRowsPerStrip" },
      {  0x502c, "exif:ThumbnailStripBytesCount" },
      {  0x502d, "exif:ThumbnailResolutionX" },
      {  0x502e, "exif:ThumbnailResolutionY" },
      {  0x502f, "exif:ThumbnailPlanarConfig" },
      {  0x5030, "exif:ThumbnailResolutionUnit" },
      {  0x5031, "exif:ThumbnailTransferFunction" },
      {  0x5032, "exif:ThumbnailSoftwareUsed" },
      {  0x5033, "exif:ThumbnailDateTime" },
      {  0x5034, "exif:ThumbnailArtist" },
      {  0x5035, "exif:ThumbnailWhitePoint" },
      {  0x5036, "exif:ThumbnailPrimaryChromaticities" },
      {  0x5037, "exif:ThumbnailYCbCrCoefficients" },
      {  0x5038, "exif:ThumbnailYCbCrSubsampling" },
      {  0x5039, "exif:ThumbnailYCbCrPositioning" },
      {  0x503A, "exif:ThumbnailRefBlackWhite" },
      {  0x503B, "exif:ThumbnailCopyRight" },
      {  0x5090, "exif:LuminanceTable" },
      {  0x5091, "exif:ChrominanceTable" },
      {  0x5100, "exif:FrameDelay" },
      {  0x5101, "exif:LoopCount" },
      {  0x5110, "exif:PixelUnit" },
      {  0x5111, "exif:PixelPerUnitX" },
      {  0x5112, "exif:PixelPerUnitY" },
      {  0x5113, "exif:PaletteHistogram" },
      {  0x1000, "exif:RelatedImageFileFormat" },
      {  0x1001, "exif:RelatedImageLength" },
      {  0x1002, "exif:RelatedImageWidth" },
      {  0x800d, "exif:ImageID" },
      {  0x80e3, "exif:Matteing" },
      {  0x80e4, "exif:DataType" },
      {  0x80e5, "exif:ImageDepth" },
      {  0x80e6, "exif:TileDepth" },
      {  0x828d, "exif:CFARepeatPatternDim" },
      {  0x828e, "exif:CFAPattern2" },
      {  0x828f, "exif:BatteryLevel" },
      {  0x8298, "exif:Copyright" },
      {  0x829a, "exif:ExposureTime" },
      {  0x829d, "exif:FNumber" },
      {  0x83bb, "exif:IPTC/NAA" },
      {  0x84e3, "exif:IT8RasterPadding" },
      {  0x84e5, "exif:IT8ColorTable" },
      {  0x8649, "exif:ImageResourceInformation" },
      {  0x8769, "exif:ExifOffset" },  /* specs as "Exif IFD Pointer"? */
      {  0x8773, "exif:InterColorProfile" },
      {  0x8822, "exif:ExposureProgram" },
      {  0x8824, "exif:SpectralSensitivity" },
      {  0x8825, "exif:GPSInfo" }, /* specs as "GPSInfo IFD Pointer"? */
      {  0x8827, "exif:PhotographicSensitivity" },
      {  0x8828, "exif:OECF" },
      {  0x8829, "exif:Interlace" },      
      {  0x882a, "exif:TimeZoneOffset" },
      {  0x882b, "exif:SelfTimerMode" },
      {  0x8830, "exif:SensitivityType" },
      {  0x8831, "exif:StandardOutputSensitivity" },
      {  0x8832, "exif:RecommendedExposureIndex" },
      {  0x8833, "exif:ISOSpeed" },
      {  0x8834, "exif:ISOSpeedLatitudeyyy" },
      {  0x8835, "exif:ISOSpeedLatitudezzz" },
      {  0x9000, "exif:ExifVersion" },
      {  0x9003, "exif:DateTimeOriginal" },
      {  0x9004, "exif:DateTimeDigitized" },
      {  0x9010, "exif:OffsetTime" },
      {  0x9011, "exif:OffsetTimeOriginal" },
      {  0x9012, "exif:OffsetTimeDigitized" },
      {  0x9101, "exif:ComponentsConfiguration" },
      {  0x9102, "exif:CompressedBitsPerPixel" },
      {  0x9201, "exif:ShutterSpeedValue" },
      {  0x9202, "exif:ApertureValue" },
      {  0x9203, "exif:BrightnessValue" },
      {  0x9204, "exif:ExposureBiasValue" },
      {  0x9205, "exif:MaxApertureValue" },
      {  0x9206, "exif:SubjectDistance" },
      {  0x9207, "exif:MeteringMode" },
      {  0x9208, "exif:LightSource" },
      {  0x9209, "exif:Flash" },
      {  0x920a, "exif:FocalLength" },
      {  0x920b, "exif:FlashEnergy" },
      {  0x920c, "exif:SpatialFrequencyResponse" },
      {  0x920d, "exif:Noise" },
      {  0x9214, "exif:SubjectArea" },
      {  0x9290, "exif:SubSecTime" },
      {  0x9291, "exif:SubSecTimeOriginal" },
      {  0x9292, "exif:SubSecTimeDigitized" },    
      {  0x9211, "exif:ImageNumber" },
      {  0x9212, "exif:SecurityClassification" },
      {  0x9213, "exif:ImageHistory" },
      {  0x9214, "exif:SubjectArea" },
      {  0x9215, "exif:ExposureIndex" },
      {  0x9216, "exif:TIFF-EPStandardID" },
      {  0x927c, "exif:MakerNote" },
      {  0x9286, "exif:UserComment" },
      {  0x9290, "exif:SubSecTime" },
      {  0x9291, "exif:SubSecTimeOriginal" },
      {  0x9292, "exif:SubSecTimeDigitized" },    
      {  0x9400, "exif:Temperature" },
      {  0x9401, "exif:Humidity" },
      {  0x9402, "exif:Pressure" },
      {  0x9403, "exif:WaterDepth" },
      {  0x9404, "exif:Acceleration" },
      {  0x9405, "exif:CameraElevationAngle" },    
      {  0x9C9b, "exif:WinXP-Title" },
      {  0x9C9c, "exif:WinXP-Comments" },
      {  0x9C9d, "exif:WinXP-Author" },
      {  0x9C9e, "exif:WinXP-Keywords" },
      {  0x9C9f, "exif:WinXP-Subject" },      
      {  0xa000, "exif:FlashPixVersion" },
      {  0xa001, "exif:ColorSpace" },
      {  0xa002, "exif:PixelXDimension" },
      {  0xa003, "exif:PixelYDimension" },
      {  0xa004, "exif:RelatedSoundFile" },
      {  0xa005, "exif:InteroperabilityOffset" },
      {  0xa20b, "exif:FlashEnergy" },
      {  0xa20c, "exif:SpatialFrequencyResponse" },
      {  0xa20d, "exif:Noise" },
      {  0xa20e, "exif:FocalPlaneXResolution" },
      {  0xa20f, "exif:FocalPlaneYResolution" },
      {  0xa210, "exif:FocalPlaneResolutionUnit" },
      {  0xa214, "exif:SubjectLocation" },
      {  0xa215, "exif:ExposureIndex" },
      {  0xa216, "exif:TIFF/EPStandardID" },
      {  0xa217, "exif:SensingMethod" },
      {  0xa300, "exif:FileSource" },
      {  0xa301, "exif:SceneType" },
      {  0xa302, "exif:CFAPattern" },
      {  0xa401, "exif:CustomRendered" },
      {  0xa402, "exif:ExposureMode" },
      {  0xa403, "exif:WhiteBalance" },
      {  0xa404, "exif:DigitalZoomRatio" },
      {  0xa405, "exif:FocalLengthIn35mmFilm" },
      {  0xa406, "exif:SceneCaptureType" },
      {  0xa407, "exif:GainControl" },
      {  0xa408, "exif:Contrast" },
      {  0xa409, "exif:Saturation" },
      {  0xa40a, "exif:Sharpness" },
      {  0xa40b, "exif:DeviceSettingDescription" },
      {  0xa40c, "exif:SubjectDistanceRange" },
      {  0xa420, "exif:ImageUniqueID" },
      {  0xa430, "exif:CameraOwnerName" },
      {  0xa431, "exif:BodySerialNumber" },
      {  0xa432, "exif:LensSpecification" },
      {  0xa433, "exif:LensMake" },
      {  0xa434, "exif:LensModel" },
      {  0xa435, "exif:LensSerialNumber" },
      {  0xc4a5, "exif:PrintImageMatching" },
      {  0xa500, "exif:Gamma" },
      {  0xc640, "exif:CR2Slice" },
      { 0x10000, "exif:GPSVersionID" },
      { 0x10001, "exif:GPSLatitudeRef" },
      { 0x10002, "exif:GPSLatitude" },
      { 0x10003, "exif:GPSLongitudeRef" },
      { 0x10004, "exif:GPSLongitude" },
      { 0x10005, "exif:GPSAltitudeRef" },
      { 0x10006, "exif:GPSAltitude" },
      { 0x10007, "exif:GPSTimeStamp" },
      { 0x10008, "exif:GPSSatellites" },
      { 0x10009, "exif:GPSStatus" },
      { 0x1000a, "exif:GPSMeasureMode" },
      { 0x1000b, "exif:GPSDop" },
      { 0x1000c, "exif:GPSSpeedRef" },
      { 0x1000d, "exif:GPSSpeed" },
      { 0x1000e, "exif:GPSTrackRef" },
      { 0x1000f, "exif:GPSTrack" },
      { 0x10010, "exif:GPSImgDirectionRef" },
      { 0x10011, "exif:GPSImgDirection" },
      { 0x10012, "exif:GPSMapDatum" },
      { 0x10013, "exif:GPSDestLatitudeRef" },
      { 0x10014, "exif:GPSDestLatitude" },
      { 0x10015, "exif:GPSDestLongitudeRef" },
      { 0x10016, "exif:GPSDestLongitude" },
      { 0x10017, "exif:GPSDestBearingRef" },
      { 0x10018, "exif:GPSDestBearing" },
      { 0x10019, "exif:GPSDestDistanceRef" },
      { 0x1001a, "exif:GPSDestDistance" },
      { 0x1001b, "exif:GPSProcessingMethod" },
      { 0x1001c, "exif:GPSAreaInformation" },
      { 0x1001d, "exif:GPSDateStamp" },
      { 0x1001e, "exif:GPSDifferential" },
      { 0x1001f, "exif:GPSHPositioningError" },
      { 0x00000, "" }
    };  /* http://www.cipa.jp/std/documents/e/DC-008-Translation-2016-E.pdf */

  const StringInfo
    *profile;

  const unsigned char
    *directory,
    *exif;

  DirectoryInfo
    directory_stack[MaxDirectoryStack];

  EndianType
    endian;

  MagickBooleanType
    status;

  ssize_t
    i;

  size_t
    entry,
    length,
    number_entries,
    tag,
    tag_value;

  SplayTreeInfo
    *exif_resources;

  ssize_t
    all,
    id,
    level,
    offset,
    tag_offset;

  static int
    tag_bytes[] = {0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8};

  /*
    If EXIF data exists, then try to parse the request for a tag.
  */
  profile=GetImageProfile(image,"exif");
  if (profile == (const StringInfo *) NULL)
    return(MagickFalse);
  if ((property == (const char *) NULL) || (*property == '\0'))
    return(MagickFalse);
  while (isspace((int) ((unsigned char) *property)) != 0)
    property++;
  if (strlen(property) <= 5)
    return(MagickFalse);
  all=0;
  tag=(~0UL);
  switch (*(property+5))
  {
    case '*':
    {
      /*
        Caller has asked for all the tags in the EXIF data.
      */
      tag=0;
      all=1; /* return the data in description=value format */
      break;
    }
    case '!':
    {
      tag=0;
      all=2; /* return the data in tagid=value format */
      break;
    }
    case '#':
    case '@':
    {
      int
        c;

      size_t
        n;

      /*
        Check for a hex based tag specification first.
      */
      tag=(*(property+5) == '@') ? 1UL : 0UL;
      property+=6;
      n=strlen(property);
      if (n != 4)
        return(MagickFalse);
      /*
        Parse tag specification as a hex number.
      */
      n/=4;
      do
      {
        for (i=(ssize_t) n-1L; i >= 0; i--)
        {
          c=(*property++);
          tag<<=4;
          if ((c >= '0') && (c <= '9'))
            tag|=(c-'0');
          else
            if ((c >= 'A') && (c <= 'F'))
              tag|=(c-('A'-10));
            else
              if ((c >= 'a') && (c <= 'f'))
                tag|=(c-('a'-10));
              else
                return(MagickFalse);
        }
      } while (*property != '\0');
      break;
    }
    default:
    {
      /*
        Try to match the text with a tag name instead.
      */
      for (i=0; ; i++)
      {
        if (EXIFTag[i].tag == 0)
          break;
        if (LocaleCompare(EXIFTag[i].description,property) == 0)
          {
            tag=(size_t) EXIFTag[i].tag;
            break;
          }
      }
      break;
    }
  }
  if (tag == (~0UL))
    return(MagickFalse);
  length=GetStringInfoLength(profile);
  if (length < 6)
    return(MagickFalse);
  exif=GetStringInfoDatum(profile);
  while (length != 0)
  {
    if (ReadPropertyByte(&exif,&length) != 0x45)
      continue;
    if (ReadPropertyByte(&exif,&length) != 0x78)
      continue;
    if (ReadPropertyByte(&exif,&length) != 0x69)
      continue;
    if (ReadPropertyByte(&exif,&length) != 0x66)
      continue;
    if (ReadPropertyByte(&exif,&length) != 0x00)
      continue;
    if (ReadPropertyByte(&exif,&length) != 0x00)
      continue;
    break;
  }
  if (length < 16)
    return(MagickFalse);
  id=(ssize_t) ReadPropertySignedShort(LSBEndian,exif);
  endian=LSBEndian;
  if (id == 0x4949)
    endian=LSBEndian;
  else
    if (id == 0x4D4D)
      endian=MSBEndian;
    else
      return(MagickFalse);
  if (ReadPropertyUnsignedShort(endian,exif+2) != 0x002a)
    return(MagickFalse);
  /*
    This the offset to the first IFD.
  */
  offset=(ssize_t) ReadPropertySignedLong(endian,exif+4);
  if ((offset < 0) || (size_t) offset >= length)
    return(MagickFalse);
  /*
    Set the pointer to the first IFD and follow it were it leads.
  */
  status=MagickFalse;
  directory=exif+offset;
  level=0;
  entry=0;
  tag_offset=0;
  exif_resources=NewSplayTree((int (*)(const void *,const void *)) NULL,
    (void *(*)(void *)) NULL,(void *(*)(void *)) NULL);
  do
  {
    /*
      If there is anything on the stack then pop it off.
    */
    if (level > 0)
      {
        level--;
        directory=directory_stack[level].directory;
        entry=directory_stack[level].entry;
        tag_offset=directory_stack[level].offset;
      }
    if ((directory < exif) || (directory > (exif+length-2)))
      break;
    /*
      Determine how many entries there are in the current IFD.
    */
    number_entries=(size_t) ReadPropertyUnsignedShort(endian,directory);
    for ( ; entry < number_entries; entry++)
    {
      unsigned char
        *p,
        *q;

      size_t
        format;

      ssize_t
        number_bytes,
        components;

      q=(unsigned char *) (directory+(12*entry)+2);
      if (q > (exif+length-12))
        break;  /* corrupt EXIF */
      if (GetValueFromSplayTree(exif_resources,q) == q)
        break;
      (void) AddValueToSplayTree(exif_resources,q,q);
      tag_value=(size_t) ReadPropertyUnsignedShort(endian,q)+tag_offset;
      format=(size_t) ReadPropertyUnsignedShort(endian,q+2);
      if (format >= (sizeof(tag_bytes)/sizeof(*tag_bytes)))
        break;
      if (format == 0)
        break;  /* corrupt EXIF */
      components=(ssize_t) ReadPropertySignedLong(endian,q+4);
      if (components < 0)
        break;  /* corrupt EXIF */
      number_bytes=(size_t) components*tag_bytes[format];
      if (number_bytes < components)
        break;  /* prevent overflow */
      if (number_bytes <= 4)
        p=q+8;
      else
        {
          ssize_t
            dir_offset;

          /*
            The directory entry contains an offset.
          */
          dir_offset=(ssize_t) ReadPropertySignedLong(endian,q+8);
          if ((dir_offset < 0) || (size_t) dir_offset >= length)
            continue;
          if (((size_t) dir_offset+number_bytes) < (size_t) dir_offset)
            continue;  /* prevent overflow */
          if (((size_t) dir_offset+number_bytes) > length)
            continue;
          p=(unsigned char *) (exif+dir_offset);
        }
      if ((all != 0) || (tag == (size_t) tag_value))
        {
          char
            buffer[MaxTextExtent],
            *value;

          if ((p < exif) || (p > (exif+length-tag_bytes[format])))
            break;
          value=(char *) NULL;
          *buffer='\0';
          switch (format)
          {
            case EXIF_FMT_BYTE:
            case EXIF_FMT_UNDEFINED:
            {
              value=(char *) NULL;
              if (~((size_t) number_bytes) >= 1)
                value=(char *) AcquireQuantumMemory((size_t) number_bytes+1UL,
                  sizeof(*value));
              if (value != (char *) NULL)
                {
                  for (i=0; i < (ssize_t) number_bytes; i++)
                  {
                    value[i]='.';
                    if (isprint((int) p[i]) != 0) 
                      value[i]=(char) p[i];
                  }
                  value[i]='\0';
                }
              break;
            }
            case EXIF_FMT_SBYTE:
            {
              EXIFMultipleValues(1,"%.20g",(double) (*(signed char *) p1));
              break;
            }
            case EXIF_FMT_SSHORT:
            {
              EXIFMultipleValues(2,"%hd",ReadPropertySignedShort(endian,p1));
              break;
            }
            case EXIF_FMT_USHORT:
            {
              EXIFMultipleValues(2,"%hu",ReadPropertyUnsignedShort(endian,p1));
              break;
            }
            case EXIF_FMT_ULONG:
            {
              EXIFMultipleValues(4,"%.20g",(double)
                ReadPropertyUnsignedLong(endian,p1));
              break;
            }
            case EXIF_FMT_SLONG:
            {
              EXIFMultipleValues(4,"%.20g",(double)
                ReadPropertySignedLong(endian,p1));
              break;
            }
            case EXIF_FMT_URATIONAL:
            {
              EXIFMultipleFractions(8,"%.20g/%.20g",(double)
                ReadPropertyUnsignedLong(endian,p1),(double)
                ReadPropertyUnsignedLong(endian,p1+4));
              break;
            }
            case EXIF_FMT_SRATIONAL:
            {
              EXIFMultipleFractions(8,"%.20g/%.20g",(double)
                ReadPropertySignedLong(endian,p1),(double)
                ReadPropertySignedLong(endian,p1+4));
              break;
            }
            case EXIF_FMT_SINGLE:
            {
              EXIFMultipleValues(4,"%.20g",(double)
                ReadPropertySignedLong(endian,p1));
              break;
            }
            case EXIF_FMT_DOUBLE:
            {
              EXIFMultipleValues(8,"%.20g",(double)
                ReadPropertySignedLong(endian,p1));
              break;
            }
            case EXIF_FMT_STRING:
            default:
            {
              if ((p < exif) || (p > (exif+length-number_bytes)))
                break;
              value=(char *) NULL;
              if (~((size_t) number_bytes) >= 1)
                value=(char *) AcquireQuantumMemory((size_t) number_bytes+1UL,
                  sizeof(*value));
              if (value != (char *) NULL)
                {
                  ssize_t
                    i;

                  for (i=0; i < (ssize_t) number_bytes; i++)
                  {
                    value[i]='.';
                    if ((isprint((int) p[i]) != 0) || (p[i] == '\0'))
                      value[i]=(char) p[i];
                  }
                  value[i]='\0';
                }
              break;
            }
          }
          if (value != (char *) NULL)
            {
              char
                *key;

              const char
                *p;

              key=AcquireString(property);
              switch (all)
              {
                case 1:
                {
                  const char
                    *description;

                  ssize_t
                    i;

                  description="unknown";
                  for (i=0; ; i++)
                  {
                    if (EXIFTag[i].tag == 0)
                      break;
                    if (EXIFTag[i].tag == tag_value)
                      {
                        description=EXIFTag[i].description;
                        break;
                      }
                  }
                  (void) FormatLocaleString(key,MaxTextExtent,"%s",
                    description);
                  if (level == 2)
                    (void) SubstituteString(&key,"exif:","exif:thumbnail:");
                  break;
                }
                case 2:
                {
                  if (tag_value < 0x10000)
                    (void) FormatLocaleString(key,MaxTextExtent,"#%04lx",
                      (unsigned long) tag_value);
                  else
                    if (tag_value < 0x20000)
                      (void) FormatLocaleString(key,MaxTextExtent,"@%04lx",
                        (unsigned long) (tag_value & 0xffff));
                    else
                      (void) FormatLocaleString(key,MaxTextExtent,"unknown");
                  break;
                }
                default:
                {
                  if (level == 2)
                    (void) SubstituteString(&key,"exif:","exif:thumbnail:");
                }
              }
              p=(const char *) NULL;
              if (image->properties != (void *) NULL)
                p=(const char *) GetValueFromSplayTree((SplayTreeInfo *)
                  image->properties,key);
              if (p == (const char *) NULL)
                (void) SetImageProperty((Image *) image,key,value);
              value=DestroyString(value);
              key=DestroyString(key);
              status=MagickTrue;
            }
        }
        if ((tag_value == TAG_EXIF_OFFSET) ||
            (tag_value == TAG_INTEROP_OFFSET) || (tag_value == TAG_GPS_OFFSET))
          {
            ssize_t
              offset;

            offset=(ssize_t) ReadPropertySignedLong(endian,p);
            if (((size_t) offset < length) && (level < (MaxDirectoryStack-2)))
              {
                ssize_t
                  tag_offset1;

                tag_offset1=(ssize_t) ((tag_value == TAG_GPS_OFFSET) ? 0x10000 :
                  0);
                directory_stack[level].directory=directory;
                entry++;
                directory_stack[level].entry=entry;
                directory_stack[level].offset=tag_offset;
                level++;
                /*
                  Check for duplicate tag.
                */
                for (i=0; i < level; i++)
                  if (directory_stack[i].directory == (exif+tag_offset1))
                    break;
                if (i < level)
                  break;  /* duplicate tag */
                directory_stack[level].directory=exif+offset;
                directory_stack[level].offset=tag_offset1;
                directory_stack[level].entry=0;
                level++;
                if ((directory+2+(12*number_entries)+4) > (exif+length))
                  break;
                offset=(ssize_t) ReadPropertySignedLong(endian,directory+2+(12*
                  number_entries));
                if ((offset != 0) && ((size_t) offset < length) &&
                    (level < (MaxDirectoryStack-2)))
                  {
                    directory_stack[level].directory=exif+offset;
                    directory_stack[level].entry=0;
                    directory_stack[level].offset=tag_offset1;
                    level++;
                  }
              }
            break;
          }
    }
  } while (level > 0);
  exif_resources=DestroySplayTree(exif_resources);
  return(status);
}