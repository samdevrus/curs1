#ifndef PARAMETERS_H
#define PARAMETERS_H
#include "linearparameter.h"
//Описания параметров

class RoomTemperature : public ParamObject
{
    Q_OBJECT
    public:
        RoomTemperature(ParamObject* pConditionerTemp):
            ParamObject (10,60,tr("Температура в комнате"))
        {
            conditionerTemp = pConditionerTemp;
        }
        float generateNextValue() {
            float randStep = qrand()%10-5;
            randStep = randStep/20;
            float nextValue = values.last() + randStep*step*step ;
            float d = 0;
            if (currentIteration>4) {
                float tempSum = conditionerTemp->getValue()+
                     conditionerTemp->getPrevValue(1) +
                     conditionerTemp->getPrevValue(2) +
                     conditionerTemp->getPrevValue(3);
                float avgTemp = tempSum / 4;
                d = avgTemp-values.last();
                d = d/10;
            }
            return nextValue + d;
        }
    protected:
        ParamObject* conditionerTemp;
};

class RoomHumidity : public ParamObject
{
    Q_OBJECT
    public:
        RoomHumidity(ParamObject* pRoomTemperature):
            ParamObject (5,100,tr("Влажность в комнате"))
        {
            roomTemperature = pRoomTemperature;
        }
        float generateNextValue() {
            float randStep = qrand()%10-5;
            randStep = randStep/40;
            float nextValue = values.last() + randStep*step*step ;
            float d = 0;
            if (currentIteration>8) {
                float tempSum = roomTemperature->getValue()+
                     roomTemperature->getPrevValue(1) +
                     roomTemperature->getPrevValue(2) +
                     roomTemperature->getPrevValue(3) +
                     roomTemperature->getPrevValue(4) +
                     roomTemperature->getPrevValue(5) +
                     roomTemperature->getPrevValue(6) +
                     roomTemperature->getPrevValue(7)
                        ;
                float avgTemp = tempSum / 8;
                d = avgTemp-values.last();
                d = d/20;
            }
            return nextValue + d;

        }
    protected:
        ParamObject* roomTemperature;
};

class AirPollution : public ParamObject
{
    Q_OBJECT
    public:
        AirPollution(ParamObject* pRoomTemperature):
            ParamObject (0,100,tr("Загрязненность воздуха"))
        {
            roomTemperature = pRoomTemperature;
        }
        float generateNextValue() {

            float randStep = qrand()%10-3;
            randStep = randStep/30;
            float nextValue = values.last() + randStep*step*step ;
            float d = 0;
            if (currentIteration>5) {
                float tempSum = roomTemperature->getValue()+
                     roomTemperature->getPrevValue(1) +
                     roomTemperature->getPrevValue(2) +
                     roomTemperature->getPrevValue(3) +
                     roomTemperature->getPrevValue(4)
                        ;
                float avgTemp = tempSum / 5;
                d = avgTemp-values.last();
                d = d/10;
            }
            return nextValue + d;

        }
    protected:
        ParamObject* roomTemperature;
};

class ElectricCurrent : public ParamObject
{
    Q_OBJECT
    public:
        ElectricCurrent(ParamObject* pVoltage):
            ParamObject (4,32,tr("Потребляемый ток"))
        {
            voltage = pVoltage;
        }
        float generateNextValue() {
            //float nextValue = values.last() + ((qrand() % 2))*step*0.01 ;

            float randStep = qrand()%10-3;
            randStep = randStep*0.1;
            float d = 0;
            d = voltage->getValue()/(step*25+ randStep);

            return d;
        }
    protected:
        ParamObject* voltage;
};

