#ifndef LISTENINGTEST_H
#define LISTENINGTEST_H

#include <QWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QInputDialog>
#include <QVector>
#include <QPair>
#include <QMessageBox>
#include <QButtonGroup>
#include <QRadioButton>

struct ListeningQuestion {
    QString question;
    QStringList options;
    int correctOption;  // Индекс правильного ответа (начиная с 0)
};

class ListeningTest : public QWidget {
    Q_OBJECT

public:
    explicit ListeningTest(QWidget *parent = nullptr);
    ~ListeningTest();

private slots:
    void startTest();
    void returnToMainMenu();
    void checkAnswers();

private:
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QPushButton *backButton;
    QPushButton *submitButton;
    QVector<QRadioButton*> answerButtons;
    QVector<ListeningQuestion> questions;
    void loadQuestions();
};
#endif
