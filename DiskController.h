#pragma once
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <cmath>
#include "HeadersHDD/Disco.h"
#include "tipos.cpp"
#include "design/lineas.cpp"
using namespace std;

class DiskController{
public:
    Disco * disco;
    int nTotalBloques;
    int sizeBloque;
    int numSectoresPorBloque;
    int NumBLoquesEnUso;
    vector<tuple<string,string,int>> info;
    string nameTable;
    
    DiskController(Disco* disco){
        this->disco = disco;
        this->nTotalBloques = disco->getNumBloques();
        this->sizeBloque = disco->capacidadDelSector * disco->sectoresPorBloque;
        this->numSectoresPorBloque = disco->sectoresPorBloque;

        int sectorActual = 1;
        if(!system("mkdir disk")){ // si no existen bloques, los crea. 
            for(int i=1; i<= nTotalBloques; i++){
                string name_bloque = "bloque";
                name_bloque += to_string(i);
                ofstream archivo("disk/"+name_bloque+".bin", std::ios::binary);

                char* buffer = new char[sizeBloque];
                if (!archivo) { std::cout << "No se pudo abrir el archivo." << std::endl; return; }
                int _pista,_superf,_plat;
                for(int i=0; i<numSectoresPorBloque;i++){
                  _pista = ((sectorActual) % disco->numSectoresPorPista == 0) ? ((sectorActual)/disco->numSectoresPorPista) : ((sectorActual)/disco->numSectoresPorPista)+1;
                  _superf = (_pista % disco->numPistasPorSuperficie == 0) ? (_pista/disco->numPistasPorSuperficie) : (_pista/disco->numPistasPorSuperficie)+1;
                  _plat = (_superf % 2 == 0) ? (_superf/2) : (_superf/2)+1;

                  ifstream input("HDD/Plato" + to_string(_plat)+"/Superficie"+to_string(_superf)+"/Pista"+to_string(_pista)+"/Sector"+to_string(sectorActual)+".bin", std::ios::binary);
                  
                  input.read(buffer + disco->capacidadDelSector * i, disco->capacidadDelSector);
                  input.close();
                  sectorActual++;
                }

                archivo.write(buffer, sizeBloque);
                delete [] buffer;
                archivo.close();
            }
            std::cout << "Se han generado los bloques correctamente" << std::endl;
        }
    }

    ~DiskController(){
        //Al finalizar el programa los bloques virtuales desaparecen
        system("rm -r disk");
    }

    //Escribe el bloque virtual al disco fisico
    void bloqueASector(int nBloque){
      ifstream archivo("disk/bloque"+to_string(nBloque)+".bin", std::ios::binary);
        for (int i=1; i<=numSectoresPorBloque; i++) {
          int nSector = numSectoresPorBloque*(nBloque-1) + i;
          //cout<<"nSector -> "<<nSector<<endl;
          int _pista,_superf,_plat;
          _pista = ((nSector) % disco->numSectoresPorPista == 0) ? ((nSector)/disco->numSectoresPorPista) : ((nSector)/disco->numSectoresPorPista)+1;
          _superf = (_pista % disco->numPistasPorSuperficie == 0) ? (_pista/disco->numPistasPorSuperficie) : (_pista/disco->numPistasPorSuperficie)+1;
          _plat = (_superf % 2 == 0) ? (_superf/2) : (_superf/2)+1;
          ofstream output("HDD/Plato" + to_string(_plat)+"/Superficie"+to_string(_superf)+"/Pista"+to_string(_pista)+"/Sector"+to_string(nSector)+".bin", std::ios::binary);
          
          char* buffer = new char[disco->capacidadDelSector];
          archivo.read(buffer, disco->capacidadDelSector);
          output.write(buffer, disco->capacidadDelSector);
          delete [] buffer;
          output.close();
        }
      archivo.close();
    }

