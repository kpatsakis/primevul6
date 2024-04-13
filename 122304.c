QPDFWriter::getTrimmedTrailer()
{
    // Remove keys from the trailer that necessarily have to be
    // replaced when writing the file.

    QPDFObjectHandle trailer = pdf.getTrailer().shallowCopy();

    // Remove encryption keys
    trailer.removeKey("/ID");
    trailer.removeKey("/Encrypt");

    // Remove modification information
    trailer.removeKey("/Prev");

    // Remove all trailer keys that potentially come from a
    // cross-reference stream
    trailer.removeKey("/Index");
    trailer.removeKey("/W");
    trailer.removeKey("/Length");
    trailer.removeKey("/Filter");
    trailer.removeKey("/DecodeParms");
    trailer.removeKey("/Type");
    trailer.removeKey("/XRefStm");

    return trailer;
}