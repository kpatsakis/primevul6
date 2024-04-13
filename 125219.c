void CCrypto::GenerateHMAC256( const uint8 *pubData, uint32 cubData, const uint8 *pubKey, uint32 cubKey, SHA256Digest_t *pOutputDigest )
{
    VPROF_BUDGET( "CCrypto::GenerateHMAC256", VPROF_BUDGETGROUP_ENCRYPTION );
	Assert( pubData );
	Assert( cubData > 0 );
	Assert( pubKey );
	Assert( cubKey > 0 );
	Assert( pOutputDigest );

    Assert( sizeof(*pOutputDigest) == crypto_auth_hmacsha256_BYTES );
    Assert( cubKey == crypto_auth_hmacsha256_KEYBYTES );

    crypto_auth_hmacsha256( *pOutputDigest, pubData, cubData, pubKey );
}