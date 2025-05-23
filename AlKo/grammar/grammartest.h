#ifndef GRAMMARTEST_H
#define GRAMMARTEST_H

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>

struct GrammarQuestion {
    QString text;
    QString answer;
    int points;
};

class GrammarTest : public QWidget {
    Q_OBJECT

public:
    explicit GrammarTest(QWidget *parent = nullptr);
    void runTest();

private slots:
    void returnToMainMenu();

private:
    void loadQuestions();
    void saveScore(int score);
    QVector<GrammarQuestion> questions;
    QPushButton *backButton;
};
#endif
