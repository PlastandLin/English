#include "speakingtest.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QDateTime>
#include <QDir>
#include <QProcess>
#include <QDebug>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QMediaFormat>
#include <QFileInfo>
#include <QStandardPaths>
#include "mainwindow.h"

SpeakingTest::SpeakingTest(QWidget *parent) : QWidget(parent) {
    mediaRecorder = new QMediaRecorder(this);
    captureSession = new QMediaCaptureSession(this);
    audioInput = new QAudioInput(this);
    player = new QMediaPlayer(this);

    referenceText = "Today I want to talk about my favorite hobby which is playing the guitar. "
                    "I started learning about five years ago, and it has become a very important part of my life. "
                    "I find it incredibly relaxing and a great way to express myself. "
                    "Learning to play the guitar was challenging at first, but with practice, I gradually improved. "
                    "Now, I can play many of my favorite songs, and I even write my own music sometimes. "
                    "I think everyone should have a hobby that they are passionate about. "
                    "It helps to relieve stress, develop new skills, and connect with other people who share similar interests. "
                    "For me, playing the guitar is more than just a hobby; it's a way to connect with my emotions and express my creativity.";

    setupUI();
    initializeAudio();
    checkDependencies();
}

SpeakingTest::~SpeakingTest() {
    // Останавливаем запись, если она активна
    if (mediaRecorder && mediaRecorder->recorderState() != QMediaRecorder::StoppedState) {
        mediaRecorder->stop();
    }

    // Освобождаем ресурсы
    delete mediaRecorder;
    delete captureSession;
    delete audioInput;
    delete player;
}
void SpeakingTest::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Speaking Test");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *instructionLabel = new QLabel(
        "Прочитайте вслух следующий текст на английском:\n\n" +
        referenceText);
    instructionLabel->setWordWrap(true);

    recordButton = new QPushButton("Начать запись");
    stopButton = new QPushButton("Остановить и оценить");
    backButton = new QPushButton("Вернуться в меню");

    stopButton->setEnabled(false);

    layout->addWidget(titleLabel);
    layout->addWidget(instructionLabel);
    layout->addWidget(recordButton);
    layout->addWidget(stopButton);
    layout->addWidget(backButton);
    layout->addStretch();

    connect(recordButton, &QPushButton::clicked, this, &SpeakingTest::startRecording);
    connect(stopButton, &QPushButton::clicked, this, &SpeakingTest::stopRecording);
    connect(backButton, &QPushButton::clicked, this, &SpeakingTest::returnToMainMenu);
    connect(mediaRecorder, &QMediaRecorder::errorOccurred, this, &SpeakingTest::handleRecorderError);
}

void SpeakingTest::initializeAudio() {
    const QAudioDevice defaultDevice = QMediaDevices::defaultAudioInput();
    if (defaultDevice.isNull()) {
        QMessageBox::critical(this, "Ошибка", "Не найдено аудиоустройство для записи");
        return;
    }

    audioInput->setDevice(defaultDevice);
    captureSession->setAudioInput(audioInput);
    captureSession->setRecorder(mediaRecorder);

    QMediaFormat format;
    format.setFileFormat(QMediaFormat::FileFormat::Wave);
    format.setAudioCodec(QMediaFormat::AudioCodec::Wave);
    mediaRecorder->setMediaFormat(format);
    mediaRecorder->setAudioSampleRate(16000);  // Vosk требует 16kHz
    mediaRecorder->setAudioBitRate(128000);
    mediaRecorder->setAudioChannelCount(1);  // Vosk требует моно запись (1 канал)
}

void SpeakingTest::checkDependencies() {
    QString modelDir = QCoreApplication::applicationDirPath() + "/vosk-model-small-en-us-0.15";
    if (!QDir(modelDir).exists()) {
        QMessageBox::warning(this, "Внимание",
                             "Модель Vosk не найдена. Поместите папку 'vosk-model-small-en-us-0.15' "
                             "в директорию с приложением.");
    }
}

void SpeakingTest::startRecording() {
    QString audioDir = QCoreApplication::applicationDirPath();
    audioDir = audioDir.left(audioDir.indexOf("/build")) + "/speaking/audio";
    QDir().mkpath(audioDir);

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    recordingPath = audioDir + "/speaking_" + timestamp + ".wav";

    mediaRecorder->setOutputLocation(QUrl::fromLocalFile(recordingPath));

    if (mediaRecorder->recorderState() == QMediaRecorder::StoppedState) {
        mediaRecorder->record();
        recordButton->setEnabled(false);
        stopButton->setEnabled(true);
        QMessageBox::information(this, "Запись", "Запись началась. Говорите сейчас!");
    }
}

void SpeakingTest::stopRecording() {
    if (mediaRecorder->recorderState() == QMediaRecorder::RecordingState) {
        mediaRecorder->stop();
        recordButton->setEnabled(true);
        stopButton->setEnabled(false);
        evaluateRecording();
    }
}

