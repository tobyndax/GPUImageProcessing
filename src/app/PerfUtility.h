#pragma once
#include <string>
#include <functional>
#include <chrono>
#include <vector>

class PerfUtility {
public:
	PerfUtility();
	~PerfUtility();
	
	void printTotalTime();
	void measureFunction(std::function<void (void)>, std::string functionName);
	void printTimeSumBetween(int startInd, int endInd = 0);
	void reset();
private: 
	std::vector<long long> durations = std::vector <long long>();
};