#ifndef PARAMCHECK_H
#define PARAMCHECK_H

#include <QObject>
#include "paramobject.h"

//Контроллер парамеетра
class ParamChecker : public QObject
{
    Q_OBJECT
    public:
        //состояние параметра
        enum State { Normal, Attention, DangerZone, Danger };
        explicit ParamChecker(ParamObject* pParamObject,int pAttentionValue, int pDangerValue);
        //проверка состояние текущего параметра:
        // true если состояние нормальное,
        // falsе в других случаях, также генерируется сигнал с новым состоянием
        bool check();
        //получить состояние по значению
        State checkState(float value);
        //получить текущее состояние
        State getState() {
            return paramState;
        }
        //получить предаварийное значение
        int getAttentionValue() {
            return attentionValue;
        }
        //получить аварийное значение
        int getDangerValue() {
            return dangerValue;
        }
    signals:
        void attetion(State pParamState);
    private:
        ParamObject* paramObject;
        //предаварийное значение
        int attentionValue;
        //аварийное значение
        int dangerValue;
        //значение аварийной зоны
        int dangerZoneStart;
        State paramState;
};

#endif // PARAMCHECK_H
