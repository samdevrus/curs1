#include "paramgroupchecker.h"
#include "datapredictor.h"

//Описание параметров
//  pGroup Группа
//  pParamCheckers Массив с контроллерами параметров
//  pParams Список параметров которые необходимо контроллировать
//  pState Аварийное состояние параметров
//  pMessage Аварийное сообщение
ParamGroupChecker::ParamGroupChecker(ParamGroup* pGroup,
                                      QMap<ParamObject*,ParamChecker*>* pParamCheckers):
    group(pGroup),
    paramCheckers(pParamCheckers),
    message("")

{

}

bool ParamGroupChecker::check() {
    QListIterator<ParamGroupSituation> paramSituationIterator(paramSituations);
    message = "";
    while(paramSituationIterator.hasNext()) {
        bool situationResult = true;
        ParamGroupSituation situation = paramSituationIterator.next();
        QListIterator<ParamObject*> paramIterator(situation.params);
        while(paramIterator.hasNext()) {
            ParamObject* param = paramIterator.next();
            ParamChecker* checker = paramCheckers->find(param).value();
            if (checker->getState()<situation.state) {
                situationResult = false;
            }
        }
        if (situationResult) {
            message = situation.message;
            return false;
        }
    }
    return true;
}

ParamGroupPrediction ParamGroupChecker::checkWithPrediction(int predictionTime) {
    ParamGroupPrediction prediction {
        true,
        QList<PredictionMessage>()
    };
    QListIterator<ParamGroupSituation> paramSituationIterator(paramSituations);
    message = "";
    while(paramSituationIterator.hasNext()) {
        ParamGroupSituation situation = paramSituationIterator.next();
        QListIterator<ParamObject*> paramIterator(situation.params);
        bool situationResult = true;
        //bool situationTime = 100;
        while(paramIterator.hasNext()) {
            ParamObject* p = paramIterator.next();
            ParamChecker* checker = paramCheckers->find(p).value();
            DataPredictor pred(p->getIterations(),
                               p->getValues());
            ParamChecker::State predictedState = ParamChecker::Normal;
            //последнее время итерации в параметре
            float parameterTime = p->getIterations().last();
            float predictedValue = pred.predictData(parameterTime+predictionTime);
            predictedState = checker->checkState(predictedValue);
            if (predictedState<situation.state) {
                situationResult = false;
                //break;
            }
        }
        if (situationResult) {
            prediction.everytingIsOk = false;
            prediction.messages.append({
                                          situation.message,
                                          predictionTime
                                       });
        }

    }
    return prediction;
}
