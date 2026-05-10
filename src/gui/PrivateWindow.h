#ifndef PRIVATEWINDOW_H
#define PRIVATEWINDOW_H

#include <QDialog>
#include <QString>
#include <memory>

class QTimer;
class QLabel;
class QPushButton;
class QMediaCaptureSession;
class QAudioInput;
class QMediaRecorder;
class QMediaPlayer;
class QAudioOutput;

class ChatController;

namespace Ui {
class PrivateWindow;
}

class PrivateWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PrivateWindow(std::shared_ptr<ChatController> controller,
                           const QString& targetUser,
                           QWidget* parent = nullptr);

    ~PrivateWindow();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    Ui::PrivateWindow* ui;

    std::shared_ptr<ChatController> controller;

    QString targetUser;

    QTimer* refreshTimer;
    QTimer* recordingTimer;

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

    QString lastRenderedHtml;

private:
    void rebuildLayout();

    void setupVoiceRecorder();
    void setupVoicePlayer();

    void onSendClicked();

    void onVoiceClicked();
    void onDeleteVoiceClicked();

    void onAttachmentClicked();
    void onCameraClicked();

    void startVoiceRecording();
    void stopVoiceRecording();

    void updateRecordingTimer();

    void preparePendingVoice();
    void clearPendingVoice();

    bool sendPendingVoiceMessage();
    bool sendAttachmentFile(const QString& filePath);

    QString createCameraOutputPath(const QString& extension,
                                   const QString& folderName) const;

    void playVoiceFile(const QString& filePath);
    void openAttachmentFile(const QString& filePath);

    void refreshMessages();
};

#endif
