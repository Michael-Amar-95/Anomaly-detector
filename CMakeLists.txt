cmake_minimum_required(VERSION 3.17)
project(ass_6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -pthread)
find_package(Threads)

add_executable(ass_6
        anomaly_detection_util.cpp
        anomaly_detection_util.h
        AnomalyDetector.h
        CLI.h
        CLI.cpp
        commands.h
        HybridAnomalyDetector.h
        HybridAnomalyDetector.cpp
        MainTrain.cpp
        minCircle.cpp
        minCircle.h
        Server.cpp
        Server.h
        SimpleAnomalyDetector.cpp
        SimpleAnomalyDetector.h
        timeseries.cpp
        timeseries.h)
