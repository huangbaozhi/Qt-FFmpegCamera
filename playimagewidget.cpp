#include "playimagewidget.h"

extern "C"
{   // 用C规则编译指定的代码
#include "libavcodec/avcodec.h"
}

PlayImageWidget::PlayImageWidget(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(1000, 1000);
}

void PlayImageWidget::repaint(AVFrame *frame)
{
    // 如果帧长宽为0则不需要绘制
    if (!frame || frame->width == 0 || frame->height == 0)
    {
        av_frame_unref(frame);   //  取消引用帧引用的所有缓冲区并重置帧字段。
        return;
    }
    this->update();
}
