#include "PrivateWindow.h"
#include "MessageRenderer.h"
#include "../logic/ChatController.h"
#include "../logic/Message.h"
#include "ui_PrivateWindow.h"

#include <QTimer>
#include <QScrollBar>
#include <QTime>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDesktopServices>
#include <QDialog>
#include <QMessageBox>
#include <QApplication>

#include <QCamera>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QVideoWidget>
#include <QImageCapture>

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

PrivateWindow::PrivateWindow(
    std::shared_ptr<ChatController> controller,
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
      attachmentButton(nullptr),
      cameraButton(nullptr),
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

    setWindowTitle("Private Chat - " + targetUser);

    rebuildLayout();
    setupVoiceRecorder();
    setupVoicePlayer();

    ui->privateDisplay->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->privateDisplay->viewport()->installEventFilter(this);

    connect(ui->sendButton, &QPushButton::clicked,
            this, &PrivateWindow::onSendClicked);

    connect(ui->messageInput, &QLineEdit::returnPressed,
            this, &PrivateWindow::onSendClicked);

    connect(voiceButton, &QPushButton::clicked,
            this, &PrivateWindow::onVoiceClicked);

    connect(deleteVoiceButton, &QPushButton::clicked,
            this, &PrivateWindow::onDeleteVoiceClicked);

    connect(attachmentButton, &QPushButton::clicked,
            this, &PrivateWindow::onAttachmentClicked);

    connect(cameraButton, &QPushButton::clicked,
            this, &PrivateWindow::onCameraClicked);

    connect(recordingTimer, &QTimer::timeout,
            this, &PrivateWindow::updateRecordingTimer);

    connect(refreshTimer, &QTimer::timeout,
            this, &PrivateWindow::refreshMessages);

    refreshMessages();
    refreshTimer->start(250);
}

PrivateWindow::~PrivateWindow()
{
    delete ui;
}

bool PrivateWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->privateDisplay->viewport() &&
        event->type() == QEvent::MouseButtonRelease) {

        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->button() == Qt::LeftButton) {
            QString link = ui->privateDisplay->anchorAt(mouseEvent->pos());

            if (link.isEmpty()) {
                QTextCursor cursor =
                    ui->privateDisplay->cursorForPosition(mouseEvent->pos());

                QTextCharFormat format = cursor.charFormat();

                if (format.isAnchor()) {
                    link = format.anchorHref();
                }
            }

            if (link.startsWith("voice:")) {
                QString encodedPath =
                    link.mid(QString("voice:").length());

                QString filePath =
                    QString::fromUtf8(
                        QByteArray::fromBase64(encodedPath.toLatin1())
                    );

                playVoiceFile(filePath);
                return true;
            }

            if (link.startsWith("open:")) {
                QString encodedPath =
                    link.mid(QString("open:").length());

                QString filePath =
                    QString::fromUtf8(
                        QByteArray::fromBase64(encodedPath.toLatin1())
                    );

                openAttachmentFile(filePath);
                return true;
            }
        }
    }

    return QDialog::eventFilter(watched, event);
}

