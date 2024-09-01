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
// g++ VMMR8.cpp `root-config --cflags --libs` -o VMMR8.out
// To execute:
// ./VMMR8.out 

using std::ifstream;
using namespace std;
class VMMR8 : public ModuleFather {
public:

int module;                                                           
string nick; 
string event;
// VMMR-8
ULong_t VMMR8_Multiplicity;
ULong_t VMMR8_Channel[1024*3];
ULong_t VMMR_Value[1024*3];
Int_t   VMMR8_time_diff_mul;
Int_t   VMMR8_Time_diff_bus[16*3];
ULong_t VMMR8_Time_diff_value[16*3];
ULong_t VMMR8_Extended_timestamp;
ULong_t VMMR8_Event_timestamp;

TH1F *VMMR8_histo[1024];
char VMMR8_name[64], VMMR8_histo_name[64];


// Constructor de MyClass
VMMR8(int indexModule, string Mote, string evet){
  module=indexModule;
  nick=Mote;
  event=evet;

  VMMR8_Multiplicity = 0;
  VMMR8_time_diff_mul = 0;
  VMMR8_Extended_timestamp = 0;
  VMMR8_Event_timestamp = 0;

  for (int i=0; i<1024*3; i++) {
    if (i<1024*3){
      VMMR8_Channel[i] = 0;
      VMMR_Value[i] = 0;
    }
    if (i<16*3){
      VMMR8_Time_diff_bus[i] = 0;
      VMMR8_Time_diff_value[i] = 0;
    }
  }
}

void initEvent() override {
  VMMR8_Multiplicity = 0;
  VMMR8_time_diff_mul = 0;
  VMMR8_Extended_timestamp = 0;
  VMMR8_Event_timestamp = 0;

  for (int i=0; i<1024*3; i++) {
    if (i<1024*3){
      VMMR8_Channel[i] = 0;
      VMMR_Value[i] = 0;
    }
    if (i<16*3){
      VMMR8_Time_diff_bus[i] = 0;
      VMMR8_Time_diff_value[i] = 0;
    }
  }
}
void readData(ifstream *f) override {
    unsigned char vmmr8_data[4];
  f->read((char*) vmmr8_data, 4);
    
  unsigned char vmmr8_data_check = (vmmr8_data[3] >> 4) & 0b1111;
    
  if (vmmr8_data_check == 0b0001) {
    if (((vmmr8_data[0] + (vmmr8_data[1] << 8) + (vmmr8_data[2] << 16) + (vmmr8_data[3] << 24)) != 0)) {
            
      unsigned long channel = (vmmr8_data[1] >> 4) + (vmmr8_data[2] << 4);
      unsigned long value = ( vmmr8_data[0] + (vmmr8_data[1] << 8) ) & 0b0000111111111111;
            
      if ((channel >= 0) && (channel < 1024)){
        VMMR8_Channel[VMMR8_Multiplicity] = channel;
        VMMR_Value[VMMR8_Multiplicity] = value;
        //cout << "channel"<< " "<< channel <<endl;
        //cout << "value" << " " << value <<endl;
        VMMR8_Multiplicity++;
        VMMR8_histo[channel]->Fill(value);
        if (VMMR8_Multiplicity > 1023) { VMMR8_Multiplicity = 1023; }
      }
    }
  } // != 0
  
  if(vmmr8_data_check == 0b0011){
    cout << "hola" ;
    unsigned int bus = vmmr8_data[3] & 0b00001111;
    unsigned long value = ( vmmr8_data[0] + (vmmr8_data[1] << 8) ) & 0b1111111111111111;
    
    if ((bus >= 0) && (bus < 16)){
    VMMR8_Time_diff_bus[VMMR8_time_diff_mul] = bus;
    VMMR8_Time_diff_value[VMMR8_time_diff_mul] = value;
    VMMR8_time_diff_mul++;
    if (VMMR8_time_diff_mul > 16*3-1) {VMMR8_time_diff_mul = 16*3-1;}
    }
  }
  
  if(vmmr8_data_check == 0b0010){
    unsigned long value = ( vmmr8_data[0] + (vmmr8_data[1] << 8) ) & 0b1111111111111111;
    //cout << "value" << value;
    VMMR8_Extended_timestamp = value;
  }
  
  if (((vmmr8_data[3] >> 6) & 0b11) == 3){ // Reading timestamp in the end of event
      VMMR8_Event_timestamp = (vmmr8_data[0] + (vmmr8_data[1] << 8) + (vmmr8_data[2] << 16) + ((vmmr8_data[3] & 0b00111111) << 24));
  }

}

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
  } else if (module_event_length == 1) {
      unsigned char dontCare[4];
      f->read((char*) dontCare, 4);
      broken_event_count++;
    }
}

 void write(Int_t &broken_event_count) override {
}