class Battery : public ParamObject
{
    Q_OBJECT
    public:
        Battery(ParamObject* pVoltage):
            ParamObject (0,100,tr("Использованный ресурс батарей"))
        {
            voltage = pVoltage;
        }
        float generateNextValue() {
            float nextValue = values.last() ;
            float d = 0;
            if (voltage->getValue()>260) {
                d = step*0.5;
            } else {
                d=  -1*step*0.5;
            }
            //qDebug()<<"d="<<d;
            return nextValue + d;
        }
    protected:
        ParamObject* voltage;
};

class WireTemperature : public ParamObject
{
    Q_OBJECT
    public:
        WireTemperature(ParamObject* pCurrent):
            ParamObject (10,300,tr("Температура проводки "))
        {
            current = pCurrent;
        }
        float generateNextValue() {
            float nextValue = values.last() ;
            float d = 0;
            if (currentIteration>2) {
                d = (current->getValue() - current->getPrevValue(1))*step*1.9;
            }
            //qDebug()<<"d="<<d;
            return nextValue + d;
        }
    protected:
        ParamObject* current;
};

class DriveLoad : public ParamObject
{
    Q_OBJECT
    public:
        DriveLoad(ParamObject* pProcessorLoad):
            ParamObject (0,100,tr("Загруженность диска"))
        {
            processorLoad = pProcessorLoad;
        }
        float generateNextValue() {
            float nextValue = values.last() + ((qrand() % 2))*step*0.05 ;
            float d = 0;
            if (currentIteration>4) {
                float tempSum = processorLoad->getValue()+
                     processorLoad->getPrevValue(1) +
                     processorLoad->getPrevValue(2) +
                     processorLoad->getPrevValue(3);
                float avgTemp = tempSum / 4;
                d = avgTemp-values.last();
                d = d/10;
            }
            //qDebug()<<"d="<<d;
            return nextValue + d;
        }
    protected:
        ParamObject* processorLoad;
};

class DriveTemperature : public ParamObject
{
    Q_OBJECT
    public:
        DriveTemperature(ParamObject* pDriveLoad):
            ParamObject (20,100,tr("Температура диска"))
        {
            driveLoad = pDriveLoad;
        }
        float generateNextValue() {
            float nextValue = values.last() + ((qrand() % 2))*step*0.05 ;
            float d = 0;
            if (currentIteration>4) {
                float tempSum = driveLoad->getValue()+
                     driveLoad->getPrevValue(1) +
                     driveLoad->getPrevValue(2) +
                     driveLoad->getPrevValue(3);
                float avgTemp = tempSum / 4;
                d = avgTemp-values.last();
                d = d/20;
            }
            return nextValue + d;
        }
    protected:
        ParamObject* driveLoad;
};

class ReadWriteErrors : public ParamObject
{
    Q_OBJECT
    public:
        ReadWriteErrors(ParamObject* pDriveLoad):
            ParamObject (0,100,tr("Количество ошибок при чтении/записи"))
        {
            driveLoad = pDriveLoad;
        }
        float generateNextValue() {
            float nextValue = values.last()  ;
            float d = 0;
            if (currentIteration>2) {
                d =driveLoad->getValue()*step*0.005 ;
            }
            return nextValue + d;
        }
    protected:
        ParamObject* driveLoad;
};

class NetworkLoad : public ParamObject
{
    Q_OBJECT
    public:
        NetworkLoad(ParamObject* pRequests):
            ParamObject (0,100,tr("Загрузка сетевого канала"))
        {
            requests = pRequests;
        }
        float generateNextValue() {
            float nextValue = values.last() + ((qrand() % 2))*step*0.05 ;
            float d = 0;
            if (currentIteration>4) {
                float tempSum = requests->getValue()+
                     requests->getPrevValue(1) +
                     requests->getPrevValue(2) +
                     requests->getPrevValue(3);
                float avgTemp = tempSum / 4;
                d = avgTemp-values.last();
                d = d/10;
            }

            //qDebug()<<"d="<<d;
            return nextValue + d;
        }
    protected:
        ParamObject* requests;
};

