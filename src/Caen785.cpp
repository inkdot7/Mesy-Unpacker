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
// To compile:
// g++ Caen785.cpp `root-config --cflags --libs` -o Caen785.out
// To execute:
// ./Caen785.out 

using std::ifstream;
using namespace std;
class Caen785 : public ModuleFather {

public:

int module;
string nick; 
string event;     

Int_t Caen785_1_Multiplicity;
Int_t Caen785_1_Channel[32*3];
Int_t Caen785_1_Value[32*3];

TH1F *Caen785_1_histo[32];
char Caen785_1_name[64], Caen785_1_histo_name[64];

Caen785(int indexModule, string Mote, string evet){
    // FALTA CÓDIGO DE REFERENCIA, PENDIENTE    
    module=indexModule;
    nick=Mote;
    event=evet;

    Caen785_1_Multiplicity=0;
    
    for (int i=0; i<32*3; i++) {
        if (i<32*3){                                               
            Caen785_1_Channel[i] = 0;                                    
            Caen785_1_Value[i] = 0;                                     
        }
    }
}

void initEvent() override {
    Caen785_1_Multiplicity=0;
    
    for (int i=0; i<32*3; i++) {
        if (i<32*3){                                               
            Caen785_1_Channel[i] = 0;                                    
            Caen785_1_Value[i] = 0;                                     
        }
    }
}

void readData(ifstream *f) override {
    unsigned char caen785_data[4];
    f->read((char*) caen785_data, 4);
//   unsigned short data_check = (caen785_data[2] >> 5) & 0b00000111111;
    unsigned short data_check = ((caen785_data[2] + (caen785_data[3] << 8)) >> 5) & 0b00000111111;

    if (data_check == 0b000000) {
    unsigned char channel = caen785_data[2] & 0b00011111;
    unsigned int value = (caen785_data[0] + (caen785_data[1] << 8))  & 0b0000111111111111;

    Caen785_1_Channel[Caen785_1_Multiplicity] = channel;
    Caen785_1_Value[Caen785_1_Multiplicity] = value;
    Caen785_1_histo[channel] -> Fill(value);
    Caen785_1_Multiplicity++;
        if (Caen785_1_Multiplicity > 32*3-1) { Caen785_1_Multiplicity = 32*3-1; }
    }
}

void read(ifstream *f, Int_t &broken_event_count) override {
  unsigned char block_read_header[4];
  f->read((char*) block_read_header, 4); // should be Type = 0xf5

  unsigned short module_event_length = (block_read_header[0] + (block_read_header[1] << 8) ) & 0b0001111111111111;
    
  if (module_event_length > 1) {
    unsigned char module_header[4];
    f->read((char*) module_header, 4);

   /* unsigned short numeroWords = (module_header[0] + (module_header[1] << 8) ) & 0b0000001111111111;
    std::cout << "Caen785" << std::endl;
    std::cout << "Module event length"<< module_event_length << std::endl;
    std::cout << "Numero de words"<< numeroWords << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));*/
        
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
    
    TString MultName = Form("Caen785_%i_Multiplicity", d ); TString MultNameI = Form("Caen785_%i_Multiplicity/I", d );// Generate a unique name for each histogram
    TString MultChan = Form("Caen785_%i_Channels", d );     TString MultChanI = Form("Caen785_%i_Channels[Caen785_%i_Multiplicity]/I", d,d);
    TString MultValu = Form("Caen785_%i_Values", d );       TString MultValuI = Form("Caen785_%i_Values[Caen785_%i_Multiplicity]/I", d,d );  
    
    EventTree->Branch(MultName, &Caen785_1_Multiplicity, MultNameI);
    EventTree->Branch(MultChan,  Caen785_1_Channel,      MultChanI);
    EventTree->Branch(MultValu,  Caen785_1_Value,        MultValuI);
}


void histoLOOP(TFile *treeFile,std::vector<TDirectory*>& refereciasFolders) override{
    int d=module;

    /*std::string eventName = "Event_" + event;
    // Cambiar al directorio con el nombre del evento
    treeFile->cd(eventName.c_str());*/

    
    
    TDirectory *Caen785_1_dir = refereciasFolders[stoi(event)]->mkdir(Form("%s_%i",nick.c_str(), d));//treeFile->mkdir(Form("Caen785_%i", d));
    
    for (Int_t i=0; i<32; i++) {
  
      Caen785_1_dir->cd();
      
      snprintf(Caen785_1_name, sizeof(Caen785_1_name), "Caen785_%i_%i",d,i);
      snprintf(Caen785_1_histo_name, sizeof(Caen785_1_histo_name),"Caen785_%i_%i ; Channel ; Counts",d,i);
      Caen785_1_histo[i] = new TH1F(Caen785_1_name, Caen785_1_histo_name, 4096, 0, 4096);
      
      gDirectory->cd("..");
    }
    gDirectory->cd("..");
}

};
