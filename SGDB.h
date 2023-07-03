#pragma once
#include "HeadersHDD/Disco.h"
#include "DiskController.h"
#include "BufferManager.h"
#include "tipos.cpp"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


class SGDB
{
public:
    Disco *disco;
    DiskController *diskController;
    BufferManager *bufferManager;

    SGDB(Disco *disco, DiskController *diskController)
    {
        this->disco = disco;
        this->diskController = diskController;
        this->bufferManager = new BufferManager(this->diskController);
    }
    ~SGDB() {}

    void createTable(string prompt)
    {
        ofstream file;
        file.open("esquema", fstream::app);
        file.seekp(0, ios::beg);
        string word;
        stringstream stream(prompt);
        getline(stream, word, ' ');
        file << word << '#';
        getline(stream, word, ' ');
        file << word;
        // Escritura
        getline(stream, word, ' ');
        string wor;
        cout << "Atributos:" << endl;
        // titanic 12 passengerid,int,survived,int,Pclass,int,Name,str,90,Sex,str,6,age,double,SibSp,int,Parch,int,Ticket,str,20,Fare,double,Cabin,str,20,Embarked,str,1
        // Muestra las columnas de las tablas
        stringstream stream1(word);
        cout << word << endl;
        while (getline(stream1, wor, ','))
        {
            file << "#" << wor;
        }

        file.close();
    }

    void mostrarPage(int pageId) { // MostrarBloque
        this->diskController->tableToVector("titanic");     
        int comprobarRegistro; 
        int sizeRegistro = 0;
        for(auto& i : this->diskController->info){
            sizeRegistro += get<2>(i);
        }
        
        char * frame = bufferManager->getPageOfBuuferPool(pageId)->data;
        //cout<<"llego aqui"<<endl;
        ifstream file;
        file.open("disk/bloque"+to_string(pageId)+".bin",ios::in | ios::binary);
        file.read(frame,this->diskController->sizeBloque);
        
        char marcador;
        marcador = *reinterpret_cast<char *>(frame + sizeRegistro-5);
        int byte = 0;
        if (marcador != '*') {
          byte+=(sizeRegistro)-4;
          cout<<"Freelist -> ";
          fun_int(frame,byte,4);
          cout<<endl;
        }
        

        for (int i=1; i<(this->diskController->sizeBloque / sizeRegistro); i++) {
          char marcador;
          marcador = *reinterpret_cast<char *>(frame + ((i+1)*sizeRegistro)-5);
          //cout<<"marcador -> "<<marcador<<endl;
          //cout<<"aqui marcador ->"<<marcador<<endl;
          if (marcador != '*') {
            if (marcador == '-') {
              cout<<"-> ";
              int pru;
              pru = *reinterpret_cast<int *>(frame + ((i+1)*sizeRegistro)-4);
              cout << pru << endl;
              byte+=sizeRegistro;
            }
            else {
              for(auto& i : this->diskController->info){
                if (get<1>(i)=="int"){
                    fun_int(frame,byte,mytipos::_INT);
                } else if(get<1>(i)=="double"){
                    fun_double(frame,byte,mytipos::_DOUBLE);
                } else if(get<1>(i)=="str"){
                    fun_char(frame,byte,get<2>(i));
                }
              }
              cout<<"\n";
            }
          }
        }
        file.close();
    }
    void search(string atributo, int objetivo){
        this->diskController->tableToVector("titanic");     
        int aux=0;
        for(auto& i : this->diskController->info){
          if(get<0>(i) != atributo){
            aux += get<2>(i);
          }
          else {
            break;
          }
         }

        //cout<<"aux-> "<<aux;

        int comprobarRegistro; 
        int sizeRegistro = 0;
        for(auto& i : this->diskController->info){
            sizeRegistro += get<2>(i);
        }

        for (int i=1; i<=45; i++) {
          char * frame = bufferManager->getPageOfBuuferPool(i)->data;
          ifstream file;
          file.open("disk/bloque"+to_string(i)+".bin",ios::in | ios::binary);
          file.read(frame,this->diskController->sizeBloque);
          
          char marcador;
          marcador = *reinterpret_cast<char *>(frame + sizeRegistro-5);
          int byte = 0;
          if (marcador != '*') {
            byte+=(sizeRegistro);
          }

          for (int i=1; i<(this->diskController->sizeBloque / sizeRegistro); i++) {
            char marcador;

            marcador = *reinterpret_cast<char *>(frame + ((i+1)*sizeRegistro)-5);
            //cout<<"aqui marcador ->"<<marcador<<endl;
            if (marcador != '*') {
              int pru = get_integer(frame, byte, aux);
              if(pru == objetivo){
                for(auto& i : this->diskController->info){
                  if (get<1>(i)=="int"){
                      fun_int(frame,byte,mytipos::_INT);
                  } else if(get<1>(i)=="double"){
                      fun_double(frame,byte,mytipos::_DOUBLE);
                  } else if(get<1>(i)=="str"){
                      fun_char(frame,byte,get<2>(i));
                  }
                }
                cout<<"\n";
              }
              else {
                byte += sizeRegistro;
              }
              //cout<<"\n";
            }
          }
          file.close();
        }
    }

