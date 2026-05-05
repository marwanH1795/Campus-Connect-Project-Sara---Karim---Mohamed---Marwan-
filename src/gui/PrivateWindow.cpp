#include "PrivateWindow.h"
#include "MessageRenderer.h"
#include "../logic/ChatController.h"
#include "ui_PrivateWindow.h"

#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <QMediaCaptureSession>
#include <QAudioInput>
#include <QMediaRecorder>
#include <QMediaFormat>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QUrl>
#include <QFileInfo>
#include <QFile>
#include <QMouseEvent>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QByteArray>

PrivateWindow::PrivateWindow(std::shared_ptr<ChatController> controller,
                             const QString& targetUser,
                             QWidget* parent)
    : QDialog(parent),
      ui(new Ui::PrivateWindow),
      controller(controller),
      targetUser(targetUser),
      refreshTimer(new QTimer(this)),
      recordingTimer(new QTimer(this)),
      recordingLabel(nullptr),
      voiceButton(nullptr),
      deleteVoiceButton(nullptr),
      audioSession(nullptr),
      audioInput(nullptr),
      audioRecorder(nullptr),
      voicePlayer(nullptr),
      voiceOutput(nullptr),
      isRecording(false),
      hasPendingVoice(false),
      recordingSeconds(0)
{
    ui->setupUi(this);
    setWindowTitle("Private Chat - " + this->targetUser);

    setupUiStyle();
    setupVoiceRecorder();
    setupVoicePlayer();

    ui->privateDisplay->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->privateDisplay->viewport()->installEventFilter(this);

    connect(ui->sendButton, &QPushButton::clicked,
            this, [this]() { onSendClicked(); });

    connect(voiceButton, &QPushButton::clicked,
            this, [this]() { onVoiceClicked(); });

    connect(deleteVoiceButton, &QPushButton::clicked,
            this, [this]() { onDeleteVoiceClicked(); });

    connect(ui->messageInput, &QLineEdit::returnPressed,
            this, [this]() { onSendClicked(); });

    connect(ui->messageInput, &QLineEdit::textChanged,
            this, [this](const QString& text) {
                this->controller->sendPrivateTypingStatus(
                    this->targetUser.toStdString(),
                    !text.trimmed().isEmpty()
                );
            });

    connect(recordingTimer, &QTimer::timeout,
            this, [this]() { updateRecordingTimer(); });

    connect(refreshTimer, &QTimer::timeout,
            this, [this]() { refreshMessages(); });

    refreshMessages();
    refreshTimer->start(250);
}

PrivateWindow::~PrivateWindow() {
    delete ui;
}

bool PrivateWindow::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->privateDisplay->viewport() &&
        event->type() == QEvent::MouseButtonRelease) {

        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->button() == Qt::LeftButton) {
            QString link = ui->privateDisplay->anchorAt(mouseEvent->pos());

            if (link.isEmpty()) {
                QTextCursor cursor = ui->privateDisplay->cursorForPosition(mouseEvent->pos());
                QTextCharFormat format = cursor.charFormat();

                if (format.isAnchor()) {
                    link = format.anchorHref();
                }
            }

            if (link.startsWith("voice:")) {
                QString encodedPath = link.mid(QString("voice:").length());
                QString filePath = QString::fromUtf8(
                    QByteArray::fromBase64(encodedPath.toLatin1())
                );

                playVoiceFile(filePath);
                return true;
            }
        }
    }

    return QDialog::eventFilter(watched, event);
}

