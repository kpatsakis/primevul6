AP4_MpegSystemSampleEntry::ToSampleDescription()
{
    return new AP4_MpegSystemSampleDescription(
        AP4_DYNAMIC_CAST(AP4_EsdsAtom, GetChild(AP4_ATOM_TYPE_ESDS)));
}