void PrivateWindow::rebuildLayout()
{
    setMinimumSize(900, 600);

    setStyleSheet(
        "QWidget {"
        "background-color:#1e1f2e;"
        "color:#e0e0f0;"
        "font-family:'Segoe UI';"
        "}"
    );

    QLayout* oldLayout = layout();

    if (oldLayout) {
        while (oldLayout->count()) {
            QLayoutItem* item = oldLayout->takeAt(0);
            delete item;
        }

        delete oldLayout;
    }

    QLayout* oldInputLayout = ui->inputBar->layout();

    if (oldInputLayout) {
        while (oldInputLayout->count()) {
            QLayoutItem* item = oldInputLayout->takeAt(0);
            delete item;
        }

        delete oldInputLayout;
    }

    QVBoxLayout* root = new QVBoxLayout(this);

    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(14);

    QLabel* title =
        new QLabel("Private Chat with " + targetUser, this);

    title->setStyleSheet(
        "font-size:18px;"
        "font-weight:bold;"
        "color:#7c6af7;"
    );

    ui->privateDisplay->setReadOnly(true);

    ui->privateDisplay->setStyleSheet(
        "QTextEdit {"
        "background-color:#16172a;"
        "border:2px solid #3a3b55;"
        "border-radius:12px;"
        "padding:10px;"
        "font-size:13px;"
        "}"
    );

    ui->inputBar->setMinimumHeight(66);

    ui->inputBar->setStyleSheet(
        "QFrame {"
        "background-color:#151525;"
        "border:2px solid #3a3b55;"
        "border-radius:22px;"
        "}"
    );

    QHBoxLayout* inputLayout =
        new QHBoxLayout(ui->inputBar);

    inputLayout->setContentsMargins(12, 8, 12, 8);
    inputLayout->setSpacing(10);

    ui->messageInput->setMinimumHeight(44);

    ui->messageInput->setStyleSheet(
        "QLineEdit {"
        "background-color:#2a2b3d;"
        "color:#e0e0f0;"
        "border:2px solid #4a4b6a;"
        "border-radius:22px;"
        "padding:0px 16px;"
        "font-size:13px;"
        "}"
    );

    recordingLabel = new QLabel("00:00", this);

    recordingLabel->hide();

    recordingLabel->setStyleSheet(
        "color:#ffcc66;"
        "font-weight:bold;"
    );

    QString roundButtonStyle =
        "QPushButton {"
        "background-color:#2a2b3d;"
        "border:2px solid #4a4b6a;"
        "border-radius:23px;"
        "font-size:18px;"
        "}"
        "QPushButton:hover {"
        "background-color:#3a3b55;"
        "border:2px solid #7c6af7;"
        "}";

    voiceButton = new QPushButton("🎤", this);
    deleteVoiceButton = new QPushButton("🗑", this);
    attachmentButton = new QPushButton("📎", this);
    cameraButton = new QPushButton("📷", this);

    voiceButton->setToolTip("Record voice message");
    deleteVoiceButton->setToolTip("Delete recorded voice");
    attachmentButton->setToolTip("Send image, video, or file");
    cameraButton->setToolTip("Take photo or record video");

    QList<QPushButton*> buttons = {
        voiceButton,
        deleteVoiceButton,
        attachmentButton,
        cameraButton
    };

    for (QPushButton* button : buttons) {
        button->setMinimumSize(46, 46);
        button->setMaximumSize(46, 46);
        button->setStyleSheet(roundButtonStyle);
    }

    deleteVoiceButton->hide();

    ui->sendButton->setMinimumSize(105, 46);

    ui->sendButton->setStyleSheet(
        "QPushButton {"
        "background-color:#3ecf8e;"
        "color:#101426;"
        "border:none;"
        "border-radius:23px;"
        "font-weight:bold;"
        "}"
    );

    inputLayout->addWidget(ui->messageInput, 1);
    inputLayout->addWidget(recordingLabel);
    inputLayout->addWidget(voiceButton);
    inputLayout->addWidget(deleteVoiceButton);
    inputLayout->addWidget(attachmentButton);
    inputLayout->addWidget(cameraButton);
    inputLayout->addWidget(ui->sendButton);

    root->addWidget(title);
    root->addWidget(ui->privateDisplay, 1);
    root->addWidget(ui->inputBar);
}

void PrivateWindow::setupVoiceRecorder()
{
    audioSession = new QMediaCaptureSession(this);

    audioInput = new QAudioInput(this);
    audioRecorder = new QMediaRecorder(this);

    audioSession->setAudioInput(audioInput);
    audioSession->setRecorder(audioRecorder);

    QMediaFormat format;

    format.setFileFormat(QMediaFormat::Wave);
    format.setAudioCodec(QMediaFormat::AudioCodec::Wave);

    audioRecorder->setMediaFormat(format);

    connect(audioRecorder,
            &QMediaRecorder::recorderStateChanged,
            this,
            [this](QMediaRecorder::RecorderState state) {

        if (state == QMediaRecorder::StoppedState) {
            preparePendingVoice();
        }
    });
}

