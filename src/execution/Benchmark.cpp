#include "ForgeSim/execution/Benchmark.hpp"

#include <iomanip>
#include <sstream>

Benchmark::Benchmark()
{
    start();
}

void Benchmark::start()
{
    startTime = std::chrono::high_resolution_clock::now();
}

double Benchmark::stopMilliseconds()
{
    const auto endTime =
        std::chrono::high_resolution_clock::now();

    const std::chrono::duration<double, std::milli> elapsed =
        endTime - startTime;

    return elapsed.count();
}

std::string Benchmark::formatMilliseconds(double milliseconds)
{
    std::ostringstream output;

    output << std::fixed
           << std::setprecision(3)
           << milliseconds
           << " ms";

    return output.str();
}