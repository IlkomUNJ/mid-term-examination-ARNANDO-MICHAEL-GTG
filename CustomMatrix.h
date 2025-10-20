#ifndef CUSTOMMATRIX_H
#define CUSTOMMATRIX_H

#include <iostream>

using namespace std;

class CustomMatrix{
public:
    bool mat[3][3]={0};
    CustomMatrix(){

    }

    //alternate way to instantiate the class
    CustomMatrix(bool m[3][3]){
        // NOTE: We assume 3x3 array size for function parameter m.
        for (int i=0;i< 3;i++) {
            for(int j=0; j < 3;j++){
                mat[i][j] = m[i][j];
            }
        }
    }

    void fillMatrix(bool m[3][3]){
        // NOTE: Assume 3x3 size for consistent behavior
        for (int i=0;i< 3;i++) {
            for(int j=0; j < 3;j++){
                mat[i][j] = m[i][j];
            }
        }
    }

    // ADDED: Overload operator== for pattern matching (Objective 3)
    bool operator==(const CustomMatrix& other) const {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (mat[i][j] != other.mat[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }
};

#endif // CUSTOMMATRIX_H