void PrivateWindow::setupVoicePlayer()
{
    voicePlayer = new QMediaPlayer(this);

    voiceOutput = new QAudioOutput(this);

    voicePlayer->setAudioOutput(voiceOutput);
}

void PrivateWindow::onSendClicked()
{
    if (hasPendingVoice) {

        if (sendPendingVoiceMessage()) {

            pendingVoiceFilePath.clear();

            hasPendingVoice = false;

            recordingSeconds = 0;

            recordingLabel->hide();

            deleteVoiceButton->hide();

            voiceButton->setText("🎤");
        }

        return;
    }

    QString text =
        ui->messageInput->text().trimmed();

    if (text.isEmpty()) {
        return;
    }

    controller->sendPrivateMessage(
        targetUser.toStdString(),
        text.toStdString()
    );

    ui->messageInput->clear();

    refreshMessages();
}

void PrivateWindow::onVoiceClicked()
{
    if (isRecording) {
        stopVoiceRecording();
    } else {
        startVoiceRecording();
    }
}

void PrivateWindow::onDeleteVoiceClicked()
{
    clearPendingVoice();
}

void PrivateWindow::onAttachmentClicked()
{
    QString filePath =
        QFileDialog::getOpenFileName(
            this,
            "Choose File",
            QDir::homePath(),
            "All Files (*.*)"
        );

    if (filePath.isEmpty()) {
        return;
    }

    sendAttachmentFile(filePath);
}

