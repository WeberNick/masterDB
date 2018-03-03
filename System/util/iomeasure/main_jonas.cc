#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include "measure.hh"

int main(int argc, char* argv[]){

    if(argc!=2){
        std::cout<<"falsche Zahl von Argumenten"<<std::endl;
        return(-1);
    }
    std::string mode = argv[1];  
    int times=10;
    Measure m;
    std::vector<double> res;
    if(mode=="write"){
        int size = 1024*1024*1;
        char text[size+1];
        for(int i=0;i<size;++i){
            text[i]='x';
        }
        text[size]='\0';
        std::ofstream myfile;
        for(int j=0;j<times;++j){
            m.start();
            myfile.open ("example.txt");
            for(int i=0;i<2048;++i){
                myfile << text<<std::endl;
            }
            m.stop();
            double len = m.mTotalTime();
            myfile.close();
            std::cout<< std::fixed << std::setprecision(3)<<len<<std::endl;
            res.push_back(len);
        }
        double av=0;
        for(int i=0;i<res.size();++i){
            av+=res[i];
        }
        std::cout<<"av: "<< av/res.size()<<std::flush;
    }
    else{
        std::streampos size;
        char * memblock;
        for(int j=0;j<times;++j){
        std::ifstream file ("example.txt", std::ios::in|std::ios::binary|std::ios::ate);
          size = file.tellg();
          memblock = new char [size];
          file.seekg (0, std::ios::beg);
          m.start();
          file.read (memblock, size);
          m.stop();
          file.close();
          double len = m.mTotalTime();
          std::cout<< std::fixed << std::setprecision(3)<<len<<std::endl;
          delete[] memblock;
          res.push_back(len);
        }
        double av=0;
        for(int i=0;i<res.size();++i){
            av+=res[i];
        }
        std::cout<<"av: "<< av/res.size()<<std::flush;
         
        
        
    }
}
/*write 2GB to HDD
17.281
22.354
23.364
22.784
16.837
19.098
20.128
23.428
23.140
23.579
av: 21.199

read 2GB from HDD
4.657
1.233
3.156
4.898
1.831
0.969
0.972
1.006
0.986
0.997
av: 2.070
*/