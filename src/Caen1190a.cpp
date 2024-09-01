/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Daughter class to translate data from a Caen1190 module:                                                                        //
//                                                                                                                                 //
// Inherits structure from the ModuleFather.h                                                                                      //
//  Internal methods designed for the Caen1190 modules                                                                             //                                                                                                                                                                                                    
// The methods need to be summoned in the CORRECT OREDER, the Unpacker takes care of this thanks to the config file                //
// The output is:                                                                                                                  //
//    1-> Root branches containing relevant info that will be sitched to a TTree                                                    //
//    2-> Histograms done with the info of the branches, used to compare with Mesytec as a sanity check                           //
// DFR                                                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../include/ModuleFather.h"
#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"

#include <stdio.h>
#include <string.h>
#include <fstream>

#include <sstream>

#include <sstream>
#include <cstdio>



using std::ifstream;
using namespace std;


class Caen1190a : public ModuleFather {

public:

int module;     
Int_t Caen1190a_Multiplicity;
Int_t Caen1190a_Channel[128];
Int_t Caen1190a_Value[128];

Caen1190a(int indexModule){
    module=indexModule;
    Caen1190a_Multiplicity=0;

    for (int i=0; i<128; i++) {
        Caen1190a_Channel[i]=0;
        Caen1190a_Value[i]=0;
    }
}
// Initialize variables
void initEvent() override {
    Caen1190a_Multiplicity=0;

    for (int i=0; i<128; i++) {
        Caen1190a_Channel[i]=0;
        Caen1190a_Value[i]=0;
    }
}

// Method to transcribe Data
// For a frame that has been validadted, this method transcribes relevant parameteres from hex to numerical

void readData(ifstream *f) override {

    unsigned char caen1190a_data[4];
    f->read((char*) caen1190a_data, 4);
    unsigned short caen1190a_data_check = (caen1190a_data[3] >> 3) & 0b11111;
    
    if ((caen1190a_data_check == 0b00000) && ((caen1190a_data[0] + (caen1190a_data[1] << 8) + (caen1190a_data[2] << 16) + (caen1190a_data[3] << 24)) != 0)) 
    {
        unsigned char channel = ((caen1190a_data[2] + (caen1190a_data[3] << 8) ) >> 3) & 0b000001111111;
        unsigned long value = (caen1190a_data[0]  + (caen1190a_data[1] << 8) 
                              + (caen1190a_data[2] << 16) ) & 0b000001111111111111111111;
                              
        Caen1190a_Channel[Caen1190a_Multiplicity] = channel;
        Caen1190a_Value[Caen1190a_Multiplicity] = value;
        Caen1190a_Multiplicity++;
        if(Caen1190a_Multiplicity > 127){Caen1190a_Multiplicity = 127;}
    }

}
// Method to filter make sure the frame is good (NOTE: I should have though of a better name, change for final version)
void read(ifstream *f, Int_t &broken_event_count) override {
    unsigned char block_read_header[4];
    f->read((char*) block_read_header, 4); // should be Type = 0xf5
    unsigned short module_event_length = (block_read_header[0] + (block_read_header[1] << 8) ) & 0b0001111111111111;
    
    if (module_event_length > 1) {
        for(int i = 0 ; i < module_event_length; i++) {
            readData(f);
        }
    } else if (module_event_length == 1) {
        unsigned char dontcare[4];
        f->read((char*) dontcare, 4);
        broken_event_count++;
    }
}

}
