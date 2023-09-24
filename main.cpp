#include <iostream>
#include "./hashStatic.h"

using namespace std;

template<typename Record>
int GetKeyFromRecord(const Record& player) {
    // Supongamos que la clave es el nombre del jugador
    return player.value;
}



template<typename TK, typename Record>
void pRead(HashStatic<TK,Record> &hashIndex){
    long num;
    cin >> num;

    vector<Player> res = hashIndex.search(num);
    
    cout << res.size() << endl;

    for(auto item : res){
        cout << item.to_string() << endl;
    }
}


template<typename TK, typename Record>
void pInsert(HashStatic<TK,Record> &hashIndex){
    
    //primera
    //Record player("2855", "arequipa", "FCPORCINOS", 28, 28,"LATERAL", 2855, 28,170,68);
    //Record player("3200", "arequipa", "FCPORCINOS", 28, 28,"LATERAL", 3200, 28,170,68);

    //segunda
    //Record player("2800", "arequipa", "FCPORCINOS", 28, 28,"LATERAL", 2800, 28,170,68);
    //Record player("2900", "arequipa", "FCPORCINOS", 28, 28,"LATERAL", 2900, 28,170,68);

    //tercera
    //Record player("3400", "arequipa", "FCPORCINOS", 28, 28,"LATERAL", 3400, 28,170,68);
    Record player("3300", "arequipa", "FCPORCINOS", 28, 28,"LATERAL", 3300, 28,170,68);
    //cout << player.to_string() << endl;
    hashIndex.insert(player);
}

template<typename TK, typename Record>
void pRemove(HashStatic<TK,Record> &hashIndex){
    long key;
    cin >> key;

    hashIndex.remove(key);   
}

template<typename TK, typename Record>
void pRange(HashStatic<TK,Record> &hashIndex){
    long inf,sup;
    cin >> inf >> sup;

    vector<Record> res = hashIndex.rangeSearch(inf,sup);

    cout << "tamanho: " << res.size() << endl;

    for(auto item : res){
        cout << item.to_string() << endl;
    }
}



int main() {

    //la key sea value
    HashStatic<long, Player> hashIndex(20, "database/Fifa 23 Players Data Short.csv", "datafile", "bucketfile",  GetKeyFromRecord<Player>);

    //cout << " DataPage: " << sizeof(DataPage<long>) << endl;
    
    //del: 120  search 135
    //2280 3200
    //
    //pInsert(hashIndex);
    //pRead(hashIndex);
    //pRemove(hashIndex);
    //pRead(hashIndex);
    
    
    pRange(hashIndex);
    return 0;
}