
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QMap>

int readScore(const QString &filename) {
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int score;
        in >> score;
        return score;
    }
    return 0;
}

QString levelFromAverage(double avg) {
    if (avg >= 9) return "C2 - Mastery";
    if (avg >= 8) return "C1 - Advanced";
    if (avg >= 7) return "B2 - Upper-Intermediate";
    if (avg >= 5.5) return "B1 - Intermediate";
    if (avg >= 4) return "A2 - Elementary";
    return "A1 - Beginner";
}

void showFinalResult() {
    int grammar = readScore("grammarscore.txt");
    int speaking = readScore("result.txt");
    int writing = readScore("writingscore.txt");
    int listening = readScore("listeningscore.txt");

    double average = (grammar + speaking + writing + listening) / 4.0;
    QString level = levelFromAverage(average);

    QMessageBox::information(nullptr, "Final Result",
                             QString("Grammar: %1/10\nSpeaking: %2/10\nWriting: %3/10\nListening: %4/10\n\nAverage: %5\nLevel: %6")
                             .arg(grammar).arg(speaking).arg(writing).arg(listening)
                             .arg(QString::number(average, 'f', 2)).arg(level));
}
