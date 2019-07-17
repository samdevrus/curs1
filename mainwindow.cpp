#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "parameters.h"
#include "datapredictor.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    secondsFromStart(0),
    updateIntervalmSec(1000),
    currentMode(NormalMode)
{
    //Инициализация графического интерфейса
    ui->setupUi(this);
    setWindowTitle(tr("Cистема контроля за цифровой вычислительной структурой"));
    QWidget* centralWidget = new QWidget();
    this->setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QWidget *topBar = new QWidget();
    mainLayout->addWidget(topBar);
    topBar->setLayout(new QHBoxLayout());
    topBar->layout()->addWidget(new QLabel(tr("Состояние: ")));
    lState = new QLabel(tr("1 режим"));
    topBar->layout()->addWidget(lState);
    topBar->layout()->addWidget(new QLabel(tr(" Прогноз: ")));
    lPrediction = new QLabel(tr("система работает нормально"));
    topBar->layout()->addWidget(lPrediction);

    parametersTable = new QTableWidget(25,5);
    parametersTable->setHorizontalHeaderLabels(QStringList()
                                               <<tr("Название параметра")
                                               <<tr("Текущее значение")
                                               <<tr("Опасное значение")
                                               <<tr("Аварийное значение")
                                               <<tr("Действия")
                                               );
    parametersTable->hideColumn(3);
    parametersTable->verticalHeader()->hide();
    parametersTable->setWordWrap(true);
    parametersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    parametersTable->horizontalHeader()->setStretchLastSection(true);
    parametersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(parametersTable, SIGNAL(cellClicked(int,int)),
            this, SLOT(tableCellClick(int,int)));
    chart = new QChart();
    chart->setTheme(QChart::ChartThemeDark);
    chart->legend()->show();
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 50);
    axisX->setTickCount(1);
    axisX->setLabelFormat("%.2f");
    chart->setAxisX(axisX);
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 100);
    axisY->setTickCount(1);
    axisY->setLabelFormat("%.2f");
    chart->setAxisY(axisY);
    parametersChart = new QChartView();
    parametersChart->setChart(chart);
    parametersChart->setRenderHint(QPainter::Antialiasing);
    teRecomendations = new QTextEdit(tr("нет рекомендаций"));
    teRecomendations->setReadOnly(true);
    pbApplyRecommendations = new QPushButton(tr("Предпринять действия"));
    pbApplyRecommendations->setEnabled(false);
    connect(pbApplyRecommendations,&QPushButton::clicked,
            this,&MainWindow::resetProblemParams);
    QWidget *bottomBar = new QWidget();
    bottomBar->setMaximumHeight(120);
    bottomBar->setLayout(new QHBoxLayout());
    bottomBar->layout()->addWidget(teRecomendations);
    bottomBar->layout()->addWidget(pbApplyRecommendations);

    mainLayout->addWidget(parametersTable,1);
    mainLayout->addWidget(parametersChart,1);
    mainLayout->addWidget(new QLabel(tr("Описание аварийной ситуации")));
    mainLayout->addWidget(bottomBar);
    centralWidget->setLayout(mainLayout);

    lInterval = new QLabel(QString::number(updateIntervalmSec/1000));
    lTimeUpdated = new QLabel(QTime::currentTime().toString());
    statusBar()->addWidget(new QLabel(tr("Интервал обновления (в секундах)")));
    statusBar()->addWidget(lInterval);
    statusBar()->addPermanentWidget(new QLabel(tr("Последнее время обновления ")));
    statusBar()->addPermanentWidget(lTimeUpdated);

    //Инициализация параметров
    declareParameters();
    int rowNumber = 0;
    QListIterator<ParamGroup*> groupIterator(paramGroups);
    while(groupIterator.hasNext()) {
        ParamGroup* group = groupIterator.next();
        //Добавление групп параметров в таблицу
        parametersTable->setSpan(rowNumber,0,1,5);
        QTableWidgetItem *groupCellItem =
                new QTableWidgetItem(group->getName());
        groupCellItem->setTextAlignment(Qt::AlignCenter);
        parametersTable->setItem(rowNumber,0,groupCellItem);

        QListIterator<ParamObject*> paramIterator = group->getParamIterator();
        paramGroupRows.insert(group,rowNumber);
        rowNumber++;
        while(paramIterator.hasNext()) {
            ParamObject* param = paramIterator.next();
            QPushButton* btnReset = new QPushButton(tr("Сброс"));
            QObject::connect(param,&ParamObject::valueChanged,
                                     this,&MainWindow::paramValueChanged);
            QObject::connect(btnReset,&QPushButton::clicked,
                                     param,&ParamObject::reset);
            //Создание серии параметра для графика
            QLineSeries* series = new QLineSeries();
            series->setName(param->getName());
            series->append(secondsFromStart,param->getValue());
            if (chart->series().count()<4) {
                chart->addSeries(series);
                series->attachAxis(chart->axisX());
                series->attachAxis(chart->axisY());
            }
            parameterLines.insert(param,series);
            //заполнение таблицы
            ParamChecker* check = parameterChecks[param];
            int parameterValue = param->getValue();
            int parameterAttentionValue = check->getAttentionValue();
            int parameterDangerValue = check->getDangerValue();
            parametersTable->setItem(rowNumber,0,
                                             new QTableWidgetItem(param->getName()));
            QTableWidgetItem* valueItem = new QTableWidgetItem(QString::number(param->getValue()));
            valueItem->setTextAlignment(Qt::AlignCenter);
            QTableWidgetItem* attItem = new QTableWidgetItem(QString::number(parameterAttentionValue));
            attItem->setTextAlignment(Qt::AlignCenter);
            QTableWidgetItem* dangItem = new QTableWidgetItem(QString::number(parameterDangerValue));
            dangItem->setTextAlignment(Qt::AlignCenter);

            parametersTable->setItem(rowNumber,1,valueItem);
            parametersTable->setItem(rowNumber,2,attItem);
            parametersTable->setItem(rowNumber,3,dangItem);
            parametersTable->setCellWidget(rowNumber,4,btnReset);            
            rowNumber++;
            //запуск генерации параметра
            param->start();
        }
    }
    //Запуск таймера для анализа данных
    timer.setInterval(updateIntervalmSec);
    connect(&timer,SIGNAL(timeout()),this,SLOT(onTimer()));
    timer.start();
}

