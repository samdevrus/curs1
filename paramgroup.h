#ifndef PARAMGROUP_H
#define PARAMGROUP_H

#include <QObject>
#include "paramobject.h"

//Группа параметров
class ParamGroup : public QObject
{
    Q_OBJECT
    public:
        explicit ParamGroup(QString pGroupName);
        //получить максимальное текущее значение параметра среди группы
        int getTopValue();
        //получить максимальное значение среди группы которое могут достигать параметры
        int getParamsMaxValue();

        void addParameter(ParamObject* param) {
            params.append(param);
        }
        QString getName() {
            return groupName;
        }
        QListIterator<ParamObject*> getParamIterator() {
            return QListIterator<ParamObject*>(params);
        }
         int count() {
            return params.count();
        }
        bool hasParameter(ParamObject* param) {
            return (params.indexOf(param)!=-1);
        }
    protected:
        QString groupName;
        QList<ParamObject*> params;
        int topValue;
};

#endif // PARAMGROUP_H
