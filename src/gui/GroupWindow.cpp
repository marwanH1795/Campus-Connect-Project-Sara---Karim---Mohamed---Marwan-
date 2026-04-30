#include "GroupWindow.h"
#include "PrivateWindow.h"
#include "MessageRenderer.h"
#include "../logic/ChatController.h"
#include "../logic/Message.h"
#include "ui_GroupWindow.h"

#include <QTimer>
#include <QScrollBar>
#include <QTime>
#include <QStringList>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
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

GroupWindow::GroupWindow(std::shared_ptr<ChatController> controller, QWidget* parent)
    : QDialog(parent),
      ui(new Ui::Dialog),
      refreshTimer(new QTimer(this)),
      recordingTimer(new QTimer(this)),
      controller(controller),
      groupUsersList(nullptr),
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

    rebuildLayout();
    setupVoiceRecorder();
    setupVoicePlayer();

    ui->groupDisplay->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->groupDisplay->viewport()->installEventFilter(this);

    connect(ui->createButton, &QPushButton::clicked,
            this, [this]() { onCreateClicked(); });

    connect(ui->joinButton, &QPushButton::clicked,
            this, [this]() { onJoinClicked(); });

    connect(ui->sendButton, &QPushButton::clicked,
            this, [this]() { onSendClicked(); });

    connect(voiceButton, &QPushButton::clicked,
            this, [this]() { onVoiceClicked(); });

    connect(deleteVoiceButton, &QPushButton::clicked,
            this, [this]() { onDeleteVoiceClicked(); });

    connect(ui->groupNameInput, &QLineEdit::returnPressed,
            this, [this]() { onJoinClicked(); });

    connect(ui->groupMessageInput, &QLineEdit::returnPressed,
            this, [this]() { onSendClicked(); });

    connect(ui->groupMessageInput, &QLineEdit::textChanged,
            this, [this](const QString& text) {
                QString group = ui->groupSelector->currentText().trimmed();

                if (!group.isEmpty()) {
                    this->controller->sendGroupTypingStatus(
                        group.toStdString(),
                        !text.trimmed().isEmpty()
                    );
                }

                refreshMessages();
            });

    connect(ui->groupSelector, &QComboBox::currentTextChanged,
            this, [this](const QString& groupName) {
                QString group = groupName.trimmed();

                lastRenderedHtml.clear();
                lastSelectedGroup = group;

                if (group.isEmpty()) {
                    ui->statusLabel->setText("No group selected.");
                    ui->groupMessageInput->setPlaceholderText("Create or join a group first...");
                } else {
                    ui->statusLabel->setText("Current group: " + group);
                    ui->groupMessageInput->setPlaceholderText("Type a message...");
                    this->controller->sendGroupTypingStatus(group.toStdString(), false);
                }

                refreshMessages();
            });

    connect(groupUsersList, &QListWidget::itemClicked,
            this, [this](QListWidgetItem* item) {
                if (!item) {
                    return;
                }

                QString text = item->text().trimmed();
                text.remove("🟢 ");
                text.remove("🔴 ");
                text.remove(" (You)");
                text = text.trimmed();

                QString me = QString::fromStdString(this->controller->getCurrentUsername());

                if (text.isEmpty() || text == me) {
                    return;
                }

                openPrivateWindow(text);
            });

    connect(recordingTimer, &QTimer::timeout,
            this, [this]() { updateRecordingTimer(); });

    connect(refreshTimer, &QTimer::timeout,
            this, [this]() {
                refreshGroups();
                refreshGroupUsers();
                refreshMessages();
            });

    refreshGroups();
    refreshGroupUsers();
    refreshMessages();
    refreshTimer->start(300);
}

GroupWindow::~GroupWindow() {
    delete ui;
}

