#ifndef SPEAKINGTEST_H
#define SPEAKINGTEST_H

#include <QWidget>
#include <QMediaRecorder>
#include <QAudioInput>
#include <QMediaCaptureSession>
#include <QMediaFormat>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QFile>
#include <QPushButton>
#include <QDebug>
#include <QCoreApplication>
#include <QProcess>
#include <QDir>
#include <QMediaPlayer>

    struct PronunciationResult {
    int score = 0;
    double accuracy = 0;
    double duration = 0;
    QStringList incorrectWords;
    QString recognizedText;
};

class SpeakingTest : public QWidget {
    Q_OBJECT

public:
    explicit SpeakingTest(QWidget *parent = nullptr);
    ~SpeakingTest() override;  // Добавлен override

private slots:
    void startRecording();
    void stopRecording();
    void evaluateRecording();
    void returnToMainMenu();
    void handleRecorderError(QMediaRecorder::Error error, const QString &errorString);

private:
    QMediaRecorder *mediaRecorder;
    QMediaCaptureSession *captureSession;
    QAudioInput *audioInput;
    QMediaPlayer *player;
    QPushButton *recordButton;
    QPushButton *stopButton;
    QPushButton *backButton;
    QString recordingPath;
    QString referenceText;

    void setupUI();
    void initializeAudio();
    void checkDependencies();
    PronunciationResult analyzeWithVosk(const QString &audioPath);
};

#endif
