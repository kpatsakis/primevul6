decrypt(MPI output, MPI a, MPI b, ELG_secret_key *skey )
{
    MPI t1 = mpi_alloc_secure( mpi_get_nlimbs( skey->p ) );

    mpi_normalize (a);
    mpi_normalize (b);

    /* output = b/(a^x) mod p */
    mpi_powm( t1, a, skey->x, skey->p );
    mpi_invm( t1, t1, skey->p );
    mpi_mulm( output, b, t1, skey->p );
#if 0
    if( DBG_CIPHER ) {
	log_mpidump("elg decrypted x= ", skey->x);
	log_mpidump("elg decrypted p= ", skey->p);
	log_mpidump("elg decrypted a= ", a);
	log_mpidump("elg decrypted b= ", b);
	log_mpidump("elg decrypted M= ", output);
    }
#endif
    mpi_free(t1);
}