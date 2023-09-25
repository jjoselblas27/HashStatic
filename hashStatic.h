#include <iostream>
#include <functional>
#include "../auxiliar/nodos.h"

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

el next es por el freelist. Las primeras M paginas no se pueden eliminar aunque esten vacias
*/

/*estructura del dataFile con freelist tipo LIFO
header
record next
record next
......
record next
*/


template<typename TK, typename Record = AudioFeatures,
        typename getLlave = std::function<TK(const Record&)>> 
class HashStatic{
    string dataFile;
    string bucketFile;
    long M;

    getLlave getKey;
    std::hash<TK> hashFunction;
    void buildHashIndex(){        
        long trash = -1;
        //creo el bucketFile con el header y las M primeras paginas vacias.
        std::ofstream fileBucket(bucketFile, ios::binary);
        fileBucket.seekp(0,ios::beg);
        fileBucket.write((char*) &trash, sizeof(long));

        DataPage<TK> dataPage;
        dataPage.principal = true;

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

            Pares<TK> par(getKey(record), position);
            //inserto en el bucketFile
            this->insertPares(par);

            //posicion relativa en el datafile.
            position = (fileData.tellg() - sizeof(long) )/ (sizeof(record) + sizeof(long));
        }
        
        fileData.close();
    }
    
    void insertPares(Pares<TK> par){
        std::fstream fileBucket(bucketFile, ios::in|ios::out|ios::binary);
        
        long header;
        DataPage<TK> dataPage;
        fileBucket.read((char*) &header, sizeof(long));

        //encuentro la posicion relativa con el hash
        
        long mainPos = hashFunction(par.key) % M;
        //long mainPos = 0;

        fileBucket.seekg(sizeof(long) + mainPos*sizeof(dataPage), ios::beg);
        fileBucket.read((char*) &dataPage, sizeof(dataPage));

        dataPage.show();

        //si no se puede insertar me voy a las paginas overflow, donde se maneja un freelist.
        if(!dataPage.insert(par)){
            long overPos = dataPage.next;
            if(overPos > 0){

                fileBucket.seekg(overPos, ios::beg);
                fileBucket.read((char*) &dataPage, sizeof(dataPage));
            }
            //si no hay next o no puedo insertar, creo una nueva pagina
            if(overPos == -1 || !dataPage.insert(par)){
                DataPage<TK> nueva;
                nueva.insert(par);
                nueva.next = overPos;
                long newPos = 0;
                //manejo el freelist, y obtengo el newPos.
                if(header == -1){
                    fileBucket.seekp(0, ios::end);
                    newPos = fileBucket.tellp();
                    fileBucket.write((char*) &nueva, sizeof(nueva));
                    fileBucket.write((char*) &header, sizeof(header)); //header == -1
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
                fileBucket.write((char*) &dataPage, sizeof(dataPage));

            }else{
                fileBucket.seekp(overPos, ios::beg);
                fileBucket.write((char*) &dataPage, sizeof(dataPage));
            }
        }
        else{
            fileBucket.seekp(sizeof(long) + mainPos*sizeof(dataPage), ios::beg);
            fileBucket.write((char*) &dataPage, sizeof(dataPage));
        }

        fileBucket.close();
    }

    //devuelve las posiciones a eliminar en el datafile.
    vector<long> removePares(TK key){
        std::fstream fileBucket(bucketFile, ios::in|ios::out|ios::binary);
        
        DataPage<TK> dataPage;
        vector<long> pos_records; 
        
        //posicion absoluta 
        long absPos = sizeof(long) + (hashFunction(key) % M)*sizeof(dataPage);
        //long absPos = sizeof(long);
        dataPage.next = absPos;
        long parentPage = absPos;
        do{
            absPos = dataPage.next;
            fileBucket.seekg(absPos, ios::beg);
            fileBucket.read((char*) &dataPage, sizeof(dataPage));
            bool cambio = false;


            dataPage.show();
            // cambio el ultimo por el que estoy revisando
            for (int i = 0; i < dataPage.count; i++)
            {
                if (dataPage.page[i].key == key)
                {
                    cambio = true;
                    pos_records.push_back(dataPage.page[i].position);

                    dataPage.page[i] = dataPage.page[dataPage.count - 1];
                    dataPage.count--;
                    i--;
                }
            }
            cout << "new count:" << dataPage.count << endl;
            //eliminare una pagina si es que no es principal
            if(dataPage.count == 0 && !dataPage.principal){
                cout << "cambiazo  " << endl;
                //modificacion del padre
                DataPage<TK> padre;
                fileBucket.seekg(parentPage, ios::beg);
                fileBucket.read((char*) &padre, sizeof(padre));
                padre.next = dataPage.next;

                fileBucket.seekp(parentPage, ios::beg);
                fileBucket.write((char*) &padre, sizeof(padre));

                //modificacion debido al freelist
                long header;
                fileBucket.seekg(0, ios::beg);
                fileBucket.read((char *)&header, sizeof(header));

                // sobreescribo el next de absPos
                fileBucket.seekp(absPos + sizeof(dataPage), ios::beg);
                fileBucket.write((char *)&header, sizeof(header));

                // sobreescribo en header
                fileBucket.seekp(0, ios::beg);
                fileBucket.write((char *)&absPos, sizeof(absPos));

                //parentPage mantiene su posicion.
            }else {
                if(cambio){
                    cout << "cambio true" << endl;
                //si hubo algun cambio lo escribo en memoria
                    fileBucket.seekp(absPos, ios::beg);
                    fileBucket.write((char *)&dataPage, sizeof(dataPage));
                }

                parentPage = absPos;
            }

        }while(dataPage.next != -1);

        fileBucket.close();

        return pos_records;
    }

    //retorna la posicion en el datafile
    vector<long> searchPares(TK key){

        std::fstream fileBucket(bucketFile, ios::in|ios::out|ios::binary);
        DataPage<TK> dataPage;
        vector<long> pos_records; 
        //posicion absoluta 
        long absPos = sizeof(long) + (hashFunction(key) % M)*sizeof(dataPage);
        //long absPos = sizeof(long);
        do
        {

            fileBucket.seekg(absPos, ios::beg);
            fileBucket.read((char*) &dataPage, sizeof(dataPage));
            vector<long> temp = dataPage.search(key);
            
            dataPage.show();

            pos_records.insert(pos_records.end(), temp.begin(), temp.end());

            absPos = dataPage.next;

        } while (absPos != -1);
        
        fileBucket.close();

        return pos_records;
    }


public:
    HashStatic(int M, string csv, string data, string value, getLlave getKey){
        this->M = M;
        this->dataFile = "./indices/"  + data + ".bin";
        this->bucketFile = "./indices/bucket_" + value + ".bin";
        this->getKey = getKey;

        if(!existe(this->dataFile)){
            cout << "no existe " << endl;
            load<Record>(csv, this->dataFile);
            cout << "hice el load" << endl;
            buildHashIndex();
            cout << "contrui hash " << endl;
        }
        
    }

    vector<Record> search(TK key){
        vector<long> pos_records = searchPares(key);

        ifstream fileData(dataFile, ios::binary);
        std::vector<Record> listRecords;

        Record record;
        for (long pos: pos_records){
            fileData.seekg(sizeof(long) + pos*(sizeof(Record) + sizeof(long)), ios::beg);
            fileData.read((char*) &record, sizeof(record));

            listRecords.push_back(record);
        }

        fileData.close();

        return listRecords;
    }

    void insert(Record record){
        long pos = insertDataFile<Record>(record, dataFile);
        Pares<TK> par(getKey(record), pos);

        insertPares(par);
    }

    void remove(TK key){
        vector<long> pos_records = removePares(key);

        deleteListDataFile<Record>(pos_records, dataFile);
    }

    vector<Record> rangeSearch(TK inf, TK sup){
        ifstream fileBucket(bucketFile, ios::binary);
        vector<long> pos_records;
        DataPage<TK> dataPage;

        for(int i = 0; i< M; i++){
            long absPos = sizeof(long) + (i)*sizeof(dataPage);
            do
            {

                fileBucket.seekg(absPos, ios::beg);
                fileBucket.read((char*) &dataPage, sizeof(dataPage));
                vector<long> temp = dataPage.range(inf,sup);
                
                dataPage.show();

                pos_records.insert(pos_records.end(), temp.begin(), temp.end());

                absPos = dataPage.next;

            } while (absPos != -1);
        }

        fileBucket.close();


        ifstream fileData(dataFile, ios::binary);
        std::vector<Record> listRecords;

        Record record;
        for (long pos: pos_records){
            fileData.seekg(sizeof(long) + pos*(sizeof(Record) + sizeof(long)), ios::beg);
            fileData.read((char*) &record, sizeof(record));

            listRecords.push_back(record);
        }

        fileData.close();

        return listRecords;
    }


};