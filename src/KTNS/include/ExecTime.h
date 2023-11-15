#ifndef __EXECTIME_H__
#define __EXECTIME_H__

#include <chrono>
#include <iostream>
#include <string>
#include <fstream> 

class ExecTime{
	private:
		std::chrono::high_resolution_clock::time_point start;
		std::string filename;

	public:
		ExecTime(std::string filename_);
		void getTime();
		~ExecTime();
};

#endif
