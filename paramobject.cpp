#include "paramobject.h"

//Описание параметров
//  pStartValue Начальное значение
//  pMaxValue Максимальное значение которое может принимать параметр
//  name Название параметра
ParamObject::ParamObject(int pStartValue, int pMaxValue, QString name):QObject(),
    currentIteration(1)
{

    iterationTimeMsec = 1000;
    values.append(pStartValue);
    iterations.append(0);
    parameterName = name;
    maxValue = pMaxValue;

    float iterationSec =  (float)(iterationTimeMsec/1000);
    step = (pMaxValue-pStartValue);
    step = step/(iterationSec*30);
    QObject::connect(&timer,SIGNAL(timeout()),this,SLOT(updateValue()));
}

void ParamObject::start() {
    timer.start(iterationTimeMsec);
}

float ParamObject::getPrevValue(int fromEnd) {
    if (values.count()-1<fromEnd) return 0;
    return values.at(values.count()-fromEnd-1);
}

void ParamObject::updateValue() {
    float nextValue = this->generateNextValue();
    if (nextValue>maxValue) {
        nextValue = maxValue;
    }
    if (nextValue<0) {
        nextValue = 0;
    }
    values.append(nextValue);
    iterations.append(currentIteration);
    currentIteration++;
    emit(valueChanged(nextValue));
}

void ParamObject::reset() {
    float nextValue = values.first();
    currentIteration = 1;
    values.clear();
    iterations.clear();
    values.append(nextValue);
    iterations.append(0);
    emit(valueChanged(nextValue));
}

float ParamObject::generateNextValue() {
   float nextValue = values.last()+qrand() % 2;
   return nextValue;
}
