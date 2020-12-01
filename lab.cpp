#include <iostream>
#include <cstdio>

using namespace std;


static const int MASS = 10;
static const int SIZE_VALUE = 65;
static const int SIZE_KEY = 20;
static const int COUNT_PARTS_TEL = 3;


struct TKey_value {
    int sizeCountry;
    int sizeRegion;
    int sizeBody;
    int index;
};

struct TValue {
    char value[SIZE_VALUE];
};

struct TKey {
    char key[SIZE_KEY];
};

template<typename valueType = TKey_value>
class TVector {
public:
    TVector() {
        this->size = sizeof(valueType) * 2;
        this->data = (valueType *)malloc(this->size);
    }
    TVector(int storageSize) {
        this->size = storageSize;
        this->data = (valueType *)malloc(this->size);
    }
    virtual ~TVector() {}

    valueType& operator[] (const int& index) {
       return this->data[index];
    }

    void Resize(int storageSize){
        this->size = storageSize;
        this->data = (valueType *)realloc(this->data, sizeof(valueType) * this->size);
    }

    valueType* getData() { return this->data; }

protected:
    valueType *data;
    int size;
};

int Sum_size(int count, TVector<TKey_value> output, int i) {
    if (count == 1)
        return output[i].sizeCountry;
    if (count == 2)
        return output[i].sizeCountry + output[i].sizeRegion;
    return output[i].sizeCountry + output[i].sizeRegion + output[i].sizeBody;
}

int GetInputIndex(TKey_value* value, int i){
    return value[i].index;
}

char StorageIndex(TVector<TKey_value> output, int j, TKey *keys, int level, int rank) {
    int index = GetInputIndex(output.getData(), j);
    int size = Sum_size(level, output, j);
    return keys[index].key[size + (--level) - rank];
}

int LevelSize(TVector<TKey_value> output, int j, int level, int rank) {
    if (level == 3)
        return output[j].sizeBody - rank - 1;
    else if (level == 2)
        return output[j].sizeRegion - rank - 1;
    else return output[j].sizeCountry - rank - 1;
}

TVector<TKey_value> Counting(int rank, TVector<TKey_value> output, TVector<TKey_value> tmp, int count, TKey *keys, int level) {
    int mass[MASS];
    int x;
    for (int j = 0; j < MASS; j++){
        mass[j] = 0;
    }

    for (int j = 0; j < count; j++){
        x = LevelSize(output, j, level, rank);

        if (x < 0){
            mass[0]++;
            continue;
        }
        mass[StorageIndex(output, j, keys, level, rank) - '0']++;
    }

    for (int j = 1; j < MASS; j++){
        mass[j] += mass[j - 1];
    }
    for (int j = count - 1; j >= 0; j--){
        x = LevelSize(output, j, level, rank);
        if (x < 0){
            tmp[--mass[0]] = output[j];
            continue;
        }
        tmp[--mass[StorageIndex(output, j, keys, level, rank) - '0']] = output[j];
    }
    return tmp;
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    TValue *values;
    values = (TValue *)malloc(2 * sizeof(TValue));
    TKey *keys;
    keys = (TKey *)malloc(2 * sizeof(TKey));

    TVector<TKey_value> output;

    int storageSize = 2;
    int count = 0;

    int i_symbol;
    int step;

    int cutTelSizeMax[COUNT_PARTS_TEL] = {0, 0, 0};

    while (cin >> keys[count].key >> values[count].value) {
        int cutTelSize[] = {0, 0, 0};
        step = 0;
        for (i_symbol = 1; keys[count].key[i_symbol] != '\0'; ++i_symbol) {
            if (keys[count].key[i_symbol] != '-'){
                ++cutTelSize[step];
            } else {
                ++step;
            }
        }
        output[count].sizeCountry = cutTelSize[0];
        output[count].sizeRegion = cutTelSize[1];
        output[count].sizeBody = cutTelSize[2];
        for(int i = 0; i < COUNT_PARTS_TEL; ++i){
            if (cutTelSize[i] > cutTelSizeMax[i])
                cutTelSizeMax[i] = cutTelSize[i];
        }
        output[count].index = count;
        ++count;
        if (count >= storageSize) {
            storageSize = storageSize + storageSize;
            values = (TValue *)realloc(values, sizeof(TValue) * storageSize);
            keys = (TKey *)realloc(keys, sizeof(TKey) * storageSize);
            output.Resize(storageSize);
        }
    }

    TVector<TKey_value> tmp(storageSize);
    bool revolver = 1;
    // RadixSort(q, )
    for (int q = 0; q < cutTelSizeMax[2]; q++){
        if (q % 2 == 0){
            tmp = Counting(q, output, tmp, count, keys, 3);
            revolver = 0;
        }else{
            output = Counting(q, tmp, output, count, keys, 3);
            revolver = 1;
        }
    }

    if (revolver) {
        for (int q = 0; q < cutTelSizeMax[1]; q++){
            if (q % 2 == 0){
                tmp = Counting(q, output, tmp, count, keys, 2);
                revolver = 0;
            }else{
                output = Counting(q, tmp, output, count, keys, 2);
                revolver = 1;
            }
        }
    }else{
        for (int q = 0; q < cutTelSizeMax[1]; q++){
            if (q % 2 == 0){
                output = Counting(q, tmp, output, count, keys, 2);
                revolver = 1;
            }else{
                tmp = Counting(q, output, tmp, count, keys, 2);
                revolver = 0;
            }
        }
    }

    if (revolver){
        for (int q = 0; q < cutTelSizeMax[0]; q++){
            if (q % 2 == 0){
                tmp = Counting(q, output, tmp, count, keys, 1);
                revolver = 0;
            }else{
                output = Counting(q, tmp, output, count, keys, 1);
                revolver = 1;
            }
        }
    }else{
        for (int q = 0; q < cutTelSizeMax[0]; q++){
            if (q % 2 == 0){
                output = Counting(q, tmp, output, count, keys, 1);
                revolver = 1;
            }else{
                tmp = Counting(q, output, tmp, count, keys, 1);
                revolver = 0;
            }
        }
    }

    if (revolver){
        for (int q = 0; q < count; q++)
            printf("%s\t%s\n",keys[output[q].index].key, values[output[q].index].value);
    }else{
        for (int q = 0; q < count; q++)
            printf("%s\t%s\n",keys[tmp[q].index].key, values[tmp[q].index].value);
    }

    free(values);
    values = nullptr;
    free(keys);
    keys = nullptr;

    return 0;
}