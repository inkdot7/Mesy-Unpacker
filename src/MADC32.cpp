/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Daughter class to translate data from a MDPP16 module:                                                                          //
//                                                                                                                                 //
// Inherits structure from the ModuleFather.h                                                                                      //
//  Internal methods designed for the MADC32 modules (info:https://www.mesytec.com/products/datasheets/MADC-32.pdf page 10)        //                                                                      
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

#include <string>

#include <thread>
#include <chrono>

using std::ifstream;
using namespace std;
class MADC32 : public ModuleFather {
public:

int module;                                                           
string nick; 
string event;
////////////////////////// MADC32 ///////////////////////////////
// this module have 32 channels but data follows this scheme:  //
//   ADC Value               - channels [0-31]                 //
//   TDC time difference     - channels [32-63]                //
//   Trigger time difference - channels [64(T0),65(T1)]        //
/////////////////////////////////////////////////////////////////

Int_t   MADC32_1_Multiplicity;
Int_t   MADC32_1_Channel[32*3];
ULong_t MADC32_1_Value[32*3];
ULong_t MADC32_1_Extended_timestamp;
ULong_t MADC32_1_Event_timestamp;

TH1F *MADC32_1_histo[32];
char MADC32_1_name[64], MADC32_1_histo_name[64];


// Constructor de MyClass
MADC32(int indexModule, string Mote, string evet){
    module=indexModule;
    nick=Mote;
    event=evet;

    MADC32_1_Multiplicity = 0;                 
    MADC32_1_Extended_timestamp = 0;   
    MADC32_1_Event_timestamp = 0; 

    for (int i=0; i<32*3; i++) {
        MADC32_1_Channel[i] = 0;           
        MADC32_1_Value[i] = 0;             
    }
}
// Initialize variables
void initEvent() override {

    MADC32_1_Multiplicity = 0;                 
    MADC32_1_Extended_timestamp = 0;   
    MADC32_1_Event_timestamp = 0; 

    for (int i=0; i<32*3; i++) {
        MADC32_1_Channel[i] = 0;           
        MADC32_1_Value[i] = 0;             
    }
}
// Method to transcribe Data
// For a frame that has been validadted, this method transcribes relevant parameteres from hex to numerical
void readData(ifstream *f) override {
  unsigned char madc32_data[4];
  f->read((char*) madc32_data, 4);
    
  unsigned char madc32_data_check = (madc32_data[2] + (madc32_data[3] << 8) & 0b1111111111100000) >> 5;

  if (madc32_data_check == 0b00000100000) { 
    if (((madc32_data[0] + (madc32_data[1] << 8) + (madc32_data[2] << 16) + (madc32_data[3] << 24)) != 0)) {
            
      unsigned int channel = madc32_data[2] & 0b00011111;
      unsigned long value = ( madc32_data[0] + (madc32_data[1] << 8) ) & 0b0001111111111111;
            
      if ( (channel >= 0) && (channel <= 31)){
        MADC32_1_Channel[MADC32_1_Multiplicity] = channel;
        MADC32_1_Value[MADC32_1_Multiplicity] = value;
        MADC32_1_Multiplicity++;
        MADC32_1_histo[channel] -> Fill(value);
        if (MADC32_1_Multiplicity > 32*3-1) { MADC32_1_Multiplicity = 32*3-1; }
      }
    } // != 0
  }
  
  if(madc32_data_check == 0b00000100100){
      MADC32_1_Extended_timestamp = (madc32_data[0] + (madc32_data[1] << 8) ) & 0b1111111111111111;
  }
  
  if (((madc32_data[3] >> 6) & 0b11) == 3){ // Reading timestamp in the end of event
        MADC32_1_Event_timestamp = (madc32_data[0] + (madc32_data[1] << 8) + (madc32_data[2] << 16) + ((madc32_data[3] & 0b00111111) << 24));
    }  
}
// Method to filter make sure the frame is good (NOTE: I should have though of a better name, change for final version)
void read(ifstream *f, Int_t &broken_event_count) override {
    
    unsigned char block_read_header[4];
    f->read((char*) block_read_header, 4); // should be Type = 0xf5
    unsigned short module_event_length = (block_read_header[0] + (block_read_header[1] << 8) ) & 0b0001111111111111;
    
    if (module_event_length > 1) {
    unsigned char module_header[4];
    f->read((char*) module_header, 4);

    for (int i=0; i < module_event_length-1; i++) {
        readData(f);
    }
  } 
  else if (module_event_length == 1) {
        unsigned char dontCare[4];
        f->read((char*) dontCare, 4);
        broken_event_count++;
    }
}

 void write(Int_t &broken_event_count) override {

}
// Create branches for the Ttree
void createTree(TTree *EventTree) override{
    int d=module;
                                                                       
    TString MultName = Form("MADC32_%i_Multiplicity", d ); TString MultNameI = Form("MADC32_%i_Multiplicity/I", d );// Generate a unique name for each histogram
    TString MultChan = Form("MADC32_%i_Channels", d );     TString MultChanI = Form("MADC32_%i_Channels[MADC32_%i_Multiplicity]/I", d,d);
    TString MultValu = Form("MADC32_%i_Values", d );       TString MultValuL = Form("MADC32_%i_Values[MADC32_%i_Multiplicity]/L", d,d );                                                                                                  
                                                                                    
    TString MADCExtendedTimestamp = Form("MADC32_%i_Extended_timestamps", d );    TString MADCExtendedTimestampL = Form("MADC32_%i_Extended_timestamps/L", d );// Generate a unique name for each histogram
    TString MADCEventTimestamp    = Form("MADC32_%i_timestamps", d );             TString MADCEventTimestampL = Form("MADC32_%i_Event_timestamps/L", d);
    
    EventTree->Branch(MultName, &MADC32_1_Multiplicity, MultNameI);
    EventTree->Branch(MultChan,  MADC32_1_Channel,      MultChanI);
    EventTree->Branch(MultValu,  MADC32_1_Value,        MultValuL);
                                                                                                      
    EventTree->Branch(MADCExtendedTimestamp, &MADC32_1_Extended_timestamp, MADCExtendedTimestampL);
    EventTree->Branch(MADCEventTimestamp, &MADC32_1_Event_timestamp, MADCEventTimestampL);
    
    //std::cout << "El abrol corre de lujo " << std::endl;
}                                                                                                                  

// Sanity check histograms
void histoLOOP(TFile *treeFile,std::vector<TDirectory*>& refereciasFolders) override{
    int d=module;

    TDirectory *MADC32_1_dir = refereciasFolders[stoi(event)]->mkdir(Form("%s_%i",nick.c_str(),d));//treeFile->mkdir(Form("MADC32_%i", d));

    for (Int_t i=0; i<32; i++) {
      MADC32_1_dir ->cd();
      
      snprintf(MADC32_1_name, sizeof(MADC32_1_name), "MADC32_%i_%i",d,i);
      snprintf(MADC32_1_histo_name, sizeof(MADC32_1_histo_name),"MADC32_%i_%i ; Channel ; Counts",d,i);
      MADC32_1_histo[i] = new TH1F(MADC32_1_name, MADC32_1_histo_name, 2*4096, 0, 2*4096);
      
      gDirectory->cd("..");
     
    }
     gDirectory->cd("..");
}

};
