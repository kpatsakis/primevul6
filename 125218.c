void CCrypto::GenerateSHA256Digest( const void *pData, size_t cbData, SHA256Digest_t *pOutputDigest )
{
    VPROF_BUDGET( "CCrypto::GenerateSHA256Digest", VPROF_BUDGETGROUP_ENCRYPTION );
	Assert( pData );
    Assert( pOutputDigest );

    crypto_hash_sha256( *pOutputDigest, static_cast<const unsigned char*>(pData), cbData );
}