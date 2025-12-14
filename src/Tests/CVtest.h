//
// Created by 76426 on 2025/12/14.
//

#ifndef XTEST_CVTEST_H
#define XTEST_CVTEST_H

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <iostream>
#include <vector>
#include <chrono>

// 测试 OpenCV 核心功能
void test_opencv_core() {
    std::cout << "=== OpenCV Core Module Test ===" << std::endl;
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

    // 1. 基础矩阵操作
    std::cout << "\n1. 基础矩阵操作:" << std::endl;

    // 创建矩阵
    cv::Mat mat1 = cv::Mat::eye(3, 3, CV_32F);
    cv::Mat mat2 = cv::Mat::ones(3, 3, CV_32F) * 2.5f;

    std::cout << "单位矩阵:\n" << mat1 << std::endl;
    std::cout << "常数矩阵:\n" << mat2 << std::endl;

    // 矩阵运算
    cv::Mat add_result = mat1 + mat2;
    cv::Mat mul_result = mat1 * mat2;
    cv::Mat dot_result;
    cv::multiply(mat1, mat2, dot_result);

    std::cout << "矩阵相加:\n" << add_result << std::endl;
    std::cout << "矩阵相乘:\n" << mul_result << std::endl;
    std::cout << "矩阵点乘:\n" << dot_result << std::endl;

    // 2. 矩阵属性
    std::cout << "\n2. 矩阵属性:" << std::endl;
    cv::Mat img = cv::Mat::zeros(100, 200, CV_8UC3);
    std::cout << "尺寸: " << img.size() << std::endl;
    std::cout << "行数: " << img.rows << std::endl;
    std::cout << "列数: " << img.cols << std::endl;
    std::cout << "通道数: " << img.channels() << std::endl;
    std::cout << "类型: " << cv::typeToString(img.type()) << std::endl;
    std::cout << "数据类型: " << cv::depthToString(img.depth()) << std::endl;

    // 3. 访问像素
    std::cout << "\n3. 像素访问:" << std::endl;
    cv::Mat small(3, 3, CV_8UC3, cv::Scalar(0, 0, 255));

    // 使用 at 方法
    small.at<cv::Vec3b>(1, 1) = cv::Vec3b(255, 0, 0);
    std::cout << "3x3 BGR矩阵:\n";
    for (int i = 0; i < small.rows; i++) {
        for (int j = 0; j < small.cols; j++) {
            cv::Vec3b pixel = small.at<cv::Vec3b>(i, j);
            std::cout << "(" << (int)pixel[2] << ","
                      << (int)pixel[1] << ","
                      << (int)pixel[0] << ") ";
        }
        std::cout << std::endl;
    }

    // 使用 ptr 方法（更高效）
    uchar* ptr = small.ptr<uchar>(0);
    ptr[0] = 100;  // B
    ptr[1] = 150;  // G
    ptr[2] = 200;  // R

    // 4. 矩阵变形和操作
    std::cout << "\n4. 矩阵变形:" << std::endl;
    cv::Mat src = (cv::Mat_<float>(2, 3) << 1, 2, 3, 4, 5, 6);
    std::cout << "原始矩阵:\n" << src << std::endl;

    // 转置
    cv::Mat transposed;
    cv::transpose(src, transposed);
    std::cout << "转置矩阵:\n" << transposed << std::endl;

    // 5. 基本算法
    std::cout << "\n5. 基本算法:" << std::endl;
    cv::Mat A = (cv::Mat_<double>(2, 2) << 4, 1, 2, 3);
    cv::Mat B = (cv::Mat_<double>(2, 1) << 1, 2);
    cv::Mat X;

    // 6. 矩阵统计
    std::cout << "\n6. 矩阵统计:" << std::endl;
    cv::Mat random_mat = cv::Mat::zeros(5, 5, CV_32F);
    cv::randu(random_mat, 0.0f, 1.0f);
    std::cout << "随机矩阵:\n" << random_mat << std::endl;

    double min_val, max_val;
    cv::Point min_loc, max_loc;
    cv::minMaxLoc(random_mat, &min_val, &max_val, &min_loc, &max_loc);
    std::cout << "最小值: " << min_val << " 位置: " << min_loc << std::endl;
    std::cout << "最大值: " << max_val << " 位置: " << max_loc << std::endl;

    cv::Scalar mean, stddev;
    cv::meanStdDev(random_mat, mean, stddev);
    std::cout << "平均值: " << mean[0] << std::endl;
    std::cout << "标准差: " << stddev[0] << std::endl;

    // 7. 性能测试
    std::cout << "\n7. 性能测试:" << std::endl;
    cv::Mat large_mat(1000, 1000, CV_32F);
    cv::randu(large_mat, 0.0f, 100.0f);

    auto start = std::chrono::high_resolution_clock::now();
    cv::Mat result;
    cv::exp(large_mat, result);  // 计算指数
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "对1000x1000矩阵计算指数耗时: " << duration.count() << " ms" << std::endl;

    // 8. 数据类型转换
    std::cout << "\n8. 数据类型转换:" << std::endl;
    cv::Mat int_mat = (cv::Mat_<int>(2, 2) << 100, 200, 300, 400);
    std::cout << "整数矩阵:\n" << int_mat << std::endl;

    cv::Mat float_mat;
    int_mat.convertTo(float_mat, CV_32F, 1.0/255.0);
    std::cout << "转换为浮点矩阵:\n" << float_mat << std::endl;

    // 9. 特殊矩阵
    std::cout << "\n9. 特殊矩阵:" << std::endl;
    cv::Mat diag_mat = cv::Mat::diag(cv::Mat::ones(5, 1, CV_32F));
    std::cout << "对角矩阵:\n" << diag_mat << std::endl;

    cv::Mat eye_mat = cv::Mat::eye(4, 4, CV_32F);
    std::cout << "单位矩阵:\n" << eye_mat << std::endl;

    cv::Mat zeros_mat = cv::Mat::zeros(3, 3, CV_32FC3);
    std::cout << "三通道零矩阵大小: " << zeros_mat.size() << std::endl;

    // 10. 矩阵比较和逻辑运算
    std::cout << "\n10. 矩阵比较和逻辑运算:" << std::endl;
    cv::Mat m1 = (cv::Mat_<uchar>(2, 2) << 10, 20, 30, 40);
    cv::Mat m2 = (cv::Mat_<uchar>(2, 2) << 20, 20, 20, 50);
    cv::Mat cmp_result;

    cv::compare(m1, m2, cmp_result, cv::CMP_GT);  // 大于比较
    std::cout << "矩阵比较(大于):\n" << cmp_result << std::endl;

    // 11. 随机数生成
    std::cout << "\n11. 随机数生成:" << std::endl;
    cv::RNG rng(12345);  // 固定种子
    std::cout << "随机整数(0-100): " << rng.uniform(0, 100) << std::endl;
    std::cout << "随机浮点数(0-1): " << rng.uniform(0.0f, 1.0f) << std::endl;

    cv::Mat random_gaussian(1, 10, CV_32F);
    rng.fill(random_gaussian, cv::RNG::NORMAL, 0.0, 1.0);
    std::cout << "高斯随机数: " << random_gaussian << std::endl;


    // 13. 实用工具函数
    std::cout << "\n13. 实用工具函数:" << std::endl;
    std::cout << "获取构建信息:\n" << cv::getBuildInformation() << std::endl;

    int64 tick_count = cv::getTickCount();
    // 模拟一些工作
    cv::Mat temp(100, 100, CV_32F);
    cv::randu(temp, 0.0f, 1.0f);
    double freq = cv::getTickFrequency();
    int64 tick_count_end = cv::getTickCount();

    double elapsed = (tick_count_end - tick_count) / freq;
    std::cout << "耗时: " << elapsed * 1000 << " ms" << std::endl;

}

#endif //XTEST_CVTEST_H