void GroupWindow::rebuildLayout() {
    setMinimumSize(900, 600);
    setStyleSheet(
        "QDialog {"
        "background-color:#1e1f2e;"
        "color:#e0e0f0;"
        "font-family:'Segoe UI', Arial;"
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

    ui->groupNameInput->setPlaceholderText("Group name...");
    ui->groupNameInput->setMinimumHeight(46);

    ui->groupMessageInput->setPlaceholderText("Create or join a group first...");
    ui->groupMessageInput->setMinimumHeight(44);

    ui->createButton->setText("+  Create Group");
    ui->joinButton->setText("→  Join Group");
    ui->sendButton->setText("Send  ➤");

    ui->groupDisplay->setReadOnly(true);

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(14);

    QHBoxLayout* topRow = new QHBoxLayout();
    topRow->setSpacing(12);
    topRow->addWidget(ui->groupNameInput, 1);
    topRow->addWidget(ui->createButton);

    QHBoxLayout* secondRow = new QHBoxLayout();
    secondRow->setSpacing(12);
    secondRow->addWidget(ui->joinButton);
    secondRow->addWidget(ui->groupSelector, 1);

    QHBoxLayout* mainArea = new QHBoxLayout();
    mainArea->setSpacing(14);

    ui->groupDisplay->setStyleSheet(
        "QTextEdit {"
        "background-color:#16172a;"
        "color:#dcdcf0;"
        "border:2px solid #3a3b55;"
        "border-radius:12px;"
        "padding:10px;"
        "font-size:13px;"
        "}"
    );

    QFrame* usersPanel = new QFrame(this);
    usersPanel->setMinimumWidth(190);
    usersPanel->setMaximumWidth(230);
    usersPanel->setStyleSheet(
        "QFrame {"
        "background-color:#16172a;"
        "border:2px solid #3a3b55;"
        "border-radius:12px;"
        "}"
    );

    QVBoxLayout* usersLayout = new QVBoxLayout(usersPanel);
    usersLayout->setContentsMargins(10, 10, 10, 10);
    usersLayout->setSpacing(8);

    QLabel* usersTitle = new QLabel("Group Users", usersPanel);
    usersTitle->setStyleSheet(
        "QLabel {"
        "color:#7c6af7;"
        "font-weight:bold;"
        "font-size:13px;"
        "border:none;"
        "}"
    );

    groupUsersList = new QListWidget(usersPanel);
    groupUsersList->setStyleSheet(
        "QListWidget {"
        "background-color:transparent;"
        "color:#e0e0f0;"
        "border:none;"
        "font-size:12px;"
        "}"
        "QListWidget::item {"
        "padding:8px;"
        "border-radius:8px;"
        "}"
        "QListWidget::item:hover {"
        "background-color:#2a2b3d;"
        "}"
        "QListWidget::item:selected {"
        "background-color:#7c6af7;"
        "color:white;"
        "}"
    );

    QLabel* usersHint = new QLabel("Click user\nto open private chat", usersPanel);
    usersHint->setStyleSheet(
        "QLabel {"
        "color:#9aa0c3;"
        "font-style:italic;"
        "font-size:10px;"
        "border:none;"
        "}"
    );

    usersLayout->addWidget(usersTitle);
    usersLayout->addWidget(groupUsersList, 1);
    usersLayout->addWidget(usersHint);

    mainArea->addWidget(ui->groupDisplay, 1);
    mainArea->addWidget(usersPanel);

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

    ui->groupMessageInput->setStyleSheet(
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
        "QPushButton:hover {"
        "background-color:#55e0a3;"
        "}"
        "QPushButton:pressed {"
        "background-color:#28a570;"
        "}"
    );

    ui->createButton->setStyleSheet(
        "QPushButton {"
        "background-color:#7c6af7;"
        "color:white;"
        "border:none;"
        "border-radius:23px;"
        "font-weight:bold;"
        "}"
    );

    ui->joinButton->setStyleSheet(
        "QPushButton {"
        "background-color:#2a2b3d;"
        "color:#8b7cff;"
        "border:2px solid #7c6af7;"
        "border-radius:23px;"
        "font-weight:bold;"
        "}"
    );

    ui->groupNameInput->setStyleSheet(
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

    ui->groupSelector->setStyleSheet(
        "QComboBox {"
        "background-color:#2a2b3d;"
        "color:#e0e0f0;"
        "border:2px solid #4a4b6a;"
        "border-radius:16px;"
        "padding:6px 12px;"
        "font-size:13px;"
        "}"
    );

    ui->statusLabel->setStyleSheet(
        "QLabel {"
        "background-color:#303030;"
        "color:white;"
        "border-radius:8px;"
        "padding:7px 10px;"
        "font-style:italic;"
        "font-weight:bold;"
        "}"
    );

    inputLayout->addWidget(ui->groupMessageInput, 1);
    inputLayout->addWidget(recordingLabel);
    inputLayout->addWidget(voiceButton);
    inputLayout->addWidget(deleteVoiceButton);
    inputLayout->addWidget(ui->sendButton);

    root->addLayout(topRow);
    root->addLayout(secondRow);
    root->addLayout(mainArea, 1);
    root->addWidget(inputBar);
    root->addWidget(ui->statusLabel);

    setLayout(root);
}

bool GroupWindow::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->groupDisplay->viewport() &&
        event->type() == QEvent::MouseButtonRelease) {

        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->button() == Qt::LeftButton) {
            QString link = ui->groupDisplay->anchorAt(mouseEvent->pos());

            if (link.isEmpty()) {
                QTextCursor cursor = ui->groupDisplay->cursorForPosition(mouseEvent->pos());
                QTextCharFormat format = cursor.charFormat();

                if (format.isAnchor()) {
                    link = format.anchorHref();
                }
            }

            if (link.startsWith("voice:")) {
                QString encodedPath = link.mid(QString("voice:").length());
                QString filePath = QString::fromUtf8(QByteArray::fromBase64(encodedPath.toLatin1()));
                playVoiceFile(filePath);
                return true;
            }
        }
    }

    return QDialog::eventFilter(watched, event);
}

