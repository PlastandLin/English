#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QFile>

class GrammarTest;
class WritingTest;
class ListeningTest;
class SpeakingTest;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void returnToMainMenu();

private slots:
    void startGrammarTest();
    void startWritingTest();
    void startListeningTest();
    void startSpeakingTest();
    void showFinalResults();

private:
    Ui::MainWindow *ui;
    void setupUI();
    void loadStyleSheet();
    void showMainMenu();
    QWidget *currentTest = nullptr;
};
