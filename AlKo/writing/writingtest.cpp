#include "writingtest.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include "mainwindow.h"

void WritingTest::loadDictionary() {
    dictionary.clear();

    // 1. Путь к исходной папке проекта (где должен быть файл english_words.txt)
    QString projectDir = QCoreApplication::applicationDirPath();
    projectDir = projectDir.left(projectDir.indexOf("/build")); // Убираем часть пути с /build
    QString dictPath = projectDir + "/writing/english_words.txt";

    qDebug() << "Looking for dictionary at:" << dictPath;

    // 2. Загрузка словаря
    QFile dictFile(dictPath);
    if (dictFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&dictFile);
        int loadedWords = 0;
        while (!in.atEnd()) {
            QString word = in.readLine().trimmed().toLower();
            if (!word.isEmpty()) {
                dictionary.insert(word);
                loadedWords++;
            }
        }
        dictFile.close();
        qDebug() << "Successfully loaded" << loadedWords << "words from dictionary";
    } else {
        // Запасной вариант - базовый набор слов
        QStringList basicWords = {
            "i", "like", "hobby", "sport", "music", "read", "play", "my",
            "the", "and", "to", "of", "a", "in", "is", "it", "that", "for",
            "you", "he", "she", "we", "they", "this", "are", "as", "with",
            "have", "has", "was", "were", "be", "been", "at", "on", "but",
            "not", "by", "from", "or", "an", "do", "did", "can", "could",
            "will", "would", "what", "when", "where", "which", "who", "how"
        };
        dictionary = QSet<QString>(basicWords.begin(), basicWords.end());
        qDebug() << "Using default dictionary with" << dictionary.size() << "words";
    }
}

WritingTest::WritingTest(QWidget *parent) : QWidget(parent) {
    // Проверка путей перед загрузкой словаря
    qDebug() << "Current directory:" << QDir::currentPath();
    qDebug() << "Application directory:" << QCoreApplication::applicationDirPath();

    loadDictionary();

    // Проверка загрузки словаря
    qDebug() << "Dictionary contains 'the':" << dictionary.contains("the");
    qDebug() << "Dictionary contains 'computer':" << dictionary.contains("computer");

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Writing Test");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *instructionLabel = new QLabel(
        "Напишите сочинение на 120-150 слов про ваши хобби.\n"
        "Если будет меньше 120 слов - работа не засчитается.\n"
        "Если больше 150 слов - вы получите минус балл.");
    instructionLabel->setWordWrap(true);

    textEdit = new QTextEdit;
    textEdit->setPlaceholderText("Write your essay about hobbies here...");

    QPushButton *checkButton = new QPushButton("Проверить текст");
    QPushButton *saveButton = new QPushButton("Сохранить результаты");
    backButton = new QPushButton("Назад в меню");

    layout->addWidget(titleLabel);
    layout->addWidget(instructionLabel);
    layout->addWidget(textEdit);
    layout->addWidget(checkButton);
    layout->addWidget(saveButton);
    layout->addWidget(backButton);

    connect(checkButton, &QPushButton::clicked, this, &WritingTest::checkText);
    connect(saveButton, &QPushButton::clicked, this, &WritingTest::saveResults);
    connect(backButton, &QPushButton::clicked, this, &WritingTest::returnToMainMenu);
}

void WritingTest::checkText() {
    QString text = textEdit->toPlainText();

    // Проверка на не-английские символы
    QRegularExpression nonEnglishRegex("[^a-zA-Z\\s.,!?';:\"'\\-()\\[\\]{}]");
    QRegularExpressionMatch match = nonEnglishRegex.match(text);

    if (match.hasMatch()) {
        QMessageBox::warning(this, "Ошибка",
                             QString("Текст содержит не-английские символы: '%1'")
                                 .arg(match.captured()));
        return;
    }

    // Подсчёт слов
    QStringList words = text.split(QRegularExpression("[\\s\\n]+"), Qt::SkipEmptyParts);
    int wordCount = words.size();

    if (wordCount < 120) {
        QMessageBox::warning(this, "Ошибка",
                             QString("Слишком мало слов: %1 (требуется минимум 120)").arg(wordCount));
        return;
    }

    highlightErrors(text);
    int errorCount = countErrors(text);
    int score = calculateScore(wordCount, errorCount);

    QMessageBox::information(this, "Результат проверки",
                             QString("Найдено ошибок: %1\nКоличество слов: %2\nПредварительная оценка: %3/10")
                                 .arg(errorCount).arg(wordCount).arg(score));
}

int WritingTest::calculateScore(int wordCount, int errorCount) {
    int score = 10;

    // Штраф за превышение длины
    if (wordCount > 150) {
        score -= 1;
    }

    // Штраф за ошибки (1 балл за каждые 5 ошибок)
    int errorPenalty = errorCount / 5;
    score -= errorPenalty;

    return qBound(0, score, 10); // Ограничиваем диапазон 0-10
}

void WritingTest::highlightErrors(const QString &text) {
    QTextCursor cursor(textEdit->document());
    cursor.beginEditBlock();

    // Очищаем предыдущее форматирование
    QTextCharFormat normalFormat;
    cursor.select(QTextCursor::Document);
    cursor.setCharFormat(normalFormat);

    // Формат для ошибок
    QTextCharFormat errorFormat;
    errorFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    errorFormat.setUnderlineColor(Qt::red);

    // Разбиваем текст на слова с сохранением позиций
    QRegularExpression wordRegex("\\b[a-zA-Z']+\\b");
    QRegularExpressionMatchIterator i = wordRegex.globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString word = match.captured().toLower();

        // Проверяем слово по словарю
        if (!dictionary.contains(word)) {
            cursor.setPosition(match.capturedStart());
            cursor.setPosition(match.capturedEnd(), QTextCursor::KeepAnchor);
            cursor.setCharFormat(errorFormat);
        }
    }

    cursor.endEditBlock();
}

int WritingTest::countErrors(const QString &text) {
    int errors = 0;

    // Проверка заглавных букв в начале предложения
    errors += text.count(QRegularExpression("(^|\\.\\s+)([a-z])"));

    // Проверка на повторяющиеся пробелы
    errors += text.count(QRegularExpression("\\s{2,}"));

    // Проверка пунктуации (отсутствие пробела после знака препинания)
    errors += text.count(QRegularExpression("[.,!?][a-zA-Z]"));

    // Подсчёт орфографических ошибок
    QRegularExpression wordRegex("\\b[a-zA-Z']+\\b");
    QRegularExpressionMatchIterator i = wordRegex.globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString word = match.captured().toLower();
        if (!dictionary.contains(word)) {
            errors++;
        }
    }

    return errors;
}

void WritingTest::saveResults() {
    QString text = textEdit->toPlainText();
    QStringList words = text.split(QRegularExpression("[\\s\\n]+"), Qt::SkipEmptyParts);
    int wordCount = words.size();

    if (wordCount < 120) {
        QMessageBox::warning(this, "Ошибка", "Нельзя сохранить: меньше 120 слов!");
        return;
    }

    int errorCount = countErrors(text);
    int score = calculateScore(wordCount, errorCount);

    QFile file("writing_score.txt");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << score;
    }

    QMessageBox::information(this, "Результаты сохранены",
                             QString("Ваша оценка: %1/10").arg(score));
}

void WritingTest::returnToMainMenu() {
    if (MainWindow *mw = qobject_cast<MainWindow*>(parent())) {
        mw->returnToMainMenu();
    }
}