void GroupWindow::setupVoiceRecorder() {
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
                ui->groupMessageInput->setPlaceholderText("Voice failed: " + errorString);
            });
}

void GroupWindow::setupVoicePlayer() {
    voicePlayer = new QMediaPlayer(this);
    voiceOutput = new QAudioOutput(this);

    voicePlayer->setAudioOutput(voiceOutput);
    voiceOutput->setVolume(1.0);

    connect(voicePlayer, &QMediaPlayer::errorOccurred,
            this, [this](QMediaPlayer::Error, const QString& errorString) {
                ui->groupMessageInput->setPlaceholderText("Playback failed: " + errorString);
            });
}

void GroupWindow::onCreateClicked() {
    QString groupName = ui->groupNameInput->text().trimmed();

    if (groupName.isEmpty()) {
        ui->statusLabel->setText("Type a group name first.");
        return;
    }

    if (controller->createGroup(groupName.toStdString())) {
        controller->getState().joinGroup(groupName.toStdString());

        lastSelectedGroup = groupName;
        refreshGroups();

        int index = ui->groupSelector->findText(groupName);
        if (index >= 0) {
            ui->groupSelector->setCurrentIndex(index);
        }

        ui->groupNameInput->clear();
        ui->groupMessageInput->setFocus();
        ui->statusLabel->setText("Created and selected group: " + groupName);
    } else {
        ui->statusLabel->setText("Failed to create group.");
    }
}

void GroupWindow::onJoinClicked() {
    QString groupName = ui->groupNameInput->text().trimmed();

    if (groupName.isEmpty()) {
        ui->statusLabel->setText("Type the group name you want to join.");
        return;
    }

    if (controller->joinGroup(groupName.toStdString())) {
        controller->getState().joinGroup(groupName.toStdString());

        lastSelectedGroup = groupName;
        refreshGroups();

        int index = ui->groupSelector->findText(groupName);
        if (index >= 0) {
            ui->groupSelector->setCurrentIndex(index);
        }

        ui->groupNameInput->clear();
        ui->groupMessageInput->setFocus();
        ui->statusLabel->setText("Joined and selected group: " + groupName);
    } else {
        ui->statusLabel->setText("Failed to join group.");
    }
}

void GroupWindow::onSendClicked() {
    QString selectedGroup = ui->groupSelector->currentText().trimmed();

    if (selectedGroup.isEmpty()) {
        ui->statusLabel->setText("Create or join a group first.");
        return;
    }

    if (hasPendingVoice) {
        if (sendPendingVoiceMessage(selectedGroup)) {
            pendingVoiceFilePath.clear();
            hasPendingVoice = false;
            recordingSeconds = 0;

            recordingLabel->setText("00:00");
            recordingLabel->hide();
            deleteVoiceButton->hide();
            voiceButton->setText("🎤");
            ui->groupMessageInput->setPlaceholderText("Type a message...");
        }

        return;
    }

    QString content = ui->groupMessageInput->text().trimmed();

    if (content.isEmpty()) {
        ui->statusLabel->setText("Message cannot be empty.");
        return;
    }

    if (sendTextMessage(selectedGroup, content)) {
        ui->groupMessageInput->clear();
        ui->groupMessageInput->setFocus();
    }
}

