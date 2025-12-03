#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QElapsedTimer>
#include <QOpenGLTexture>
#include "../core/systems/RenderEngine/RenderEngine.h"

class RenderWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit RenderWidget(QWidget *parent = nullptr);
    ~RenderWidget();

signals:
    void fpsUpdated(int fps);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void scheduleNextFrame();

public slots:
    void markMaterialDirty();

private:
    QElapsedTimer elapsedTimer;
    QElapsedTimer fpsTimer;
    RenderEngine *renderEngine;
    void renderFrame();
    void updateFPS();

    int width;
    int height;

    int frameCount;
    bool isRendering;
    bool glInitialized;
    bool textureInitialized; // Track if texture has been allocated once

    // Mouse tracking for camera
    QPoint lastMousePos;
    bool mousePressed;
    float deltaTime;

    // OpenGL resources
    GLuint textureID;
    GLuint pbo; // Pixel Buffer Object
    QOpenGLShaderProgram *shaderProgram;
    QOpenGLBuffer *vbo;
    QOpenGLVertexArrayObject *vao;

    void setupOpenGLResources();
    void updateTextureFromKernel();
};
