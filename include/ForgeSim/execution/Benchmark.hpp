#pragma once

#include <chrono>
#include <string>

class Benchmark
{
public:
    Benchmark();

    void start();

    double stopMilliseconds();

    static std::string formatMilliseconds(double milliseconds);

private:
    std::chrono::high_resolution_clock::time_point startTime;
};