void createTree(TTree *EventTree) override{
  int d=module;
                                                            
  TString LongMultName = Form("VMMR8_%i_Multiplicity", d); TString LongMultNameI = Form("VMMR8_%i_Multiplicity/I", d );// Generate a unique name for each histogram
  TString LongMultChan = Form("VMMR8_%i_Channels", d);     TString LongMultChanI = Form("VMMR8_%i_Channels[VMMR8_%i_Multiplicity]/I", d,d); 
  TString LongMultValu = Form("VMMR8_%i_Values", d);       TString LongMultValuL = Form("VMMR_%i_Values[VMMR8_%i_Multiplicity]/L", d,d );


  TString TDCtimeDiffMul = Form("VMMR8_%i_time_diff_mul", d);   TString TDCtimeDiffMulI = Form("VMMR8_%i_time_diff_mul/I", d );// Generate a unique name for each histogram 
  TString TDCtimeDiffbus = Form("VMMR8_%i_time_diff_buss",d);   TString TDCtimeDiffbusI = Form("VMMR8_%i_time_diff_buss[VMMR8_%i_time_diff_mul]/I", d,d );
  TString TDCtimeDiffVal = Form("VMMR8_%i_time_diff_Values",d); TString TDCtimeDiffValL = Form("VMMR8_%i_time_diff_Values[VMMR8_%i_time_diff_mul]/L", d,d );
    
  TString ExtendTimestamp = Form("VMMR8_%i_Extended_timestamps", d );TString ExtendTimestampL = Form("VMMR8_%i_Extended_timestamps/L", d);// Generate a unique name for each histogram
  TString EventTimestamp  = Form("VMMR8_%i_Event_timestamps", d );   TString EventTimestampL  = Form("VMMR8_%i_Event_timestamps/L", d);                                                    
  
  
  EventTree->Branch(LongMultName,  &VMMR8_Multiplicity,  LongMultNameI);
  EventTree->Branch(LongMultChan,   VMMR8_Channel,       LongMultChanI);
  EventTree->Branch(LongMultValu,   VMMR_Value,          LongMultValuL);
  
  EventTree->Branch(TDCtimeDiffMul, &VMMR8_time_diff_mul,  TDCtimeDiffMulI);
  EventTree->Branch(TDCtimeDiffbus,  VMMR8_Time_diff_bus,  TDCtimeDiffbusI);
  EventTree->Branch(TDCtimeDiffVal,  VMMR8_Time_diff_value,TDCtimeDiffValL);
                                                                                                   
  EventTree->Branch(ExtendTimestamp,  &VMMR8_Extended_timestamp, ExtendTimestampL);
  EventTree->Branch(EventTimestamp,     &VMMR8_Event_timestamp,  EventTimestampL);
  
  
  //std::cout << "El abrol corre de lujo " << std::endl;
}                                                                                                                  

void histoLOOP(TFile *treeFile,std::vector<TDirectory*>& refereciasFolders) override{
  int d=module;
  /*std::string eventName = "Event_" + event;
  // Cambiar al directorio con el nombre del evento
  treeFile->cd(eventName.c_str());*/

  TDirectory *VMMR8_dir = refereciasFolders[stoi(event)]->mkdir(Form("%s_%i",nick.c_str(),d));//treeFile->mkdir(Form("VMMR8_%i",d));
  for (Int_t i=0; i<1024; i++) {
    VMMR8_dir->cd();
    snprintf(VMMR8_name,sizeof(VMMR8_name),"VMMR_%i",i);
    snprintf(VMMR8_histo_name, sizeof(VMMR8_histo_name),"VMMR_%i ; Channel ; Counts",i);
    VMMR8_histo[i] = new TH1F(VMMR8_name, VMMR8_histo_name, 4096, 0, 4096);
    gDirectory->cd("..");
    //VMMR8_dir->cd();
    //VMMR8_histo[i] -> Write();
    
  }
  gDirectory->cd("..");
}

