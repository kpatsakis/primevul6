int next_huffcode( abitreader *huffw, huffTree *ctree, Billing min_bill, Billing max_bill)
{
    int node = 0;


    while ( node < 256 ) {
#if defined(ENABLE_BILLING) || !defined(NDEBUG)
        write_bit_bill(min_bill, false, 1);
        if (min_bill != max_bill) {
            min_bill = (Billing)((int)min_bill + 1);
        }
#endif
        node = ( huffw->read( 1 ) == 1 ) ?
                ctree->r[ node ] : ctree->l[ node ];
        if ( node == 0 ) break;
    }

    return ( node - 256 );
}