class RouterLoad : public ParamObject
{
    Q_OBJECT
    public:
        RouterLoad(ParamObject* pRequests):
            ParamObject (0,100,tr("Загрузка маршрутизатора"))
        {
            requests = pRequests;
        }
        float generateNextValue() {
            float nextValue = values.last() + ((qrand() % 2))*step*0.01 ;
            float d = 0;
            if (currentIteration>4) {
                float tempSum = requests->getValue()+
                     requests->getPrevValue(1) +
                     requests->getPrevValue(2) +
                     requests->getPrevValue(3);
                float avgTemp = tempSum / 4;
                d = avgTemp-values.last();
                d = d/20;
            }
            //qDebug()<<"d="<<d;
            return nextValue + d;
        }
    protected:
        ParamObject* requests;
};

class NetworkErrors : public ParamObject
{
    Q_OBJECT
    public:
        NetworkErrors(ParamObject* pRouterLoad):
            ParamObject (0,100,tr("Ошибки сети"))
        {
            routerLoad = pRouterLoad;
        }
        float generateNextValue() {
            //float nextValue = values.last() + ((qrand() % 2))*step*0.05 ;
            float d = 0;
            //if (currentIteration>2) {
                d = routerLoad->getValue()*step*0.005;
            //}
            //qDebug()<<"d="<<d;
            return values.last() + d;
        }
    protected:
        ParamObject* routerLoad;
};

class ChipsTempetature : public ParamObject
{
    Q_OBJECT
    public:
        ChipsTempetature(ParamObject* pFilterPollution):
            ParamObject (20,100,tr("Температура чипов платы"))
        {
            filterPollution = pFilterPollution;
        }
        float generateNextValue() {
            float nextValue = values.last() + ((qrand() % 2))*step*0.01 ;
            float d = 0;
            if (currentIteration>4) {
                float tempSum = filterPollution->getValue()+
                     filterPollution->getPrevValue(1) +
                     filterPollution->getPrevValue(2) +
                     filterPollution->getPrevValue(3);
                float avgTemp = tempSum / 4;
                d = avgTemp-values.last();
                d = d/30;
            }
            //qDebug()<<"d="<<d;
            return nextValue + d;
        }
    protected:
        ParamObject* filterPollution;
};

class NoiseLevel : public ParamObject
{
    Q_OBJECT
    public:
        NoiseLevel(ParamObject* pFilterPollution):
            ParamObject (10,100,tr("Уровень шума"))
        {
            filterPollution = pFilterPollution;
        }
        float generateNextValue() {
            float nextValue = values.last() + ((qrand() % 2))*step*0.01 ;
            float d = 0;
            if (currentIteration>4) {
                float tempSum = filterPollution->getValue()+
                     filterPollution->getPrevValue(1) +
                     filterPollution->getPrevValue(2) +
                     filterPollution->getPrevValue(3);
                float avgTemp = tempSum / 4;
                d = avgTemp-values.last();
                d = d/40;
            }
            return nextValue + d;
        }
    protected:
        ParamObject* filterPollution;
};

class ServerOutTemperature : public ParamObject
{
    Q_OBJECT
    public:
        ServerOutTemperature(ParamObject* pChipsTemp):
            ParamObject (30,100,tr("Температура на выходе из корпуса"))
        {
            chipsTemp = pChipsTemp;
        }
        float generateNextValue() {
            float nextValue = values.last() + ((qrand() % 2))*step*0.01 ;
            float d = 0;
            if (currentIteration>4) {
                float tempSum = chipsTemp->getValue()+
                     chipsTemp->getPrevValue(1) +
                     chipsTemp->getPrevValue(2) +
                     chipsTemp->getPrevValue(3);
                float avgTemp = tempSum / 4;
                d = avgTemp-values.last();
                d = d/10;
            }
            //qDebug()<<"d="<<d;
            return nextValue + d;
        }
    protected:
        ParamObject* chipsTemp;
};

#endif // PARAMETERS_H