void PrivateWindow::setupUiStyle() {
    recordingLabel = new QLabel("00:00", this);
    recordingLabel->hide();

    voiceButton = new QPushButton("🎤", this);
    voiceButton->setMinimumSize(46, 46);
    voiceButton->setMaximumSize(46, 46);
    voiceButton->setToolTip("Record voice message");

    deleteVoiceButton = new QPushButton("🗑", this);
    deleteVoiceButton->setMinimumSize(46, 46);
    deleteVoiceButton->setMaximumSize(46, 46);
    deleteVoiceButton->setToolTip("Delete recorded voice");
    deleteVoiceButton->hide();

    ui->inputLayout->insertWidget(1, recordingLabel);
    ui->inputLayout->insertWidget(2, voiceButton);
    ui->inputLayout->insertWidget(3, deleteVoiceButton);

    QString roundButtonStyle =
        "QPushButton {"
        "background-color:#2a2b3d;"
        "color:#ffffff;"
        "border:2px solid #4a4b6a;"
        "border-radius:23px;"
        "font-size:18px;"
        "}"
        "QPushButton:hover {"
        "background-color:#3a3b55;"
        "border:2px solid #7c6af7;"
        "}"
        "QPushButton:pressed {"
        "background-color:#7c6af7;"
        "}";

    voiceButton->setStyleSheet(roundButtonStyle);
    deleteVoiceButton->setStyleSheet(roundButtonStyle);

    recordingLabel->setStyleSheet(
        "QLabel {"
        "color:#ffcc66;"
        "font-weight:bold;"
        "font-size:13px;"
        "padding:0 8px;"
        "}"
    );

    ui->messageInput->setStyleSheet(
        "QLineEdit {"
        "background-color:#2a2b3d;"
        "color:#e0e0f0;"
        "border:2px solid #4a4b6a;"
        "border-radius:22px;"
        "padding:0px 16px;"
        "font-size:13px;"
        "}"
        "QLineEdit:focus {"
        "border:2px solid #7c6af7;"
        "}"
    );

    ui->sendButton->setStyleSheet(
        "QPushButton {"
        "background-color:#3ecf8e;"
        "color:#101426;"
        "border:none;"
        "border-radius:23px;"
        "font-size:13px;"
        "font-weight:bold;"
        "}"
        "QPushButton:hover {"
        "background-color:#55e0a3;"
        "}"
        "QPushButton:pressed {"
        "background-color:#28a570;"
        "}"
    );
}

void PrivateWindow::setupVoiceRecorder() {
    audioSession = new QMediaCaptureSession(this);
    audioInput = new QAudioInput(this);
    audioRecorder = new QMediaRecorder(this);

    audioSession->setAudioInput(audioInput);
    audioSession->setRecorder(audioRecorder);

    QMediaFormat format;
    format.setFileFormat(QMediaFormat::Wave);
    format.setAudioCodec(QMediaFormat::AudioCodec::Wave);
    audioRecorder->setMediaFormat(format);
    audioRecorder->setQuality(QMediaRecorder::NormalQuality);

    connect(audioRecorder, &QMediaRecorder::recorderStateChanged,
            this, [this](QMediaRecorder::RecorderState state) {
                if (state == QMediaRecorder::StoppedState) {
                    preparePendingVoice();
                }
            });

    connect(audioRecorder, &QMediaRecorder::errorOccurred,
            this, [this](QMediaRecorder::Error, const QString& errorString) {
                isRecording = false;
                hasPendingVoice = false;
                recordingTimer->stop();

                recordingLabel->hide();
                deleteVoiceButton->hide();
                voiceButton->setText("🎤");

                ui->messageInput->setPlaceholderText("Voice failed: " + errorString);
            });
}

void PrivateWindow::setupVoicePlayer() {
    voicePlayer = new QMediaPlayer(this);
    voiceOutput = new QAudioOutput(this);

    voicePlayer->setAudioOutput(voiceOutput);
    voiceOutput->setVolume(1.0);

    connect(voicePlayer, &QMediaPlayer::errorOccurred,
            this, [this](QMediaPlayer::Error, const QString& errorString) {
                ui->messageInput->setPlaceholderText("Playback failed: " + errorString);
            });
}

void PrivateWindow::onSendClicked() {
    if (hasPendingVoice) {
        if (sendPendingVoiceMessage()) {
            pendingVoiceFilePath.clear();
            hasPendingVoice = false;
            recordingSeconds = 0;

            recordingLabel->setText("00:00");
            recordingLabel->hide();
            deleteVoiceButton->hide();
            voiceButton->setText("🎤");
            ui->messageInput->setPlaceholderText("Type a message...");
        }

        return;
    }

    QString text = ui->messageInput->text().trimmed();

    if (text.isEmpty()) {
        return;
    }

    if (sendTextMessage(text)) {
        ui->messageInput->clear();
        ui->messageInput->setFocus();
    }
}

void PrivateWindow::onVoiceClicked() {
    if (isRecording) {
        stopVoiceRecording();
    } else {
        startVoiceRecording();
    }
}

void PrivateWindow::onDeleteVoiceClicked() {
    clearPendingVoice();
}

void PrivateWindow::startVoiceRecording() {
    clearPendingVoice();

    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);

    if (baseDir.isEmpty()) {
        baseDir = QDir::homePath();
    }

    QDir dir(baseDir + "/CampusConnectVoice");

    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString fileName =
        "private_voice_" +
        QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") +
        ".wav";

    pendingVoiceFilePath = dir.absoluteFilePath(fileName);

    audioRecorder->setOutputLocation(QUrl::fromLocalFile(pendingVoiceFilePath));
    audioRecorder->record();

    isRecording = true;
    hasPendingVoice = false;
    recordingSeconds = 0;

    recordingLabel->setText("00:00");
    recordingLabel->show();
    deleteVoiceButton->hide();

    recordingTimer->start(1000);

    voiceButton->setText("⏹");
    ui->messageInput->setPlaceholderText("Recording voice message...");
}

