#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "grammar/grammartest.h"
#include "writing/writingtest.h"
#include "listening/listeningtest.h"
#include "speaking/speakingtest.h"
#include "level/leveltest.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setupUI();
    loadStyleSheet();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupUI() {
    setWindowTitle("English Level Test");
    resize(800, 600);
    showMainMenu();
}

void MainWindow::showMainMenu() {
    // Очищаем предыдущий интерфейс
    if (currentTest) {
        currentTest->deleteLater();
        currentTest = nullptr;
    }

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QLabel *titleLabel = new QLabel("Welcome to English Level Test");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    QPushButton *grammarButton = new QPushButton("Grammar Test");
    QPushButton *writingButton = new QPushButton("Writing Test");
    QPushButton *listeningButton = new QPushButton("Listening Test");
    QPushButton *speakingButton = new QPushButton("Speaking Test");
    QPushButton *resultsButton = new QPushButton("View Final Results");

    QList<QPushButton*> buttons = {grammarButton, writingButton, listeningButton, speakingButton, resultsButton};
    for (auto btn : buttons) {
        btn->setMinimumHeight(50);
        btn->setStyleSheet("font-size: 16px; margin: 5px;");
    }

    layout->addWidget(grammarButton);
    layout->addWidget(writingButton);
    layout->addWidget(listeningButton);
    layout->addWidget(speakingButton);
    layout->addWidget(resultsButton);
    layout->addStretch();

    setCentralWidget(centralWidget);

    connect(grammarButton, &QPushButton::clicked, this, &MainWindow::startGrammarTest);
    connect(writingButton, &QPushButton::clicked, this, &MainWindow::startWritingTest);
    connect(listeningButton, &QPushButton::clicked, this, &MainWindow::startListeningTest);
    connect(speakingButton, &QPushButton::clicked, this, &MainWindow::startSpeakingTest);
    connect(resultsButton, &QPushButton::clicked, this, &MainWindow::showFinalResults);
}

void MainWindow::returnToMainMenu() {
    showMainMenu();
}

void MainWindow::startGrammarTest() {
    currentTest = new GrammarTest(this);
    setCentralWidget(currentTest);
}

void MainWindow::startWritingTest() {
    currentTest = new WritingTest(this);
    setCentralWidget(currentTest);
}

void MainWindow::startListeningTest() {
    currentTest = new ListeningTest(this);
    setCentralWidget(currentTest);
}

void MainWindow::startSpeakingTest() {
    currentTest = new SpeakingTest(this);
    setCentralWidget(currentTest);
}

void MainWindow::showFinalResults() {
    LevelTest::showFinalResult();
}

void MainWindow::loadStyleSheet() {
    QFile file(":/styles/style.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
    }
}
