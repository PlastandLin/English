#ifndef WRITINGTEST_H
#define WRITINGTEST_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QSet>
#include <QStringList>

class WritingTest : public QWidget {
    Q_OBJECT

public:
    explicit WritingTest(QWidget *parent = nullptr);

private slots:
    void checkText();
    void saveResults();
    void returnToMainMenu();

private:
    QTextEdit *textEdit;
    QPushButton *backButton;
    QSet<QString> dictionary;

    void loadDictionary();
    void highlightErrors(const QString &text);
    int countErrors(const QString &text);
    int calculateScore(int wordCount, int errorCount);
};
#endif