    void showTable(string nameTable){
        int numRegistros;
        this->diskController->tableToVector(nameTable);
        ifstream numRegistros_dictionary("dictionary/numRegistros.bin", std::ios::binary);
        numRegistros_dictionary.read(reinterpret_cast<char*>(&numRegistros), sizeof(int));
        numRegistros_dictionary.close(); 

        for (int i = 1; i<=diskController->nTotalBloques; i++) {
        mostrarPage(i); 
        }
    }
    int get_integer(char *frame, int byte, int sizeBytes)
    {
        int pru;
        pru = *reinterpret_cast<int *>(frame + byte + sizeBytes);
        return pru;
    }
    
    void fun_int(char *frame, int &byte, int sizeBytes)
    {
        int pru;
        pru = *reinterpret_cast<int *>(frame + byte);
        std::cout << pru << "|";
        byte += sizeBytes;
    }
    void fun_float(char *frame, int &byte, int sizeBytes)
    {
        int flo;
        flo = *reinterpret_cast<float *>(frame + byte);
        std::cout << flo << "|";
        byte += sizeBytes;
    }
    void fun_double(char *frame, int &byte, int sizeBytes)
    {
        int dou;
        dou = *reinterpret_cast<double *>(frame + byte);
        std::cout << dou << "|";
        byte += sizeBytes;
    }
    void fun_char(char *frame, int &byte, int sizeBytes)
    {
        char texto;
        for (int i = 0; i < sizeBytes; i++)
        {
            texto = *reinterpret_cast<char *>(frame + byte);
            std::cout << texto;
            byte += 1;
        }
        std::cout << "|";
    }