void MainWindow::resetProblemParams() {
    QListIterator<ParamObject*> paramIterator(problemParams);
    while(paramIterator.hasNext()) {
        ParamObject* param = paramIterator.next();
        param->reset();
    }
}

void MainWindow::onTimer() {
    timeIterations.append(secondsFromStart);

    //проверка всех параметров, подсчет состояний и подсветка
    int normalParam = 0;
    int attentionParamCount = 0;
    int dangerZoneParamCount = 0;
    int dangerParamCount = 0;
    QMapIterator<ParamGroup*, int> paramGroupRow(paramGroupRows);
    while (paramGroupRow.hasNext()) {
        paramGroupRow.next();
        ParamGroup* group = paramGroupRow.key();
        int rowIndex = paramGroupRow.value()+1;
        QListIterator<ParamObject*> paramIterator = group->getParamIterator();
        while (paramIterator.hasNext()) {
            ParamObject* param = paramIterator.next();
            ParamChecker* checker = parameterChecks[param];
            QTableWidgetItem* item = parametersTable->item(rowIndex,1);
            if (checker->check()) {
                item->setBackgroundColor(normalColor);
                normalParam++;
            } else {
                if (checker->getState()==ParamChecker::State::Attention) {
                    item->setBackgroundColor(attentionColor);
                    attentionParamCount++;
                }
                if (checker->getState()==ParamChecker::State::DangerZone) {
                    item->setBackgroundColor(dangerZoneColor);
                    dangerZoneParamCount++;
                }
                if (checker->getState()==ParamChecker::State::Danger) {
                    item->setBackgroundColor(dangerColor);
                    dangerParamCount++;
                }
            }
            rowIndex++;
        }

    }
    //смена текущего режима в зависимости от текущих состояний
    if ((attentionParamCount + dangerZoneParamCount + dangerParamCount) == 0) {
        currentMode = NormalMode;
    }
    if (attentionParamCount>0) {
        currentMode = AttentionMode;
    }
//    if (dangerZoneParamCount + dangerParamCount>1) {
//        currentMode = DangerMode;
//    }
    lTimeUpdated->setText(QTime::currentTime().toString());
    if (currentMode!=Mode::NormalMode) {
        //запуск функции прогноза
        checkState();
    }
    QString modeText = "";
    switch (currentMode) {
        case Mode::NormalMode :
            modeText = "1 режим";
            parametersTable->hideColumn(3);
            parametersTable->showColumn(2);
            teRecomendations->clear();
            lPrediction->setText(tr("система работает нормально"));
            pbApplyRecommendations->setEnabled(false);
        break;
        case Mode::AttentionMode :
            modeText = "2 режим";
            parametersTable->hideColumn(2);
            parametersTable->showColumn(3);
        break;
        case Mode::DangerMode :
            modeText = "3 режим";
            //запуск функции прогноза
            //checkState();
            lPrediction->setText(tr("авария наступила"));
        break;
    }
    lState->setText(modeText);
    //изменение диапазона графика
    parametersChart->chart()->axisX()->setRange(0,secondsFromStart-1);
    parametersChart->chart()->axisY()->setRange(0,selectedGroup->getTopValue()+10);

    secondsFromStart++;
}

