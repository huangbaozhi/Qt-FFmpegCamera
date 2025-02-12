#include "widget.h"
#include "ui_widget.h"

#include <QCameraInfo>
#include <QFileDialog>

extern "C" {        // 用C规则编译指定的代码
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}
Q_DECLARE_METATYPE(AVFrame)  //注册结构体，否则无法通过信号传递AVFrame

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowTitle(QString("Qt+ffmpeg打开本地摄像头录像Demo V%1").arg(APP_VERSION));

    // 使用QOpenGLWindow绘制
    //playImage = new PlayImage;
    m_pShowVideoLbl = new QLabel;
#if USE_WINDOW
    ui->verticalLayout->addWidget(QWidget::createWindowContainer(playImage));   // 这一步加载速度要比OpenGLWidget慢一点
#else
    //ui->verticalLayout->addWidget(playImage);
    ui->verticalLayout->addWidget(m_pShowVideoLbl);
#endif

    m_readThread = new ReadThread();
    //connect(m_readThread, &ReadThread::repaint, playImage, &PlayImage::repaint, Qt::BlockingQueuedConnection);
    connect(m_readThread, &ReadThread::repaint, this, &Widget::onFrameReady);
    connect(m_readThread, &ReadThread::playState, this, &Widget::on_playState);

    // 获取可用摄像头列表
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for(auto camera : cameras)
    {
#if defined(Q_OS_WIN)
        ui->com_url->addItem("video=" + camera.description());    // ffmpeg在Window下要使用video=description()
#elif defined(Q_OS_LINUX)
        ui->com_url->addItem(camera.deviceName());                // ffmpeg在linux下要使用deviceName()
#elif defined(Q_OS_MAC)
#endif
    }
}

Widget::~Widget()
{
    // 释放视频读取线程
    if(m_readThread)
    {
        // 由于使用了BlockingQueuedConnection，所以在退出时如果信号得不到处理就会卡死，所以需要取消绑定
        //disconnect(m_readThread, &ReadThread::repaint, playImage, &PlayImage::repaint);
        m_readThread->close();
        m_readThread->wait();
        delete m_readThread;
    }
    delete ui;

    delete ui;
}

/**
 * @brief 录制视频保存到本地
 */
void Widget::on_but_open_clicked()
{
    if(ui->but_open->text() == "开始播放")
    {
        m_readThread->open(ui->com_url->currentText());
    }
    else
    {
        m_readThread->close();
    }
}

void Widget::on_playState(ReadThread::PlayState state)
{
    if(state == ReadThread::play)
    {
        this->setWindowTitle(QString("正在播放：%1").arg(m_readThread->url()));
        ui->but_open->setText("停止播放");
    }
    else
    {
        ui->but_open->setText("开始播放");
        this->setWindowTitle(QString("Qt+ffmpeg打开本地摄像头录像Demo V%1").arg(APP_VERSION));
    }
}


void Widget::on_but_save_clicked()
{
    if(ui->but_save->text() == "开始录制")
    {
        m_readThread->savaVideo(QString("%1.mp4").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss")));
        ui->but_save->setText("停止");
    }
    else
    {
        m_readThread->stop();
        ui->but_save->setText("开始录制");
    }
}

void Widget::onFrameReady(AVFrame *frame)
{
    if (!frame) return;

    // YUV -> RGB
    QImage image(frame->width, frame->height, QImage::Format_RGB888);
    uint8_t *data[1] = {image.bits()};
    int linesize[1] = {3 * frame->width};

    static SwsContext *swsCtx = sws_getContext(frame->width, frame->height, AV_PIX_FMT_YUV420P,
                                               frame->width, frame->height, AV_PIX_FMT_RGB24,
                                               SWS_BILINEAR, nullptr, nullptr, nullptr);

    sws_scale(swsCtx, frame->data, frame->linesize, 0, frame->height, data, linesize);

    m_pShowVideoLbl->setPixmap(QPixmap::fromImage(image));
}

