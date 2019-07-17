#ifndef PARAMOBJECT_H
#define PARAMOBJECT_H

#include <QObject>
#include <QVector>
#include <QTimer>

//Параметр
class ParamObject : public QObject
{
    Q_OBJECT

    public:
        ParamObject(int pStartValue, int pMaxValue,QString name);
        //Получить текущее значение
        float getValue() {
            return values.last();
        }
        //Получить предыдущее значение на fromEnd позиций с конца
        float getPrevValue(int fromEnd);

        int getMaxValue() {
            return maxValue;
        }
        QVector<float> getValues() {
            return QVector<float>::fromList(values);
        }
        QVector<float> getIterations() {
            return iterations;
        }
        QString getName() {
            return parameterName;
        }
    signals:
        void valueChanged(float pNewValue);
    public slots:
        //запуск генерации
        void start();
        //генерирование нового значения
        void updateValue();
        //сброс параметра, очищение всех данных
        void reset();
    protected:
        //функция генерирования нового значения
        virtual float generateNextValue();
        //список значений
        QList<float> values;
        QVector<float> iterations;
        QTimer timer;
        int maxValue;
        //шаг увеличения функции
        float step;
        //время в мсек на итерацию
        int iterationTimeMsec;
        //номер текущего значения
        int currentIteration;
        QString parameterName;
};


#endif // PARAMOBJECT_H