    int printUbicacionRegistro(int objetivo){
        this->diskController->tableToVector("titanic");     

        int sizeRegistro = 0;
        for(auto& i : this->diskController->info){
            sizeRegistro += get<2>(i);
        }

        for (int i=1; i<=diskController->NumBLoquesEnUso; i++) {
          int bloque = i-1;
          char * frame = bufferManager->getPageOfBuuferPool(i)->data;
          ifstream file;
          file.open("disk/bloque"+to_string(i)+".bin",ios::in | ios::binary);
          file.read(frame,this->diskController->sizeBloque);
          
          char marcador;
          marcador = *reinterpret_cast<char *>(frame + sizeRegistro-5);
          int byte = 0;
          if (marcador != '*') {
            byte+=(sizeRegistro);
          }

          for (int i=1; i<(this->diskController->sizeBloque / sizeRegistro); i++) {
            char marcador;

            marcador = *reinterpret_cast<char *>(frame + ((i+1)*sizeRegistro)-5);
            if (marcador != '*') {
              int pru = get_integer(frame, byte, 0);
              if(pru == objetivo){
                for(auto& i : this->diskController->info){
                  if (get<1>(i)=="int"){
                      fun_int(frame,byte,mytipos::_INT);
                  } else if(get<1>(i)=="double"){
                      fun_double(frame,byte,mytipos::_DOUBLE);
                  } else if(get<1>(i)=="str"){
                      fun_char(frame,byte,get<2>(i));
                  }
                }

                int nSector = ((i*sizeRegistro) % disco->capacidadDelSector == 0) ? ((i*sizeRegistro)/disco->capacidadDelSector) : ((i*sizeRegistro)/disco->capacidadDelSector)+1;
                nSector = nSector+(8*bloque);

                return bloque+1; 
              }
              else {
                byte += sizeRegistro;
              }
            }
          }
          file.close();
        }
        return 0;
    }
    void deleteRegistro(int objetivo){
        this->diskController->tableToVector("titanic");     

        int sizeRegistro = 0;
        for(auto& i : this->diskController->info){
            sizeRegistro += get<2>(i);
        }

        for (int i=1; i<=diskController->nTotalBloques; i++) {
          bool SeEncontro = false;
          int bloque = i-1;
          char * frame = bufferManager->getPageOfBuuferPool(i)->data;
          ifstream file;
          file.open("disk/bloque"+to_string(i)+".bin",ios::in | ios::binary);
          file.read(frame,this->diskController->sizeBloque);
          
          char marcador;
          marcador = *reinterpret_cast<char *>(frame + sizeRegistro-5);
          int byte = 0;
          if (marcador != '*') {
            byte+=(sizeRegistro);
          }
          else {
            return;
          }

          for (int i=1; i<(this->diskController->sizeBloque / sizeRegistro); i++) {
            char marcador;

            marcador = *reinterpret_cast<char *>(frame + ((i+1)*sizeRegistro)-5);
            if (marcador != '*') {
              int pru = get_integer(frame, byte, 0);
              if(pru == objetivo){
                SeEncontro = true;
                //cout<<"lo encontro"<<endl;
                for(int i=0; i<sizeRegistro-4; i++){
                  frame[byte+1] = '-';
                  byte++;
                }
                byte+=4;
                break;
              }
              else {
                byte += sizeRegistro;
              }
            }
          }
          if(SeEncontro){
            int sustiByte = sizeRegistro-4;
            int aux = 0;
            while (true) {
              char c = *reinterpret_cast<char *>(frame + aux + sustiByte -1);
              //cout<<"c-> "<<c<<endl;
              if ((c != '*') && (c == '-')) {
                int free = get_integer(frame,sustiByte+aux,0);
                if (free == 0) {
                  //cout<<"ingresa = 0"<<endl;
                  //cout<<"reemplaza "<<sustiByte+aux<<"-> "<<byte-sizeRegistro;
                  *reinterpret_cast<int*>(&frame[sustiByte+aux]) = byte-sizeRegistro;
                    file.close();
  
                    *reinterpret_cast<int*>(&frame[byte-4]) = 0;
                    ofstream save;
                    save.open("disk/bloque"+to_string(i)+".bin",ios::out | ios::binary);
                    save.write(frame,diskController->sizeBloque);
                    save.close();
                    diskController->bloqueASector(i);
                    return;
                }
                else{
                  aux = free;
                }
              }
            }
          }

          file.close();
        }
    }

