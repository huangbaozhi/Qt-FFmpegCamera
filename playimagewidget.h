#ifndef PLAYIMAGEWIDGET_H
#define PLAYIMAGEWIDGET_H

#include <QWidget>

struct AVFrame;

class PlayImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayImageWidget(QWidget *parent = nullptr);

    void repaint(AVFrame* frame);

signals:

private:
    QSize  m_size;

};

#endif // PLAYIMAGEWIDGET_H
