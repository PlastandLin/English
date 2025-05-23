#include "leveltest.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPixmap>
#include <QApplication>
#include <QScreen>

void LevelTest::showFinalResult() {
    // Загрузка результатов
    int grammar = loadScore("grammar_score.txt");
    int writing = loadScore("writing_score.txt");
    int speaking = loadScore("speaking_score.txt");
    int listening = loadScore("listening_score.txt");

    int total = grammar + writing + speaking + listening;
    int maxTotal = 40;
    QString level;

    // Определение уровня
    if (total <= 10) level = "A1";
    else if (total <= 20) level = "A2";
    else if (total <= 30) level = "B1";
    else if (total <= 35) level = "B2";
    else if (total <= 38) level = "C1";
    else level = "C2";

    // Формирование текста результатов
    QString message = QString(
                          "<h2 style='font-size: 20pt;'>Ваш уровень английского: <span style='color: #2c7be5;'>%1</span></h2>"
                          "<table style='font-size: 14pt; margin: 10px;'>"
                          "<tr><td><b>Грамматика:</b></td><td>%2/10</td></tr>"
                          "<tr><td><b>Письмо:</b></td><td>%3/10</td></tr>"
                          "<tr><td><b>Говорение:</b></td><td>%4/10</td></tr>"
                          "<tr><td><b>Аудирование:</b></td><td>%5/10</td></tr>"
                          "<tr><td><b>Общий счет:</b></td><td>%6/%7</td></tr>"
                          "</table>"
                          ).arg(level)
                          .arg(grammar).arg(writing).arg(speaking).arg(listening)
                          .arg(total).arg(maxTotal);

    QString descriptions = QString(
                               "<div style='font-size: 14pt; margin: 10px;'>"
                               "<h3 style='color: #2c7be5;'>Описание уровня %1:</h3>"
                               "<p>%2</p>"
                               "</div>"
                               ).arg(level).arg(getLevelDescription(level));

    // Создание диалогового окна
    QDialog resultDialog;
    resultDialog.setWindowTitle("Результаты тестирования");

    // Настройка размера окна (60% от экрана)
    QSize screenSize = QApplication::primaryScreen()->size();
    resultDialog.resize(screenSize.width() * 0.6, screenSize.height() * 0.7);

    QVBoxLayout *mainLayout = new QVBoxLayout(&resultDialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Изображение сверху
    QLabel *imageLabel = new QLabel();
    QPixmap pixmap("level/images/photo.jpg");
    if (!pixmap.isNull()) {
        pixmap = pixmap.scaled(900, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(pixmap);
        imageLabel->setAlignment(Qt::AlignCenter);
    } else {
        imageLabel->setText("<div style='color: red; font-size: 14pt;'>Изображение не загружено</div>");
        imageLabel->setAlignment(Qt::AlignCenter);
    }
    mainLayout->addWidget(imageLabel);

    // Текст результатов
    QLabel *resultLabel = new QLabel(message);
    resultLabel->setTextFormat(Qt::RichText);
    resultLabel->setWordWrap(true);
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setStyleSheet("QLabel { margin: 15px; }");
    mainLayout->addWidget(resultLabel);

    // Описание уровня
    QLabel *descLabel = new QLabel(descriptions);
    descLabel->setTextFormat(Qt::RichText);
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignLeft);
    descLabel->setStyleSheet("QLabel { "
                             "background: #f8f9fa; "
                             "padding: 15px; "
                             "border-radius: 5px; "
                             "border: 1px solid #dee2e6; "
                             "}");
    mainLayout->addWidget(descLabel);

    // Кнопка закрытия
    QPushButton *closeButton = new QPushButton("Закрыть");
    closeButton->setStyleSheet(
        "QPushButton {"
        "font-size: 14pt;"
        "padding: 10px 25px;"
        "background: #2c7be5;"
        "color: white;"
        "border: none;"
        "border-radius: 5px;"
        "}"
        "QPushButton:hover { background: #1a68d1; }"
        );
    mainLayout->addWidget(closeButton, 0, Qt::AlignRight);

    QObject::connect(closeButton, &QPushButton::clicked, &resultDialog, &QDialog::accept);

    resultDialog.exec();
}

QString LevelTest::getLevelDescription(const QString &level) {
    static QMap<QString, QString> descriptions = {
        {"A1", "Beginner — начальный. Понимает слова и фразы в простых ситуациях (приветствие, прощание). Может представиться, рассказать о своих интересах. Читает простые тексты: вывески, инструкции. Пишет короткие сообщения."},
        {"A2", "Pre Intermediate — ниже среднего. Может участвовать в коротких беседах на повседневные темы. Читает простые тексты и письма. Понимает основную повседневную речь. Пишет простые письма и записки. Может использовать основные времена и конструкции, но с ошибками."},
        {"B1", "Intermediate — средний. Люди с этим уровнем способны справляться с большинством ситуаций, возникающих при путешествии в англоязычных странах. Они могут писать короткие тексты, обсуждать знакомые темы и высказывать своё мнение. Основные характеристики уровня B1: понимание основных идей текста на знакомую тематику, способность общаться в типичных ситуациях и выражать свои мысли на разные темы, умение писать простые сочинения и письма. "},
        {"B2", "Upper Intermediate — выше среднего. Этот уровень характеризует уверенное владение английским. Люди с уровнем B2 могут понимать более сложные тексты и общаться на профессиональные темы. Они способны вести беседу с носителями языка на разные темы, не испытывая значительных трудностей. Основные характеристики уровня B2: способность понимать сложные тексты, как на профессиональные, так и на бытовые темы, умение вести дискуссию и защищать свою точку зрения, владение английским для использования в учебной или профессиональной среде. "},
        {"C1", "Advanced — продвинутый. Люди с этим уровнем могут читать сложные тексты, вести беседы на незнакомые темы и писать детализированные эссе. Основные характеристики уровня C1: понимание длинных текстов и сложных рассуждений, умение выражать свои мысли спонтанно и без труда, возможность вести профессиональную переписку и писать сложные тексты. "},
        {"C2", "Proficiency — совершенный. Это самый высокий уровень владения английским языком. Люди с этим уровнем могут понимать любой письменный или устный текст, включая академические работы и сложные технические документы. Они могут спонтанно и уверенно вести дискуссии, используя сложную лексику."}
    };
    return descriptions.value(level, "Неизвестный уровень");
}

int LevelTest::loadScore(const QString &filename) {
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        return in.readLine().toInt();
    }
    return 0;
}
