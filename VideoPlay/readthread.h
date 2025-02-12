#ifndef READTHREAD_H
#define READTHREAD_H

#include "videodecode.h"
#include "videosave.h"

#include <QObject>
#include <QThread>
#include <QTime>

class VideoDecode;
class VideoSave;
class PlayImage;
struct AVFrame;

class ReadThread : public QThread
{
    Q_OBJECT

public:
    enum PlayState      // 视频播放状态
    {
        play,
        end
    };
public:
    explicit ReadThread(QObject *parent = nullptr);

    ~ReadThread() override;

    void open(const QString& url = QString());  // 打开视频
    void pause(bool flag);                      // 暂停视频
    void close();                               // 关闭视频
    const QString& url();                       // 获取打开的视频地址
    void savaVideo(const QString& fileName);    // 录制视频
    void stop();                                // 停止录制

protected:
    void run() override;

signals:
    void repaint(AVFrame* frame);               // 重绘
    void playState(PlayState state);            // 视频播放状态发送改变时触发

private:
    VideoDecode* m_videoDecode = nullptr;       // 视频解码类
    VideoSave*   m_videoSave   = nullptr;       // 视频编码保存类
    QString      m_url;                         // 打开的视频地址
    bool         m_play        = false;         // 播放控制

};

#endif // READTHREAD_H
