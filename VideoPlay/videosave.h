#ifndef VIDEOSAVE_H
#define VIDEOSAVE_H

#include <QString>
#include <qmutex.h>


struct AVCodecParameters;
struct AVFormatContext;
struct AVCodecContext;
struct AVStream;
struct AVFrame;
struct AVPacket;
struct AVOutputFormat;

class VideoSave
{
public:
    VideoSave();
    ~VideoSave();

    bool open(AVStream *inStream, const QString& fileName);
    void write(AVFrame* frame);
    void close();

private:
    void showError(int err);

private:
    AVFormatContext* m_formatContext = nullptr;
    AVCodecContext * m_codecContext  = nullptr;    // 编码器上下文
    AVStream       * m_videoStream   = nullptr;
    AVPacket       * m_packet        = nullptr;    // 数据包
    int m_index = 0;
    bool             m_writeHeader   = false;      // 是否写入头
    QMutex           m_mutex;
};

#endif // VIDEOSAVE_H