    //Transforma la informacion de una tabla en un vector para mejor manejo
    void tableToVector(){ //cambios
        this->info.clear();
        ifstream data;
        data.open("esquema",ios::in);
        string linea;
        while(getline(data,linea)){
            stringstream stream(linea);
            string word;
            getline(stream,word,'#');
            if (word == nameTable) {
                break;
            }
        }
        //Lleva a la memoria dinamica la ubicacion de los bytes por cada atributo
        //cout<<linea<<endl;
        stringstream stream(linea);
        string w1,w2,w3;
        getline(stream,w1,'#');
        getline(stream,w1,'#');
        while (!stream.eof()) {
            getline(stream,w1,'#');
            getline(stream,w2,'#');
            if ((w2 == "int")){
                info.push_back(make_tuple(w1,w2,mytipos::_INT));
                continue;
            } else if(w2 == "double"){
                info.push_back(make_tuple(w1,w2,mytipos::_DOUBLE));
                continue;
            } else if(w2 == "float"){
                info.push_back(make_tuple(w1,w2,mytipos::_FLOAT));
                continue;
            } else if(w2 == "bool"){
                info.push_back(make_tuple(w1,w2,mytipos::_BOOL));
                continue;
            }
            getline(stream,w3,'#');
            info.push_back(make_tuple(w1,w2,stoi(w3)));
        }
        //cout<<info.size()<<"tam"<<endl;
        //   for(int i=0; i<(int)info.size(); i++){
        //     cout<<get<0>(info[i])<<"-"; 
        //     cout<<get<1>(info[i])<<"-"; 
        //     cout<<get<2>(info[i])<<"\n"; 
        //   }
        data.close();
    }

    
    //Importa la tabla al disco
    void uploadTableToDisk(string fileToImport, string tablaName){ 
        tableToVector();//cambios
        cout<<"size -> "<<info.size()<<endl;
        cout<<"name -> "<<tablaName<<endl;
        convertCSV_inTuplas(fileToImport,nameTable,(int)info.size()); //cambios

        const char* carpetaDisco = "disco\\";

        int contador = 0;
        
        char caracter;
        int entero;
        float decimal;
        double decimalGrande;

        ifstream archivoTupla(nameTable); //cambios
        //ifstream archivoTupla("titanic");
        ofstream salida("titanicbinario.bin", std::ios::binary);
        
        char c;
        char z;

        string linea;
        string palabra;
        int contadorBytes = 0;
        int numRegistrosDeLaTabla = 0;
        while(getline(archivoTupla,linea)){
            stringstream stream(linea);
            int indice = 0;
            while(getline(stream,palabra,'#')){ // palabra 5    20
                if(get<1>(info[indice])=="int"){
                    if(palabra==""){
                        entero = -1;
                    } else {
                        entero = stoi(palabra);
                    }
                    salida.write(reinterpret_cast<char*>(&entero), sizeof(entero));
                } else if (get<1>(info[indice])=="double"){
                    if(palabra==""){
                        decimalGrande = -1;
                    } else {
                        decimalGrande = stod(palabra);
                    }
                    salida.write(reinterpret_cast<char*>(&decimalGrande), sizeof(decimalGrande));
                } else if (get<1>(info[indice])=="str"){
                    for (auto& i : palabra){
                        salida.write(reinterpret_cast<char*>(&i), sizeof(char));
                    }
                    int bytesPorCompletar =  get<2>(info[indice]) - palabra.size();
                    for (auto i=0; i<bytesPorCompletar; i++){
                        z = ' ';
                        salida.write(reinterpret_cast<char*>(&z), sizeof(char));
                    }
                }
                indice++;
            }
            numRegistrosDeLaTabla++;
        }

        //cout<<"\n-------numRegistrosDeLaTabla: "<<numRegistrosDeLaTabla<<"\n";
        fstream dictionary1("dictionary/dictionary.bin",ios::in | ios::out | std::ios::binary);
        int posicion = buscarTablaenDictionary(nameTable)+100;
        //cout<<"Posicion -> "<<posicion<<endl;
        //cout<<"NUMERO DE REGIS -> "<<numRegistrosDeLaTabla<<endl;
        dictionary1.seekp(posicion);
        dictionary1.write(reinterpret_cast<char*>(&numRegistrosDeLaTabla), sizeof(int));
        dictionary1.close();

        //for ( auto& atributo : info ){
        //    cout<<get<0>(atributo)<<"-"; 
        //    cout<<get<1>(atributo)<<"-"; 
        //    cout<<get<2>(atributo)<<"\n"; 
        //}
        
        archivoTupla.close();
        salida.close();

        int sizeRegistro = 0;
        for(auto& i : info){
            if(get<1>(i)=="int"){
                sizeRegistro += 4;
            } else if (get<1>(i)=="double"){
                sizeRegistro += 8;
            } else if (get<1>(i)=="str"){
                sizeRegistro += get<2>(i);
            }
        }
        setDataInBloques(sizeRegistro);
    }

