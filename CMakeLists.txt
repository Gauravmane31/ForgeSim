cmake_minimum_required(VERSION 3.16)

project(ForgeSim
    VERSION 0.1.0
    DESCRIPTION "Incremental Parallel Engineering Simulation Engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------------------------
# Main executable
# --------------------------------------------------

add_executable(forgesim
    src/main.cpp

    src/core/Node.cpp
    src/core/Graph.cpp
    src/core/Result.cpp
    src/core/ConstantComputation.cpp
    src/core/MultiplyComputation.cpp
    src/core/DivideComputation.cpp
    src/core/ImpactAnalyzer.cpp
    src/core/BenchmarkGraph.cpp
    src/core/WorkComputation.cpp

    src/execution/Scheduler.cpp
    src/execution/ThreadPool.cpp
    src/execution/CacheManager.cpp
    src/execution/ExecutionMetrics.cpp
    src/execution/Benchmark.cpp
)

target_include_directories(forgesim
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# --------------------------------------------------
# Testing
# --------------------------------------------------

include(CTest)

if(BUILD_TESTING)

    find_package(GTest REQUIRED)

    add_executable(forgesim_tests

        tests/NodeTest.cpp
        tests/GraphTest.cpp
        tests/ComputationTest.cpp
        tests/CacheManagerTest.cpp
        tests/ImpactAnalyzerTest.cpp
        tests/SchedulerTest.cpp

        src/core/Node.cpp
        src/core/Graph.cpp
        src/core/Result.cpp
        src/core/ConstantComputation.cpp
        src/core/MultiplyComputation.cpp
        src/core/DivideComputation.cpp
        src/core/ImpactAnalyzer.cpp

        src/execution/Scheduler.cpp
        src/execution/ThreadPool.cpp
        src/execution/CacheManager.cpp
        src/execution/ExecutionMetrics.cpp
    )

    target_include_directories(forgesim_tests
        PRIVATE
            ${PROJECT_SOURCE_DIR}/include
    )

    target_link_libraries(forgesim_tests
        PRIVATE
            GTest::gtest_main
    )

    include(GoogleTest)

    gtest_discover_tests(forgesim_tests)

endif()