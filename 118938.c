void ogs_sbi_message_free(ogs_sbi_message_t *message)
{
    int i;

    ogs_assert(message);

    if (message->NFProfile)
        OpenAPI_nf_profile_free(message->NFProfile);
    if (message->ProblemDetails)
        OpenAPI_problem_details_free(message->ProblemDetails);
    if (message->PatchItemList) {
        OpenAPI_lnode_t *node = NULL;
        OpenAPI_list_for_each(message->PatchItemList, node)
            OpenAPI_patch_item_free(node->data);
        OpenAPI_list_free(message->PatchItemList);
    }

    if (message->SubscriptionData)
        OpenAPI_subscription_data_free(message->SubscriptionData);
    if (message->NotificationData)
        OpenAPI_notification_data_free(message->NotificationData);
    if (message->SearchResult)
        OpenAPI_search_result_free(message->SearchResult);
    if (message->AuthenticationInfo)
        OpenAPI_authentication_info_free(message->AuthenticationInfo);
    if (message->AuthenticationInfoRequest)
        OpenAPI_authentication_info_request_free(
                message->AuthenticationInfoRequest);
    if (message->AuthenticationInfoResult)
        OpenAPI_authentication_info_result_free(
                message->AuthenticationInfoResult);
    if (message->AuthenticationSubscription)
        OpenAPI_authentication_subscription_free(
                message->AuthenticationSubscription);
    if (message->UeAuthenticationCtx)
        OpenAPI_ue_authentication_ctx_free(message->UeAuthenticationCtx);
    if (message->ConfirmationData)
        OpenAPI_confirmation_data_free(message->ConfirmationData);
    if (message->ConfirmationDataResponse)
        OpenAPI_confirmation_data_response_free(
                message->ConfirmationDataResponse);
    if (message->AuthEvent)
        OpenAPI_auth_event_free(message->AuthEvent);
    if (message->Amf3GppAccessRegistration)
        OpenAPI_amf3_gpp_access_registration_free(
                message->Amf3GppAccessRegistration);
    if (message->AccessAndMobilitySubscriptionData)
        OpenAPI_access_and_mobility_subscription_data_free(
                message->AccessAndMobilitySubscriptionData);
    if (message->SmfSelectionSubscriptionData)
        OpenAPI_smf_selection_subscription_data_free(
                message->SmfSelectionSubscriptionData);
    if (message->UeContextInSmfData)
        OpenAPI_ue_context_in_smf_data_free(message->UeContextInSmfData);
    if (message->SmContextCreateData)
        OpenAPI_sm_context_create_data_free(message->SmContextCreateData);
    if (message->SmContextCreatedData)
        OpenAPI_sm_context_created_data_free(message->SmContextCreatedData);
    if (message->SmContextCreateError)
        OpenAPI_sm_context_create_error_free(message->SmContextCreateError);
    if (message->SmContextUpdateData)
        OpenAPI_sm_context_update_data_free(message->SmContextUpdateData);
    if (message->SmContextUpdatedData)
        OpenAPI_sm_context_updated_data_free(message->SmContextUpdatedData);
    if (message->SmContextUpdateError)
        OpenAPI_sm_context_update_error_free(message->SmContextUpdateError);
    if (message->SmContextReleaseData)
        OpenAPI_sm_context_release_data_free(message->SmContextReleaseData);
    if (message->SmContextReleasedData)
        OpenAPI_sm_context_released_data_free(message->SmContextReleasedData);
    if (message->SessionManagementSubscriptionData)
        OpenAPI_session_management_subscription_data_free(
                message->SessionManagementSubscriptionData);
    if (message->N1N2MessageTransferReqData)
        OpenAPI_n1_n2_message_transfer_req_data_free(
                message->N1N2MessageTransferReqData);
    if (message->N1N2MessageTransferRspData)
        OpenAPI_n1_n2_message_transfer_rsp_data_free(
                message->N1N2MessageTransferRspData);
    if (message->N1N2MsgTxfrFailureNotification)
        OpenAPI_n1_n2_msg_txfr_failure_notification_free(
                message->N1N2MsgTxfrFailureNotification);
    if (message->SmContextStatusNotification)
        OpenAPI_sm_context_status_notification_free(
                message->SmContextStatusNotification);
    if (message->PolicyAssociationRequest)
        OpenAPI_policy_association_request_free(
                message->PolicyAssociationRequest);
    if (message->PolicyAssociation)
        OpenAPI_policy_association_free(message->PolicyAssociation);
    if (message->AmPolicyData)
        OpenAPI_am_policy_data_free(message->AmPolicyData);
    if (message->SmPolicyContextData)
        OpenAPI_sm_policy_context_data_free(message->SmPolicyContextData);
    if (message->SmPolicyDecision)
        OpenAPI_sm_policy_decision_free(message->SmPolicyDecision);
    if (message->SmPolicyData)
        OpenAPI_sm_policy_data_free(message->SmPolicyData);
    if (message->SmPolicyDeleteData)
        OpenAPI_sm_policy_delete_data_free(message->SmPolicyDeleteData);
    if (message->AuthorizedNetworkSliceInfo)
        OpenAPI_authorized_network_slice_info_free(
                message->AuthorizedNetworkSliceInfo);
    if (message->PcfBinding)
        OpenAPI_pcf_binding_free(message->PcfBinding);
    if (message->AppSessionContext)
        OpenAPI_app_session_context_free(message->AppSessionContext);
    if (message->AppSessionContextUpdateDataPatch)
        OpenAPI_app_session_context_update_data_patch_free(message->AppSessionContextUpdateDataPatch);
    if (message->SmPolicyNotification)
        OpenAPI_sm_policy_notification_free(message->SmPolicyNotification);
    if (message->TerminationNotification)
        OpenAPI_termination_notification_free(message->TerminationNotification);

    for (i = 0; i < message->num_of_part; i++) {
        if (message->part[i].pkbuf)
            ogs_pkbuf_free(message->part[i].pkbuf);
    }
}