void PrivateWindow::onCameraClicked()
{
    QCameraDevice cameraDevice =
        QMediaDevices::defaultVideoInput();

    if (cameraDevice.isNull()) {

        QMessageBox::warning(
            this,
            "Camera",
            "No camera device found."
        );

        return;
    }

    QDialog cameraDialog(this);

    cameraDialog.setWindowTitle("Camera Capture");

    cameraDialog.resize(900, 700);

    QVBoxLayout* root =
        new QVBoxLayout(&cameraDialog);

    QLabel* title =
        new QLabel("Camera Preview");

    title->setStyleSheet(
        "font-size:24px;"
        "font-weight:bold;"
        "color:#7c6af7;"
    );

    root->addWidget(title);

    QVideoWidget* videoWidget =
        new QVideoWidget();

    videoWidget->setMinimumHeight(500);

    root->addWidget(videoWidget);

    QLabel* timerLabel =
        new QLabel("Video time: 00:00");

    timerLabel->hide();

    QLabel* statusLabel =
        new QLabel("Ready.");

    root->addWidget(timerLabel);
    root->addWidget(statusLabel);

    QHBoxLayout* buttons =
        new QHBoxLayout();

    QPushButton* takePhotoButton =
        new QPushButton("📸 Take Photo");

    QPushButton* startVideoButton =
        new QPushButton("⏺ Start Video");

    QPushButton* stopVideoButton =
        new QPushButton("⏹ Stop Video");

    QPushButton* sendButton =
        new QPushButton("Send ✅");

    QPushButton* deleteButton =
        new QPushButton("Delete 🗑");

    QPushButton* closeButton =
        new QPushButton("Close");

    buttons->addWidget(takePhotoButton);
    buttons->addWidget(startVideoButton);
    buttons->addWidget(stopVideoButton);
    buttons->addWidget(sendButton);
    buttons->addWidget(deleteButton);
    buttons->addWidget(closeButton);

    root->addLayout(buttons);

    QMediaCaptureSession captureSession;

    QCamera camera(cameraDevice);

    QImageCapture imageCapture;

    QAudioInput videoAudioInput;

    QMediaRecorder videoRecorder;

    captureSession.setCamera(&camera);
    captureSession.setVideoOutput(videoWidget);
    captureSession.setImageCapture(&imageCapture);
    captureSession.setAudioInput(&videoAudioInput);
    captureSession.setRecorder(&videoRecorder);

    QMediaFormat videoFormat;

    videoFormat.setFileFormat(QMediaFormat::MPEG4);

    videoRecorder.setMediaFormat(videoFormat);

    QString capturedFilePath;
    QString pendingVideoFilePath;

    bool isVideoRecording = false;

    int videoSeconds = 0;

    QTimer videoTimer(&cameraDialog);

    auto updateTimer = [&]() {

        int minutes = videoSeconds / 60;
        int seconds = videoSeconds % 60;

        timerLabel->setText(
            QString("Video time: %1:%2")
                .arg(minutes, 2, 10, QChar('0'))
                .arg(seconds, 2, 10, QChar('0'))
        );
    };

    connect(&videoTimer,
            &QTimer::timeout,
            &cameraDialog,
            [&]() {

        videoSeconds++;

        updateTimer();
    });

    connect(takePhotoButton,
            &QPushButton::clicked,
            &cameraDialog,
            [&]() {

        QString output =
            createCameraOutputPath(
                "jpg",
                "PrivatePhotos"
            );

        QApplication::beep();

        imageCapture.captureToFile(output);

        capturedFilePath = output;

        sendButton->setEnabled(true);

        statusLabel->setText("Photo captured.");
    });

    connect(startVideoButton,
            &QPushButton::clicked,
            &cameraDialog,
            [&]() {

        pendingVideoFilePath =
            createCameraOutputPath(
                "mp4",
                "PrivateVideos"
            );

        videoRecorder.setOutputLocation(
            QUrl::fromLocalFile(
                pendingVideoFilePath
            )
        );

        QApplication::beep();

        isVideoRecording = true;

        videoSeconds = 0;

        updateTimer();

        timerLabel->show();

        videoTimer.start(1000);

        videoRecorder.record();

        statusLabel->setText("Recording video...");
    });

    connect(stopVideoButton,
            &QPushButton::clicked,
            &cameraDialog,
            [&]() {

        QApplication::beep();

        videoRecorder.stop();

        videoTimer.stop();

        timerLabel->hide();

        capturedFilePath =
            pendingVideoFilePath;

        isVideoRecording = false;

        sendButton->setEnabled(true);

        statusLabel->setText("Video recorded.");
    });

    connect(sendButton,
            &QPushButton::clicked,
            &cameraDialog,
            [&]() {

        if (capturedFilePath.isEmpty()) {
            return;
        }

        sendAttachmentFile(capturedFilePath);

        cameraDialog.accept();
    });

    connect(deleteButton,
            &QPushButton::clicked,
            &cameraDialog,
            [&]() {

        if (!capturedFilePath.isEmpty()) {
            QFile::remove(capturedFilePath);
        }

        capturedFilePath.clear();

        statusLabel->setText("Deleted.");
    });

    connect(closeButton,
            &QPushButton::clicked,
            &cameraDialog,
            &QDialog::reject);

    camera.start();

    cameraDialog.exec();

    if (isVideoRecording) {
        videoRecorder.stop();
    }

    videoTimer.stop();

    camera.stop();
}

void PrivateWindow::startVoiceRecording()
{
    clearPendingVoice();

    QString baseDir =
        QStandardPaths::writableLocation(
            QStandardPaths::MusicLocation
        );

    if (baseDir.isEmpty()) {
        baseDir = QDir::homePath();
    }

    QDir dir(baseDir + "/CampusConnectVoice");

    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString fileName =
        "private_voice_" +
        QDateTime::currentDateTime().toString(
            "yyyyMMdd_hhmmss"
        ) +
        ".wav";

    pendingVoiceFilePath =
        dir.absoluteFilePath(fileName);

    audioRecorder->setOutputLocation(
        QUrl::fromLocalFile(
            pendingVoiceFilePath
        )
    );

    audioRecorder->record();

    isRecording = true;

    hasPendingVoice = false;

    recordingSeconds = 0;

    recordingLabel->setText("00:00");

    recordingLabel->show();

    deleteVoiceButton->hide();

    recordingTimer->start(1000);

    voiceButton->setText("⏹");
}

void PrivateWindow::stopVoiceRecording()
{
    if (!isRecording) {
        return;
    }

    isRecording = false;

    recordingTimer->stop();

    audioRecorder->stop();

    voiceButton->setText("🎤");
}

