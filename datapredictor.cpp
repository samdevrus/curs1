#include "datapredictor.h"
#include <QDebug>

DataPredictor::DataPredictor(QVector<float> pX, QVector<float> pY):
    polinomPower(2)
{

    a = QVector<float>(polinomPower,0);
    x = pX;
    y = pY;

    processData();
}

void DataPredictor::processData() {
    //Аппроксимация функций полиномом методом наименьших квадратов
    int dataCount = x.count(),
        i=0,
        j=0,
        k=0;
    float temp=0;
    a = QVector<float>(polinomPower+1,0);
    //столбец свободных членов
    QVector<float> b(polinomPower+1,0);
    //суммы степеней x,y при неизвестных коэффициентах полинома
    QVector<QVector<float>> sums =
            QVector<QVector<float>>(polinomPower+1,
                                    QVector<float>(polinomPower+1,0));
    //заполнение начальными данными
    for(i=0; i<polinomPower+1; i++) {
        for(j=0; j<polinomPower+1; j++) {
            sums[i][j] = 0;
            for(k=0; k<dataCount; k++){
                sums[i][j] += pow(x[k], i+j);
            }
        }
    }
    for(i=0; i<polinomPower+1; i++){
        for(k=0; k<dataCount; k++){
            b[i] += pow(x[k], i) * y[k];
        }
    }

    //проверка на наличие нулей на главной диагонали и исправление
    for(i=0; i<polinomPower+1; i++) {
        if(sums[i][i]==0) {
            for(j=0; j<polinomPower+1; j++){
                if(j==i) continue;
                if(sums[j][i] !=0 && sums[i][j]!=0) {
                    for(k=0; k<polinomPower+1; k++){
                        temp = sums[j][k];
                        sums[j][k] = sums[i][k];
                        sums[i][k] = temp;
                    }
                    temp = b[j];
                    b[j] = b[i];
                    b[i] = temp;
                    break;
                }
            }
        }
    }

    for(k=0; k<polinomPower+1; k++) {
        for(i=k+1; i<polinomPower+1; i++) {
            if(sums[k][k]==0){
                //решения не существует
                return;
            }
            float M = sums[i][k] / sums[k][k];
            for(j=k; j<polinomPower+1; j++) {
                sums[i][j] -= M * sums[k][j];
            }
            b[i] -= M*b[k];
        }
    }
    for(i=polinomPower; i>=0; i--) {
        float s = 0;
        for(j = i; j<polinomPower+1; j++) {
            s = s + sums[i][j]*a[j];
        }
        a[i] = (b[i] - s) / sums[i][i];
    }
}

float DataPredictor::predictData(float pX) {
    float result = a[0];
    for(int i=1; i<=polinomPower; i++){
           result += pow(pX,i)*a[i];
    }
    return result;
}
