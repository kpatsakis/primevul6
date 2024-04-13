AP4_AtomFactory::CreateAtomFromStream(AP4_ByteStream& stream, 
                                      AP4_LargeSize&  bytes_available,
                                      AP4_Atom*&      atom)
{
    AP4_Result result;

    // NULL by default
    atom = NULL;

    // check that there are enough bytes for at least a header
    if (bytes_available < 8) return AP4_ERROR_EOS;

    // remember current stream offset
    AP4_Position start;
    stream.Tell(start);

    // read atom size
    AP4_UI32 size_32;
    result = stream.ReadUI32(size_32);
    if (AP4_FAILED(result)) {
        stream.Seek(start);
        return result;
    }
    AP4_UI64 size = size_32;

    // read atom type
    AP4_Atom::Type type;
    result = stream.ReadUI32(type);
    if (AP4_FAILED(result)) {
        stream.Seek(start);
        return result;
    }

    // handle special size values
    bool atom_is_large = false;
    bool force_64      = false;
    if (size == 0) {
        // atom extends to end of file
        AP4_LargeSize stream_size = 0;
        stream.GetSize(stream_size);
        if (stream_size >= start) {
            size = stream_size - start;
        }
    } else if (size == 1) {
        // 64-bit size
        atom_is_large = true;
        if (bytes_available < 16) {
            stream.Seek(start);
            return AP4_ERROR_INVALID_FORMAT;
        }
        stream.ReadUI64(size);
        if (size <= 0xFFFFFFFF) {
            force_64 = true;
        }
    }

    // check the size
    if ((size > 0 && size < 8) || size > bytes_available) {
        stream.Seek(start);
        return AP4_ERROR_INVALID_FORMAT;
    }

    // create the atom
    result = CreateAtomFromStream(stream, type, size_32, size, atom);
    if (AP4_FAILED(result)) return result;
    
    // if we failed to create an atom, use a generic version
    if (atom == NULL) {
        unsigned int payload_offset = 8;
        if (atom_is_large) payload_offset += 8;
        stream.Seek(start+payload_offset);
        atom = new AP4_UnknownAtom(type, size, stream);
    }

    // special case: if the atom is poorly encoded and has a 64-bit
    // size header but an actual size that fits on 32-bit, adjust the
    // object to reflect that.
    if (force_64) {
        atom->SetSize32(1);
        atom->SetSize64(size);
    }

    // adjust the available size
    bytes_available -= size;

    // skip to the end of the atom
    result = stream.Seek(start+size);
    if (AP4_FAILED(result)) {
        delete atom;
        atom = NULL;
        return result;
    }
    
    return AP4_SUCCESS;
}