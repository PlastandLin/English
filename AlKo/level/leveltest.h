#ifndef LEVELTEST_H
#define LEVELTEST_H

#include <QString>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>

class LevelTest {
public:
    static void showFinalResult();

private:
    static QString getLevelDescription(const QString &level);
    static QString getVocabularySize(const QString &level);
    static int loadScore(const QString &filename);
};

#endif // LEVELTEST_H
