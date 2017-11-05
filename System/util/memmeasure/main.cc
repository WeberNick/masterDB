#include "measure.hh"

#include "immintrin.h"
#include <stdlib.h>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>

typedef std::vector<double> double_vt;
const size_t G_NUMBER_OF_TUPLES = 1000000;
const size_t G_NO_ATTR = 100;
const size_t G_ATTR_SIZE = sizeof(uint32_t);
const size_t G_TUPLE_SIZE = G_NO_ATTR * G_ATTR_SIZE;
const size_t G_CHUNK_SIZE = G_NUMBER_OF_TUPLES * G_TUPLE_SIZE;
const size_t G_ALIGNMENT = 32;
const std::string G_PATH = "data/";

const double NS = (1000.0L * 1000.0L * 1000.0L);

double secToNanoSec(const double aTime, const double aNumber) 
{
	return (aTime / aNumber) * NS;
}

void printSettings()
{
	std::cout << "Numb. of Tuples: " << G_NUMBER_OF_TUPLES << std::endl;
	std::cout << "Numb. of Attr.:  " << G_NO_ATTR << std::endl;
	std::cout << "Attribute Size:  " << G_ATTR_SIZE << " bytes" << std::endl;
	std::cout << "Tuple Size:      " << G_TUPLE_SIZE << " bytes" << std::endl;
	std::cout << "Alignment:       " << G_ALIGNMENT << " bytes" << std::endl;
}

void allocateMemory(std::byte*& aInputChunk, std::byte*& aOutputChunk)
{
	if(posix_memalign((void**)&aInputChunk, G_ALIGNMENT, G_CHUNK_SIZE) != 0)
	{
		std::cerr << "An error ocurred while allocating memory for the input chunk" << std::endl;
	}
	if(posix_memalign((void**)&aOutputChunk, G_ALIGNMENT, G_CHUNK_SIZE) != 0)
	{
		std::cerr << "An error ocurred while allocating memory for the output chunk" << std::endl;
	}
}

void freeMemory(std::byte*& aInputChunk, std::byte*& aOutputChunk)
{
	free(aInputChunk);
	aInputChunk = nullptr;
	free(aOutputChunk);
	aOutputChunk = nullptr;
}

void fillInputChunkWithData(std::byte* aInputChunk)
{
	uint32_t lCounter = 1;
	for(size_t i = 0; i < G_NUMBER_OF_TUPLES * G_NO_ATTR; ++i)
	{
		*((uint32_t*)aInputChunk) = lCounter++;
		aInputChunk += G_ATTR_SIZE;
	}
}

void printToFile(const double_vt& aResult, const std::string aFileName)
{
	std::ofstream out;
	std::string lPathToFile = G_PATH + aFileName;
	std::cout << "Write results in file '" << aFileName << "' at '" << lPathToFile << "'" << std::endl;
	out.open(lPathToFile.c_str(), std::ios::out | std::ios::trunc);
	if(out.is_open())
	{
		for(size_t i = 0; i < aResult.size(); ++i)
		{
			out << (i+1) << ' ' << std::setprecision(3) << std::fixed << aResult[i] << std::endl;
		}
		out.close();
	}
}

void measure_ManualWrites(std::byte* const aInputChunk, std::byte* const aOutputChunk)
{
	std::cout << "Manual Writes" << std::endl;
	double_vt lMeasureResults(G_NO_ATTR, 0);

	std::byte* lInPointer;
	std::byte* lOutPointer;

	for(size_t lNoAttr = 0; lNoAttr < G_NO_ATTR; ++lNoAttr)
	{	
		std::cout << "Attribute " << (lNoAttr+1) << "/" << G_NO_ATTR << std::endl;
		Measure lMeasure;
		lMeasure.start();
		for(size_t i = 0; i < G_NUMBER_OF_TUPLES; ++i)
		{	
			lInPointer = aInputChunk + (i * G_TUPLE_SIZE);
			lOutPointer = aOutputChunk + (i * G_TUPLE_SIZE);
			for(size_t j = 0; j <= lNoAttr; ++j)
			{
				*((uint32_t*)(lOutPointer)) = *((uint32_t*)(lInPointer));
				lInPointer += G_ATTR_SIZE;
				lOutPointer += G_ATTR_SIZE;
			}
		}
		lMeasure.stop();
		lMeasureResults[lNoAttr] = secToNanoSec(lMeasure.mTotalTime(), G_NUMBER_OF_TUPLES);
	}
	printToFile(lMeasureResults, "manual.txt");
}

void measure_NormalWrites(std::byte* const aInputChunk, std::byte* const aOutputChunk)

