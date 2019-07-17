#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QtCharts>
#include <QTimer>
#include "paramobject.h"
#include "paramchecker.h"
#include "paramgroup.h"
#include "paramgroupchecker.h"

using namespace QtCharts;

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
    public slots:
        //обработка нового значения параметра
        void paramValueChanged(int value);
        //обработка щелчка по таблице
        void tableCellClick(int row, int column);
        //обработка данных по таймеру
        void onTimer();
        //сброс параметров которые могут привести к аварии
        void resetProblemParams();
        //проверка и прогнозирование параметров
        void checkState();
        //удаление всех серий с графика
        void removeAllSeries();
    private:
        Ui::MainWindow *ui;
        //состояние
        enum Mode {NormalMode, AttentionMode, DangerMode};
        //функция создания параметров
        void declareParameters();
        //группы параметров
        QList<ParamGroup*> paramGroups;
        //аварийные параметры
        QList<ParamObject*> problemParams;
        //связь групп со строками в таблице
        QMap<ParamGroup*,int> paramGroupRows;
        //связь параметра с проверяющим объектом
        QMap<ParamObject*,ParamChecker*> parameterChecks;
        //связь группы с проверяющим объектом
        QMap<ParamGroup*,ParamGroupChecker*> groupChecks;
        //связь параметра с серией на графике
        QMap<ParamObject*, QLineSeries*> parameterLines;
        //вектор времени
        QVector<float> timeIterations;
        QTimer timer;
        QTableWidget *parametersTable;
        QChartView* parametersChart;
        QTextEdit *teRecomendations;
        QPushButton *pbApplyRecommendations;
        QChart *chart;
        QLabel *lTimeUpdated;
        QLabel *lInterval;
        QLabel *lState;
        QLabel *lPrediction;
        //выбранная группа
        ParamGroup* selectedGroup;
        Mode currentMode;
        //интервал обновления данных
        int updateIntervalmSec;
        //текущее значение времени в секундах со старта программы
        int secondsFromStart;
        //цвета для подсветки значений параметров
        const QColor normalColor = QColor(34,139,34);
        const QColor attentionColor = QColor(204,204,0);
        const QColor dangerZoneColor = QColor(255,140,0);
        const QColor dangerColor = QColor(220,20,60);
};

#endif // MAINWINDOW_H
