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

// To compile:
// g++ MDPP16.cpp `root-config --cflags --libs` -o MDPP16.out
// To execute:
// ./MDPP16.out 

using std::ifstream;
using namespace std;
class MDPP16 : public ModuleFather {
public:

int module;                                                           


// TFile

// TTree

Int_t   MDPP16_1_ilong_Multiplicity;
Int_t   MDPP16_1_ilong_Channel[16*3];
ULong_t MDPP16_1_ilong_Value[16*3];
Int_t   MDPP16_1_ishort_Multiplicity;
Int_t   MDPP16_1_ishort_Channel[16*3];
ULong_t MDPP16_1_ishort_Value[16*3];
Int_t   MDPP16_1_TDC_time_diff_mul;
Int_t   MDPP16_1_TDC_time_diff_Channel[16*3];
ULong_t MDPP16_1_TDC_time_diff_Value[16*3];
Int_t   MDPP16_1_Trigger_time_diff_mul;
Int_t   MDPP16_1_Trigger_time_diff_Channel[16];
ULong_t MDPP16_1_Trigger_time_diff_Value[16];
ULong_t MDPP16_1_Extended_timestamp;
ULong_t MDPP16_1_Event_timestamp;

TH1F *MDPP16_1_ilong_histo[16], *MDPP16_1_ishort_histo[16];
char MDPP16_1_ilong_name[64], MDPP16_1_ilong_histo_name[64];
char MDPP16_1_ishort_name[64], MDPP16_1_ishort_histo_name[64];
// Constructor de MyClass
MDPP16(int indexModule){
    module=indexModule;

    MDPP16_1_ilong_Multiplicity = 0;   
    MDPP16_1_ishort_Multiplicity = 0;       
    MDPP16_1_TDC_time_diff_mul = 0;         
    MDPP16_1_Trigger_time_diff_mul = 0;     
    MDPP16_1_Extended_timestamp = 0;        
    MDPP16_1_Event_timestamp = 0;           
    for (int i=0; i<1024*3; i++) {
        if (i<16*3){
            // Module 1                                 
            MDPP16_1_ilong_Channel[i] = 0;                 
            MDPP16_1_ilong_Value[i] = 0;                
            MDPP16_1_ishort_Channel[i] = 0;                
            MDPP16_1_ishort_Value[i] = 0;               
            MDPP16_1_TDC_time_diff_Channel[i] = 0;  
            MDPP16_1_TDC_time_diff_Value[i] = 0;        
        }
        if (i<16){
            // Module 1                                 
            MDPP16_1_Trigger_time_diff_Channel[i] = 0;  
            MDPP16_1_Trigger_time_diff_Value[i] = 0;    
        }
    }
}

void initEvent() override {
  MDPP16_1_ilong_Multiplicity = 0;   
    MDPP16_1_ishort_Multiplicity = 0;       
    MDPP16_1_TDC_time_diff_mul = 0;         
    MDPP16_1_Trigger_time_diff_mul = 0;     
    MDPP16_1_Extended_timestamp = 0;        
    MDPP16_1_Event_timestamp = 0;           
    for (int i=0; i<1024*3; i++) {
        if (i<16*3){
            // Module 1                                 
            MDPP16_1_ilong_Channel[i] = 0;                 
            MDPP16_1_ilong_Value[i] = 0;                
            MDPP16_1_ishort_Channel[i] = 0;                
            MDPP16_1_ishort_Value[i] = 0;               
            MDPP16_1_TDC_time_diff_Channel[i] = 0;  
            MDPP16_1_TDC_time_diff_Value[i] = 0;        
        }
        if (i<16){
            // Module 1                                 
            MDPP16_1_Trigger_time_diff_Channel[i] = 0;  
            MDPP16_1_Trigger_time_diff_Value[i] = 0;    
        }
    }
}
void readData(ifstream *f) override {
unsigned char mdpp16_qdc_data[4];
  f->read((char*) mdpp16_qdc_data, 4);
    
  unsigned char mdpp16_qdc_data_check = (mdpp16_qdc_data[3] >> 4) & 0b1111;
  //cout << mdpp16_qdc_data_check << endl;
/*   cout << "mdpp16_qdc_data_check: " << static_cast<int>(mdpp16_qdc_data_check) << endl;
cout << "mdpp16_qdc_data[3]: " << static_cast<int>(mdpp16_qdc_data[3]) << endl; */

  if (mdpp16_qdc_data_check == 0b0001) {//AQUI NO ESTA ENTRANDO

        
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

      if(mdpp16_qdc_data_check == 0b0010){// AQUI NUNCA ENTRA

        MDPP16_1_Extended_timestamp = (mdpp16_qdc_data[0] + (mdpp16_qdc_data[1] << 8) ) & 0b1111111111111111;
    
   
      }
  
  if (((mdpp16_qdc_data[3] >> 6) & 0b11) == 3){// Reading timestamp in the end of event AQUI SI LLEGA A ENTRAR
    
    MDPP16_1_Event_timestamp = (mdpp16_qdc_data[0] + (mdpp16_qdc_data[1] << 8) + (mdpp16_qdc_data[2] << 16) + ((mdpp16_qdc_data[3] & 0b00111111) << 24));
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
  } else if (module_event_length == 1) {//AQUI JAMAS ESTA ENTRANDO
      unsigned char dontCare[4];
      f->read((char*) dontCare, 4);
      broken_event_count++;
      std::cout << "SALIO" << std::endl;
    }
}

 void write(Int_t &broken_event_count) override {
}

void createTree(TTree *EventTree) override{
    // MDPP16_QDC 1 
    int d=module;
                                                                   
    TString LongMultName       = Form("MDPP16_%i_ilong_Multiplicity", d );           
    TString LongMultNameI         = Form("MDPP16_%i_ilong_Multiplicity/I", d );// Generate a unique name for each histogram
    
    TString LongMultChan       = Form("MDPP16_%i_ilong_Channels", d );               
    TString LongMultChanI         = Form("MDPP16_%i_ilong_Channels[MDPP16_%i_ilong_Multiplicity]/I", d,d);
    
    TString LongMultValu       = Form("MDPP16_%i_ilong_Values", d );                 
    TString LongMultValuL         = Form("MDPP16_%i_ilong_Values[MDPP16_%i_ilong_Multiplicity]/L", d,d );
                                                                                                             
    TString ShortMultName       = Form("MDPP16_%i_ishort_Multiplicity", d );         
    TString ShortMultNameI        = Form("MDPP16_%i_ishort_Multiplicity/I", d );// Generate a unique name for each histogram
    
    TString ShortMultChan       = Form("MDPP16_%i_ishort_Channels", d );
    TString ShortMultChanI        = Form("MDPP16_%i_ishort_Channels[MDPP16_%i_ishort_Multiplicity]/I", d,d );
    
    TString ShortMultValu       = Form("MDPP16_%i_ishort_Values", d );
    TString ShortMultValuL        = Form("MDPP16_%i_ishort_Values[MDPP16_%i_ishort_Multiplicity]/L", d,d );
                                                                                    
    TString TDCtimeDiffMul      = Form("MDPP16_%i_TDC_time_diff_mul", d );
    TString TDCtimeDiffMulI       = Form("MDPP16_%i_TDC_time_diff_mul/I", d );// Generate a unique name for each histogram
    
    TString TDCtimeDiffCha      = Form("MDPP16_%i_TDC_time_diff_Channels", d );
    TString TDCtimeDiffChaI       = Form("MDPP16_%i_TDC_time_diff_Channels[MDPP16_%i_TDC_time_diff_mul]/I", d,d );
    
    TString TDCtimeDiffVal      = Form("MDPP16_%i_TDC_time_diff_Values", d );
    TString TDCtimeDiffValL       = Form("MDPP16_%i_TDC_time_diff_Values[MDPP16_%i_TDC_time_diff_mul]/L", d,d );
    
    TString TriggertimeDiffMul  = Form("MDPP16_%i_Trigger_time_diff_mul", d );
    TString TriggertimeDiffMulI  = Form("MDPP16_%i_Trigger_time_diff_mul/I", d );// Generate a unique name for each histogram
    
    TString TriggertimeDiffCha  = Form("MDPP16_%i_Trigger_time_diff_Channels", d );
    TString TriggertimeDiffChaI  = Form("MDPP16_%i_Trigger_time_diff_Channels[MDPP16_%i_Trigger_time_diff_mul]/I", d,d );                                                    
    
    TString TriggertimeDiffVal  = Form("MDPP16_%i_Trigger_time_diff_Values", d );
    TString TriggertimeDiffValL  = Form("MDPP16_%i_Trigger_time_diff_Values[MDPP16_%i_Trigger_time_diff_mul]/L", d,d );
    
    TString ExtendedTimestamps  = Form("MDPP16_%i_Extended_timestamps", d );          
    TString ExtendedTimestampsI  = Form("MDPP16_%i_Extended_timestamps/L", d );
    
    TString EventTimestamps     = Form("MDPP16_%i_Event_timestamps", d );
    TString EventTimestampsL     = Form("MDPP16_%i_Event_timestamps/L", d );
    

    EventTree->Branch(LongMultName,        &MDPP16_1_ilong_Multiplicity,         LongMultNameI);
    EventTree->Branch(LongMultChan,         MDPP16_1_ilong_Channel,              LongMultChanI);
    EventTree->Branch(LongMultValu,         MDPP16_1_ilong_Value,                LongMultValuL);
    
    EventTree->Branch(ShortMultName,        &MDPP16_1_ishort_Multiplicity,       ShortMultNameI);
    EventTree->Branch(ShortMultChan,         MDPP16_1_ishort_Channel,            ShortMultChanI);
    EventTree->Branch(ShortMultValu,         MDPP16_1_ishort_Value,              ShortMultValuL);
                                                                                                      
    EventTree->Branch(TDCtimeDiffMul,      &MDPP16_1_TDC_time_diff_mul,          TDCtimeDiffMulI);
    EventTree->Branch(TDCtimeDiffCha,       MDPP16_1_TDC_time_diff_Channel,      TDCtimeDiffChaI);
    EventTree->Branch(TDCtimeDiffVal,       MDPP16_1_TDC_time_diff_Value,        TDCtimeDiffValL);
                                                                                                     
    EventTree->Branch(TriggertimeDiffMul,  &MDPP16_1_Trigger_time_diff_mul,      TriggertimeDiffMulI);
    EventTree->Branch(TriggertimeDiffCha,   MDPP16_1_Trigger_time_diff_Channel,  TriggertimeDiffChaI);
    EventTree->Branch(TriggertimeDiffVal,   MDPP16_1_Trigger_time_diff_Value,    TriggertimeDiffValL);
                                                                                                     
    EventTree->Branch(ExtendedTimestamps,  &MDPP16_1_Extended_timestamp,         ExtendedTimestampsI);
    EventTree->Branch(EventTimestamps,     &MDPP16_1_Event_timestamp,            EventTimestampsL);
    
    //TDirectory *MDPP16_1_ilong_dir = EventTree->mkdir("MDPP16_%i_ilong", d );
    //TDirectory *MDPP16_1_ishort_dir = EventTree->mkdir("MDPP16_%i_ishort", d );
    
    
    //std::cout << "The tree is OK " << std::endl;
}                                                                                                                  

void histoLOOP() override{
    int d=module;
    for (Int_t i=0; i<1024; i++) {

    if (i<16) {
      snprintf(MDPP16_1_ilong_name, sizeof(MDPP16_1_ilong_name), "MDPP16_%i_ilong_%i",d,i);
      snprintf(MDPP16_1_ilong_histo_name, sizeof(MDPP16_1_ilong_histo_name), "MDPP16_%i_ilong_%i ; Channel ; Counts",d,i);
      MDPP16_1_ilong_histo[i] = new TH1F(MDPP16_1_ilong_name, MDPP16_1_ilong_histo_name, 4096, 0, 65536);
            
      snprintf(MDPP16_1_ishort_name, sizeof(MDPP16_1_ishort_name), "MDPP16_%i_ishort_%i",d,i);
      snprintf(MDPP16_1_ishort_histo_name, sizeof(MDPP16_1_ishort_histo_name), "MDPP16_%i_ishort_%i ; Channel ; Counts",d,i);
      MDPP16_1_ishort_histo[i] = new TH1F(MDPP16_1_ishort_name, MDPP16_1_ishort_histo_name, 4096, 0, 65536);
    }
    }
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