void PrivateWindow::stopVoiceRecording() {
    if (!isRecording) {
        return;
    }

    isRecording = false;
    recordingTimer->stop();
    audioRecorder->stop();

    voiceButton->setText("🎤");
    ui->messageInput->setPlaceholderText("Voice ready. Click Send or Delete.");
}

void PrivateWindow::updateRecordingTimer() {
    recordingSeconds++;

    int minutes = recordingSeconds / 60;
    int seconds = recordingSeconds % 60;

    recordingLabel->setText(
        QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'))
    );
}

void PrivateWindow::preparePendingVoice() {
    if (pendingVoiceFilePath.isEmpty()) {
        return;
    }

    QFileInfo info(pendingVoiceFilePath);

    if (!info.exists() || info.size() == 0) {
        ui->messageInput->setPlaceholderText("Voice file was not recorded.");
        return;
    }

    hasPendingVoice = true;
    deleteVoiceButton->show();
    recordingLabel->show();
    ui->messageInput->setPlaceholderText("Voice ready. Click Send or Delete.");
}

void PrivateWindow::clearPendingVoice() {
    if (isRecording) {
        audioRecorder->stop();
        recordingTimer->stop();
        isRecording = false;
    }

    if (!pendingVoiceFilePath.isEmpty()) {
        QFile::remove(pendingVoiceFilePath);
    }

    pendingVoiceFilePath.clear();
    hasPendingVoice = false;
    recordingSeconds = 0;

    recordingLabel->setText("00:00");
    recordingLabel->hide();
    deleteVoiceButton->hide();
    voiceButton->setText("🎤");
    ui->messageInput->setPlaceholderText("Type a message...");
}

bool PrivateWindow::sendTextMessage(const QString& text) {
    bool sent = controller->sendPrivateMessage(
        targetUser.toStdString(),
        text.toStdString()
    );

    if (sent) {
        controller->sendPrivateTypingStatus(targetUser.toStdString(), false);
        refreshMessages();
    }

    return sent;
}

bool PrivateWindow::sendPendingVoiceMessage() {
    QFileInfo info(pendingVoiceFilePath);

    if (!info.exists() || info.size() == 0) {
        ui->messageInput->setPlaceholderText("Voice file not found or empty");
        return false;
    }

    bool sent = controller->sendPrivateAttachment(
        targetUser.toStdString(),
        pendingVoiceFilePath,
        "audio/wav"
    );

    if (sent) {
        controller->sendPrivateTypingStatus(targetUser.toStdString(), false);
        refreshMessages();
    } else {
        ui->messageInput->setPlaceholderText("Failed to send voice message");
    }

    return sent;
}

void PrivateWindow::playVoiceFile(const QString& filePath) {
    QFileInfo info(filePath);

    if (!info.exists()) {
        ui->messageInput->setPlaceholderText("Voice file not found");
        return;
    }

    voicePlayer->stop();
    voicePlayer->setSource(QUrl::fromLocalFile(filePath));
    voicePlayer->play();

    ui->messageInput->setPlaceholderText("Playing voice message...");
}

void PrivateWindow::refreshMessages() {
    auto messages = controller->getState()
                        .getPrivateMessagesWithUser(targetUser.toStdString());

    QString me =
        QString::fromStdString(controller->getCurrentUsername()).trimmed().toLower();

    QScrollBar* scrollBar = ui->privateDisplay->verticalScrollBar();
    bool shouldAutoScroll = scrollBar->value() >= scrollBar->maximum() - 25;

    QString html;
    html += "<html><body style='background:#0f0f1a; color:white; font-family:Segoe UI, Arial; margin:16px;'>";

    for (const auto& msg : messages) {
        html += MessageRenderer::renderMessage(msg, me);
    }

    if (controller->getState().isPrivateUserTyping(targetUser.toStdString())) {
        html += MessageRenderer::renderTyping(targetUser + " is typing...");
    }

    html += "</body></html>";

    if (html == lastRenderedHtml) {
        return;
    }

    lastRenderedHtml = html;
    ui->privateDisplay->setHtml(html);

    if (shouldAutoScroll) {
        ui->privateDisplay->verticalScrollBar()->setValue(
            ui->privateDisplay->verticalScrollBar()->maximum()
        );
    }
}
