#pragma once
#include <iostream>
#include <unordered_map>
#include "DiskController.h"
#include "Page.h"

class BufferManager {
public:
    DiskController * diskController;

    vector<Page> bufferPool;
    int sizeBufferPool;
    int sizePage;
    
    vector< vector<int> > pageTableLRU; //pageTableLRU
    vector< vector<int> > pageTableMRU; //pageTableMRU
    vector<int> free_list;

    BufferManager(DiskController * diskController){
        this->sizeBufferPool = 4;
        this->diskController = diskController;
        this->sizePage = this->diskController->sizeBloque;
        bufferPool.resize(this->sizeBufferPool);

        for (auto& frame : bufferPool){
            frame = Page(this->sizePage);
        }

        pageTableLRU.resize(sizeBufferPool);
        for(auto& frame : pageTableLRU){
            frame.resize(5); // frameId,Page,id,DirtyBit,PinCount,LastUsed
        }
        
        for (int i=0; i<pageTableLRU.size(); i++){
            for (int j=0; j<pageTableLRU[0].size(); j++){
                if(j==0){
                    pageTableLRU[i][j] = i;
                } else {
                    pageTableLRU[i][j] = -1;
                }
            }
        }
        pageTableMRU.resize(sizeBufferPool);
        for(auto& frame : pageTableMRU){
            frame.resize(5); // frameId,Page,id,DirtyBit,PinCount,LastUsed
        }
        for (int i=0; i<pageTableMRU.size(); i++){
            for (int j=0; j<pageTableMRU[0].size(); j++){
                if(j==0){
                    pageTableMRU[i][j] = i;
                } else {
                    pageTableMRU[i][j] = -1;
                }
            }
        }


    }
    ~BufferManager() { }


    auto getPageOfBuuferPool(int pageId){
        Page* pagina = MRU(pageId);
        return pagina;
    }


    Page* LRU(int pageId){
        for (int i=0; i<pageTableLRU.size(); i++){
            if(pageTableLRU[i][1]==pageId){
                pageTableLRU[i][3]++;
                for(int k=0; k<pageTableLRU.size(); k++){
                    if(pageTableLRU[k][1]!=-1){
                        pageTableLRU[k][4]++;
                    }
                }
                pageTableLRU[i][4] = 0;
                return &bufferPool[i];
            }
        }
        for (int i=0; i<pageTableLRU.size(); i++){
            if(pageTableLRU[i][1]==-1){
                for(int k=0; k<pageTableLRU.size(); k++){
                    if(pageTableLRU[k][1]!=-1){
                        pageTableLRU[k][4]++;
                    }
                }
                pageTableLRU[i][1] = pageId;
                pageTableLRU[i][2] = 0;
                pageTableLRU[i][3] = 0;
                pageTableLRU[i][4] = 0;
                
                return &bufferPool[i];
            }  
        }

        int mayor = pageTableLRU[0][4];
        int pos=0;
        for (int i=0; i<pageTableLRU.size(); i++){
            if(pageTableLRU[i][2]==0 && pageTableLRU[i][4]>mayor){
                mayor = pageTableLRU[i][4];
                pos = i;
            }
        }

        if(pos!=-1){
            for(int k=0; k<pageTableLRU.size(); k++){
                if(pageTableLRU[k][1]!=-1){
                    pageTableLRU[k][4]++;
                }
            }
            pageTableLRU[pos][1] = pageId;
            pageTableLRU[pos][2] = 0;
            pageTableLRU[pos][3] = 0;
            pageTableLRU[pos][4] = 0;
            return &bufferPool[pos];
        }
        //Implementar el pin y unpin para el dirty bit
        //
       return nullptr;
    }

    Page* Clock(int pageId){
        return nullptr;
    }

    Page* MRU(int pageId){
        for (int i=0; i<pageTableMRU.size(); i++){
            if(pageTableMRU[i][1]==pageId){
                pageTableMRU[i][3]++;
                for(int k=0; k<pageTableMRU.size(); k++){
                    if(pageTableMRU[k][1]!=-1){
                        pageTableMRU[k][4]++;
                    }
                }
                pageTableMRU[i][4] = 0;
                return &bufferPool[i];
            }
        }
        for (int i=0; i<pageTableMRU.size(); i++){
            if(pageTableMRU[i][1]==-1){
                for(int k=0; k<pageTableMRU.size(); k++){
                    if(pageTableMRU[k][1]!=-1){
                        pageTableMRU[k][4]++;
                    }
                }
                pageTableMRU[i][1] = pageId;
                pageTableMRU[i][2] = 0;
                pageTableMRU[i][3] = 0;
                pageTableMRU[i][4] = 0;
                
                return &bufferPool[i];
            }  
        }

        int menor = pageTableMRU[0][4];
        int pos=0;
        for (int i=0; i<pageTableMRU.size(); i++){
            if(pageTableMRU[i][2]==0 && pageTableMRU[i][4]<menor){
                menor = pageTableMRU[i][4];
                pos = i;
            }
        }

        if(pos!=-1){
            for(int k=0; k<pageTableMRU.size(); k++){
                if(pageTableMRU[k][1]!=-1){
                    pageTableMRU[k][4]++;
                }
            }
            pageTableMRU[pos][1] = pageId;
            pageTableMRU[pos][2] = 0;
            pageTableMRU[pos][3] = 0;
            pageTableMRU[pos][4] = 0;
            return &bufferPool[pos];
        }

       return nullptr;
    }


    void showpageTableLRU(){
        cout<<"\n LRU tabla \n";
        for (int i=0; i<pageTableLRU.size(); i++){
            for (int j=0; j<pageTableLRU[0].size(); j++){
                cout<<pageTableLRU[i][j]<<"\t";
            }
            cout<<"\n";
        }
        cout<<"\n";
    }
    void showpageTableMRU(){
        cout<<"\n MRU tabla \n";
        for (int i=0; i<pageTableMRU.size(); i++){
            for (int j=0; j<pageTableMRU[0].size(); j++){
                cout<<pageTableMRU[i][j]<<"\t";
            }
            cout<<"\n";
        }
        cout<<"\n";
    }
};


