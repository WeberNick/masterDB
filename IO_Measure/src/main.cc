#include "measure.hh"

#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>

#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

int main(const int argc, const char* argv[])
{
	bool lReadMode;
	std::string lOutputFile = argv[1];
	/* Check if user input is valid */
	if(argc != 3)
	{
		std::cerr << "Usage: './main [PathToFile] [MODE]' where 'MODE' can either be 'read' or 'write'" << std::endl;
		return 0;
	}
	else if(!(strcmp(argv[2], "read") == 0 || strcmp(argv[2], "write") == 0)) //C functions
	{
		std::cerr << "Invalid mode. Only 'read' or 'write' allowed!" << std::endl;
		return 0;
	}
	else if(strcmp(argv[2], "read") == 0)
	{
		lReadMode = true;
		std::cout << "Read Mode!" << std::endl;
	}
	else
	{
		lReadMode = false;
		lOutputFile.append("Out");
		std::cout << "Write Mode!" << std::endl;
	}

	/* Initialize helper variables */
	const char* C_RESULT_FILE_NAME = (lReadMode) ? "read.txt" : "write.txt";
	const int C_MODE = (lReadMode) ? O_RDONLY : O_CREAT;
	const size_t C_FILE_SIZE_IN_BYTES = 1024 * 2000000;
	const size_t C_BUF_SIZE_IN_BYTES = 8192;
	if(C_FILE_SIZE_IN_BYTES % C_BUF_SIZE_IN_BYTES != 0)
	{
		std::cerr << "Buffer size must be a factor of the file size" << std::endl;
		return -1;
	}
	const size_t C_NUMB_ITERS = C_FILE_SIZE_IN_BYTES / C_BUF_SIZE_IN_BYTES;
	const size_t C_NUMB_RUNS = 10;


	std::vector<double> lMeasurements;
	std::cout << "Measurement is repeated " << C_NUMB_RUNS << " times." << std::endl;
	for(size_t runs = 0; runs < C_NUMB_RUNS; ++runs)
	{
		std::cout << "Run (" << (runs + 1) << "/" << C_NUMB_RUNS << ")" << std::endl;
		int fdescr;
		if((fdescr = open(lOutputFile.c_str(), C_MODE)) == -1)
		{
			std::cerr << "Can not open '" << lOutputFile << "'" << std::endl;
			return -1;
		}
		else
		{
			char lBuffer[C_BUF_SIZE_IN_BYTES];
			for(size_t i = 0; i < C_BUF_SIZE_IN_BYTES; ++i)
			{
				lBuffer[i] = 'X';
			}
		
			Measure lMeasure;
			if(lReadMode)
			{
				lMeasure.start();
				for(size_t i = 0; i < C_NUMB_ITERS; i++)
				{
					read(fdescr, &lBuffer, C_BUF_SIZE_IN_BYTES);
				}
				lMeasure.stop();
			}
			else
			{
				lMeasure.start();
				for(size_t i = 0; i < C_NUMB_ITERS; i++)
				{
					write(fdescr, &lBuffer, C_BUF_SIZE_IN_BYTES);
				}
				lMeasure.stop();
			}
			lMeasurements.push_back(lMeasure.mTotalTime());
	  		

	  		std::ofstream lResultFile;
			lResultFile.open(C_RESULT_FILE_NAME, std::ofstream::out | std::ofstream::trunc);

	  		double avg = 0;
	  		const size_t lNoMeasurements = lMeasurements.size();
	  		for(size_t i = 0; i < lNoMeasurements; ++i)
	  		{
	  			avg += lMeasurements[i];
	  			lResultFile << "Measurement " << i << ": " << std::fixed << std::setprecision(3) << lMeasurements[i] << std::endl;
	  		}

	  		lResultFile << "Avergae: " << std::fixed << std::setprecision(3) << (avg / lNoMeasurements) << std::endl;
		  	lResultFile.close();

			if(close(fdescr) == -1)
			{
				std::cerr << "An error occured while closing the file!" << std::endl;
				return -1;
			}
			std::cout << "File closed successfully!" << std::endl;
		}
	}
	return 0;
}