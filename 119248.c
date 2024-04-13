TEST(DenseOpticalFlow_DIS, ReferenceAccuracy)
{
    Mat frame1, frame2, GT;
    ASSERT_TRUE(readRubberWhale(frame1, frame2, GT));
    int presets[] = {DISOpticalFlow::PRESET_ULTRAFAST, DISOpticalFlow::PRESET_FAST, DISOpticalFlow::PRESET_MEDIUM};
    float target_RMSE[] = {0.86f, 0.74f, 0.49f};
    cvtColor(frame1, frame1, COLOR_BGR2GRAY);
    cvtColor(frame2, frame2, COLOR_BGR2GRAY);

    Ptr<DenseOpticalFlow> algo;

    // iterate over presets:
    for (int i = 0; i < 3; i++)
    {
        Mat flow;
        algo = DISOpticalFlow::create(presets[i]);
        algo->calc(frame1, frame2, flow);
        ASSERT_EQ(GT.rows, flow.rows);
        ASSERT_EQ(GT.cols, flow.cols);
        EXPECT_LE(calcRMSE(GT, flow), target_RMSE[i]);
    }
}