#ifndef PLAYIMAGE_H
#define PLAYIMAGE_H

#include <QWidget>

#include <QOpenGLFunctions_3_3_Core>
#include <qopenglshaderprogram.h>
#include <QOpenGLTexture>
#include <qopenglpixeltransferoptions.h>
#include <QOpenGLWidget>

struct AVFrame;

//class PlayImage : public QOpenGLWidget, public  QOpenGLFunctions_3_3_Core
//{
//    Q_OBJECT
//public:
//    //explicit PlayImage(QWidget *parent = nullptr);

//    explicit PlayImage(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

//    ~PlayImage() override;

//    void repaint(AVFrame* frame);             // 重绘

//signals:

//protected:
//    void initializeGL() override;               // 初始化gl
//    void resizeGL(int w, int h) override;       // 窗口尺寸变化
//    void paintGL() override;                    // 刷新显示

//private:
//    QOpenGLShaderProgram* m_program = nullptr;
//    QOpenGLTexture* m_texY = nullptr;           // 存储YUV图像中的Y数据
//    QOpenGLTexture* m_texU = nullptr;           // 存储YUV图像中的U数据
//    QOpenGLTexture* m_texV = nullptr;           // 存储YUV图像中的V数据
//    QOpenGLPixelTransferOptions m_options;

//    GLuint VBO = 0;       // 顶点缓冲对象,负责将数据从内存放到缓存，一个VBO可以用于多个VAO
//    GLuint VAO = 0;       // 顶点数组对象,任何随后的顶点属性调用都会储存在这个VAO中，一个VAO可以有多个VBO
//    GLuint EBO = 0;       // 元素缓冲对象,它存储 OpenGL 用来决定要绘制哪些顶点的索引
//    QSize  m_size;
//    QSizeF  m_zoomSize;
//    QPointF m_pos;

//};

#define USE_WINDOW 0    // 1:使用QOpenGLWindow显示, 0：使用QOpenGLWidget显示

#if USE_WINDOW
#include <QOpenGLWindow>
class PlayImage : public QOpenGLWindow, public  QOpenGLFunctions_3_3_Core
#else
#include <QOpenGLWidget>
class PlayImage : public QOpenGLWidget, public  QOpenGLFunctions_3_3_Core
#endif
{
    Q_OBJECT
public:
#if USE_WINDOW
    explicit PlayImage(UpdateBehavior updateBehavior = NoPartialUpdate, QWindow *parent = nullptr);
#else
    explicit PlayImage(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
#endif
     ~PlayImage() override;

    void repaint(AVFrame* frame);             // 重绘


protected:
    void initializeGL() override;               // 初始化gl
    void resizeGL(int w, int h) override;       // 窗口尺寸变化
    void paintGL() override;                    // 刷新显示

private:
    QOpenGLShaderProgram* m_program = nullptr;
    QOpenGLTexture* m_texY = nullptr;           // 存储YUV图像中的Y数据
    QOpenGLTexture* m_texU = nullptr;           // 存储YUV图像中的U数据
    QOpenGLTexture* m_texV = nullptr;           // 存储YUV图像中的V数据
    QOpenGLPixelTransferOptions m_options;

    GLuint VBO = 0;       // 顶点缓冲对象,负责将数据从内存放到缓存，一个VBO可以用于多个VAO
    GLuint VAO = 0;       // 顶点数组对象,任何随后的顶点属性调用都会储存在这个VAO中，一个VAO可以有多个VBO
    GLuint EBO = 0;       // 元素缓冲对象,它存储 OpenGL 用来决定要绘制哪些顶点的索引
    QSize  m_size;
    QSizeF  m_zoomSize;
    QPointF m_pos;
};

#endif // PLAYIMAGE_H
