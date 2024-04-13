ospf_read (struct thread *thread)
{
  int ret;
  struct stream *ibuf;
  struct ospf *ospf;
  struct ospf_interface *oi;
  struct ip *iph;
  struct ospf_header *ospfh;
  u_int16_t length;
  struct interface *ifp;

  /* first of all get interface pointer. */
  ospf = THREAD_ARG (thread);

  /* prepare for next packet. */
  ospf->t_read = thread_add_read (master, ospf_read, ospf, ospf->fd);

  /* read OSPF packet. */
  stream_reset(ospf->ibuf);
  if (!(ibuf = ospf_recv_packet (ospf->fd, &ifp, ospf->ibuf)))
    return -1;
  
  /* Note that there should not be alignment problems with this assignment
     because this is at the beginning of the stream data buffer. */
  iph = (struct ip *) STREAM_DATA (ibuf);
  /* Note that sockopt_iphdrincl_swab_systoh was called in ospf_recv_packet. */

  if (ifp == NULL)
    /* Handle cases where the platform does not support retrieving the ifindex,
       and also platforms (such as Solaris 8) that claim to support ifindex
       retrieval but do not. */
    ifp = if_lookup_address (iph->ip_src);
  
  if (ifp == NULL)
    return 0;

  /* IP Header dump. */
    if (IS_DEBUG_OSPF_PACKET(0, RECV))
	    ospf_ip_header_dump (iph);

  /* Self-originated packet should be discarded silently. */
  if (ospf_if_lookup_by_local_addr (ospf, NULL, iph->ip_src))
    {
      if (IS_DEBUG_OSPF_PACKET (0, RECV))
        {
          zlog_debug ("ospf_read[%s]: Dropping self-originated packet",
                     inet_ntoa (iph->ip_src));
        }
      return 0;
    }

  /* Advance from IP header to OSPF header (iph->ip_hl has been verified
     by ospf_recv_packet() to be correct). */
  stream_forward_getp (ibuf, iph->ip_hl * 4);

  /* Make sure the OSPF header is really there. */
  if (stream_get_endp (ibuf) - stream_get_getp (ibuf) < OSPF_HEADER_SIZE)
  {
    zlog_debug ("ospf_read: ignored OSPF packet with undersized (%u bytes) header",
                stream_get_endp (ibuf) - stream_get_getp (ibuf));
    return -1;
  }

  /* Now it is safe to access all fields of OSPF packet header. */
  ospfh = (struct ospf_header *) STREAM_PNT (ibuf);

  /* associate packet with ospf interface */
  oi = ospf_if_lookup_recv_if (ospf, iph->ip_src, ifp);

  /* ospf_verify_header() relies on a valid "oi" and thus can be called only
     after the passive/backbone/other checks below are passed. These checks
     in turn access the fields of unverified "ospfh" structure for their own
     purposes and must remain very accurate in doing this. */

  /* If incoming interface is passive one, ignore it. */
  if (oi && OSPF_IF_PASSIVE_STATUS (oi) == OSPF_IF_PASSIVE)
    {
      char buf[3][INET_ADDRSTRLEN];

      if (IS_DEBUG_OSPF_EVENT)
	zlog_debug ("ignoring packet from router %s sent to %s, "
		    "received on a passive interface, %s",
		    inet_ntop(AF_INET, &ospfh->router_id, buf[0], sizeof(buf[0])),
		    inet_ntop(AF_INET, &iph->ip_dst, buf[1], sizeof(buf[1])),
		    inet_ntop(AF_INET, &oi->address->u.prefix4,
			      buf[2], sizeof(buf[2])));

      if (iph->ip_dst.s_addr == htonl(OSPF_ALLSPFROUTERS))
	{
	  /* Try to fix multicast membership.
	   * Some OS:es may have problems in this area,
	   * make sure it is removed.
	   */
	  OI_MEMBER_JOINED(oi, MEMBER_ALLROUTERS);
	  ospf_if_set_multicast(oi);
	}
      return 0;
  }


  /* if no local ospf_interface, 
   * or header area is backbone but ospf_interface is not
   * check for VLINK interface
   */
  if ( (oi == NULL) ||
      (OSPF_IS_AREA_ID_BACKBONE(ospfh->area_id)
      && !OSPF_IS_AREA_ID_BACKBONE(oi->area->area_id))
     )
    {
      if ((oi = ospf_associate_packet_vl (ospf, ifp, iph, ospfh)) == NULL)
        {
          if (IS_DEBUG_OSPF_EVENT)
            zlog_debug ("Packet from [%s] received on link %s"
                        " but no ospf_interface",
                        inet_ntoa (iph->ip_src), ifp->name);
          return 0;
        }
    }

  /* else it must be a local ospf interface, check it was received on 
   * correct link 
   */
  else if (oi->ifp != ifp)
    {
      if (IS_DEBUG_OSPF_EVENT)
        zlog_warn ("Packet from [%s] received on wrong link %s",
                   inet_ntoa (iph->ip_src), ifp->name); 
      return 0;
    }
  else if (oi->state == ISM_Down)
    {
      char buf[2][INET_ADDRSTRLEN];
      zlog_warn ("Ignoring packet from %s to %s received on interface that is "
      		 "down [%s]; interface flags are %s",
		 inet_ntop(AF_INET, &iph->ip_src, buf[0], sizeof(buf[0])),
		 inet_ntop(AF_INET, &iph->ip_dst, buf[1], sizeof(buf[1])),
	         ifp->name, if_flag_dump(ifp->flags));
      /* Fix multicast memberships? */
      if (iph->ip_dst.s_addr == htonl(OSPF_ALLSPFROUTERS))
        OI_MEMBER_JOINED(oi, MEMBER_ALLROUTERS);
      else if (iph->ip_dst.s_addr == htonl(OSPF_ALLDROUTERS))
	OI_MEMBER_JOINED(oi, MEMBER_DROUTERS);
      if (oi->multicast_memberships)
	ospf_if_set_multicast(oi);
      return 0;
    }

  /*
   * If the received packet is destined for AllDRouters, the packet
   * should be accepted only if the received ospf interface state is
   * either DR or Backup -- endo.
   */
  if (iph->ip_dst.s_addr == htonl (OSPF_ALLDROUTERS)
  && (oi->state != ISM_DR && oi->state != ISM_Backup))
    {
      zlog_warn ("Dropping packet for AllDRouters from [%s] via [%s] (ISM: %s)",
                 inet_ntoa (iph->ip_src), IF_NAME (oi),
                 LOOKUP (ospf_ism_state_msg, oi->state));
      /* Try to fix multicast membership. */
      SET_FLAG(oi->multicast_memberships, MEMBER_DROUTERS);
      ospf_if_set_multicast(oi);
      return 0;
    }

  /* Verify more OSPF header fields. */
  ret = ospf_verify_header (ibuf, oi, iph, ospfh);
  if (ret < 0)
  {
    if (IS_DEBUG_OSPF_PACKET (0, RECV))
      zlog_debug ("ospf_read[%s]: Header check failed, "
                  "dropping.",
                  inet_ntoa (iph->ip_src));
    return ret;
  }

  /* Show debug receiving packet. */
  if (IS_DEBUG_OSPF_PACKET (ospfh->type - 1, RECV))
    {
      if (IS_DEBUG_OSPF_PACKET (ospfh->type - 1, DETAIL))
        {
          zlog_debug ("-----------------------------------------------------");
          ospf_packet_dump (ibuf);
        }

      zlog_debug ("%s received from [%s] via [%s]",
                 LOOKUP (ospf_packet_type_str, ospfh->type),
                 inet_ntoa (ospfh->router_id), IF_NAME (oi));
      zlog_debug (" src [%s],", inet_ntoa (iph->ip_src));
      zlog_debug (" dst [%s]", inet_ntoa (iph->ip_dst));

      if (IS_DEBUG_OSPF_PACKET (ospfh->type - 1, DETAIL))
	zlog_debug ("-----------------------------------------------------");
  }

  stream_forward_getp (ibuf, OSPF_HEADER_SIZE);

  /* Adjust size to message length. */
  length = ntohs (ospfh->length) - OSPF_HEADER_SIZE;

  /* Read rest of the packet and call each sort of packet routine. */
  switch (ospfh->type)
    {
    case OSPF_MSG_HELLO:
      ospf_hello (iph, ospfh, ibuf, oi, length);
      break;
    case OSPF_MSG_DB_DESC:
      ospf_db_desc (iph, ospfh, ibuf, oi, length);
      break;
    case OSPF_MSG_LS_REQ:
      ospf_ls_req (iph, ospfh, ibuf, oi, length);
      break;
    case OSPF_MSG_LS_UPD:
      ospf_ls_upd (iph, ospfh, ibuf, oi, length);
      break;
    case OSPF_MSG_LS_ACK:
      ospf_ls_ack (iph, ospfh, ibuf, oi, length);
      break;
    default:
      zlog (NULL, LOG_WARNING,
	    "interface %s: OSPF packet header type %d is illegal",
	    IF_NAME (oi), ospfh->type);
      break;
    }

  return 0;
}