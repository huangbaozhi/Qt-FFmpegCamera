#ifndef WIDGET_H
#define WIDGET_H

#include "playimage.h"
#include "readthread.h"

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QListWidget>
#include <QMediaPlayer>
#include <QVideoWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_but_open_clicked();

    void on_playState(ReadThread::PlayState state);

    void on_but_save_clicked();

    void onFrameReady(AVFrame *frame);

    void on_playButton_clicked();

    QString getVideoFilePathFromDatabase();

private:
    Ui::Widget *ui;

    //PlayImage* playImage = nullptr;
    QLabel *m_pShowVideoLbl = nullptr;
    QListWidget *listWidget;
    ReadThread* m_readThread = nullptr;

    QMediaPlayer *mediaPlayer;
    QVideoWidget *videoWidget;
};
#endif // WIDGET_H
