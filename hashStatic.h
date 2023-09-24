#include <iostream>
#include <functional>
#include "./nodos.h"

/* estructura del bucketFIle con freelist tipo LIFO. 
header
record
record
......
record
--------- fin de los M primeras paginas
record next
record next
......
record next

el next es por el freelist. Las primeras M paginas no se pueden eliminar.
*/

/*estructura del dataFile con freelist tipo LIFO
header
record next
record next
......
record next
*/


template<typename TK, typename Record = Player,
        typename getLlave = std::function<TK(const Record&)>> 
class HashStatic{
    string dataFile;
    string bucketFile;

    long M;

    void buildHashIndex(){        
        long trash = -1;
        //creo el bucketFile con el header y las M primeras paginas vacias.
        std::ofstream fileBucket(bucketFile, ios::binary);
        fileBucket.write((char*) &trash, sizeof(long));
        DataPage<TK> dataPage;

        for(int i = 0; i < M; i++){
            fileBucket.write((char*) &dataPage, sizeof(dataPage));
        }

        fileBucket.close();

        std::ifstream fileData(dataFile, ios::binary);
        Record record;

        fileData.seekg(sizeof(long), ios::beg);
        long position = 0;
        while(fileData.read((char*) &record, sizeof(record))){
            fileData.read((char*) &trash, sizeof(long));

            Pares<TK> par(getLlave(record), position);
            //inserto en el bucketFile
            this->insertPares(par);

            //posicion relativa en el datafile.
            position = (fileData.tellg() - sizeof(long) )/ (sizeof(record) + sizeof(long));
        }
        
        fileData.close();
        cout << "acabe" << endl;
    }
    
    void insertPares(Pares<TK> par){
        std::fstream fileBucket(bucketFile, ios::in|ios::out|ios::binary);
        long header;
        DataPage<TK> dataPage;
        fileBucket.read((char*) &header, sizeof(long));
        //encuentro la posicion relativa con el hash
        long mainPos = std::hash(par.key) % M;

        fileBucket.seekg(sizeof(long) + mainPos*sizeof(dataPage), ios::beg);
        fileBucket.read((char*) &dataPage, sizeof(dataPage));

        //si no se puede insertar me voy a las paginas overflow, donde se maneja un freelist.
        if(!dataPage.insert(par)){
            overPos = dataPage.next;
            if(overPos != -1){
                fileBucket.seekg(overPos, ios::beg);
                fileBucket.read((char*) &dataPage, sizeof(dataPage));
            }
            //si no hay next o no puedo insertar, creo una nueva pagina
            if(overPos == -1 || !dataPage.insert(par)){
                dataPage<TK> nueva;
                nueva.insert(par);
                nueva.next = overPos;
                long newPos = 0;

                //manejo el freelist, y obtengo el newPos.
                if(header == -1){
                    fileBucket.seekp(0, ios::end);
                    newPos = fileBucket.tellp();
                    fileBucket.write((char*) &nueva, sizeof(nueva));
                    fileBucket.write((char*) header, sizeof(header)); //header == -1
                }else{
                    newPos = header;
                    long next;
                    long empty = -1;

                    fileBucket.seekg(header + sizeof(DataPage<TK>), ios::beg);
                    fileBucket.read((char*) &next, sizeof(long));

                    fileBucket.seekp(newPos,ios::beg);
                    fileBucket.write((char*) &nueva, sizeof(nueva));
                    fileBucket.write((char*) &empty, sizeof(long));

                    //cambio el header
                    fileBucket.seekp(0,ios::beg);
                    fileBucket.write((char*) &next, sizeof(long));
                }

                //modifico el next  de la pagina mainPos y le ingreso el newPos
                fileBucket.seekg(sizeof(long) + mainPos*sizeof(dataPage), ios::beg);
                fileBucket.read((char*) &dataPage, sizeof(dataPage));
                dataPage.next = newPos;

                fileBucket.seekp(sizeof(long) + mainPos*sizeof(dataPage), ios::beg);
                file.write((char*) &dataPage, sizeof(dataPage));

            }else{
                fileBucket.seekp(overPos, ios::beg);
                fileBucket.write((char*) &dataPage, sizeof(dataPage));
            }
        }
        else{
            fileBucket.seekp(sizeof(long) + mainPos*sizeof(dataPage), ios::beg);
            fileBucket.write((char*) &dataPage, sizeof(dataPage));
        }

        bucketFile.close();
    }

    //devuelve las posiciones a eliminar en el datafile.
    vector<long> removePares(Pares<TK>);

    //retorna la posicion en el datafile
    vector<long> searchPares(TK key);


public:
    HashStatic(int M, string csv, string dataFile, string bucketFile){
        this->M = M;
        this->dataFile = dataFile;
        this->bucketFile = bucketFile;

        if(!existe(dataFile)){
            load(csv, dataFile);

            buildHashIndex();
        }
    }

    vector<Record> search(TK key);

    void insert(Record record);

    void remove(TK key);

    vector<Record> rangeSearch(TK inf, TK sup);


};