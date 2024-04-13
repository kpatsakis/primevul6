bool PxMDecoder::checkSignature( const String& signature ) const
{
    return signature.size() >= 3 && signature[0] == 'P' &&
           '1' <= signature[1] && signature[1] <= '6' &&
           isspace(signature[2]);
}