PronunciationResult SpeakingTest::analyzeWithVosk(const QString &audioPath) {
    PronunciationResult result;
    // Подготовка Python скрипта для Vosk
    QString script = R"(
import sys
import os
import json
from vosk import Model, KaldiRecognizer

def transcribe_audio(audio_path, model_path):
    if not os.path.exists(model_path):
        print("Model not found at: " + model_path)
        return ""

    model = Model(model_path)
    wf = wave.open(audio_path, "rb")
    rec = KaldiRecognizer(model, wf.getframerate())
    rec.SetWords(True)

    results = []
    while True:
        data = wf.readframes(4000)
        if len(data) == 0:
            break
        if rec.AcceptWaveform(data):
            results.append(json.loads(rec.Result())["text"])

    results.append(json.loads(rec.FinalResult())["text"])
    return " ".join(results)

if __name__ == "__main__":
    import wave
    audio_path = sys.argv[1]
    model_path = sys.argv[2]
    print(transcribe_audio(audio_path, model_path))
)";

    // Сохраняем скрипт во временный файл
    QString scriptPath = QDir::tempPath() + "/vosk_recognizer.py";
    QFile scriptFile(scriptPath);
    if (!scriptFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to create temp script file";
        return result;
    }
    scriptFile.write(script.toUtf8());
    scriptFile.close();

    // Запускаем Python скрипт
    QProcess python;
    QString modelPath = QCoreApplication::applicationDirPath() + "/vosk-model-small-en-us-0.15";
    python.start("python", {scriptPath, audioPath, modelPath});

    if (!python.waitForFinished(10000)) {
        qDebug() << "Vosk recognition failed:" << python.errorString();
        return result;
    }

    QString recognizedText = QString::fromUtf8(python.readAllStandardOutput()).trimmed();
    qDebug() << "Recognized text:" << recognizedText;
    result.recognizedText = recognizedText;
    // Анализ длительности
    player->setSource(QUrl::fromLocalFile(audioPath));
    QEventLoop loop;
    connect(player, &QMediaPlayer::mediaStatusChanged, &loop, &QEventLoop::quit);
    loop.exec();
    result.duration = player->duration() / 1000.0;

    // Сравнение с оригинальным текстом
    QStringList refWords = referenceText.split(' ', Qt::SkipEmptyParts);
    QStringList recWords = recognizedText.split(' ', Qt::SkipEmptyParts);

    int correctWords = 0;
    for (const QString &word : refWords) {
        if (recWords.contains(word, Qt::CaseInsensitive)) {
            correctWords++;
        } else {
            result.incorrectWords.append(word);
        }
    }

    result.accuracy = (correctWords * 100.0) / refWords.size();
    result.score = qMin(10, static_cast<int>(result.accuracy / 10.0));

    // Коррекция оценки на основе длительности
    if (result.duration < 5) {
        result.score = qMax(1, result.score - 2);
    } else if (result.duration > 10) {
        result.score = qMin(10, result.score + 1);
    }

    return result;
}

void SpeakingTest::evaluateRecording() {
    QFileInfo fileInfo(recordingPath);
    if (!fileInfo.exists() || fileInfo.size() < 1024) {
        QMessageBox::warning(this, "Ошибка", "Запись не удалась. Размер файла слишком маленький.");
        return;
    }

    PronunciationResult result = analyzeWithVosk(recordingPath);

    // Сохраняем результат
    QFile file("speaking_score.txt");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << result.score;
        file.close();
    }

    // Показываем результаты
    QString message = QString(
                          "<h3>Результат оценки произношения</h3>"
                          "<p><b>Общий балл:</b> %1/10</p>"
                          "<p><b>Точность:</b> %2%</p>"
                          "<p><b>Длина записи:</b> %3 сек</p>"
                          "<p><b>Проблемные слова:</b> %4</p>"
                          "<p><b>Распознанный текст:</b><br>%5</p>")
                          .arg(result.score)
                          .arg(result.accuracy, 0, 'f', 1)
                          .arg(result.duration, 0, 'f', 1)
                          .arg(result.incorrectWords.join(", "))
                          .arg(result.recognizedText);

    QMessageBox::information(this, "Результат", message);
}

void SpeakingTest::handleRecorderError(QMediaRecorder::Error error, const QString &errorString) {
    Q_UNUSED(error)
    QMessageBox::critical(this, "Ошибка записи", errorString);
    recordButton->setEnabled(true);
    stopButton->setEnabled(false);
}

void SpeakingTest::returnToMainMenu() {
    if (mediaRecorder->recorderState() != QMediaRecorder::StoppedState) {
        mediaRecorder->stop();
    }

    if (MainWindow *mw = qobject_cast<MainWindow*>(parent())) {
        mw->returnToMainMenu();
    }
}
