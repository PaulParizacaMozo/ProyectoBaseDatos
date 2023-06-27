#include <iostream>
#include <cstring>
#include "DiskController.h"
#include "Disco.h"
#include "SGDB.h"
using namespace std;


int main(){
    Disco myDisk;
    DiskController myDiskController(&myDisk);
    SGDB sistema(&myDisk,& myDiskController);

    cin.get();

    int opc;
    bool salir = false;
    while(salir==false){
        cout<<"\n-------------------| MENU |--------------------\n";
        cout<<"\nIngrese una opcion:\n";
        cout<<"[1] Menu de Disco\n";
        cout<<"[2] Menu del sistema de Base de Datos\n";
        cin>>opc;
        if(opc==1){
            int opc2;
            cout<<"[1] Imprimir Metadata Sector n\n";
            cout<<"[2] Imprimir Metadata Bloque n\n";
            cout<<"[3] Imprimir Metadata Registro n\n";
            cout<<"[4] Mostrar informacion del disco n\n";
            cin>>opc2;
            cin.ignore();
            if(opc2==1){
                int numSector;
                cout<<"Ingrese un numero de sector => ";
                cin>>numSector;
                myDiskController.printSector(numSector);
            } else if(opc2==2){
                int numBloque;
                cout<<"Ingrese un numero de sector => ";
                cin>>numBloque;
                //myDiskController.printBloque(numBloque);
            }else if (opc2 == 4) {
                myDiskController.disco->showInfoDisco();
            }

        } else if(opc==2){

            int opc2;
            cout<<"[1] Crear Tabla\n";
            cout<<"[2] Insert File ()\n";
            cout<<"[3] Mostrar Bloque n\n";
            cout<<"[4] Insert 1 registro n\n"; // ident si es fijo o variable de acuerdo al esquema de tablas, e insertarlo en binario
            cout<<"[5] Delete 1 registro n\n";
            cout<<"[6] SELECT * FROM titanic\n";
            cin>>opc2;
            cin.ignore();

            if(opc2==1){
                cout<<" >> ";
                string prompt;
                getline(cin,prompt);
                sistema.createTable(prompt);
            } else if(opc2==2){
                string prompt, prompt2;
                getline(cin,prompt);
                getline(cin,prompt2);
                myDiskController.uploadTableToDisk(prompt,prompt2);
            } else if(opc2==3){
                cout<<" >> Ingrese el numero de bloque: ";
                string prompt;
                getline(cin,prompt);
                sistema.mostrarPage(stoi(prompt));
                sistema.bufferManager->showPageTable();
                //cout<<"MUESTRA BLOQUE 100 ----\n";
                //sistema.mostrarPage(40);
                //sistema.mostrarPage(41);
                //sistema.mostrarPage(42);
                //cout<<"\n---\n";
            } else if(opc2==6){
                sistema.showTable("titanic");
                sistema.bufferManager->showPageTable();
            }

        } else if(opc==0){
            salir = true;
        }
    }
    return 0;
}