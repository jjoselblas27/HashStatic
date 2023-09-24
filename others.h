#ifndef OTHERS_H
#define OTHERS_H

#include <iostream>
#include <string>
#include "./record.h"
using namespace std;

bool existe(string name)
{
    ifstream archivo(name);

    bool res = archivo.good();
    archivo.close();

    return res;
}

template <typename TK>
struct Pares
{
    TK key;
    long position;

    Pares(){};
    Pares(TK key, long position)
    {
        this->key = key;
        this->position = position;
    }

    TK getKey()
    {
        return this->key;
    }

    long getPos()
    {
        return position;
    }
};

template <typename Record>
void load(string database, string dataFile)
{
    std::fstream csvFile(database, std::ios::in);
    std::fstream File(dataFile, ios::out | ios::binary);

    // creo el header
    long first = -1;
    File.write((char *)&first, sizeof(first));

    std::string line;
    std::getline(csvFile, line);
    while (std::getline(csvFile, line))
    {
        std::string knownAs;
        std::string nationality;
        std::string clubName;
        std::string clubJerseyNumberStr;
        std::string overallStr;
        std::string bestPosition;
        std::string valueStr;
        std::string ageStr;
        std::string heightStr;
        std::string weightStr;

        std::stringstream lineStream(line);

        std::getline(lineStream, knownAs, ',');
        std::getline(lineStream, nationality, ',');
        std::getline(lineStream, clubName, ',');
        std::getline(lineStream, clubJerseyNumberStr, ',');
        std::getline(lineStream, overallStr, ',');
        std::getline(lineStream, bestPosition, ',');
        std::getline(lineStream, valueStr, ',');
        std::getline(lineStream, ageStr, ',');
        std::getline(lineStream, heightStr, ',');
        std::getline(lineStream, weightStr, ',');

        int clubJerseyNumber = std::stoi(clubJerseyNumberStr);
        int overall = std::stoi(overallStr);
        int value = std::stoi(valueStr);
        int age = std::stoi(ageStr);
        int height = std::stoi(heightStr);
        int weight = std::stoi(weightStr);

        Record record(knownAs, nationality, clubName, clubJerseyNumber, overall, bestPosition, value, age, height, weight);

        File.write((char *)&record, sizeof(record));
        File.write((char *)&first, sizeof(first));
    }

    csvFile.close();
    File.close();
}

#endif // OTHERS_H