    int buscarTablaenDictionary(string objetivo){
        fstream dictionary("dictionary/dictionary.bin",ios::in | ios::out | std::ios::binary);
        dictionary.seekg(0,ios::end);
        int tamañoDictionary = dictionary.tellg();
        dictionary.seekg(0,ios::beg);
        int byte=12;//Ahi inician las tablas
        while(byte<=tamañoDictionary){
            dictionary.seekg(byte);
            string temp = "";
            char c;
            for(int i = 0; i<100; i++){
                dictionary.read(reinterpret_cast<char*>(&c), sizeof(c));
                temp += c;
            }
            //cout<<temp;
            string aux;
            for (char ch : temp) {
                if (!std::isspace(ch)) {
                    aux += ch;
                }
            }
            if(aux == nameTable){
                dictionary.close();
                return byte;
            }
            else{
                byte += 116;
            }
        }
        dictionary.close();
        return 0;
    }

void setDataInBloques(int sizeRegistro){

        int contador = 1;
        int cnt = 1;
        int contadorBytesTotal;
        int contAux = 0;
        int numRegistros=0;

        fstream numRegistrosPorTabla_dictionary("dictionary/dictionary.bin",ios::in | ios::out | std::ios::binary);
        int posicion = buscarTablaenDictionary(nameTable)+100;
        numRegistrosPorTabla_dictionary.seekg(posicion);
        numRegistrosPorTabla_dictionary.read(reinterpret_cast<char*>(&numRegistros), sizeof(int));
        numRegistrosPorTabla_dictionary.close();

        ifstream file("titanicbinario.bin", std::ios::binary);
        //ofstream schemaBloques("dictionary/schemaBloquesFijos.bin", std::ios::binary);
        
        file.seekg(0,ios::end);
        int sizeFile = file.tellg();
        file.seekg(0,ios::beg);

        int almacenamientoTotal = this->nTotalBloques*this->sizeBloque;
        
        int contadorRegistros = 0;
        fstream _dictionary("dictionary/dictionary.bin",ios::in | ios::out | std::ios::binary);
        _dictionary.seekg(4);
        _dictionary.read(reinterpret_cast<char*>(&contador), sizeof(int));
        contador++;
        _dictionary.close();
        while((contAux < almacenamientoTotal) && (contadorRegistros < numRegistros)){ //sizeFile
            //cout<<"entro"<<endl;
            ofstream bloque("disk/bloque"+to_string(contador)+".bin", std::ios::binary);
            char marcador = '-'; //puntero inicial de la freelist que se inicializara en 0
            bloque.write(reinterpret_cast<char*>(&marcador), sizeof(char));
            bloque.seekp(sizeRegistro-sizeof(int)-sizeof(char));

            int freelist = 0; //puntero inicial de la freelist que se inicializara en 0
            bloque.write(reinterpret_cast<char*>(&marcador), sizeof(char));
            bloque.write(reinterpret_cast<char*>(&freelist), sizeof(int));
            contadorBytesTotal = sizeRegistro*2;
            
            while((contadorBytesTotal <= sizeBloque) && (contadorRegistros < numRegistros)){ // sizeRegistro <= (sizeBloque-contadorBytesTotal)
                char* buffer = new char[sizeRegistro];
                file.read(buffer,sizeRegistro);
                bloque.write(buffer,sizeRegistro);
                contadorBytesTotal += sizeRegistro;
                delete [] buffer;
                contadorRegistros++;
                //cout<<"contadorRegistros ->>>>>>>>>>> "<<contadorRegistros<<endl;
            }

            contAux += (contadorBytesTotal-sizeRegistro);
            //int numRegistrosEnBloque = (contAux / (sizeRegistro*contador)) - 1;
            //schemaBloques.write(reinterpret_cast<char*>(&contadorRegistros), sizeof(int));
            
            char * buffer = new char[sizeBloque-contadorBytesTotal+sizeRegistro];
            // Rellenar el buffer con datos 
            for (int j = 0; j < (sizeBloque - contadorBytesTotal+sizeRegistro); j++) {
                buffer[j] = '*';
            }
            bloque.write(buffer, (sizeBloque - contadorBytesTotal+sizeRegistro));
            delete [] buffer;

            contAux += (sizeBloque-contadorBytesTotal+sizeRegistro);
            //cout<<"contAux: "<<contAux<<"\t\t";

            bloque.close();

            bloqueASector(contador);        
            contador++;
        }
        contador--;
            fstream dictionary("dictionary/dictionary.bin",ios::in | ios::out | std::ios::binary);
            int inicioBLoque,finBLoque;
            dictionary.seekp(0,ios::beg);
            dictionary.seekg(4); 
            dictionary.read(reinterpret_cast<char*>(&inicioBLoque), sizeof(int));
            dictionary.seekp(0,ios::beg);

            if(inicioBLoque == 0){
                dictionary.seekp(0,ios::beg);
                int posicion = buscarTablaenDictionary(nameTable)+104;
                dictionary.seekp(posicion);
                inicioBLoque++;
                dictionary.write(reinterpret_cast<char*>(&inicioBLoque), sizeof(int));
                dictionary.write(reinterpret_cast<char*>(&contador), sizeof(int));
                dictionary.close();
            }
            else {
                dictionary.seekp(0,ios::beg);
                int posicion = buscarTablaenDictionary(nameTable)+104;
                dictionary.seekp(posicion);
                inicioBLoque++;
                dictionary.write(reinterpret_cast<char*>(&inicioBLoque), sizeof(int));
                dictionary.write(reinterpret_cast<char*>(&contador), sizeof(int));
            dictionary.close();
            }


            fstream dictionary1("dictionary/dictionary.bin",ios::in | ios::out | std::ios::binary);
            dictionary1.seekp(0,ios::beg);
            dictionary1.seekp(4);
            dictionary1.write(reinterpret_cast<char*>(&contador), sizeof(int));
            dictionary1.close();

        file.close();
        this->NumBLoquesEnUso = contador;
        //schemaBloques.close();
    }

void showDictionary(){
    ifstream dictionary("dictionary/dictionary.bin",std::ios::binary);
    dictionary.seekg(0,ios::end);
    int tamañoDictionary = dictionary.tellg();
    dictionary.seekg(0,ios::beg);
    //cout<<"Size -> "<<tamañoDictionary<<endl;
    int byte = 0;
    int auxInt;
    string auxStr;
    dictionary.read(reinterpret_cast<char*>(&auxInt), sizeof(int));
    cout<<"Numero total de bloques: "<<auxInt<<endl;
    dictionary.read(reinterpret_cast<char*>(&auxInt), sizeof(int));
    cout<<"Numero de bloques en uso: "<<auxInt<<endl;
    dictionary.read(reinterpret_cast<char*>(&auxInt), sizeof(int));
    cout<<"Numero de total de tablas: "<<auxInt<<endl;
    byte=12;
    while(byte<tamañoDictionary){
        string temp = "";
        char c;
        for(int i = 0; i<100; i++){
            dictionary.read(reinterpret_cast<char*>(&c), sizeof(c));
            temp += c;
        }
        //cout<<temp;
        string aux;
        for (char ch : temp) {
            if (!std::isspace(ch)) {
                aux += ch;
            }
        }
        cout<<" ->Tabla 1 : "<<aux<<endl;
        dictionary.read(reinterpret_cast<char*>(&auxInt), sizeof(int));
        cout<<"   Numero de total de registros: "<<auxInt<<endl;
        dictionary.read(reinterpret_cast<char*>(&auxInt), sizeof(int));
        cout<<"   Bloque inicial: "<<auxInt<<endl;
        dictionary.read(reinterpret_cast<char*>(&auxInt), sizeof(int));
        cout<<"   Bloque final: "<<auxInt<<endl;
        dictionary.read(reinterpret_cast<char*>(&auxInt), sizeof(int));
        cout<<"   Fijo o Variable: "<<auxInt<<endl;
        byte+=116;
    }
    dictionary.close();
}

