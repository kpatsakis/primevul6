AP4_AtomFactory::CreateAtomFromStream(AP4_ByteStream& stream, 
                                      AP4_UI32        type,
                                      AP4_UI32        size_32,
                                      AP4_UI64        size_64,
                                      AP4_Atom*&      atom)
{
    bool atom_is_large = (size_32 == 1);
    bool force_64 = (size_32==1 && ((size_64>>32) == 0));
    
    // create the atom
    if (GetContext() == AP4_ATOM_TYPE_STSD) {
        // sample entry
        if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
        switch (type) {
          case AP4_ATOM_TYPE_MP4A:
            atom = new AP4_Mp4aSampleEntry(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_MP4V:
            atom = new AP4_Mp4vSampleEntry(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_MP4S:
            atom = new AP4_Mp4sSampleEntry(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_ENCA:
            atom = new AP4_EncaSampleEntry(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_ENCV:
            atom = new AP4_EncvSampleEntry(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_DRMS:
            atom = new AP4_DrmsSampleEntry(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_DRMI:
            atom = new AP4_DrmiSampleEntry(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_AVC1:
          case AP4_ATOM_TYPE_AVC2:
          case AP4_ATOM_TYPE_AVC3:
          case AP4_ATOM_TYPE_AVC4:
          case AP4_ATOM_TYPE_DVAV:
          case AP4_ATOM_TYPE_DVA1:
            atom = new AP4_AvcSampleEntry(type, size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_HEV1:
          case AP4_ATOM_TYPE_HVC1:
          case AP4_ATOM_TYPE_DVHE:
          case AP4_ATOM_TYPE_DVH1:
            atom = new AP4_HevcSampleEntry(type, size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_ALAC:
          case AP4_ATOM_TYPE_AC_3:
          case AP4_ATOM_TYPE_EC_3:
          case AP4_ATOM_TYPE_DTSC:
          case AP4_ATOM_TYPE_DTSH:
          case AP4_ATOM_TYPE_DTSL:
          case AP4_ATOM_TYPE_DTSE:
            atom = new AP4_AudioSampleEntry(type, size_32, stream, *this);
            break;
            
          case AP4_ATOM_TYPE_RTP_:
            atom = new AP4_RtpHintSampleEntry(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_STPP:
            atom = new AP4_SubtitleSampleEntry(type, size_32, stream, *this);
            break;

          default: {
            // try all the external type handlers
            AP4_List<TypeHandler>::Item* handler_item = m_TypeHandlers.FirstItem();
            while (handler_item) {
                TypeHandler* handler = handler_item->GetData();
                if (AP4_SUCCEEDED(handler->CreateAtom(type, size_32, stream, GetContext(), atom))) {
                    break;
                }
                handler_item = handler_item->GetNext();
            }

            // no custom handler, create a generic entry
            if (atom == NULL) {
                atom = new AP4_UnknownSampleEntry(type, (AP4_UI32)size_64, stream);
            }

            break;
          }
        }
    } else {
        // regular atom
        switch (type) {
          case AP4_ATOM_TYPE_MOOV:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_MoovAtom::Create(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_MVHD:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_MvhdAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_MEHD:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_MehdAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_MFHD:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_MfhdAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_TRAK:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_TrakAtom::Create(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_TREX:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_TrexAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_HDLR:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_HdlrAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_TKHD:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_TkhdAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_TFHD:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_TfhdAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_TRUN:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_TrunAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_TFRA:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_TfraAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_MFRO:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_MfroAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_MDHD:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_MdhdAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_STSD:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_StsdAtom::Create(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_STSC:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_StscAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_STCO:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_StcoAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_CO64:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_Co64Atom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_STSZ:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_StszAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_STZ2:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_Stz2Atom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_STTS:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_SttsAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_CTTS:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_CttsAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_STSS:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_StssAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_IODS:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_IodsAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_ESDS:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_EsdsAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_AVCC:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_AvccAtom::Create(size_32, stream);
            break;
            
          case AP4_ATOM_TYPE_HVCC:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_HvccAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_DVCC:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_DvccAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_HVCE:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_HvccAtom::Create(size_32, stream);
            if (atom) {
                atom->SetType(AP4_ATOM_TYPE_HVCE);
            }
            break;

          case AP4_ATOM_TYPE_AVCE:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_AvccAtom::Create(size_32, stream);
            if (atom) {
                atom->SetType(AP4_ATOM_TYPE_AVCE);
            }
            break;

    #if !defined(AP4_CONFIG_MINI_BUILD)
          case AP4_ATOM_TYPE_UUID: {
              AP4_UI08 uuid[16];
              AP4_Result result = stream.Read(uuid, 16);
              if (AP4_FAILED(result)) return result;
              
              if (AP4_CompareMemory(uuid, AP4_UUID_PIFF_TRACK_ENCRYPTION_ATOM, 16) == 0) {
                  atom = AP4_PiffTrackEncryptionAtom::Create((AP4_UI32)size_64, stream);
              } else if (AP4_CompareMemory(uuid, AP4_UUID_PIFF_SAMPLE_ENCRYPTION_ATOM, 16) == 0) {
                  atom = AP4_PiffSampleEncryptionAtom::Create((AP4_UI32)size_64, stream);
              } else {
                  atom = new AP4_UnknownUuidAtom(size_64, uuid, stream);
              }
              break;
          }
            
          case AP4_ATOM_TYPE_8ID_:
            atom = new AP4_NullTerminatedStringAtom(type, size_64, stream);
            break;

          case AP4_ATOM_TYPE_8BDL:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_8bdlAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_DREF:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_DrefAtom::Create(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_URL:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_UrlAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_ELST:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_ElstAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_VMHD:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_VmhdAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_SMHD:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_SmhdAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_NMHD:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_NmhdAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_STHD:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_SthdAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_HMHD:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_HmhdAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_FRMA:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_FrmaAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_SCHM:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_SchmAtom::Create(size_32, &m_ContextStack, stream);
            break;

          case AP4_ATOM_TYPE_FTYP:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_FtypAtom::Create(size_32, stream);
            break;
                        
          case AP4_ATOM_TYPE_TIMS:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_TimsAtom::Create(size_32, stream);
            break;
     
          case AP4_ATOM_TYPE_SDP_:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_SdpAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_IKMS:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_IkmsAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_ISFM:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_IsfmAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_ISLT:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_IsltAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_ODHE:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_OdheAtom::Create(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_OHDR:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_OhdrAtom::Create(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_ODDA:
            atom = AP4_OddaAtom::Create(size_64, stream);
            break;

          case AP4_ATOM_TYPE_ODAF:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_OdafAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_GRPI:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_GrpiAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_IPRO:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_IproAtom::Create(size_32, stream, *this);
            break;

          case AP4_ATOM_TYPE_RTP_:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_RtpAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_TFDT:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_TfdtAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_TENC:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_TencAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_SENC:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_SencAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_SAIZ:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_SaizAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_SAIO:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_SaioAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_PDIN:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_PdinAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_BLOC:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_BlocAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_AINF:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_AinfAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_PSSH:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_PsshAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_SIDX:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_SidxAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_SBGP:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_SbgpAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_SGPD:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_SgpdAtom::Create(size_32, stream);
            break;

          case AP4_ATOM_TYPE_MKID:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            if (GetContext() == AP4_ATOM_TYPE_MARL) {
                atom = AP4_MkidAtom::Create(size_32, stream);
            }
            break;

          case AP4_ATOM_TYPE_DEC3:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            if (GetContext() == AP4_ATOM_TYPE_EC_3 || GetContext() == AP4_ATOM_TYPE_ENCA) {
                atom = AP4_Dec3Atom::Create(size_32, stream);
            }
            break;

          // track ref types
          case AP4_ATOM_TYPE_HINT:
          case AP4_ATOM_TYPE_CDSC:
          case AP4_ATOM_TYPE_SYNC:
          case AP4_ATOM_TYPE_MPOD:
          case AP4_ATOM_TYPE_DPND:
          case AP4_ATOM_TYPE_IPIR:
          case AP4_ATOM_TYPE_ALIS:
          case AP4_ATOM_TYPE_CHAP:
            if (GetContext() == AP4_ATOM_TYPE_TREF) {
                if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
                atom = AP4_TrefTypeAtom::Create(type, size_32, stream);
            }
            break;

    #endif // AP4_CONFIG_MINI_BUILD

          // container atoms
          case AP4_ATOM_TYPE_MOOF:
          case AP4_ATOM_TYPE_MVEX:
          case AP4_ATOM_TYPE_TRAF:
          case AP4_ATOM_TYPE_TREF:
          case AP4_ATOM_TYPE_MFRA:
          case AP4_ATOM_TYPE_HNTI:
          case AP4_ATOM_TYPE_STBL:
          case AP4_ATOM_TYPE_MDIA:
          case AP4_ATOM_TYPE_DINF:
          case AP4_ATOM_TYPE_MINF:
          case AP4_ATOM_TYPE_SCHI:
          case AP4_ATOM_TYPE_SINF:
          case AP4_ATOM_TYPE_UDTA:
          case AP4_ATOM_TYPE_ILST:
          case AP4_ATOM_TYPE_EDTS: 
          case AP4_ATOM_TYPE_MDRI:
          case AP4_ATOM_TYPE_WAVE:
            if (atom_is_large) return AP4_ERROR_INVALID_FORMAT;
            atom = AP4_ContainerAtom::Create(type, size_64, false, force_64, stream, *this);
            break;

          // containers, only at the top
          case AP4_ATOM_TYPE_MARL:
            if (GetContext() == 0) {
                atom = AP4_ContainerAtom::Create(type, size_64, false, force_64, stream, *this);
            }
            break;
            
          // full container atoms
          case AP4_ATOM_TYPE_META:
          case AP4_ATOM_TYPE_ODRM:
          case AP4_ATOM_TYPE_ODKM:
            atom = AP4_ContainerAtom::Create(type, size_64, true, force_64, stream, *this);
            break;

          case AP4_ATOM_TYPE_FREE:
          case AP4_ATOM_TYPE_WIDE:
          case AP4_ATOM_TYPE_MDAT:
            // generic atoms
            break;
            
          default: {
            // try all the external type handlers
            AP4_List<TypeHandler>::Item* handler_item = m_TypeHandlers.FirstItem();
            while (handler_item) {
                TypeHandler* handler = handler_item->GetData();
                if (AP4_SUCCEEDED(handler->CreateAtom(type, size_32, stream, GetContext(), atom))) {
                    break;
                }
                handler_item = handler_item->GetNext();
            }

            break;
          }
        }
    }
    
    return AP4_SUCCESS;
}