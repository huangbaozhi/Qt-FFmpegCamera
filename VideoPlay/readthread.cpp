#include "readthread.h"
#include "videodecode.h"
#include "videosave.h"

#include <QVariant>

#include <playimage.h>
#include <qimage.h>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

extern "C"
{   // 用C规则编译指定的代码
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

ReadThread::ReadThread(QObject *parent) : QThread(parent)
{
    m_videoDecode = new VideoDecode();
    m_videoSave = new VideoSave();

    // 注册自定义枚举类型，否则信号槽无法发送
    qRegisterMetaType<PlayState>("PlayState");
}

ReadThread::~ReadThread()
{
    if (m_videoSave)
    {
        delete m_videoSave;
    }
    if (m_videoDecode)
    {
        delete m_videoDecode;
    }
}

/**
 * @brief      传入播放的视频地址并开启线程
 * @param url
 */
void ReadThread::open(const QString& url)
{
    if (!this->isRunning())
    {
        m_url = url;
        emit this->start();
    }
}

/**
 * @brief 关闭播放
 */
void ReadThread::close()
{
    m_play = false;
}

/**
 * @brief    返回当前播放的地址
 * @return
 */
const QString& ReadThread::url()
{
    return m_url;
}

void ReadThread::savaVideo(const QString& fileName)
{
    m_videoSave->open(m_videoDecode->getVideoStream(), fileName);

    if (!m_videoDecode->getVideoStream())
    {
        qDebug() << "无效的视频流";
        return;
    }
    else
    {
        qDebug() << "视频流正常输出！！！";
    }

    // 获取视频流的相关信息
    AVCodecParameters* codecParams = m_videoDecode->getVideoStream()->codecpar;
    const char* codecName = avcodec_get_name(codecParams->codec_id);
    int width = codecParams->width;
    int height = codecParams->height;
    double frameRate = (double)m_videoDecode->getVideoStream()->avg_frame_rate.num / m_videoDecode->getVideoStream()->avg_frame_rate.den;

    // 连接数据库
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen() && !db.open()) {
        qDebug() << "无法连接到数据库";
        return;
    }

    // 插入数据到数据库
    QSqlQuery query;
    query.prepare("INSERT INTO video_streams (video_name, codec_name, width, height, frame_rate) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(fileName);        // 视频文件路径
    query.addBindValue(QString(codecName)); // 编码格式
    query.addBindValue(width);           // 视频宽度
    query.addBindValue(height);          // 视频高度
    query.addBindValue(frameRate);       // 帧率

    if (!query.exec()) {
        qDebug() << "保存视频流到数据库失败: " << query.lastError().text();
    } else {
        qDebug() << "视频流信息已成功保存到数据库";
    }

}

void ReadThread::stop()
{
    m_videoSave->close();
}

/**
 * @brief      非阻塞延时
 * @param msec 延时毫秒
 */
void sleepMsec(int msec)
{
    if (msec <= 0)
        return;
    QEventLoop loop;                                 //定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));   //创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();                                     //事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}

void ReadThread::run()
{
    bool ret = m_videoDecode->open(m_url);   // 打开网络流时会比较慢，如果放到Ui线程会卡
    if (ret)
    {
        m_play = true;
        emit playState(play);
    }
    else
    {
        qWarning() << "打开失败！";
    }

    // 循环读取视频图像
    while (m_play)
    {
        AVFrame* frame = m_videoDecode->read();   // 读取视频图像
        if (frame)
        {
            m_videoSave->write(frame);
            emit repaint(frame);
        }
        else
        {
            sleepMsec(1);   // 这里不能使用QThread::msleep()延时，否则会很不稳定
        }
    }

    qDebug() << "关闭摄像头！";
    m_videoDecode->close();
    emit playState(end);
}