{
	std::cout << "Normal Writes" << std::endl;
	double_vt lMeasureResults(G_NO_ATTR, 0);
	
	std::byte* lInPointer;
	std::byte* lOutPointer;

	for(size_t lNoAttr = 0; lNoAttr < G_NO_ATTR; ++lNoAttr)
	{	
		std::cout << "Attribute " << (lNoAttr+1) << "/" << G_NO_ATTR << std::endl;
		const size_t lBytesToCopy = (lNoAttr + 1)  * G_ATTR_SIZE;
		Measure lMeasure;
		lMeasure.start();
		for(size_t i = 0; i < G_NUMBER_OF_TUPLES; ++i)
		{
			lInPointer = aInputChunk + (i * G_TUPLE_SIZE);
			lOutPointer = aOutputChunk + (i * G_TUPLE_SIZE);
			memcpy((void*)lOutPointer , (void*)lInPointer, lBytesToCopy);
		}
		lMeasure.stop();
		lMeasureResults[lNoAttr] = secToNanoSec(lMeasure.mTotalTime(), G_NUMBER_OF_TUPLES);
	}
	std::cout << "Begin Input: " << aInputChunk << " / Begin Output: " << aOutputChunk << std::endl;
	std::cout << "End Input: " << lInPointer << " / End Output: " << lOutPointer << std::endl;
	printToFile(lMeasureResults, "normal.txt");
}

void measure_NonTemporalWrites(std::byte* const aInputChunk, std::byte* const aOutputChunk)
{
	std::cout << "Streaming Writes" << std::endl;
	double_vt lMeasureResults(G_NO_ATTR, 0);

	std::byte* lInPointer;
	std::byte* lOutPointer;

	const size_t lStreamWidth = sizeof(__m256i); //32 bytes
	const size_t lNoAttrPerStream = lStreamWidth / G_ATTR_SIZE; //32 bytes / 4 = 8attrPerWrite

	uint lNextTuple;
	uint lBytesToAlign;
	uint lOffset;


	for(size_t lNoAttr = 0; lNoAttr < G_NO_ATTR; ++lNoAttr)
	{	
		std::cout << "Attribute " << (lNoAttr+1) << "/" << G_NO_ATTR << std::endl;
		Measure lMeasure;
		lMeasure.start();
		for(size_t i = 0; i < G_NUMBER_OF_TUPLES; ++i)
		{	
			lNextTuple = i * G_TUPLE_SIZE;
			lBytesToAlign = ((lNextTuple + 31) & ~(uint) 0x1F) - lNextTuple; 
			lOffset = lNextTuple - lBytesToAlign;
			int j = 0 - (lBytesToAlign / G_ATTR_SIZE);
			//std::cout << "Next Tuple: " << lNextTuple
				//<< ",  Alignment: " << lBytesToAlign
				//<< ",  Offset: " << lOffset
				//<< ",  j = " << j
				//<< std::endl;
			lInPointer = aInputChunk + lOffset;
			lOutPointer = aOutputChunk + lOffset; 
			do 
			{
				//__m256i a = _mm256_load_si256((__m256i*)lInPointer);
				//_mm256_stream_si256((__m256i*)lOutPointer, a);
				_mm256_stream_si256((__m256i *)&lOutPointer[0], _mm256_load_si256((__m256i *)&lInPointer[0]));		
				lInPointer += lStreamWidth; 
				lOutPointer += lStreamWidth; 
				j += lNoAttrPerStream;
			}
			while (j <= lNoAttr);
		}
		lMeasure.stop();
		lMeasureResults[lNoAttr] = secToNanoSec(lMeasure.mTotalTime(), G_NUMBER_OF_TUPLES);
	}

	printToFile(lMeasureResults, "streaming.txt");
}

void printChunk(const std::byte* const aOutputChunk)
{
	for(size_t i = 0; i < G_NUMBER_OF_TUPLES * G_NO_ATTR; i++) {
		std::cout << *(uint32_t*)(aOutputChunk + (i * G_ATTR_SIZE)) << std::endl;
	}
}

int main()
{
	printSettings();

	std::byte* lInputChunk;
	std::byte* lOutputChunk;
	allocateMemory(lInputChunk, lOutputChunk);

	fillInputChunkWithData(lInputChunk);

	measure_ManualWrites(lInputChunk, lOutputChunk);
	//printChunk(lOutputChunk);
	//system("read");
	measure_NormalWrites(lInputChunk, lOutputChunk);
	//printChunk(lOutputChunk);
	//system("read");
	measure_NonTemporalWrites(lInputChunk, lOutputChunk);
	//printChunk(lOutputChunk);
	//system("read");
	freeMemory(lInputChunk, lOutputChunk);
	return 0;
}
