bool readRubberWhale(Mat &dst_frame_1, Mat &dst_frame_2, Mat &dst_GT)
{
    const string frame1_path = getRubberWhaleFrame1();
    const string frame2_path = getRubberWhaleFrame2();
    const string gt_flow_path = getRubberWhaleGroundTruth();

    dst_frame_1 = imread(frame1_path);
    dst_frame_2 = imread(frame2_path);
    dst_GT = readOpticalFlow(gt_flow_path);

    if (dst_frame_1.empty() || dst_frame_2.empty() || dst_GT.empty())
        return false;
    else
        return true;
}