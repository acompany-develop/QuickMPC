#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include "gtest/gtest.h"
#include "Math/Math.hpp"
#include "Share/Share.hpp"
#include "Server/ComputationToComputationContainer/Server.hpp"
#include "Job/Jobs/MeshCodeJob.hpp"

#include "LogHeader/Logger.hpp"

TEST(MeshCodeTest, BulkMeshCode)
{
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    int N = 100;
    double latitude_d = 35.70078;
    double longitude_d = 139.7147499;
    std::vector<Share> latitude(N, Share(FixedPoint(std::to_string(latitude_d / n_parties))));
    std::vector<Share> longitude(N, Share(FixedPoint(std::to_string(longitude_d / n_parties))));

    double a = fmod(latitude_d * 60, 40);
    double p = (latitude_d * 60 - a) / 40;
    double b = fmod(a, 5);
    double q = (a - b) / 5;
    double c = fmod(b * 60, 30);
    double r = (b * 60 - c) / 30;
    double d = fmod(c, 15);
    double s = (c - d) / 15;
    double e = fmod(d, 7.5);
    double t = (d - e) / 7.5;

    double f = fmod(longitude_d - 100, 1.0);
    double u = longitude_d - 100 - f;
    double g = fmod(f * 60, 7.5);
    double v = (f * 60 - g) / 7.5;
    double h = fmod(g * 60, 45);
    double w = (g * 60 - h) / 45;
    double i = fmod(h, 22.5);
    double x = (h - i) / 22.5;
    double j = fmod(i, 11.25);
    double y = (i - j) / 11.25;

    double m = (s * 2) + (x + 1);
    double n = (t * 2) + (y + 1);

    std::vector<std::vector<double>> expected(N, std::vector<double>{p, u, q, v, r, w, m, n});

    double error = 0.001;

    std::chrono::system_clock::time_point start, end;  // 型は auto で可
    start = std::chrono::system_clock::now();          // 計測開始時間

    auto func = qmpc::Job::MeshCodeFunction(latitude, longitude);
    std::vector<std::vector<Share>> ret;
    ret.reserve(N);
    ret = func.meshcode_transform();

    end = std::chrono::system_clock::now();  // 計測終了時間
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                         .count();  //処理に要した時間をミリ秒に変換
    spdlog::info("meshcode took {0} ms.", elapsed);

    for (const auto &meshcode : ret)
    {
        open(meshcode);
        std::vector<FixedPoint> result = recons(meshcode);
        for (int i = 0; i < N; ++i)
        {
            EXPECT_NEAR(result[0].getDoubleVal(), expected[i][0], error);
            EXPECT_NEAR(result[1].getDoubleVal(), expected[i][1], error);
            EXPECT_NEAR(result[2].getDoubleVal(), expected[i][2], error);
            EXPECT_NEAR(result[3].getDoubleVal(), expected[i][3], error);
            EXPECT_NEAR(result[4].getDoubleVal(), expected[i][4], error);
            EXPECT_NEAR(result[5].getDoubleVal(), expected[i][5], error);
            EXPECT_NEAR(result[6].getDoubleVal(), expected[i][6], error);
            EXPECT_NEAR(result[7].getDoubleVal(), expected[i][7], error);
        }
    }
}