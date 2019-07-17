#include "paramgroup.h"

ParamGroup::ParamGroup(QString pGroupName) : QObject(),
   groupName(pGroupName),
   topValue(0)
{

}

int ParamGroup::getTopValue() {
    QListIterator<ParamObject*> iterator(params);
    while(iterator.hasNext()) {
        ParamObject* param = iterator.next();
        if (param->getValue()>topValue) {
            topValue = param->getValue();
        }
    }
    return topValue;
}

int ParamGroup::getParamsMaxValue() {
    QListIterator<ParamObject*> paramIterator(params);
    int maxValue = 1;
    while (paramIterator.hasNext()) {
        ParamObject* param = paramIterator.next();
        if (param->getMaxValue()>maxValue) {
            maxValue = param->getMaxValue();
        }
    }
    return maxValue;
}