void MainWindow::checkState() {
    QStringList reccomendations;
    problemParams.clear();
    //время до аварии (в секундах)
    int timeUntil = 100;

    QMap<ParamGroup*, int> groupTimes;

    QListIterator<ParamGroup*> groupIterator(paramGroups);
    while(groupIterator.hasNext()) {
        ParamGroup* group = groupIterator.next();
        ParamGroupChecker* groupChecker = groupChecks.find(group).value();
        QStringList groupReccomendations;
        int groupTime = 100;
        //поиск проблемных параметров
        QListIterator<ParamObject*> paramIterator = group->getParamIterator();
        while(paramIterator.hasNext()) {
            ParamObject* p = paramIterator.next();
            ParamChecker* check = parameterChecks[p];
            //если параметр в аварийном состоянии то его пропускаем
            ParamChecker::State paramState =  check->checkState(p->getValue());
            if ((paramState == ParamChecker::DangerZone) ||
                    (paramState==ParamChecker::Danger)) {
                problemParams.append(p);
            }
        }
        int predictionTime = 0;
        int predictionStep = 1;
        bool checkResult =  groupChecker->check();
        ParamGroupPrediction prediction;
        if (checkResult == false) {
            reccomendations.append(groupChecker->getMessage()+
                                   tr(". Авария наступила "));
            currentMode = DangerMode;
            continue;
        }
        //поиск времени когда произойдет авария
        for (predictionTime= secondsFromStart+predictionStep;
             predictionTime<secondsFromStart+predictionStep*50;
             predictionTime+=predictionStep) {
            //float parameterTime = p->getIterations().last();
            float timeDiff =predictionTime - secondsFromStart;
            prediction = groupChecker->checkWithPrediction(timeDiff);
            if (prediction.everytingIsOk == false) {
                break;
            }
         }
        if (prediction.everytingIsOk == false) {
            //int t  = predictionTime - secondsFromStart;

            QListIterator<PredictionMessage> predMessageIterator(prediction.messages);
            while (predMessageIterator.hasNext()) {

                PredictionMessage predMessage = predMessageIterator.next();
                if (predMessage.timeUntil<timeUntil) {
                    timeUntil = predMessage.timeUntil;
                }
                if (predMessage.timeUntil<groupTime) {
                    groupTime = predMessage.timeUntil;
                }
                reccomendations.append(predMessage.message+
                                   tr(". Авария через ")+
                                   QString::number(predMessage.timeUntil) +
                                   tr(" секунд"));
            }
        }
        groupTimes.insert(group,groupTime);
    }
    QMapIterator<ParamGroup*, int> groupTimesIterator(groupTimes);
    ParamGroup* dangerGroup = NULL;
    int dangerGroupTime = 100;
    while(groupTimesIterator.hasNext()) {
        groupTimesIterator.next();
        if (groupTimesIterator.value()<dangerGroupTime) {
            dangerGroup = groupTimesIterator.key();
            dangerGroupTime = groupTimesIterator.value();
        }
    }
    if (!(dangerGroup == NULL)) {
        lPrediction->setText(
                             dangerGroup->getName() +
                            tr(". Авария через ") +
                            QString::number(dangerGroupTime)+
                             tr(" секунд"));
    } else {
        lPrediction->setText(tr("В ближайшее время не будет аварии"));
    }
    pbApplyRecommendations->setEnabled(!problemParams.empty());
    teRecomendations->setText(reccomendations.join("\n"));
}