void PrivateWindow::updateRecordingTimer()
{
    recordingSeconds++;

    int minutes = recordingSeconds / 60;
    int seconds = recordingSeconds % 60;

    recordingLabel->setText(
        QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'))
    );
}

void PrivateWindow::preparePendingVoice()
{
    QFileInfo info(pendingVoiceFilePath);

    if (!info.exists() || info.size() == 0) {
        return;
    }

    hasPendingVoice = true;

    deleteVoiceButton->show();
}

void PrivateWindow::clearPendingVoice()
{
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

    recordingLabel->hide();

    deleteVoiceButton->hide();

    voiceButton->setText("🎤");
}

bool PrivateWindow::sendPendingVoiceMessage()
{
    QFileInfo info(pendingVoiceFilePath);

    if (!info.exists() ||
        info.size() == 0) {

        return false;
    }

    bool sent =
        controller->sendPrivateAttachment(
            targetUser.toStdString(),
            pendingVoiceFilePath,
            "audio/wav"
        );

    if (sent) {
        refreshMessages();
    }

    return sent;
}

bool PrivateWindow::sendAttachmentFile(
    const QString& filePath)
{
    QFileInfo info(filePath);

    if (!info.exists() ||
        info.size() <= 0) {

        return false;
    }

    QMimeDatabase database;

    QMimeType mimeType =
        database.mimeTypeForFile(filePath);

    QString type =
        mimeType.isValid()
        ? mimeType.name()
        : "application/octet-stream";

    bool sent =
        controller->sendPrivateAttachment(
            targetUser.toStdString(),
            filePath,
            type
        );

    if (sent) {
        refreshMessages();
    }

    return sent;
}

QString PrivateWindow::createCameraOutputPath(
    const QString& extension,
    const QString& folderName) const
{
    QString baseDir =
        QStandardPaths::writableLocation(
            QStandardPaths::PicturesLocation
        );

    if (baseDir.isEmpty()) {
        baseDir = QDir::homePath();
    }

    QDir dir(
        baseDir +
        "/CampusConnect/" +
        folderName
    );

    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString fileName =
        "camera_" +
        QDateTime::currentDateTime().toString(
            "yyyyMMdd_hhmmss_zzz"
        ) +
        "." +
        extension;

    return dir.absoluteFilePath(fileName);
}

void PrivateWindow::playVoiceFile(
    const QString& filePath)
{
    QFileInfo info(filePath);

    if (!info.exists()) {
        return;
    }

    voicePlayer->stop();

    voicePlayer->setSource(
        QUrl::fromLocalFile(filePath)
    );

    voicePlayer->play();
}

void PrivateWindow::openAttachmentFile(
    const QString& filePath)
{
    QFileInfo info(filePath);

    if (!info.exists()) {
        return;
    }

    QDesktopServices::openUrl(
        QUrl::fromLocalFile(filePath)
    );
}

void PrivateWindow::refreshMessages()
{
    auto messages =
        controller->getState()
            .getPrivateMessagesWithUser(
                targetUser.toStdString()
            );

    QString me =
        QString::fromStdString(
            controller->getCurrentUsername()
        ).trimmed().toLower();

    QScrollBar* scrollBar =
        ui->privateDisplay->verticalScrollBar();

    bool autoScroll =
        scrollBar->value() >=
        scrollBar->maximum() - 25;

    QString html;

    html +=
        "<html><body style='"
        "background:#0f0f1a;"
        "color:white;"
        "font-family:Segoe UI, Arial;"
        "margin:16px;'>";

    for (const auto& msg : messages) {

        html +=
            MessageRenderer::renderMessage(
                msg,
                me
            );
    }

    html += "</body></html>";

    if (html == lastRenderedHtml) {
        return;
    }

    lastRenderedHtml = html;

    ui->privateDisplay->setHtml(html);

    if (autoScroll) {

        ui->privateDisplay
            ->verticalScrollBar()
            ->setValue(
                ui->privateDisplay
                    ->verticalScrollBar()
                    ->maximum()
            );
    }
}