    //void readSchemaBloquesFijos(){
    //    int numRegistros;
    //    ifstream schemaBloques("dictionary/schemaBloquesFijos.bin", std::ios::binary);
    //    int i = 1;
    //    while( i <= this->nTotalBloques){
    //        schemaBloques.read(reinterpret_cast<char*>(&numRegistros), sizeof(int));
    //        cout<<"bloque "<<i<<" => "<<numRegistros<<"\n";
    //        i++;
    //    }
    //    schemaBloques.close();
    //}

    void convertCSV_inTuplas(string fileAimportar, string newfile,int natributos){
        cout<<"size -> "<<natributos<<endl;
        tableToVector();
        ifstream i(fileAimportar, ios::in); 
        ofstream f(newfile,ios::out); 
        bool reemplazarActivo = true;
        int indice=0;
        string word="";
        if (i && f) {
            char caracter;
            while (i.get(caracter)) {
                if (((caracter == ',')||(caracter =='\n')) && reemplazarActivo == true ) {
                    if(indice<natributos){ //natributos
                        f<<word;
                        f<<'#';
                        word="";
                        indice++;
                        if (indice >= natributos) { //natributos//cambio
                            indice=0;
                            f<<'\n';
                        }
                    }
                } else if(caracter == '"'){
                    reemplazarActivo = (!reemplazarActivo);
                } else{
                    if(caracter != '\r')
                        word+=caracter;
                }
            }
            cout << "Archivo importado exitosamente." << endl;
        }
        i.close(); 
        f.close(); 
    }