void MainWindow::tableCellClick(int row, int column) {
    QMapIterator<ParamGroup*, int> paramGroupRow(paramGroupRows);
    int groupRow = -1;
    ParamGroup* group = NULL;
    //поиск группы соответствующей данной строке
    while (paramGroupRow.hasNext()) {
        paramGroupRow.next();
        if ((paramGroupRow.value()<=row) &
                ((paramGroupRow.value()+paramGroupRow.key()->count())>=row))
        {
            group = paramGroupRow.key();
            groupRow = paramGroupRow.value();
            break;
        }
    }
    if (group!=NULL) {
        //chart->axisY()->setRange(0,group->getParamsMaxValue());
        chart->setTitle(group->getName());
        this->removeAllSeries();
        selectedGroup = group;
        QListIterator<ParamObject*> paramIterator = group->getParamIterator();
        while (paramIterator.hasNext()) {
            ParamObject* param = paramIterator.next();
            QLineSeries *series = parameterLines.find(param).value();
            chart->addSeries(series);
            series->attachAxis(chart->axisX());
            series->attachAxis(chart->axisY());
        }
    }
}

void MainWindow::paramValueChanged(int value) {
    //обновление данных в таблице и на графике при получении сигнала
    ParamObject* senderObject = (ParamObject*) this->sender();
    QMapIterator<ParamGroup*, int> paramGroupRow(paramGroupRows);
    int row = -1;
    bool showSeries = false;
    while (paramGroupRow.hasNext()) {
        paramGroupRow.next();
        ParamGroup* group = paramGroupRow.key();
        QListIterator<ParamObject*> paramIterator = group->getParamIterator();
        int rowIndex = paramGroupRow.value()+1;
        while (paramIterator.hasNext()) {
            ParamObject* param = paramIterator.next();
            if (param == senderObject) {
                row = rowIndex;
                showSeries = selectedGroup == group;
                break;
            }
            rowIndex++;
        }
        if (row>=0) break;
    }
    QLineSeries *series = parameterLines.find(senderObject).value();
    series->append(secondsFromStart,value);
    QTableWidgetItem* item = parametersTable->item(row,1);
    item->setText(QString::number(value));
}