  void insertarRegistro(){
    this->diskController->tableToVector("titanic");     

    int sizeRegistro = 0;
    for(auto& i : this->diskController->info){
      sizeRegistro += get<2>(i);
    }

    //cout<<1<<endl;
    int i;
    for (i = 0; i<=diskController->nTotalBloques; i++) {
      char * frame = bufferManager->getPageOfBuuferPool(i)->data;
      ifstream file;
      file.open("disk/bloque"+to_string(i)+".bin",ios::in | ios::binary);
      file.read(frame,this->diskController->sizeBloque);
      
      char marcador;
      int ptr;
      marcador = *reinterpret_cast<int *>(frame + sizeRegistro-5);
      if (marcador == '*') {
        break;
      }

      ptr = *reinterpret_cast<int *>(frame + sizeRegistro-4);
      //cout<<"ptr-> "<<ptr<<endl;
      if (ptr == 0){

        //cout<<2<<endl;
        continue;
      }
      else{
        int entero;
        double doble;
        string cad;
        int prePtr = ptr;
        int aux = 0;
        int sustiByte = sizeRegistro-4;

        //cout<<2<<endl;
        while (true) {
          ptr = get_integer(frame,sustiByte+ptr,0);
          //cout<<"ptr-> "<<ptr<<endl;
          if(ptr == 0){

            //cout<<"preptr-> "<<prePtr<<endl;
            *reinterpret_cast<int*>(&frame[aux+sustiByte]) = 0;

            cout<<" Ingrese los datos: \n";
            for (auto& i : this->diskController->info ) {
              cout<<"Ingrese "<<get<0>(i)<< "-> "; 
              if (get<1>(i)=="int"){
                  getline(cin,cad); 
                  if(cad==""){
                      entero = -1;
                  } else {
                      entero = stoi(cad);
                  }
                  *reinterpret_cast<int*>(&frame[prePtr]) = entero;
                  prePtr+=get<2>(i);
              } else if(get<1>(i)=="double"){
                  getline(cin,cad); 
                  if(cad==""){
                      doble = -1;
                  } else {
                      doble = stod(cad);
                  }
                  *reinterpret_cast<double*>(&frame[prePtr]) = doble;
                  prePtr+=get<2>(i);
              } else if(get<1>(i)=="str"){
                  getline(cin,cad); 
                  cad.resize(get<2>(i), ' ');
                  int tam=get<2>(i);
                  for(int i=0; i<tam;i++){
                    frame[prePtr] = cad[i];
                    prePtr++;
                  }
              }
            }
            ofstream save;
            save.open("disk/bloque"+to_string(i)+".bin",ios::out | ios::binary);
            save.write(frame,diskController->sizeBloque);
            save.close();
            diskController->bloqueASector(i);
            return;
          }
          else {
            aux = prePtr;
            prePtr = ptr;
          }
        }
      }
    }
    ifstream file;
    file.open("disk/bloque"+to_string(i-1)+".bin",ios::in | ios::binary);
    char * frame = bufferManager->getPageOfBuuferPool(i)->data;
    file.read(frame,this->diskController->sizeBloque);
    
    char marcador;
    int aux =0;
    string cad;
    int entero;
    double doble;
    int ptr;
    while (true) {
      marcador = *reinterpret_cast<int *>(frame + aux);
      if (marcador == '*') {
        break;
      }
      aux += sizeRegistro;
    }
            cout<<" Ingrese los datos: \n";
            for (auto& i : this->diskController->info ) {
              cout<<"Ingrese "<<get<0>(i)<< "-> "; 
              if (get<1>(i)=="int"){
                  getline(cin,cad); 
                  if(cad==""){
                      entero = -1;
                  } else {
                      entero = stoi(cad);
                  }
                  *reinterpret_cast<int*>(&frame[aux]) = entero;
                  aux+=get<2>(i);
              } else if(get<1>(i)=="double"){
                  getline(cin,cad); 
                  if(cad==""){
                      doble = -1;
                  } else {
                      doble = stod(cad);
                  }
                  *reinterpret_cast<double*>(&frame[aux]) = doble;
                  aux+=get<2>(i);
              } else if(get<1>(i)=="str"){
                  getline(cin,cad); 
                  cad.resize(get<2>(i), ' ');
                  int tam=get<2>(i);
                  for(int i=0; i<tam;i++){
                    frame[aux] = cad[i];
                    aux++;
                  }
              }
            }
            ofstream save;
            save.open("disk/bloque"+to_string(i-1)+".bin",ios::out | ios::binary);
            save.write(frame,diskController->sizeBloque);
            save.close();
            diskController->bloqueASector(i-1);
  }
};
