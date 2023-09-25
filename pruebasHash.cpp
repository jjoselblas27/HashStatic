#include <iostream>
#include "hashStatic.h"

template<typename Record>
int GetKeyFromRecord(const Record& audio) {
    // Supongamos que la clave es el nombre del jugador
    return audio.duration_ms;
}


template<typename TK, typename Record>
void pRead(HashStatic<TK,Record> &hashStatic){
    TK key;
    cin >> key;

    vector<AudioFeatures> res = hashStatic.search(key);
    
    cout << res.size() << endl;

    for(auto item : res){
        cout << item.to_string() << endl;
    }
}


template<typename TK, typename Record>
void pInsert(HashStatic<TK,Record> &hashStatic){
    
    //primera
    Record audio("CCAAEAAEAAEA",0.0304,0.565,30000,0.736);
    //Record audio("BAAAEAAEAAEA",0.0304,0.565,31000,0.736);

    //segunda
    //Record audio("jjAAEAAEAAEA",0.0304,0.565,3200,0.736);
    //Record audio("DAAAEAAEAAEA",0.0304,0.565,3300,0.736);

    hashStatic.insert(audio);

}

template<typename TK, typename Record>
void pRemove(HashStatic<TK,Record> &hashStatic){
    TK key;
    cin >> key;

    hashStatic.remove(key);   
}

template<typename TK, typename Record>
void pRangCe(HashStatic<TK,Record> &hashStatic){
    TK inf,sup;
    cin >> inf >> sup;

    vector<Record> res = hashStatic.rangeSearch(inf,sup);

    cout << "tamanho: " << res.size() << endl;

    for(auto item : res){
        cout << item.to_string() << endl;
    }
}


int main() {

    //la key sea value
    HashStatic<int, AudioFeatures> hashStatic(200,"../database/audio_features_1k.csv", "datafile", "duration_ms",  GetKeyFromRecord<AudioFeatures>);

    cout << " DataPage: " << sizeof(DataPage<int>) << endl;
    
    //pInsert(hashStatic);
    pRead(hashStatic);
    //pRemove(hashStatic);
    //pRead(hashStatic);
    //pRange(hashStatic);
    return 0;
}