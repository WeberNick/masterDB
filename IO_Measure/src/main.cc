#include "measure.hh"

#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#ifdef __linux__
#define O_NOCACHE O_DIRECT
#elif __APPLE__
#define O_NOCACHE F_NOCACHE
#endif

int main(const int argc, const char* argv[])
{
	/* Check if user input has enough arguments */
	if(argc != 4)
	{
		std::cerr 	<< "==================================================\n"
					<< "Usage:\t'./main [CREATE/PATH] [MODE] [STATUS FLAG]'\n\n"
					<< "\t-where 'CREATE/PATH' can be either 'create' to create a 2GB sized file in the home directory and use this file\n"
					<< "\t\tOR a path from the current directory to a file already created\n"
					<< "\t-where 'MODE' can either be 'read' or 'write'\n"
					<< "\t-where 'STATUS FLAG' can either be 'simple' or 'direct' for simple/direct IO\n"
					<< "==================================================\n"
					<< std::endl;
		return -1;
	}

	const bool lCreate = (std::string(argv[1]) == "create") ? true : false ;
	const std::string lMode = argv[2];
	const std::string lStatusFlag = argv[3];

	/* Creating the dummy file */
	if(lCreate)
	{
		std::cout << "A 2GB sized dummy file is created..." << std::endl;
		system("dd if=/dev/zero of=$HOME/BigFile bs=1024 count=2000000");
		system("echo A 2GB sized dummy file was created in the home directory: $HOME");
	}

	/* Check if user input is valid */
	if((lMode != "read") && (lMode != "write"))
	{
		std::cerr << "Invalid mode. Only 'read' or 'write' are allowed!" << std::endl;
		return -1;
	}
	else if((lStatusFlag != "simple") && (lStatusFlag != "direct"))
	{
		std::cerr << "Invalid status flag. Only 'simple' or 'direct' are allowed!" << std::endl;
		return -1;
	}

	std::string temp = (lCreate) ? std::string(getenv("HOME")) + "/BigFile" : argv[1];
	const char* C_FILE_NAME =  (lMode == "read") ? temp.c_str() : temp.append("Out").c_str();

	std::cout << "Path to file: '" <<  C_FILE_NAME << "'\n"
				<< "Selected mode: '" << lMode << "'\n"
				<< "Status flag: '" << lStatusFlag << "'\n"
				<< std::endl;

	/* Initialize helper variables */
	const int C_MODE = (lMode == "read") ? 
							((lStatusFlag == "simple") ? O_RDONLY | O_SYNC : O_RDONLY | O_NOCACHE | O_SYNC ) : 
							((lStatusFlag == "simple") ? O_WRONLY | O_CREAT | O_SYNC : O_WRONLY | O_CREAT | O_NOCACHE | O_SYNC );
	const char* C_RESULT_FILE_NAME = (lMode == "read") ? "read.txt" : "write.txt";
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
		if((fdescr = open(C_FILE_NAME, C_MODE, 0644)) == -1)
		{
			std::cerr << "Can not open '" << C_FILE_NAME << "'" << std::endl;
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
			if(lMode == "read")
			{
				lMeasure.start();
				for(size_t i = 0; i < C_NUMB_ITERS; i++)
				{
					size_t lRandomOffset = (rand() % C_NUMB_ITERS) * C_BUF_SIZE_IN_BYTES;
					// read(fdescr, &lBuffer, C_BUF_SIZE_IN_BYTES);
					pread(fdescr, &lBuffer, C_BUF_SIZE_IN_BYTES, lRandomOffset);
				}
				lMeasure.stop();
			}
			else
			{
				lMeasure.start();
				for(size_t i = 0; i < C_NUMB_ITERS; i++)
				{
					size_t lRandomOffset = (rand() % C_NUMB_ITERS) * C_BUF_SIZE_IN_BYTES;
					// write(fdescr, &lBuffer, C_BUF_SIZE_IN_BYTES);
					pwrite(fdescr, &lBuffer, C_BUF_SIZE_IN_BYTES, lRandomOffset);
				}
				lMeasure.stop();
			}
			lMeasurements.push_back(lMeasure.mTotalTime());

			if(close(fdescr) == -1)
			{
				std::cerr << "An error occured while closing the file!" << std::endl;
				return -1;
			}
			std::cout << "File closed successfully!" << std::endl;
		}
	}

	/* print measurement results to file */
	std::ofstream lResultFile;
	lResultFile.open(C_RESULT_FILE_NAME, std::ofstream::out | std::ofstream::trunc);

	double avg = 0;
	const size_t lNoMeasurements = lMeasurements.size();
	for(size_t i = 0; i < lNoMeasurements; ++i)
	{
		avg += lMeasurements[i];
		lResultFile << "Measurement " << i << ": " << std::fixed << std::setprecision(3) << lMeasurements[i] << std::endl;
	}
	lResultFile << "Average: " << std::fixed << std::setprecision(3) << (avg / lNoMeasurements) << std::endl;
	lResultFile.close();

	return 0;
}