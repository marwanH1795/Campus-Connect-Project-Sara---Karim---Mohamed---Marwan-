#include "ChatWindow.h"
#include "GroupWindow.h"
#include "PrivateWindow.h"
#include "MessageRenderer.h"
#include "../logic/ChatController.h"
#include "ui_ChatWindow.h"

#include <QTimer>
#include <QScrollBar>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
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

ChatWindow::ChatWindow(std::shared_ptr<ChatController> controller, QWidget* parent)
    : QWidget(parent),
      ui(new Ui::ChatWindow),
      refreshTimer(new QTimer(this)),
      recordingTimer(new QTimer(this)),
      userList(nullptr),
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
      recordingSeconds(0),
      controller(controller)
{
    ui->setupUi(this);

    setupUserListPanel();
    setupVoiceRecorder();
    setupVoicePlayer();

    ui->chatDisplay->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->chatDisplay->viewport()->installEventFilter(this);

    connect(ui->sendButton, &QPushButton::clicked,
            this, [this]() { onSendClicked(); });

    connect(ui->groupButton, &QPushButton::clicked,
            this, [this]() { onOpenGroupWindow(); });

    connect(voiceButton, &QPushButton::clicked,
            this, [this]() { onVoiceClicked(); });

    connect(deleteVoiceButton, &QPushButton::clicked,
            this, [this]() { onDeleteVoiceClicked(); });

    connect(attachmentButton, &QPushButton::clicked,
            this, [this]() { onAttachmentClicked(); });

    connect(cameraButton, &QPushButton::clicked,
            this, [this]() { onCameraClicked(); });

    connect(ui->messageInput, &QLineEdit::returnPressed,
            this, [this]() { onSendClicked(); });

    connect(ui->messageInput, &QLineEdit::textChanged,
            this, [this](const QString& text) {
                this->controller->sendTypingStatus(!text.trimmed().isEmpty());
            });

    connect(userList, &QListWidget::itemClicked,
            this, [this](QListWidgetItem* item) {
                if (!item) {
                    return;
                }

                QString name = item->text();
                name.remove(" (You)");

                QString me = QString::fromStdString(this->controller->getCurrentUsername());

                if (name == me) {
                    return;
                }

                this->openPrivateWindow(name);
            });

    connect(recordingTimer, &QTimer::timeout,
            this, [this]() { updateRecordingTimer(); });

    connect(refreshTimer, &QTimer::timeout,
            this, [this]() {
                refreshMessages();
                refreshUserList();
            });

    refreshMessages();
    refreshUserList();
    refreshTimer->start(250);
}

ChatWindow::~ChatWindow() {
    delete ui;
}

