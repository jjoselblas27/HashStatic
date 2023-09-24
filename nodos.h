#ifndef NODOS_H
#define NODOS_H

#include <iostream>
#include <vector>
#include "./others.h"

#define PAGE_SIZE 2000

template <typename TK>
// inline static constexpr long N = (PAGE_SIZE - sizeof(int) - sizeof(long)) / (sizeof(Pares<TK>));
inline static constexpr long N = 2;

template <typename TK>
struct DataPage
{
    Pares<TK> page[N<TK>];
    int count;
    long next;
    bool principal;

    DataPage()
    {
        count = 0;
        next = -1;
        principal = false;
    }

    bool insert(Pares<TK> par)
    {
        if (count == N<TK>)
            return false;

        page[count] = par;
        count++;
        return true;
    }

    long getNext() { return next; }

    // busco entre los hijos y retorno coincidencias.
    std::vector<long> search(TK key)
    {
        std::vector<long> res;
        for (int i = 0; i < count; i++)
        {
            if (page[i].key == key)
            {
                res.push_back(page[i].position);
            }
        }

        return res;
    }

    // busco entre los hijos y retorno coincidencias.
    std::vector<long> range(TK inf, TK sup)
    {
        std::vector<long> res;
        for (int i = 0; i < count; i++)
        {
            if (inf <= page[i].key && page[i].key < sup)
            {
                res.push_back(page[i].position);
            }
        }
        return res;
    }

    void reset()
    {
        this->count = 0;
        this->next = -1;
    }

    void show()
    {
        for (int i = 0; i < count; i++)
        {
            cout << "(" << page[i].key << ", " << page[i].position << "), ";
        }
        cout << ". next: " << next << endl;
    }
};

// inserto en el dataFile y retorno su posicion relativa.
template<typename Record>
long insertDataFile(Record &record, string dataFile)
{
    fstream FileData(dataFile, ios::in | ios::out | ios::binary);
    // datafile freeelist tipo LIFO
    long header;
    FileData.seekg(0, ios::beg);
    FileData.read((char *)&header, sizeof(header));

    // posicion donde escribire.
    long position = 0;
    if (header < 0)
    {
        FileData.seekp(0, ios::end);

        // transformo a posicion relativa
        position = (FileData.tellp() - sizeof(long)) / (sizeof(Record) + sizeof(long));

        FileData.write((char *)&record, sizeof(record));
        FileData.write((char *)&header, sizeof(header)); // header == -1
        
    }
    else
    {
        Record temp2;
        long empty = -1; // solo guarda -1
        long next = 0;

        position = header;

        // ubico el puntero con la posicion absoluta
        FileData.seekg(sizeof(long) + position * (sizeof(record) + sizeof(long)) + sizeof(record), ios::beg);
        FileData.read((char *)&next, sizeof(next));

        FileData.seekp(sizeof(long) + position * (sizeof(record) + sizeof(long)), ios::beg);
        FileData.write((char *)&record, sizeof(record));
        FileData.write((char *)&empty, sizeof(empty));

        // cambio el header
        FileData.seekp(0, ios::beg);
        FileData.write((char *)&next, sizeof(next));
    }

    FileData.close();

    return position;
}


// tengo un vector con posiciones relativas.
template<typename Record>
void deleteListDataFile(vector<long> positions, string dataFile)
{
    fstream File(dataFile, ios::in | ios::out | ios::binary);
    for (long pos : positions)
    {

        long header;
        File.seekg(0, ios::beg);
        File.read((char *)&header, sizeof(header));

        // sobreescribo en pos
        File.seekp((sizeof(long) + pos * (sizeof(Record) + sizeof(long)) + sizeof(Record)), ios::beg);
        File.write((char *)&header, sizeof(header));

        // sobreescribo en header
        File.seekp(0, ios::beg);
        File.write((char *)&pos, sizeof(pos));
    }

    File.close();
}



#endif // NODOS_H