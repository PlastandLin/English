#include "listeningtest.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include "mainwindow.h"

ListeningTest::ListeningTest(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Listening Test");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *instructionLabel = new QLabel("Послушайте аудио и ответьте на вопросы:");
    QPushButton *playButton = new QPushButton("Включить аудиозапись");
    submitButton = new QPushButton("Проверить");
    backButton = new QPushButton("Назад в меню");

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);

    layout->addWidget(titleLabel);
    layout->addWidget(instructionLabel);
    layout->addWidget(playButton);

    // Загружаем вопросы
    loadQuestions();
    for (const auto &question : questions) {
        QLabel *qLabel = new QLabel(question.question);
        qLabel->setWordWrap(true);
        layout->addWidget(qLabel);

        QButtonGroup *group = new QButtonGroup(this);
        for (int i = 0; i < question.options.size(); ++i) {
            QRadioButton *radio = new QRadioButton(question.options[i]);
            layout->addWidget(radio);
            answerButtons.append(radio);
            group->addButton(radio);
        }
    }

    layout->addWidget(submitButton);
    layout->addWidget(backButton);
    layout->addStretch();

    connect(playButton, &QPushButton::clicked, this, [=]() {
        player->setSource(QUrl::fromLocalFile("listening/audio/05Meeting a new friend in class--.mp3"));  // Путь к аудио
        player->play();
    });

    connect(submitButton, &QPushButton::clicked, this, &ListeningTest::checkAnswers);
    connect(backButton, &QPushButton::clicked, this, &ListeningTest::returnToMainMenu);
}

void ListeningTest::loadQuestions() {
    questions = {
        {
            "1. Where is Laura originally from?",
            {"a) Sydney, Australia", "b) Adelaide, Australia", "c) Australia", "d) Auckland, New Zealand"},
            2  // Правильный ответ: c (индекс 2)
        },
        {
            "2. How many siblings does Sam have?",
            {"a) A large family of 8 siblings", "b) Seven", "c) More than a handful of siblings", "d) A close-knit family of 6-8"},
            1  // Правильный ответ: b (индекс 1)
        },
        {
            "3. What musical instruments does Laura play?",
            {"a) Piano and clarinet, and is learning the guitar", "b) Guitar and clarinet, and is learning the piano",
             "c) Clarinet, piano, and guitar", "d) Guitar, piano, and violin"},
            2  // Правильный ответ: c (индекс 2)
        },
        {
            "4. Why did Laura move to New York?",
            {"a) She moved to New York with her parents a few years ago, then went to college somewhere else",
             "b) She moved to New York for high school, then stayed to complete a degree",
             "c) She moved to New York with her parents a few years ago and stayed for college after graduating from high school.",
             "d) She moved to New York for a semester abroad, then decided to study for a degree"},
            2  // Правильный ответ: c (индекс 2)
        },
        {
            "5. What instrument does Sam try to play?",
            {"a) Sam plays the bass guitar in a band, but badly", "b) Sam plays the guitar, but often borrows his friend's bass",
             "c) Sam has a bass guitar he tries to play", "d) Sam tries to play the bass guitar, but he admits he's not very good at it."},
            3  // Правильный ответ: d (индекс 3)
        }
    };
}

void ListeningTest::checkAnswers() {
    int correct = 0;
    int total = questions.size();
    int buttonIndex = 0;

    for (int i = 0; i < total; ++i) {
        if (answerButtons[buttonIndex + questions[i].correctOption]->isChecked()) {
            correct++;
        }
        buttonIndex += questions[i].options.size();
    }

    int score = (correct * 10) / total;  // Переводим в 10-балльную шкалу
    QFile file("listening_score.txt");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << score;
    }

    QMessageBox::information(this, "Result",
                             QString("Correct answers: %1/%2\nScore: %3/10").arg(correct).arg(total).arg(score));
}

void ListeningTest::returnToMainMenu() {
    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->stop();
    }

    if (MainWindow *mw = qobject_cast<MainWindow*>(parent())) {
        mw->returnToMainMenu();
    }
}

void ListeningTest::startTest() {
    bool ok;
    QString answer = QInputDialog::getText(this, "Question",
                                           "What was the main topic of the audio?",
                                           QLineEdit::Normal, "", &ok);

    if (ok && !answer.isEmpty()) {
        int score = answer.contains("weather", Qt::CaseInsensitive) ? 1 : 0;

        QFile file("listening_score.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << score;
        }

        QMessageBox::information(this, "Result",
                                 QString("Your answer: %1\nScore: %2/1").arg(answer).arg(score));
    }
}

ListeningTest::~ListeningTest() {
    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->stop();
    }
}
