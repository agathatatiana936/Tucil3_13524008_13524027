#pragma once

#include <string>

class SearchResult {
private:
    bool solutionFound;
    std::string solutionPath;
    int totalCost;
    double executionTimeMs;
    int iterationsCount;
    std::string message;

public:
    SearchResult();
    SearchResult(bool found, const std::string& path, int cost,
                 double timeMs, int iterations, const std::string& message);
    ~SearchResult() = default;

    bool isSolutionFound() const;
    const std::string& getSolutionPath() const;
    int getTotalCost() const;
    double getExecutionTimeMs() const;
    int getIterationsCount() const;
    const std::string& getMessage() const;

    void setSolutionFound(bool found);
    void setSolutionPath(const std::string& path);
    void setTotalCost(int cost);
    void setExecutionTimeMs(double timeMs);
    void setIterationsCount(int iterations);
    void setMessage(const std::string& message);

    void printSummary() const;
};
