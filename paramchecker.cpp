#include "paramchecker.h"

ParamChecker::ParamChecker(ParamObject* pParamObject,int pAttentionValue, int pDangerValue) : QObject(),
    paramObject(pParamObject),attentionValue(pAttentionValue),dangerValue(pDangerValue),
    paramState(Normal)
{
    dangerZoneStart = dangerValue - ((dangerValue-attentionValue)/2);
}

bool ParamChecker::check() {
    int value = paramObject->getValue();
    paramState = checkState(value);
    if (value < attentionValue) {
        return true;
    }
    emit(attetion(paramState));
    return false;
}

ParamChecker::State ParamChecker::checkState(float value) {
    State st = Normal;
    if (value>=attentionValue) {
        st = Attention;
    }
    if (value>=dangerZoneStart) {
        st = DangerZone;
    }
    if (value>=dangerValue) {
        st = Danger;
    }
    return st;
}
