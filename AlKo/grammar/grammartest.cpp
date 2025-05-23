#include "grammartest.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "mainwindow.h"

GrammarTest::GrammarTest(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *label = new QLabel("Grammar Test");
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 20px; font-weight: bold;");

    QPushButton *startButton = new QPushButton("Начать тест");
    backButton = new QPushButton("Назад в меню");

    layout->addWidget(label);
    layout->addWidget(startButton);
    layout->addWidget(backButton);
    layout->addStretch();

    connect(startButton, &QPushButton::clicked, this, &GrammarTest::runTest);
    connect(backButton, &QPushButton::clicked, this, &GrammarTest::returnToMainMenu);

    loadQuestions();
}

void GrammarTest::loadQuestions() {
    questions = {
        // Простые вопросы (1 балл)
        {"Выберите правильный вариант:\nHe _____ a book. [read/reads]", "reads", 1},
        {"Выберите правильный вариант:\nThey _____ soccer on weekends. [play/plays]", "play", 1},
        {"Выберите правильный предлог:\nThe book is ______ the table. [on/in]", "on", 1},
        {"Выберите правильный предлог:\nI am going ______ the store. [to/at]", "to", 1},
        {"Выберите правильную форму глагола:\nShe ______ studying right now. [is/are]", "is", 1},
        {"Выберите правильную форму глагола:\nThey ______ finished their homework yet. [have/has]", "have", 1},
        {"Выберите правильный вариант:\nThe cat _____ on the sofa. [sleep/sleeps]", "sleeps", 1},
        {"Выберите правильный вариант:\nMy friend _____ in London. [live/lives]", "lives", 1},
        {"Выберите правильный вариант:\nWe _____ a car. [have/has]", "have", 1},
        {"Выберите правильный вариант:\nThe train _____ at 10 AM. [leave/leaves]", "leaves", 1},
        {"Выберите правильный вариант:\nHe _____ to the music. [dance/dances]", "dances", 1},
        {"Выберите правильный вариант:\nShe _____ English very well. [speak/speaks]", "speaks", 1},
        {"Выберите правильный вариант:\nThey _____ tennis every weekend. [play/plays]", "play", 1},

        // Сложные вопросы (2 балла)
        {"Выберите правильное время:\nIf I __  the answer, I would tell you. [knew / would know / have known]", "knew", 2},
        {"Выберите правильную форму глагола:\nHe asked me where I __  on vacation last year. [went / had gone / have gone]", "had gone", 2},
        {"Выберите правильный модальный глагол:\nYou __  see a doctor if you have a fever. [must / should / might]", "should", 2},
        {"Выберите правильное время:\nBy the time we arrived, the movie __ . [started / had started / was starting]", "had started", 2},
        {"Выберите правильный предлог:\nShe is very good __ playing the piano. [at / in / on]", "at", 2},
        {"Выберите правильное условное наклонение:\nIf she __ harder, she would have passed the exam. [studied / had studied / would have studied]", "had studied", 2},
        {"Выберите правильную форму глагола:\nHe is said __ a very talented musician. [to be / being / be]", "to be", 2}
    };
}

void GrammarTest::runTest() {
    int totalScore = 0;
    int maxPossibleScore = 0;

    for (const auto &question : questions) {
        maxPossibleScore += question.points;

        bool ok;
        QString answer = QInputDialog::getText(this, "Grammar Test",
                                               question.text,
                                               QLineEdit::Normal, "", &ok);

        if (ok && answer.trimmed().toLower() == question.answer) {
            totalScore += question.points;
        }
    }

    int scaledScore = (totalScore * 10) / maxPossibleScore;
    saveScore(scaledScore);

    QMessageBox::information(this, "Result",
                             QString("Счет по 10-бальной шкале: %1/%2\nОбщий счет: %3/%4")
                                 .arg(scaledScore).arg(10)
                                 .arg(totalScore).arg(maxPossibleScore));
}

void GrammarTest::saveScore(int score) {
    QFile file("grammar_score.txt");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << score;
    }
}

void GrammarTest::returnToMainMenu() {
    if (MainWindow *mw = qobject_cast<MainWindow*>(parent())) {
        mw->returnToMainMenu();
    }
}
