#ifndef RECORD_HPP
#define RECORD_HPP

#include <iostream>
#include <fstream>
#include <sstream>

#pragma pack(push, 1)
struct Player {
    char knownAs[100]{'\0'};
    char nationality[100]{'\0'};
    char clubName[100]{'\0'};
    int clubJerseyNumber;
    int overall;
    char bestPosition[4]{'\0'};
    long value;
    int age;
    int height;
    int weight;

    Player() = default;

    Player(std::string knownAs, std::string nationality, std::string clubName, int clubJerseyNumber, 
            int overall, std::string bestPosition, long value, int age, int height, int weight) : 
        clubJerseyNumber(clubJerseyNumber), overall(overall), age(age), height(height), weight(weight) {
        std::copy(knownAs.begin(), knownAs.end(), this->knownAs);
        std::copy(nationality.begin(), nationality.end(), this->nationality);
        std::copy(clubName.begin(), clubName.end(), this->clubName);
        std::copy(bestPosition.begin(), bestPosition.end(), this->bestPosition);
        this->value = value;
    }

    std::string to_string() {
        std::stringstream stream;
        stream << "(" << knownAs << ", " << nationality << ", " << clubName << ", " << clubJerseyNumber << ", " << overall
                << ", " << bestPosition << ", " << value << ", " << age << ", " << height << ", " << weight << ")";
        return stream.str();
    }
};
#pragma pack(pop)

#endif // RECORD_HPP