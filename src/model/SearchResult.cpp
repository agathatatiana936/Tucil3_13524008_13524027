#include "model/SearchResult.hpp"
#include <iostream>

SearchResult::SearchResult()
    : solutionFound(false), totalCost(0), executionTimeMs(0.0), iterationsCount(0) {}

SearchResult::SearchResult(bool found, const std::string& path, int cost,
                           double timeMs, int iterations, const std::string& message)
    : solutionFound(found), solutionPath(path), totalCost(cost),
      executionTimeMs(timeMs), iterationsCount(iterations), message(message) {}

bool SearchResult::isSolutionFound() const { return solutionFound; }

const std::string& SearchResult::getSolutionPath() const { return solutionPath; }

int SearchResult::getTotalCost() const { return totalCost; }

double SearchResult::getExecutionTimeMs() const { return executionTimeMs; }

int SearchResult::getIterationsCount() const { return iterationsCount; }

const std::string& SearchResult::getMessage() const { return message; }

void SearchResult::setSolutionFound(bool found) { solutionFound = found; }

void SearchResult::setSolutionPath(const std::string& path) { solutionPath = path; }

void SearchResult::setTotalCost(int cost) { totalCost = cost; }

void SearchResult::setExecutionTimeMs(double timeMs) { executionTimeMs = timeMs; }

void SearchResult::setIterationsCount(int iterations) { iterationsCount = iterations; }

void SearchResult::setMessage(const std::string& msg) { message = msg; }

void SearchResult::printSummary() const {
    std::cout << "========================================\n";
    if (solutionFound) {
        std::cout << "Solusi Yang Ditemukan : " << solutionPath << '\n';
        std::cout << "Cost dari Solusi      : " << totalCost << '\n';
    } else {
        std::cout << "Solusi tidak ditemukan.\n";
    }
    std::cout << "Waktu eksekusi        : " << executionTimeMs << " ms\n";
    std::cout << "Banyak iterasi        : " << iterationsCount << " iterasi\n";
    if (!message.empty()) {
        std::cout << "Pesan                 : " << message << '\n';
    }
    std::cout << "========================================\n";
}
