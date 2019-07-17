#ifndef PARAMGROUPCHECKER_H
#define PARAMGROUPCHECKER_H

#include <QObject>
#include <QMap>
#include "paramchecker.h"
#include "paramgroup.h"

struct ParamGroupSituation {
    QList<ParamObject*> params;
    ParamChecker::State state;
    QString message;
};

struct PredictionMessage {
    QString message;
    int timeUntil;
};

struct ParamGroupPrediction {
    bool everytingIsOk;
    QList<PredictionMessage> messages;
};

//Контроллер группы параметров
class ParamGroupChecker
{
    public:
        ParamGroupChecker(ParamGroup* pGroup,QMap<ParamObject*,ParamChecker*>* pParamCheckers);

        void addSituation(ParamGroupSituation situation) {
            paramSituations.append(situation);
        }
        //проверить текущий статус -
        //true если все в порядке
        //false в случае аварии
        bool check();
        //проверка с использование аппроксимации в заданное время
        //predictionTime - относительное время от текущего в секундах
        ParamGroupPrediction checkWithPrediction(int predictionTime);
        QString getMessage() {
            return message;
        }
    private:
        QList<ParamGroupSituation> paramSituations;
        QString message;
        ParamGroup* group;
        QMap<ParamObject*,ParamChecker*>* paramCheckers;
};

#endif // PARAMGROUPCHECKER_H