    void printSector(int nSector){
        cout<<"\n"<<lineas::drawLinea(75)<<" | SECTOR "<<nSector<<" | "<<lineas::drawLinea(75)<<"\n";
        
        int _pista,_superf,_plat;
        _pista = ((nSector) % disco->numSectoresPorPista == 0) ? ((nSector)/disco->numSectoresPorPista) : ((nSector)/disco->numSectoresPorPista)+1;
        _superf = (_pista % disco->numPistasPorSuperficie == 0) ? (_pista/disco->numPistasPorSuperficie) : (_pista/disco->numPistasPorSuperficie)+1;
        _plat = (_superf % 2 == 0) ? (_superf/2) : (_superf/2)+1;
        char c;
        cout << "\t" << " IdSector: " << nSector << endl;
        cout << "\t" << " numDePista: " << _pista << endl;
        cout << "\t" << " numDeSuperficie: " << _superf << endl;
        cout << "\t" << " numDePlato: " << _plat << endl;
        cout<<"DATA:\n"<<lineas::linea100<<lineas::linea50<<"\n";
        ifstream input("HDD/Plato" + to_string(_plat)+"/Superficie"+to_string(_superf)+"/Pista"+to_string(_pista)+"/Sector"+to_string(nSector)+".bin", std::ios::binary);
        for(int i=0; i<this->disco->capacidadDelSector;i++){
        input.read(static_cast<char*>(&c),sizeof(char));
        cout << c;
        }        cout<<"\n"<<lineas::linea100<<lineas::linea50<<"\n";
        input.close();
    }

    void printBloque(int numBloque){
        cout<<"\n"<<lineas::drawLinea(75)<<" | BLOQUE "<<numBloque<<" | "<<lineas::drawLinea(75)<<"\n";
        //this->disco->sectores[numSector-1].showInfoSector(); // Imprime info: en que plato, superficie, pista esta
        
        
        cout<<"DATA:\n"<<lineas::linea100<<lineas::linea50<<"\n";
        ifstream bloqueUbicado("disk/bloque"+std::to_string(numBloque)+".bin",ios::binary);
        char c; int contadorFila = 1;
        while(!bloqueUbicado.eof() ){
            bloqueUbicado.read(static_cast<char*>(&c),sizeof(char));
            cout << c;
            //cout << static_cast<int>(c);
            contadorFila++;
            //if(contadorFila%125==0){ cout<<"\n"; }
        }
        bloqueUbicado.close();

        cout<<"\n"<<lineas::linea100<<lineas::linea50<<"\n";
    }

};