void MainWindow::declareParameters() {
    //создание всех групп и параметров

    ParamGroup* climateGroup = new ParamGroup(tr("Климат в комнате"));
    LinearParameter* conditionerTemp = new LinearParameter(
                tr("Температура на выходе из кондиционера"),5,100
                );
    {
        ParamChecker* check = new ParamChecker(conditionerTemp,30,50);
        parameterChecks.insert(conditionerTemp,check);
    }
    RoomTemperature* roomTemp = new RoomTemperature(conditionerTemp);
    {
        ParamChecker* check = new ParamChecker(roomTemp,30,40);
        parameterChecks.insert(roomTemp,check);
    }
    RoomHumidity* roomHumidity = new RoomHumidity(roomTemp);
    {
        ParamChecker* check = new ParamChecker(roomHumidity,50,70);
        parameterChecks.insert(roomHumidity,check);
    }
    AirPollution* airPollution = new AirPollution(roomTemp);
    {
        ParamChecker* check = new ParamChecker(airPollution,50,100);
        parameterChecks.insert(airPollution,check);
    }
    ParamGroupChecker* climateGroupChecker =
            new ParamGroupChecker(climateGroup,&parameterChecks);
    climateGroupChecker->addSituation({
                                          QList<ParamObject*>()
                                          <<roomHumidity
                                          <<airPollution,
                                          ParamChecker::DangerZone,
                                          tr("отказ системы очистки воздуха, короткое замыкание, отказ сервера")
                });
    climateGroupChecker->addSituation({
                                          QList<ParamObject*>()
                                          <<conditionerTemp
                                          <<roomTemp,
                                          ParamChecker::DangerZone,
                                          tr("отказ кондиционера, перегрев сервера, отказ сервера")
                });
    groupChecks.insert(climateGroup,climateGroupChecker);

    climateGroup->addParameter(conditionerTemp);
    climateGroup->addParameter(roomTemp);
    climateGroup->addParameter(roomHumidity);
    climateGroup->addParameter(airPollution);
    paramGroups.append(climateGroup);
    bool debug = false;
    if (!debug) {

    ParamGroup* powerGroup = new ParamGroup(tr("Электропитание"));
    LinearParameter* voltage = new LinearParameter(
                tr("Напряжение в сети"),200,360
                );
    {
        ParamChecker* check = new ParamChecker(voltage,240,260);
        parameterChecks.insert(voltage,check);
    }
    ElectricCurrent* eCurrent = new ElectricCurrent(voltage);
    {
        ParamChecker* check = new ParamChecker(eCurrent,10,16);
        parameterChecks.insert(eCurrent,check);
    }
    Battery* battery = new Battery(voltage);
    {
        ParamChecker* check = new ParamChecker(battery,60,80);
        parameterChecks.insert(battery,check);
    }
    WireTemperature* wireTemp = new WireTemperature(eCurrent);
    {
        ParamChecker* check = new ParamChecker(wireTemp,100,130);
        parameterChecks.insert(wireTemp,check);
    }
    ParamGroupChecker* powerGroupChecker =
            new ParamGroupChecker(powerGroup,&parameterChecks);
    powerGroupChecker->addSituation({
                                        QList<ParamObject*>()
                                        <<voltage
                                        <<battery,
                                        ParamChecker::Danger,
                                        tr("отключение электричества и разряд батарей , отключение сервера")
                });
    powerGroupChecker->addSituation({
                                        QList<ParamObject*>()
                                        <<wireTemp
                                        <<eCurrent,
                                        ParamChecker::Danger,
                                        tr("высыхание изоляции проводов, короткое замыкание , отключение сервера")
                });
    groupChecks.insert(powerGroup,powerGroupChecker);
    powerGroup->addParameter(voltage);
    powerGroup->addParameter(eCurrent);
    powerGroup->addParameter(battery);
    powerGroup->addParameter(wireTemp);
    paramGroups.append(powerGroup);

    ParamGroup* dataGroup = new ParamGroup(tr("Работа СХД"));
    LinearParameter* processorLoad = new LinearParameter(
                tr("Загруженность процессора"),0,100
                );
    {
        ParamChecker* check = new ParamChecker(processorLoad,90,95);
        parameterChecks.insert(processorLoad,check);
    }
    DriveLoad* driveLoad = new DriveLoad(processorLoad);
    {
        ParamChecker* check = new ParamChecker(driveLoad,80,90);
        parameterChecks.insert(driveLoad,check);
    }
    DriveTemperature* driveTemperature = new DriveTemperature(driveLoad);
    {
        ParamChecker* check = new ParamChecker(driveTemperature,40,50);
        parameterChecks.insert(driveTemperature,check);
    }
    ReadWriteErrors* rwErrors = new ReadWriteErrors(driveLoad);
    {
        ParamChecker* check = new ParamChecker(rwErrors,60,90);
        parameterChecks.insert(rwErrors,check);
    }
    ParamGroupChecker* dataGroupChecker = new ParamGroupChecker(dataGroup,&parameterChecks);
    dataGroupChecker->addSituation({
                                 QList<ParamObject*>()
                                 <<driveTemperature
                                 <<rwErrors,
                                 ParamChecker::Danger,
                                 "перегрев жестких дисков/носителей, отказ системы хранения"
                });
    dataGroupChecker->addSituation({
                                 QList<ParamObject*>()
                                 <<processorLoad
                                 <<driveLoad,
                                 ParamChecker::Danger,
                                 "отказ жесткого диска с операционной системой, отказ системы хранения"
                });
    groupChecks.insert(dataGroup,dataGroupChecker);
    dataGroup->addParameter(processorLoad);
    dataGroup->addParameter(driveLoad);
    dataGroup->addParameter(driveTemperature);
    dataGroup->addParameter(rwErrors);
    paramGroups.append(dataGroup);
    ParamGroup* networkGroup = new ParamGroup(tr("Сетевое оборудование"));
    LinearParameter* requests = new LinearParameter(
                tr("Количество входящих запросов"),0,100
                );
    {
        ParamChecker* check = new ParamChecker(requests,80,95);
        parameterChecks.insert(requests,check);
    }
    NetworkLoad* networkLoad = new NetworkLoad(requests);
    {
        ParamChecker* check = new ParamChecker(networkLoad,80,90);
        parameterChecks.insert(networkLoad,check);
    }
    RouterLoad* routerLoad = new RouterLoad(requests);
    {
        ParamChecker* check = new ParamChecker(routerLoad,60,90);
        parameterChecks.insert(routerLoad,check);
    }
    NetworkErrors* networkErrors = new NetworkErrors(routerLoad);
    {
        ParamChecker* check = new ParamChecker(networkErrors,70,90);
        parameterChecks.insert(networkErrors,check);
    }
    ParamGroupChecker* networkGroupChecker = new ParamGroupChecker(networkGroup,&parameterChecks);
    networkGroupChecker->addSituation({
                              QList<ParamObject*>()
                              <<networkLoad
                              <<routerLoad,
                              ParamChecker::Danger,
                              tr("перегрев роутера, отказ в доступе к серверу")
                });
    networkGroupChecker->addSituation({
                              QList<ParamObject*>()
                              <<requests
                              <<networkErrors,
                              ParamChecker::Danger,
                              tr("превышение допустимого порога запросов, отказ в доступе к серверу")
                });
    groupChecks.insert(networkGroup,networkGroupChecker);

    networkGroup->addParameter(requests);
    networkGroup->addParameter(networkLoad);
    networkGroup->addParameter(routerLoad);
    networkGroup->addParameter(networkErrors);
    paramGroups.append(networkGroup);

    ParamGroup* serverGroup = new ParamGroup(tr("Температура корпуса"));
    LinearParameter* filterPollution = new LinearParameter(
                tr("Загрязненность фильтров"),0,100
                );
    {
        ParamChecker* check = new ParamChecker(filterPollution,50,65);
        parameterChecks.insert(filterPollution,check);
    }
    ServerOutTemperature* outTemp = new ServerOutTemperature(filterPollution);
    {
        ParamChecker* check = new ParamChecker(outTemp,50,90);
        parameterChecks.insert(outTemp,check);
    }
    ChipsTempetature* chipsTemp = new ChipsTempetature(filterPollution);
    {
        ParamChecker* check = new ParamChecker(chipsTemp,60,90);
        parameterChecks.insert(chipsTemp,check);
    }
    NoiseLevel* noiseLevel = new NoiseLevel(chipsTemp);
    {
        ParamChecker* check = new ParamChecker(noiseLevel,70,90);
        parameterChecks.insert(noiseLevel,check);
    }
    ParamGroupChecker* serverGroupChecker = new ParamGroupChecker(serverGroup,&parameterChecks);
    serverGroupChecker->addSituation({
                                     QList<ParamObject*>()
                                     <<outTemp
                                     <<noiseLevel,
                                     ParamChecker::Danger,
                                     tr("выход из строя подшипников системы охлаждения, перегрев сервера")
                });
    serverGroupChecker->addSituation({
                                     QList<ParamObject*>()
                                     <<filterPollution
                                     <<chipsTemp,
                                     ParamChecker::Danger,
                                     tr("превышение температуры чипов сервера ")
                });
    groupChecks.insert(serverGroup,serverGroupChecker);

    serverGroup->addParameter(filterPollution);
    serverGroup->addParameter(outTemp);
    serverGroup->addParameter(chipsTemp);
    serverGroup->addParameter(noiseLevel);
    paramGroups.append(serverGroup);

        }
    selectedGroup = climateGroup;

    chart->setTitle(climateGroup->getName());
}

void MainWindow::removeAllSeries() {
    //Удаление всех серий с таблицы
    QMapIterator<ParamObject*, QLineSeries*> linesIterator(parameterLines);
    while(linesIterator.hasNext()) {

        linesIterator.next();
        if (selectedGroup->hasParameter(linesIterator.key())==false) {
            continue;
        }
        QLineSeries* series = linesIterator.value();
        chart->removeSeries(series);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