void GroupWindow::onVoiceClicked() {
    if (isRecording) {
        stopVoiceRecording();
    } else {
        startVoiceRecording();
    }
}

void GroupWindow::onDeleteVoiceClicked() {
    clearPendingVoice();
}

void GroupWindow::startVoiceRecording() {
    clearPendingVoice();

    QString selectedGroup = ui->groupSelector->currentText().trimmed();

    if (selectedGroup.isEmpty()) {
        ui->statusLabel->setText("Create or join a group before recording.");
        return;
    }

    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);

    if (baseDir.isEmpty()) {
        baseDir = QDir::homePath();
    }

    QDir dir(baseDir + "/CampusConnectVoice");

    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString fileName =
        "group_voice_" +
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
    ui->groupMessageInput->setPlaceholderText("Recording voice message...");
    ui->statusLabel->setText("Recording voice...");
}

void GroupWindow::stopVoiceRecording() {
    if (!isRecording) {
        return;
    }

    isRecording = false;
    recordingTimer->stop();
    audioRecorder->stop();

    voiceButton->setText("🎤");
    ui->groupMessageInput->setPlaceholderText("Voice ready. Click Send or Delete.");
    ui->statusLabel->setText("Voice ready. Click Send or Delete.");
}

void GroupWindow::updateRecordingTimer() {
    recordingSeconds++;

    int minutes = recordingSeconds / 60;
    int seconds = recordingSeconds % 60;

    recordingLabel->setText(
        QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'))
    );
}

void GroupWindow::preparePendingVoice() {
    if (pendingVoiceFilePath.isEmpty()) {
        return;
    }

    QFileInfo info(pendingVoiceFilePath);

    if (!info.exists() || info.size() == 0) {
        ui->groupMessageInput->setPlaceholderText("Voice file was not recorded.");
        ui->statusLabel->setText("Voice file was not recorded.");
        return;
    }

    hasPendingVoice = true;
    deleteVoiceButton->show();
    recordingLabel->show();
    ui->groupMessageInput->setPlaceholderText("Voice ready. Click Send or Delete.");
    ui->statusLabel->setText("Voice ready. Click Send or Delete.");
}

void GroupWindow::clearPendingVoice() {
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
    ui->groupMessageInput->setPlaceholderText("Type a message...");
    ui->statusLabel->setText("Voice deleted.");
}

bool GroupWindow::sendTextMessage(const QString& selectedGroup, const QString& text) {
    controller->sendGroupTypingStatus(selectedGroup.toStdString(), false);

    bool sent = controller->sendGroupMessage(
        selectedGroup.toStdString(),
        text.toStdString()
    );

    if (sent) {
        ui->statusLabel->setText("Sent to group: " + selectedGroup);
        refreshMessages();
    } else {
        ui->statusLabel->setText("Failed to send group message.");
    }

    return sent;
}

bool GroupWindow::sendPendingVoiceMessage(const QString& selectedGroup) {
    QFileInfo info(pendingVoiceFilePath);

    if (!info.exists() || info.size() == 0) {
        ui->groupMessageInput->setPlaceholderText("Voice file not found or empty");
        ui->statusLabel->setText("Voice file not found or empty.");
        return false;
    }

    QString content =
        "VOICE_FILE|" +
        info.fileName() +
        "|" +
        pendingVoiceFilePath;

    controller->sendGroupTypingStatus(selectedGroup.toStdString(), false);

    bool sent = controller->sendGroupMessage(
        selectedGroup.toStdString(),
        content.toStdString()
    );

    if (sent) {
        ui->statusLabel->setText("Voice sent to group: " + selectedGroup);
        refreshMessages();
    } else {
        ui->statusLabel->setText("Failed to send voice message.");
    }

    return sent;
}

void GroupWindow::playVoiceFile(const QString& filePath) {
    QFileInfo info(filePath);

    if (!info.exists()) {
        ui->groupMessageInput->setPlaceholderText("Voice file not found");
        return;
    }

    voicePlayer->stop();
    voicePlayer->setSource(QUrl::fromLocalFile(filePath));
    voicePlayer->play();

    ui->groupMessageInput->setPlaceholderText("Playing voice message...");
}

