#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QString>
#include <memory>

class QTimer;
class QListWidget;
class QLabel;
class QPushButton;
class QMediaCaptureSession;
class QAudioInput;
class QMediaRecorder;
class QMediaPlayer;
class QAudioOutput;
class ChatController;

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget {
    Q_OBJECT

private:
    Ui::ChatWindow* ui;
    QTimer* refreshTimer;
    QTimer* recordingTimer;

    QListWidget* userList;
    QLabel* recordingLabel;
    QPushButton* voiceButton;
    QPushButton* deleteVoiceButton;
    QPushButton* attachmentButton;
    QPushButton* cameraButton;

    QMediaCaptureSession* audioSession;
    QAudioInput* audioInput;
    QMediaRecorder* audioRecorder;
    QMediaPlayer* voicePlayer;
    QAudioOutput* voiceOutput;

    bool isRecording;
    bool hasPendingVoice;
    QString pendingVoiceFilePath;
    int recordingSeconds;

    std::shared_ptr<ChatController> controller;

    QString lastRenderedHtml;
    QString lastRenderedUsers;

public:
    explicit ChatWindow(std::shared_ptr<ChatController> controller, QWidget* parent = nullptr);
    ~ChatWindow();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setupUserListPanel();
    void setupVoiceRecorder();
    void setupVoicePlayer();

    void refreshUserList();

    void onSendClicked();
    void onVoiceClicked();
    void onDeleteVoiceClicked();
    void onAttachmentClicked();
    void onCameraClicked();
    void onOpenGroupWindow();
    void openPrivateWindow(const QString& username);

    void startVoiceRecording();
    void stopVoiceRecording();
    void updateRecordingTimer();
    void preparePendingVoice();
    void clearPendingVoice();

    bool sendTextMessage(const QString& text);
    bool sendPendingVoiceMessage();
    bool sendAttachmentFile(const QString& filePath);

    QString createCameraOutputPath(const QString& extension, const QString& folderName) const;

    void playVoiceFile(const QString& filePath);
    void openAttachmentFile(const QString& filePath);

    void refreshMessages();
};

#endif
