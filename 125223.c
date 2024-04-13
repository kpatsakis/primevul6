void CCrypto::GenerateRandomBlock( void *pubDest, int cubDest )
{
    VPROF_BUDGET( "CCrypto::GenerateRandomBlock", VPROF_BUDGETGROUP_ENCRYPTION );
	AssertFatal( cubDest >= 0 );

    randombytes_buf( pubDest, cubDest );
}