#ifndef LINEARPARAMETER_H
#define LINEARPARAMETER_H

#include <QObject>
#include <QtDebug>
#include "paramobject.h"

//Параметр с линейной функцией
class LinearParameter : public ParamObject
{
    Q_OBJECT
    public:

        LinearParameter(QString pParameterName,
                        int pStartValue,
                        int pMaxValue);
    protected:
        float generateNextValue();

};
#endif // LINEARPARAMETER_H
