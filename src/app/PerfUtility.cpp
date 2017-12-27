#include "PerfUtility.h"
#include <chrono>
#include <iostream>
#include <numeric>
PerfUtility::PerfUtility()
{

}
PerfUtility::~PerfUtility()
{

}

void PerfUtility::printTimeSumBetween(int startInd, int endInd)
{
	long long totalDuration;
	if (endInd == 0) {
		totalDuration = std::accumulate(durations.begin() + startInd, durations.end(), 0.0);
	}
	else {
		totalDuration = std::accumulate(durations.begin() + startInd, durations.begin() + endInd, 0.0);
	}
	std::cout << "Sub- execution time: ";
	std::cout << totalDuration / 1000.0f << "ms" << std::endl;
}
void PerfUtility::printTotalTime() {
	long long totalDuration = std::accumulate(durations.begin(), durations.end(), 0.0);
	std::cout << "Total execution time: ";
	std::cout <<  totalDuration / 1000.0f << "ms" << std::endl;
}

void PerfUtility::measureFunction(std::function<void (void)> func, std::string functionName)
{
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	func();

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	durations.push_back(duration);

	std::cout << "Execution of function " + functionName + " took: ";

	std::cout << duration / 1000.0f << "ms" << std::endl;
}

void PerfUtility::reset() {
	durations.clear();
}