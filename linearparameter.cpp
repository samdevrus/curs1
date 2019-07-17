#include "linearparameter.h"

LinearParameter::LinearParameter(QString pParameterName, int pStartValue, int pMaxValue) :
      ParamObject(pStartValue,pMaxValue,pParameterName)
{

}

float LinearParameter::generateNextValue() {
    //слишком медленно
    float randStep = qrand()%10-3.5;
    //для тестирования
    //float randStep = qrand()%10-2;

    randStep = randStep/10;
    float nextValue = values.last()+randStep*step;
    return nextValue;
}