void GroupWindow::refreshGroups() {
    auto groups = controller->getState().getJoinedGroups();

    QString current = ui->groupSelector->currentText().trimmed();

    if (!lastSelectedGroup.isEmpty()) {
        current = lastSelectedGroup;
    }

    QStringList newGroups;

    for (const auto& group : groups) {
        QString name = QString::fromStdString(group).trimmed();

        if (!name.isEmpty() && !newGroups.contains(name)) {
            newGroups << name;
        }
    }

    QStringList existingGroups;

    for (int i = 0; i < ui->groupSelector->count(); ++i) {
        existingGroups << ui->groupSelector->itemText(i);
    }

    if (existingGroups != newGroups) {
        ui->groupSelector->clear();
        ui->groupSelector->addItems(newGroups);
    }

    if (!current.isEmpty()) {
        int index = ui->groupSelector->findText(current);

        if (index >= 0 && ui->groupSelector->currentIndex() != index) {
            ui->groupSelector->setCurrentIndex(index);
        }
    } else if (ui->groupSelector->count() > 0 && ui->groupSelector->currentIndex() < 0) {
        ui->groupSelector->setCurrentIndex(0);
    }

    if (ui->groupSelector->count() == 0) {
        ui->groupMessageInput->setPlaceholderText("Create or join a group first...");
    }
}

void GroupWindow::refreshGroupUsers() {
    if (!groupUsersList) {
        return;
    }

    auto users = controller->getState().getOnlineUsers();

    QString rendered;

    for (const auto& user : users) {
        rendered += QString::fromStdString(user) + "|";
    }

    if (rendered == lastRenderedUsers) {
        return;
    }

    lastRenderedUsers = rendered;
    groupUsersList->clear();

    QString me = QString::fromStdString(controller->getCurrentUsername());

    for (const auto& user : users) {
        QString name = QString::fromStdString(user);

        if (name == me) {
            groupUsersList->addItem("🟢 " + name + " (You)");
        } else {
            groupUsersList->addItem("🟢 " + name);
        }
    }
}

void GroupWindow::openPrivateWindow(const QString& username) {
    PrivateWindow* privateWindow = new PrivateWindow(controller, username);
    privateWindow->setAttribute(Qt::WA_DeleteOnClose);
    privateWindow->show();
}

void GroupWindow::refreshMessages() {
    QString selectedGroup = ui->groupSelector->currentText().trimmed();

    if (selectedGroup.isEmpty()) {
        ui->groupDisplay->clear();
        lastRenderedHtml.clear();
        return;
    }

    auto messages = controller->getState()
                        .getGroupMessagesForGroup(selectedGroup.toStdString());

    auto typingUsers = controller->getState()
                           .getGroupTypingUsers(selectedGroup.toStdString());

    QString currentUser =
        QString::fromStdString(controller->getCurrentUsername()).trimmed().toLower();

    QScrollBar* scrollBar = ui->groupDisplay->verticalScrollBar();
    bool shouldAutoScroll = scrollBar->value() >= scrollBar->maximum() - 25;

    QString html;
    html += "<html><body style='background:#0f0f1a; color:white; font-family:Segoe UI, Arial; margin:16px;'>";

    for (const auto& msg : messages) {
        QString sender = QString::fromStdString(msg.getSender());
        QString content = QString::fromStdString(msg.getContent());
        QString time = QString::fromStdString(msg.getTimestamp());

        if (time.isEmpty()) {
            time = QTime::currentTime().toString("hh:mm");
        }

        bool isMe = (sender.trimmed().toLower() == currentUser);

        html += MessageRenderer::renderGroupMessage(sender, content, time, isMe);
    }

    if (!typingUsers.empty()) {
        QString typingText;

        for (const auto& user : typingUsers) {
            typingText += QString::fromStdString(user) + ", ";
        }

        typingText.chop(2);
        typingText += " is typing in " + selectedGroup + "...";

        html += MessageRenderer::renderTyping(typingText);
    }

    html += "</body></html>";

    if (html == lastRenderedHtml) {
        return;
    }

    lastRenderedHtml = html;
    ui->groupDisplay->setHtml(html);

    if (shouldAutoScroll) {
        ui->groupDisplay->verticalScrollBar()->setValue(
            ui->groupDisplay->verticalScrollBar()->maximum()
        );
    }
}
