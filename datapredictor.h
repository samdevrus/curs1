#ifndef DATAPREDICTOR_H
#define DATAPREDICTOR_H
#include <math.h>
#include <QVector>
//Класс для аппроксимации функции y = f(x) полиномом с заданой степенью
// y' = a0+a1*x+a2*x*x ...
// на основе значений y и x
class DataPredictor
{
    public:
        DataPredictor(QVector<float> pX,QVector<float> pY);
        float predictData(float pX);
    private:
        //функция вычисления коэффициентов полинома
        void processData();
        QVector<float> x;
        QVector<float> y;
        //коэффициенты полинома
        QVector<float> a;
        //степень полинома
        int polinomPower;
};

#endif // DATAPREDICTOR_H