/*void readMDPP16_QDC_data(ifstream *f)
{
  unsigned char mdpp16_qdc_data[4];
  f->read((char*) mdpp16_qdc_data, 4);
    
  unsigned char mdpp16_qdc_data_check = (mdpp16_qdc_data[3] >> 4) & 0b1111;
    
  if (mdpp16_qdc_data_check == 0b0001) {
        
    if (((mdpp16_qdc_data[0] + (mdpp16_qdc_data[1] << 8) + (mdpp16_qdc_data[2] << 16) + (mdpp16_qdc_data[3] << 24)) != 0)) {
            
      unsigned int channel = mdpp16_qdc_data[2] & 0b00111111;
      unsigned long value = ( mdpp16_qdc_data[0] + (mdpp16_qdc_data[1] << 8) ) & 0b1111111111111111;
    
    if ( (channel >= 0) && (channel < 16)){
        MDPP16_1_ilong_Channel[MDPP16_1_ilong_Multiplicity] = channel;
        MDPP16_1_ilong_Value[MDPP16_1_ilong_Multiplicity] = value;
        MDPP16_1_ilong_Multiplicity++;
        MDPP16_1_ilong_histo[channel] -> Fill(value);
        if (MDPP16_1_ilong_Multiplicity > 16*3-1) { MDPP16_1_ilong_Multiplicity = 16*3-1; }
      }
            
      if ((channel >= 48) && (channel < 64)){
        MDPP16_1_ishort_Channel[MDPP16_1_ishort_Multiplicity] = channel-48;
        MDPP16_1_ishort_Value[MDPP16_1_ishort_Multiplicity] = value;
        MDPP16_1_ishort_Multiplicity++;
        MDPP16_1_ishort_histo[channel-48] -> Fill(value);
        if (MDPP16_1_ishort_Multiplicity > 16*3-1) { MDPP16_1_ishort_Multiplicity = 16*3-1; }
      }
      
      if ((channel >= 16) && (channel < 32)){
        MDPP16_1_TDC_time_diff_Channel[MDPP16_1_TDC_time_diff_mul] = channel - 16;
        MDPP16_1_TDC_time_diff_Value[MDPP16_1_TDC_time_diff_mul] = value;
        MDPP16_1_TDC_time_diff_mul++;
        if(MDPP16_1_TDC_time_diff_mul > 16*3-1){MDPP16_1_TDC_time_diff_mul = 16*3-1;}
      }
      
      if( (channel >= 32) && (channel < 34)){
        MDPP16_1_Trigger_time_diff_Channel[MDPP16_1_Trigger_time_diff_mul] = channel-32;
        MDPP16_1_Trigger_time_diff_Value[MDPP16_1_Trigger_time_diff_mul] = value;
        MDPP16_1_Trigger_time_diff_mul++;
        if(MDPP16_1_Trigger_time_diff_mul > 16-1){MDPP16_1_Trigger_time_diff_mul = 16-1;}
      }
    }

    }
}*/
/*void readMDPP16_QDC(ifstream *f, int module) 
{
  
  unsigned char block_read_header[4];
  f->read((char*) block_read_header, 4); // should be Type = 0xf5
  unsigned short module_event_length = (block_read_header[0] + (block_read_header[1] << 8) ) & 0b0001111111111111;
    
  if (module_event_length > 1) {
    unsigned char module_header[4];
    f->read((char*) module_header, 4);
    for (int i=0; i < module_event_length-1; i++) {
      readMDPP16_QDC_data(f);
    }        
  } else if (module_event_length == 1) {
      unsigned char dontCare[4];
      f->read((char*) dontCare, 4);
      broken_event_count++;
    }
//     cout << "reading MDPP16: " << module_event_length << " values" << endl; 
}*/   

};

/*int main() {
    ifstream fp;
    const char* filename2 = "/home/daniferu/IS690/IEM_Unpacker/ZIP/IS690-isolde_v5_run002_231026_154335.mvlclst";
    ULong_t bytes_counter = 0;

    
    fp.open(filename2, ios::in | ios::binary);
    printf("%s\n", filename2);

     char dontcare[8];
    fp.read(dontcare , 8);    // Jump over MVLC_USB ascii and littleE marker
    bytes_counter += 8;

    TTree *EventTree; 
    TFile *treeFile;
    EventTree = new TTree("EventTree", "EventTree");
    treeFile = new TFile("oWo.root", "recreate"); 
    
    // Crear un objeto de la clase derivada con un valor para el constructor
    MDPP16 myObject (0);
    myObject.createTree(EventTree);
    myObject.histoLOOP();

    MDPP16 myObject2 (1);
    myObject2.createTree(EventTree);
    myObject2.histoLOOP();

    readFrame(&fp);

    
    EventTree->Write();


    treeFile->Close();
    fp.close();   
    return 0;
    
}  */
