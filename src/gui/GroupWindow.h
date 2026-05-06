#ifndef GROUPWINDOW_H
#define GROUPWINDOW_H

#include <QDialog>
#include <QString>
#include <memory>

class QTimer;
class QLabel;
class QPushButton;
class QListWidget;
class QMediaCaptureSession;
class QAudioInput;
class QMediaRecorder;
class QMediaPlayer;
class QAudioOutput;
class ChatController;

namespace Ui {
class Dialog;
}

class GroupWindow : public QDialog {
    Q_OBJECT

private:
    Ui::Dialog* ui;
    QTimer* refreshTimer;
    QTimer* recordingTimer;

    std::shared_ptr<ChatController> controller;

    QListWidget* groupUsersList;
    QLabel* recordingLabel;
    QPushButton* voiceButton;
    QPushButton* deleteVoiceButton;
    QPushButton* attachmentButton;

    QMediaCaptureSession* audioSession;
    QAudioInput* audioInput;
    QMediaRecorder* audioRecorder;
    QMediaPlayer* voicePlayer;
    QAudioOutput* voiceOutput;

    bool isRecording;
    bool hasPendingVoice;
    QString pendingVoiceFilePath;
    int recordingSeconds;

    QString lastRenderedHtml;
    QString lastSelectedGroup;
    QString lastRenderedUsers;

public:
    explicit GroupWindow(std::shared_ptr<ChatController> controller, QWidget* parent = nullptr);
    ~GroupWindow();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void rebuildLayout();
    void setupVoiceRecorder();
    void setupVoicePlayer();

    void onCreateClicked();
    void onJoinClicked();
    void onSendClicked();
    void onVoiceClicked();
    void onDeleteVoiceClicked();
    void onAttachmentClicked();

    void startVoiceRecording();
    void stopVoiceRecording();
    void updateRecordingTimer();
    void preparePendingVoice();
    void clearPendingVoice();

    bool sendTextMessage(const QString& selectedGroup, const QString& text);
    bool sendPendingVoiceMessage(const QString& selectedGroup);
    bool sendAttachmentFile(const QString& selectedGroup, const QString& filePath);

    void playVoiceFile(const QString& filePath);
    void openAttachmentFile(const QString& filePath);

    void refreshGroups();
    void refreshGroupUsers();
    void openPrivateWindow(const QString& username);
    void refreshMessages();
};

#endif
