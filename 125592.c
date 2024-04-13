int lpGetEdgeStreamID(unsigned char *lp, int first, streamID *master_id, streamID *edge_id)
{
   if (lp == NULL)
       return 0;

   unsigned char *lp_ele;

   /* We need to seek either the first or the last entry depending
    * on the direction of the iteration. */
   if (first) {
       /* Get the master fields count. */
       lp_ele = lpFirst(lp);        /* Seek items count */
       lp_ele = lpNext(lp, lp_ele); /* Seek deleted count. */
       lp_ele = lpNext(lp, lp_ele); /* Seek num fields. */
       int64_t master_fields_count = lpGetInteger(lp_ele);
       lp_ele = lpNext(lp, lp_ele); /* Seek first field. */

       /* If we are iterating in normal order, skip the master fields
        * to seek the first actual entry. */
       for (int64_t i = 0; i < master_fields_count; i++)
           lp_ele = lpNext(lp, lp_ele);

       /* If we are going forward, skip the previous entry's
        * lp-count field (or in case of the master entry, the zero
        * term field) */
       lp_ele = lpNext(lp, lp_ele);
       if (lp_ele == NULL)
           return 0;
   } else {
       /* If we are iterating in reverse direction, just seek the
        * last part of the last entry in the listpack (that is, the
        * fields count). */
       lp_ele = lpLast(lp);

       /* If we are going backward, read the number of elements this
        * entry is composed of, and jump backward N times to seek
        * its start. */
       int64_t lp_count = lpGetInteger(lp_ele);
       if (lp_count == 0) /* We reached the master entry. */
           return 0;

       while (lp_count--)
           lp_ele = lpPrev(lp, lp_ele);
   }

   lp_ele = lpNext(lp, lp_ele); /* Seek ID (lp_ele currently points to 'flags'). */

   /* Get the ID: it is encoded as difference between the master
    * ID and this entry ID. */
   streamID id = *master_id;
   id.ms += lpGetInteger(lp_ele);
   lp_ele = lpNext(lp, lp_ele);
   id.seq += lpGetInteger(lp_ele);
   *edge_id = id;
   return 1;
}