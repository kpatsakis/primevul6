static char *build_json(ogs_sbi_message_t *message)
{
    char *content = NULL;
    cJSON *item = NULL;

    ogs_assert(message);

    if (message->ProblemDetails) {
        item = OpenAPI_problem_details_convertToJSON(message->ProblemDetails);
        ogs_assert(item);
    } else if (message->NFProfile) {
        item = OpenAPI_nf_profile_convertToJSON(message->NFProfile);
        ogs_assert(item);
    } else if (message->PatchItemList) {
        OpenAPI_lnode_t *node = NULL;

        item = cJSON_CreateArray();
        ogs_assert(item);

        OpenAPI_list_for_each(message->PatchItemList, node) {
            cJSON *patchItem = OpenAPI_patch_item_convertToJSON(node->data);
            ogs_assert(patchItem);
            cJSON_AddItemToArray(item, patchItem);
        }
    } else if (message->SubscriptionData) {
        item = OpenAPI_subscription_data_convertToJSON(
                message->SubscriptionData);
        ogs_assert(item);
    } else if (message->NotificationData) {
        item = OpenAPI_notification_data_convertToJSON(
                message->NotificationData);
        ogs_assert(item);
    } else if (message->SearchResult) {
        item = OpenAPI_search_result_convertToJSON(message->SearchResult);
        ogs_assert(item);
    } else if (message->links) {
        item = ogs_sbi_links_convertToJSON(message->links);
        ogs_assert(item);
    } else if (message->AuthenticationInfo) {
        item = OpenAPI_authentication_info_convertToJSON(
                message->AuthenticationInfo);
        ogs_assert(item);
    } else if (message->AuthenticationInfoRequest) {
        item = OpenAPI_authentication_info_request_convertToJSON(
                message->AuthenticationInfoRequest);
        ogs_assert(item);
    } else if (message->AuthenticationInfoResult) {
        item = OpenAPI_authentication_info_result_convertToJSON(
                message->AuthenticationInfoResult);
        ogs_assert(item);
    } else if (message->AuthenticationSubscription) {
        item = OpenAPI_authentication_subscription_convertToJSON(
                message->AuthenticationSubscription);
        ogs_assert(item);
    } else if (message->UeAuthenticationCtx) {
        item = OpenAPI_ue_authentication_ctx_convertToJSON(
                message->UeAuthenticationCtx);
        ogs_assert(item);
    } else if (message->ConfirmationData) {
        item = OpenAPI_confirmation_data_convertToJSON(
                message->ConfirmationData);
        ogs_assert(item);
    } else if (message->ConfirmationDataResponse) {
        item = OpenAPI_confirmation_data_response_convertToJSON(
                message->ConfirmationDataResponse);
        ogs_assert(item);
    } else if (message->AuthEvent) {
        item = OpenAPI_auth_event_convertToJSON(message->AuthEvent);
        ogs_assert(item);
    } else if (message->Amf3GppAccessRegistration) {
        item = OpenAPI_amf3_gpp_access_registration_convertToJSON(
                message->Amf3GppAccessRegistration);
        ogs_assert(item);
    } else if (message->AccessAndMobilitySubscriptionData) {
        item = OpenAPI_access_and_mobility_subscription_data_convertToJSON(
                message->AccessAndMobilitySubscriptionData);
        ogs_assert(item);
    } else if (message->SmfSelectionSubscriptionData) {
        item = OpenAPI_smf_selection_subscription_data_convertToJSON(
                message->SmfSelectionSubscriptionData);
        ogs_assert(item);
    } else if (message->UeContextInSmfData) {
        item = OpenAPI_ue_context_in_smf_data_convertToJSON(
                message->UeContextInSmfData);
        ogs_assert(item);
    } else if (message->SmContextCreateData) {
        item = OpenAPI_sm_context_create_data_convertToJSON(
                message->SmContextCreateData);
        ogs_assert(item);
    } else if (message->SmContextCreatedData) {
        item = OpenAPI_sm_context_created_data_convertToJSON(
                message->SmContextCreatedData);
        ogs_assert(item);
    } else if (message->SmContextCreateError) {
        item = OpenAPI_sm_context_create_error_convertToJSON(
                message->SmContextCreateError);
        ogs_assert(item);
    } else if (message->SmContextUpdateData) {
        item = OpenAPI_sm_context_update_data_convertToJSON(
                message->SmContextUpdateData);
        ogs_assert(item);
    } else if (message->SmContextUpdatedData) {
        item = OpenAPI_sm_context_updated_data_convertToJSON(
                message->SmContextUpdatedData);
        ogs_assert(item);
    } else if (message->SmContextUpdateError) {
        item = OpenAPI_sm_context_update_error_convertToJSON(
                message->SmContextUpdateError);
        ogs_assert(item);
    } else if (message->SmContextReleaseData) {
        item = OpenAPI_sm_context_release_data_convertToJSON(
                message->SmContextReleaseData);
        ogs_assert(item);
    } else if (message->SmContextReleasedData) {
        item = OpenAPI_sm_context_released_data_convertToJSON(
                message->SmContextReleasedData);
        ogs_assert(item);
    } else if (message->SessionManagementSubscriptionData) {
        item = OpenAPI_session_management_subscription_data_convertToJSON(
                message->SessionManagementSubscriptionData);
        ogs_assert(item);
    } else if (message->N1N2MessageTransferReqData) {
        item = OpenAPI_n1_n2_message_transfer_req_data_convertToJSON(
                message->N1N2MessageTransferReqData);
        ogs_assert(item);
    } else if (message->N1N2MessageTransferRspData) {
        item = OpenAPI_n1_n2_message_transfer_rsp_data_convertToJSON(
                message->N1N2MessageTransferRspData);
        ogs_assert(item);
    } else if (message->N1N2MsgTxfrFailureNotification) {
        item = OpenAPI_n1_n2_msg_txfr_failure_notification_convertToJSON(
                message->N1N2MsgTxfrFailureNotification);
        ogs_assert(item);
    } else if (message->SmContextStatusNotification) {
        item = OpenAPI_sm_context_status_notification_convertToJSON(
                message->SmContextStatusNotification);
        ogs_assert(item);
    } else if (message->PolicyAssociationRequest) {
        item = OpenAPI_policy_association_request_convertToJSON(
                message->PolicyAssociationRequest);
        ogs_assert(item);
    } else if (message->PolicyAssociation) {
        item = OpenAPI_policy_association_convertToJSON(
                message->PolicyAssociation);
        ogs_assert(item);
    } else if (message->AmPolicyData) {
        item = OpenAPI_am_policy_data_convertToJSON(message->AmPolicyData);
        ogs_assert(item);
    } else if (message->SmPolicyContextData) {
        item = OpenAPI_sm_policy_context_data_convertToJSON(
                message->SmPolicyContextData);
        ogs_assert(item);
    } else if (message->SmPolicyDecision) {
        item = OpenAPI_sm_policy_decision_convertToJSON(
                message->SmPolicyDecision);
        ogs_assert(item);
    } else if (message->SmPolicyData) {
        item = OpenAPI_sm_policy_data_convertToJSON(message->SmPolicyData);
        ogs_assert(item);
    } else if (message->SmPolicyDeleteData) {
        item = OpenAPI_sm_policy_delete_data_convertToJSON(
                message->SmPolicyDeleteData);
        ogs_assert(item);
    } else if (message->AuthorizedNetworkSliceInfo) {
        item = OpenAPI_authorized_network_slice_info_convertToJSON(
                message->AuthorizedNetworkSliceInfo);
        ogs_assert(item);
    } else if (message->PcfBinding) {
        item = OpenAPI_pcf_binding_convertToJSON(message->PcfBinding);
        ogs_assert(item);
    } else if (message->AppSessionContext) {
        item = OpenAPI_app_session_context_convertToJSON(
                message->AppSessionContext);
        ogs_assert(item);
    } else if (message->AppSessionContextUpdateDataPatch) {
        item = OpenAPI_app_session_context_update_data_patch_convertToJSON(
                message->AppSessionContextUpdateDataPatch);
        ogs_assert(item);
    } else if (message->SmPolicyNotification) {
        item = OpenAPI_sm_policy_notification_convertToJSON(
                message->SmPolicyNotification);
        ogs_assert(item);
    } else if (message->TerminationNotification) {
        item = OpenAPI_termination_notification_convertToJSON(
                message->TerminationNotification);
        ogs_assert(item);
    }

    if (item) {
        content = cJSON_Print(item);
        ogs_assert(content);
        ogs_log_print(OGS_LOG_TRACE, "%s", content);
        cJSON_Delete(item);
    }

    return content;
}