bool ChatWindow::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->chatDisplay->viewport() &&
        event->type() == QEvent::MouseButtonRelease) {

        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->button() == Qt::LeftButton) {
            QString link = ui->chatDisplay->anchorAt(mouseEvent->pos());

            if (link.isEmpty()) {
                QTextCursor cursor = ui->chatDisplay->cursorForPosition(mouseEvent->pos());
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

            if (link.startsWith("open:")) {
                QString encodedPath = link.mid(QString("open:").length());
                QString filePath = QString::fromUtf8(
                    QByteArray::fromBase64(encodedPath.toLatin1())
                );

                openAttachmentFile(filePath);
                return true;
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void ChatWindow::setupUserListPanel() {
    QVBoxLayout* oldLayout = qobject_cast<QVBoxLayout*>(layout());

    if (!oldLayout) {
        return;
    }

    QHBoxLayout* root = new QHBoxLayout();
    root->setSpacing(14);
    root->setContentsMargins(20, 20, 20, 20);

    QVBoxLayout* chatLayout = new QVBoxLayout();
    chatLayout->setSpacing(12);

    while (oldLayout->count()) {
        QLayoutItem* item = oldLayout->takeAt(0);

        if (item && item->widget() == ui->chatDisplay) {
            chatLayout->addWidget(ui->chatDisplay, 1);
        }

        delete item;
    }

    delete oldLayout;

    QFrame* inputBar = new QFrame(this);
    inputBar->setMinimumHeight(66);
    inputBar->setStyleSheet(
        "QFrame {"
        "background-color:#151525;"
        "border:2px solid #3a3b55;"
        "border-radius:22px;"
        "}"
    );

    QHBoxLayout* inputLayout = new QHBoxLayout(inputBar);
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
        "QLineEdit:focus { border:2px solid #7c6af7; }"
    );

    recordingLabel = new QLabel("00:00", this);
    recordingLabel->hide();
    recordingLabel->setStyleSheet(
        "QLabel {"
        "color:#ffcc66;"
        "font-weight:bold;"
        "font-size:13px;"
        "padding:0px 8px;"
        "}"
    );

    voiceButton = new QPushButton("🎤", this);
    voiceButton->setMinimumSize(46, 46);
    voiceButton->setMaximumSize(46, 46);
    voiceButton->setToolTip("Record voice message");

    deleteVoiceButton = new QPushButton("🗑", this);
    deleteVoiceButton->setMinimumSize(46, 46);
    deleteVoiceButton->setMaximumSize(46, 46);
    deleteVoiceButton->setToolTip("Delete recorded voice");
    deleteVoiceButton->hide();

    attachmentButton = new QPushButton("📎", this);
    attachmentButton->setMinimumSize(46, 46);
    attachmentButton->setMaximumSize(46, 46);
    attachmentButton->setToolTip("Send image, video, or file");

    cameraButton = new QPushButton("📷", this);
    cameraButton->setMinimumSize(46, 46);
    cameraButton->setMaximumSize(46, 46);
    cameraButton->setToolTip("Take photo or record video");

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
        "QPushButton:pressed { background-color:#7c6af7; }";

    voiceButton->setStyleSheet(roundButtonStyle);
    deleteVoiceButton->setStyleSheet(roundButtonStyle);
    attachmentButton->setStyleSheet(roundButtonStyle);
    cameraButton->setStyleSheet(roundButtonStyle);

    ui->sendButton->setMinimumSize(105, 46);
    ui->sendButton->setMaximumSize(115, 46);
    ui->sendButton->setStyleSheet(
        "QPushButton {"
        "background-color:#3ecf8e;"
        "color:#101426;"
        "border:none;"
        "border-radius:23px;"
        "font-size:13px;"
        "font-weight:bold;"
        "}"
        "QPushButton:hover { background-color:#55e0a3; }"
        "QPushButton:pressed { background-color:#28a570; }"
    );

    ui->groupButton->setMinimumSize(105, 46);
    ui->groupButton->setMaximumSize(115, 46);
    ui->groupButton->setStyleSheet(
        "QPushButton {"
        "background-color:#2a2b3d;"
        "color:#8b7cff;"
        "border:2px solid #7c6af7;"
        "border-radius:23px;"
        "font-size:13px;"
        "font-weight:bold;"
        "}"
        "QPushButton:hover { background-color:#7c6af7; color:white; }"
        "QPushButton:pressed { background-color:#5a4fcf; color:white; }"
    );

    inputLayout->addWidget(ui->messageInput, 1);
    inputLayout->addWidget(recordingLabel);
    inputLayout->addWidget(voiceButton);
    inputLayout->addWidget(deleteVoiceButton);
    inputLayout->addWidget(attachmentButton);
    inputLayout->addWidget(cameraButton);
    inputLayout->addWidget(ui->sendButton);
    inputLayout->addWidget(ui->groupButton);

    chatLayout->addWidget(inputBar);

    QFrame* panel = new QFrame(this);
    panel->setMinimumWidth(170);
    panel->setMaximumWidth(210);
    panel->setStyleSheet(
        "QFrame {"
        "background-color:#16172a;"
        "border:2px solid #3a3b55;"
        "border-radius:12px;"
        "}"
    );

    QVBoxLayout* usersLayout = new QVBoxLayout(panel);
    usersLayout->setContentsMargins(10, 10, 10, 10);
    usersLayout->setSpacing(8);

    QLabel* title = new QLabel("Online Users", panel);
    title->setStyleSheet(
        "QLabel {"
        "color:#7c6af7;"
        "font-weight:bold;"
        "font-size:13px;"
        "border:none;"
        "}"
    );

    userList = new QListWidget(panel);
    userList->setStyleSheet(
        "QListWidget {"
        "background-color:transparent;"
        "color:#e0e0f0;"
        "border:none;"
        "font-size:12px;"
        "}"
        "QListWidget::item { padding:8px; border-radius:8px; }"
        "QListWidget::item:hover { background-color:#2a2b3d; }"
        "QListWidget::item:selected { background-color:#7c6af7; color:white; }"
    );

    QLabel* hint = new QLabel("Click a user\nto open private chat", panel);
    hint->setStyleSheet(
        "QLabel {"
        "color:#9aa0c3;"
        "font-style:italic;"
        "font-size:10px;"
        "border:none;"
        "}"
    );

    usersLayout->addWidget(title);
    usersLayout->addWidget(userList);
    usersLayout->addWidget(hint);

    root->addLayout(chatLayout, 1);
    root->addWidget(panel);

    setLayout(root);
}

void ChatWindow::setupVoiceRecorder() {
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

void ChatWindow::setupVoicePlayer() {
    voicePlayer = new QMediaPlayer(this);
    voiceOutput = new QAudioOutput(this);

    voicePlayer->setAudioOutput(voiceOutput);
    voiceOutput->setVolume(1.0);

    connect(voicePlayer, &QMediaPlayer::errorOccurred,
            this, [this](QMediaPlayer::Error, const QString& errorString) {
                ui->messageInput->setPlaceholderText("Playback failed: " + errorString);
            });
}

void ChatWindow::refreshUserList() {
    if (!userList) {
        return;
    }

    auto users = controller->getState().getOnlineUsers();

    QString rendered;

    for (const auto& u : users) {
        rendered += QString::fromStdString(u) + "|";
    }

    if (rendered == lastRenderedUsers) {
        return;
    }

    lastRenderedUsers = rendered;
    userList->clear();

    QString me = QString::fromStdString(controller->getCurrentUsername());

    for (const auto& u : users) {
        QString name = QString::fromStdString(u);

        if (name == me) {
            userList->addItem(name + " (You)");
        } else {
            userList->addItem(name);
        }
    }
}

void ChatWindow::onSendClicked() {
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

void ChatWindow::onVoiceClicked() {
    if (isRecording) {
        stopVoiceRecording();
    } else {
        startVoiceRecording();
    }
}

void ChatWindow::onDeleteVoiceClicked() {
    clearPendingVoice();
}

void ChatWindow::onAttachmentClicked() {
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Choose a file to send",
        QDir::homePath(),
        "All Files (*.*)"
    );

    if (filePath.isEmpty()) {
        return;
    }

    ui->messageInput->setPlaceholderText("Sending attachment...");

    if (!sendAttachmentFile(filePath)) {
        ui->messageInput->setPlaceholderText("Failed to send attachment ❌");

        QTimer::singleShot(2500, this, [this]() {
            ui->messageInput->setPlaceholderText("Type a message...");
        });
    }
}

void ChatWindow::onCameraClicked() {
    QCameraDevice cameraDevice = QMediaDevices::defaultVideoInput();

    if (cameraDevice.isNull()) {
        QMessageBox::warning(
            this,
            "Camera not found",
            "No camera device was found on this computer."
        );
        return;
    }

    QDialog cameraDialog(this);
    cameraDialog.setWindowTitle("Camera Capture");
    cameraDialog.setMinimumSize(760, 590);
    cameraDialog.setStyleSheet(
        "QDialog { background-color:#1e1f2e; color:white; }"
        "QLabel { color:#e0e0f0; }"
        "QPushButton {"
        "background-color:#2a2b3d;"
        "color:white;"
        "border:2px solid #4a4b6a;"
        "border-radius:18px;"
        "padding:8px 14px;"
        "font-weight:bold;"
        "}"
        "QPushButton:hover { background-color:#3a3b55; border:2px solid #7c6af7; }"
        "QPushButton:pressed { background-color:#7c6af7; }"
    );

    QVBoxLayout* root = new QVBoxLayout(&cameraDialog);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    QLabel* titleLabel = new QLabel("Camera Preview", &cameraDialog);
    titleLabel->setStyleSheet(
        "QLabel { color:#7c6af7; font-size:16px; font-weight:bold; }"
    );

    QVideoWidget* videoWidget = new QVideoWidget(&cameraDialog);
    videoWidget->setMinimumHeight(360);
    videoWidget->setStyleSheet(
        "QVideoWidget { background-color:#000000; border-radius:12px; }"
    );

    QLabel* timerLabel = new QLabel("Video time: 00:00", &cameraDialog);
    timerLabel->setStyleSheet(
        "QLabel { color:#ffcc66; font-weight:bold; font-size:13px; }"
    );
    timerLabel->hide();

    QLabel* statusLabel = new QLabel("Ready. Take a photo or record a video.", &cameraDialog);
    statusLabel->setStyleSheet(
        "QLabel { background-color:#303030; padding:8px; border-radius:8px; }"
    );

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(10);

    QPushButton* takePhotoButton = new QPushButton("📸 Take Photo", &cameraDialog);
    QPushButton* startVideoButton = new QPushButton("⏺ Start Video", &cameraDialog);
    QPushButton* stopVideoButton = new QPushButton("⏹ Stop Video", &cameraDialog);
    QPushButton* sendButton = new QPushButton("Send ✅", &cameraDialog);
    QPushButton* deleteButton = new QPushButton("Delete 🗑", &cameraDialog);
    QPushButton* closeButton = new QPushButton("Close", &cameraDialog);

    stopVideoButton->setEnabled(false);
    sendButton->setEnabled(false);
    deleteButton->setEnabled(false);

    buttonsLayout->addWidget(takePhotoButton);
    buttonsLayout->addWidget(startVideoButton);
    buttonsLayout->addWidget(stopVideoButton);
    buttonsLayout->addWidget(sendButton);
    buttonsLayout->addWidget(deleteButton);
    buttonsLayout->addWidget(closeButton);

    root->addWidget(titleLabel);
    root->addWidget(videoWidget, 1);
    root->addWidget(timerLabel);
    root->addWidget(statusLabel);
    root->addLayout(buttonsLayout);

    QMediaCaptureSession cameraSession;
    QCamera camera(cameraDevice);
    QImageCapture imageCapture;
    QMediaRecorder videoRecorder;
    QAudioInput videoAudioInput;

    cameraSession.setCamera(&camera);
    cameraSession.setAudioInput(&videoAudioInput);
    cameraSession.setVideoOutput(videoWidget);
    cameraSession.setImageCapture(&imageCapture);
    cameraSession.setRecorder(&videoRecorder);

    QMediaFormat videoFormat;
    videoFormat.setFileFormat(QMediaFormat::MPEG4);
    videoRecorder.setMediaFormat(videoFormat);
    videoRecorder.setQuality(QMediaRecorder::NormalQuality);

    QString capturedFilePath;
    QString pendingVideoFilePath;
    bool isVideoRecording = false;
    int videoSeconds = 0;

    QTimer videoTimer(&cameraDialog);

    auto updateVideoTimerText = [&]() {
        int minutes = videoSeconds / 60;
        int seconds = videoSeconds % 60;

        timerLabel->setText(
            QString("Video time: %1:%2")
                .arg(minutes, 2, 10, QChar('0'))
                .arg(seconds, 2, 10, QChar('0'))
        );
    };

    connect(&videoTimer, &QTimer::timeout,
            &cameraDialog,
            [&]() {
                videoSeconds++;
                updateVideoTimerText();
            });

    connect(&imageCapture, &QImageCapture::imageSaved,
            &cameraDialog,
            [&](int, const QString& fileName) {
                capturedFilePath = fileName;

                statusLabel->setText("Photo captured. Click Send or Delete.");
                sendButton->setEnabled(true);
                deleteButton->setEnabled(true);
                takePhotoButton->setEnabled(true);
                startVideoButton->setEnabled(true);
            });

    connect(&imageCapture, &QImageCapture::errorOccurred,
            &cameraDialog,
            [&](int, QImageCapture::Error, const QString& errorString) {
                statusLabel->setText("Photo capture failed: " + errorString);
                takePhotoButton->setEnabled(true);
                startVideoButton->setEnabled(true);
            });

    connect(&videoRecorder, &QMediaRecorder::recorderStateChanged,
            &cameraDialog,
            [&](QMediaRecorder::RecorderState state) {
                if (state == QMediaRecorder::RecordingState) {
                    statusLabel->setText("Recording video...");
                }

                if (state == QMediaRecorder::StoppedState && isVideoRecording) {
                    isVideoRecording = false;
                    videoTimer.stop();
                    QApplication::beep();

                    if (!pendingVideoFilePath.isEmpty() &&
                        QFileInfo::exists(pendingVideoFilePath)) {

                        capturedFilePath = pendingVideoFilePath;
                        statusLabel->setText("Video recorded. Click Send or Delete.");
                        sendButton->setEnabled(true);
                        deleteButton->setEnabled(true);
                    } else {
                        statusLabel->setText("Video file was not created.");
                    }

                    takePhotoButton->setEnabled(true);
                    startVideoButton->setEnabled(true);
                    stopVideoButton->setEnabled(false);
                }
            });

    connect(&videoRecorder, &QMediaRecorder::errorOccurred,
            &cameraDialog,
            [&](QMediaRecorder::Error, const QString& errorString) {
                isVideoRecording = false;
                videoTimer.stop();

                statusLabel->setText("Video recording failed: " + errorString);

                takePhotoButton->setEnabled(true);
                startVideoButton->setEnabled(true);
                stopVideoButton->setEnabled(false);
            });

    connect(takePhotoButton, &QPushButton::clicked,
            &cameraDialog,
            [&]() {
                if (!capturedFilePath.isEmpty()) {
                    QFile::remove(capturedFilePath);
                    capturedFilePath.clear();
                }

                QString outputPath = createCameraOutputPath("jpg", "CameraPhotos");

                statusLabel->setText("Capturing photo...");
                takePhotoButton->setEnabled(false);
                startVideoButton->setEnabled(false);
                sendButton->setEnabled(false);
                deleteButton->setEnabled(false);

                QApplication::beep();
                imageCapture.captureToFile(outputPath);
            });

    connect(startVideoButton, &QPushButton::clicked,
            &cameraDialog,
            [&]() {
                if (!capturedFilePath.isEmpty()) {
                    QFile::remove(capturedFilePath);
                    capturedFilePath.clear();
                }

                pendingVideoFilePath = createCameraOutputPath("mp4", "CameraVideos");

                videoRecorder.setOutputLocation(QUrl::fromLocalFile(pendingVideoFilePath));

                isVideoRecording = true;
                videoSeconds = 0;
                updateVideoTimerText();
                timerLabel->show();

                takePhotoButton->setEnabled(false);
                startVideoButton->setEnabled(false);
                stopVideoButton->setEnabled(true);
                sendButton->setEnabled(false);
                deleteButton->setEnabled(false);

                QApplication::beep();
                videoRecorder.record();
                videoTimer.start(1000);
            });

    connect(stopVideoButton, &QPushButton::clicked,
            &cameraDialog,
            [&]() {
                if (isVideoRecording) {
                    statusLabel->setText("Stopping video...");
                    QApplication::beep();
                    videoRecorder.stop();
                }
            });

    connect(deleteButton, &QPushButton::clicked,
            &cameraDialog,
            [&]() {
                if (!capturedFilePath.isEmpty()) {
                    QFile::remove(capturedFilePath);
                }

                capturedFilePath.clear();
                pendingVideoFilePath.clear();

                videoSeconds = 0;
                updateVideoTimerText();
                timerLabel->hide();

                sendButton->setEnabled(false);
                deleteButton->setEnabled(false);

                statusLabel->setText("Deleted. Take a new photo or video.");
            });

    connect(sendButton, &QPushButton::clicked,
            &cameraDialog,
            [&]() {
                if (capturedFilePath.isEmpty() ||
                    !QFileInfo::exists(capturedFilePath)) {

                    statusLabel->setText("No captured file to send.");
                    return;
                }

                statusLabel->setText("Sending captured file...");
                ui->messageInput->setPlaceholderText("Sending attachment...");

                if (sendAttachmentFile(capturedFilePath)) {
                    statusLabel->setText("Captured file sent ✅");
                    cameraDialog.accept();
                } else {
                    statusLabel->setText("Failed to send captured file ❌");
                    ui->messageInput->setPlaceholderText("Failed to send attachment ❌");
                }
            });

    connect(closeButton, &QPushButton::clicked,
            &cameraDialog,
            [&]() {
                if (isVideoRecording) {
                    videoRecorder.stop();
                }

                videoTimer.stop();
                cameraDialog.reject();
            });

    camera.start();
    cameraDialog.exec();

    if (isVideoRecording) {
        videoRecorder.stop();
    }

    videoTimer.stop();
    camera.stop();
}

void ChatWindow::onOpenGroupWindow() {
    GroupWindow* groupWindow = new GroupWindow(controller);
    groupWindow->setAttribute(Qt::WA_DeleteOnClose);
    groupWindow->show();
}

void ChatWindow::openPrivateWindow(const QString& username) {
    PrivateWindow* privateWindow = new PrivateWindow(controller, username);
    privateWindow->setAttribute(Qt::WA_DeleteOnClose);
    privateWindow->show();
}

void ChatWindow::startVoiceRecording() {
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
        "public_voice_" +
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

void ChatWindow::stopVoiceRecording() {
    if (!isRecording) {
        return;
    }

    isRecording = false;
    recordingTimer->stop();
    audioRecorder->stop();

    voiceButton->setText("🎤");
    ui->messageInput->setPlaceholderText("Voice ready. Click Send or Delete.");
}

void ChatWindow::updateRecordingTimer() {
    recordingSeconds++;

    int minutes = recordingSeconds / 60;
    int seconds = recordingSeconds % 60;

    recordingLabel->setText(
        QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'))
    );
}

void ChatWindow::preparePendingVoice() {
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

void ChatWindow::clearPendingVoice() {
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

bool ChatWindow::sendTextMessage(const QString& text) {
    bool sent = controller->sendPublicMessage(text.toStdString());

    if (sent) {
        controller->sendTypingStatus(false);
        refreshMessages();
    }

    return sent;
}

bool ChatWindow::sendPendingVoiceMessage() {
    QFileInfo info(pendingVoiceFilePath);

    if (!info.exists() || info.size() == 0) {
        ui->messageInput->setPlaceholderText("Voice file not found or empty");
        return false;
    }

    bool sent = controller->sendPublicAttachment(
        pendingVoiceFilePath,
        "audio/wav"
    );

    if (sent) {
        controller->sendTypingStatus(false);
        refreshMessages();
    } else {
        ui->messageInput->setPlaceholderText("Failed to send voice message");
    }

    return sent;
}

bool ChatWindow::sendAttachmentFile(const QString& filePath) {
    QFileInfo info(filePath);

    if (!info.exists() || info.size() <= 0) {
        return false;
    }

    QMimeDatabase mimeDatabase;
    QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath);

    QString type = mimeType.isValid()
        ? mimeType.name()
        : "application/octet-stream";

    bool sent = controller->sendPublicAttachment(filePath, type);

    if (sent) {
        ui->messageInput->setPlaceholderText("Attachment sent ✅");
        refreshMessages();

        QTimer::singleShot(2500, this, [this]() {
            ui->messageInput->setPlaceholderText("Type a message...");
        });
    }

    return sent;
}

QString ChatWindow::createCameraOutputPath(const QString& extension,
                                           const QString& folderName) const {
    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);

    if (baseDir.isEmpty()) {
        baseDir = QDir::homePath();
    }

    QDir dir(baseDir + "/CampusConnect/" + folderName);

    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString fileName =
        "camera_" +
        QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz") +
        "." +
        extension;

    return dir.absoluteFilePath(fileName);
}

void ChatWindow::playVoiceFile(const QString& filePath) {
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

void ChatWindow::openAttachmentFile(const QString& filePath) {
    QFileInfo info(filePath);

    if (!info.exists()) {
        ui->messageInput->setPlaceholderText("Attachment file not found");
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void ChatWindow::refreshMessages() {
    auto messages = controller->getState().getPublicMessages();
    auto typing = controller->getState().getPublicTypingUsers();

    QString me =
        QString::fromStdString(controller->getCurrentUsername()).trimmed().toLower();

    QScrollBar* scrollBar = ui->chatDisplay->verticalScrollBar();
    bool shouldAutoScroll = scrollBar->value() >= scrollBar->maximum() - 25;

    QString html;
    html += "<html><body style='background:#0f0f1a; color:white; font-family:Segoe UI, Arial; margin:16px;'>";

    for (const auto& msg : messages) {
        html += MessageRenderer::renderMessage(msg, me);
    }

    if (!typing.empty()) {
        QString text;

        for (const auto& user : typing) {
            text += QString::fromStdString(user) + ", ";
        }

        text.chop(2);
        html += MessageRenderer::renderTyping(text + " is typing...");
    }

    html += "</body></html>";

    if (html == lastRenderedHtml) {
        return;
    }

    lastRenderedHtml = html;
    ui->chatDisplay->setHtml(html);

    if (shouldAutoScroll) {
        ui->chatDisplay->verticalScrollBar()->setValue(
            ui->chatDisplay->verticalScrollBar()->maximum()
        );
    }
}
