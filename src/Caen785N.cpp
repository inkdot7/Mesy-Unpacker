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

// To compile:
// g++ CAEN785N.cpp `root-config --cflags --libs` -o CAEN785N.out
// To execute:
// ./CAEN785N.out 

using std::ifstream;
using namespace std;


class CAEN785N : public ModuleFather {
public:

int module;  
string nick; 
string event;

Int_t Caen785N_Multiplicity;
Int_t Caen785N_Channel[16*3];
Int_t Caen785N_Value[16*3];

TH1F *Caen785N_histo[32];
char Caen785N_name[64], Caen785N_histo_name[64];

CAEN785N(int indexModule, string Mote, string evet){
    module=indexModule;
    nick=Mote;
    event=evet;

    Caen785N_Multiplicity=0;
    for (int i=0; i<16*3; i++) {                                             
            Caen785N_Channel[i] = 0;                                    
            Caen785N_Value[i] = 0;                                     
    }
}


void initEvent() override {
    Caen785N_Multiplicity=0;
    for (int i=0; i<16*3; i++) {                                              
            Caen785N_Channel[i] = 0;                                    
            Caen785N_Value[i] = 0;                                     
    }
}

void readData(ifstream *f) override {
 unsigned char caen785N_data[4];
 f->read((char*) caen785N_data, 4);
//   unsigned short data_check = (caen785N_data[2] >> 5) & 0b00000111111;
 unsigned short data_check = ((caen785N_data[2] + (caen785N_data[3] << 8)) >> 5) & 0b00000111111;
  if (data_check == 0b000000) {
    unsigned char channel = (caen785N_data[2] >> 1) & 0b0001111;
    unsigned int value = (caen785N_data[0] + (caen785N_data[1] << 8))  & 0b0000111111111111;

      Caen785N_Channel[Caen785N_Multiplicity] = channel;
      Caen785N_Value[Caen785N_Multiplicity] = value;
      Caen785N_histo[channel] -> Fill(value);
      Caen785N_Multiplicity++;
        if (Caen785N_Multiplicity > 16*3-1) { Caen785N_Multiplicity = 16*3-1; }
  }

}

void read(ifstream *f, Int_t &broken_event_count) override {
  unsigned char block_read_header[4];
  f->read((char*) block_read_header, 4); // should be Type = 0xf5

  unsigned short module_event_length = (block_read_header[0] + (block_read_header[1] << 8) ) & 0b0001111111111111;
    
  if (module_event_length > 1) {
    unsigned char module_header[4];
    f->read((char*) module_header, 4);
        
    for (int i=0; i < module_event_length - 1; i++ ) {
        readData(f);
    }
    }
    else if (module_event_length == 1) { 
      //Some events have length 1, for unknown reasons
      unsigned char dontcare[4];
      f->read((char*) dontcare, 4);
      broken_event_count++;
    }

}

 void write(Int_t &broken_event_count) override {

}
void createTree(TTree *EventTree) override{
    int d=module;
    
    TString MultName = Form("Caen785N_%i_Multiplicity", d ); TString MultNameI = Form("Caen785N_%i_Multiplicity/I", d );// Generate a unique name for each histogram
    TString MultChan = Form("Caen785N_%i_Channels", d );     TString MultChanI = Form("Caen785N_%i_Channels[Caen785N_%i_Multiplicity]/I", d,d);
    TString MultValu = Form("Caen785N_%i_Values", d );       TString MultValuI = Form("Caen785N_%i_Values[Caen785N_%i_Multiplicity]/I", d,d ); 
    
    
    EventTree->Branch(MultName, &Caen785N_Multiplicity, MultNameI);
    EventTree->Branch(MultChan,  Caen785N_Channel,      MultChanI);
    EventTree->Branch(MultValu,  Caen785N_Value,        MultValuI);
}


void histoLOOP(TFile *treeFile,std::vector<TDirectory*>& refereciasFolders) override{
    int d=module;
    /*std::string eventName = "Event_" + event;
    treeFile->cd(eventName.c_str());*/
    
    TDirectory *Caen785N_dir = refereciasFolders[stoi(event)]->mkdir(Form("%s_%i",nick.c_str(), d));//treeFile->mkdir(Form("Caen785N_%i", d));
    
    for (Int_t i=0; i<32; i++) {
  
      Caen785N_dir->cd();
      
      snprintf(Caen785N_name, sizeof(Caen785N_name), "Caen785N_%i_%i",d,i);
      snprintf(Caen785N_histo_name, sizeof(Caen785N_histo_name),"Caen785N_%i_%i ; Channel ; Counts",d,i);
      Caen785N_histo[i] = new TH1F(Caen785N_name, Caen785N_histo_name, 4096, 0, 4096);
      gDirectory->cd("..");
    }
    gDirectory->cd("..");
}
};
