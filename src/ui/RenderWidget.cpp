#include "RenderWidget.h"
#include "../core/commands/CommandsManager.h"
#include "../core/camera/Camera.h"
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QOpenGLContext>
#include <fstream>

RenderWidget::RenderWidget(QWidget *parent)
    : QOpenGLWidget(parent), isRendering(false), mousePressed(false), deltaTime(0.016f),
      glInitialized(false), textureInitialized(false), textureID(0), pbo(0), shaderProgram(nullptr), vbo(nullptr), vao(nullptr)
{
    renderEngine = new RenderEngine();

    // Register callback for scene changes
    CommandsManager::getInstance().addSceneChangedCallback([this]()
                                                           {
        // Notify render engine that scene changed
        renderEngine->notifySceneChanged(); });

    CommandsManager::getInstance().addCameraChangedCallback([this]()
                                                            {
        // Notify render engine that camera changed
        renderEngine->markCameraDirty(); });

    CommandsManager::getInstance().addShapesChangedCallback([this]()
                                                            {
        // Notify render engine that shapes changed
        renderEngine->markShapesDirty(); });

    CommandsManager::getInstance().addMaterialChangedCallback([this]()
                                                              {
        // Notify render engine that material changed
        renderEngine->markMaterialDirty(); });

    // Enable mouse tracking for camera control
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    // Continuous rendering - render as fast as possible
    fpsTimer.start();
    elapsedTimer.start();
    frameCount = 0;

    // Request OpenGL 3.3 Core Profile
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format);
}

RenderWidget::~RenderWidget()
{
    makeCurrent();

    if (textureID)
    {
        glDeleteTextures(1, &textureID);
    }
    if (pbo)
    {
        glDeleteBuffers(1, &pbo);
    }
    delete shaderProgram;
    delete vbo;
    delete vao;
    delete renderEngine;

    doneCurrent();
}

void RenderWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    setupOpenGLResources();
    glInitialized = true;

    // Start the rendering loop after OpenGL is initialized
    QMetaObject::invokeMethod(this, "scheduleNextFrame", Qt::QueuedConnection);
}

void RenderWidget::setupOpenGLResources()
{
    // Create shader program for fullscreen quad
    shaderProgram = new QOpenGLShaderProgram(this);

    // Vertex shader
    const char *vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        out vec2 TexCoord;
        
        void main()
        {
            gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    // Fragment shader
    const char *fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        
        uniform sampler2D screenTexture;
        
        void main()
        {
            FragColor = texture(screenTexture, TexCoord);
        }
    )";

    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    shaderProgram->link();

    // Fullscreen quad vertices
    // Note: Y texture coordinates are flipped (1.0 at top, 0.0 at bottom)
    // to match the top-left origin of the render buffer
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 0.0f,  // Top-left: Y=0.0
        -1.0f, -1.0f, 0.0f, 1.0f, // Bottom-left: Y=1.0
        1.0f, -1.0f, 1.0f, 1.0f,  // Bottom-right: Y=1.0

        -1.0f, 1.0f, 0.0f, 0.0f, // Top-left: Y=0.0
        1.0f, -1.0f, 1.0f, 1.0f, // Bottom-right: Y=1.0
        1.0f, 1.0f, 1.0f, 0.0f}; // Top-right: Y=0.0

    vao = new QOpenGLVertexArrayObject(this);
    vao->create();
    vao->bind();

    vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo->create();
    vbo->bind();
    vbo->allocate(quadVertices, sizeof(quadVertices));

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // TexCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    vao->release();
    vbo->release();

    // Create texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderWidget::resizeGL(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, w, h);

    // Recreate texture with new size (force full allocation)
    if (textureID && w > 0 && h > 0)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureInitialized = true; // Texture is now allocated
    }
}

void RenderWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (!glInitialized || !textureID)
    {
        return;
    }

    // Bind shader and texture
    shaderProgram->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shaderProgram->setUniformValue("screenTexture", 0);

    // Draw fullscreen quad
    vao->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    vao->release();

    shaderProgram->release();
}

void RenderWidget::scheduleNextFrame()
{
    if (!isRendering && glInitialized)
    {
        isRendering = true;

        // Calculate delta time
        deltaTime = elapsedTimer.elapsed() / 1000.0f;
        elapsedTimer.restart();

        // Update camera with delta time
        Camera &camera = Camera::getInstance();
        camera.update(deltaTime);

        // Check if camera moved and reset TAA accumulation if needed
        if (camera.hasMoved() && renderEngine)
        {
            renderEngine->markCameraDirty();
            camera.clearMovedFlag();
        }

        renderFrame();
        isRendering = false;

        // Request Qt to call paintGL on next event loop iteration
        update();

        // Immediately schedule the next frame (render as fast as possible)
        QMetaObject::invokeMethod(this, "scheduleNextFrame", Qt::QueuedConnection);
    }
}

void RenderWidget::markMaterialDirty()
{
    if (renderEngine)
    {
        renderEngine->markMaterialDirty();
    }
}

void RenderWidget::renderFrame()
{
    if (width > 0 && height > 0)
    {
        renderEngine->render(width, height);

        // Get image data directly from OpenCL buffer and upload to OpenGL texture
        updateTextureFromKernel();

        updateFPS();
    }
}

void RenderWidget::updateTextureFromKernel()
{
    // Get the image data from render engine (RGB float values)
    const std::vector<float> &imageData = renderEngine->getImageData();

    if (imageData.empty())
    {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);

    // Optimization: Use glTexSubImage2D after first allocation for better performance
    // glTexImage2D reallocates the entire texture (slow)
    // glTexSubImage2D only updates existing data (fast)
    if (!textureInitialized)
    {
        // First time: allocate texture memory with glTexImage2D
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, imageData.data());
        textureInitialized = true;
    }
    else
    {
        // Subsequent frames: only update data with glTexSubImage2D (faster)
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, imageData.data());
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderWidget::updateFPS()
{
    frameCount++;
    if (fpsTimer.elapsed() >= 1000) // every second
    {
        emit fpsUpdated(frameCount);
        frameCount = 0;
        fpsTimer.restart();
    }
}

void RenderWidget::keyPressEvent(QKeyEvent *event)
{
    // Forward key events to Camera for simple key tracking
    Camera::getInstance().handleKeyPress(event->key(), true);
    QOpenGLWidget::keyPressEvent(event);
}

void RenderWidget::keyReleaseEvent(QKeyEvent *event)
{
    // Forward key events to Camera for simple key tracking
    Camera::getInstance().handleKeyPress(event->key(), false);
    QOpenGLWidget::keyReleaseEvent(event);
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
    // DISABLED: Mouse rotation temporarily disabled
    // TODO: Re-enable mouse rotation when needed
    /*
    if (mousePressed || Camera::getInstance().isFPS())
    {
        QPoint currentPos = event->pos();
        if (!lastMousePos.isNull())
        {
            float deltaX = currentPos.x() - lastMousePos.x();
            float deltaY = currentPos.y() - lastMousePos.y();
            Camera::getInstance().handleMouseMove(deltaX, deltaY);
        }
        lastMousePos = currentPos;

        // Keep mouse centered in FPS mode for continuous rotation
        if (Camera::getInstance().isFPS())
        {
            QPoint center(width / 2, height / 2);
            if ((currentPos - center).manhattanLength() > 100)
            {
                cursor().setPos(mapToGlobal(center));
                lastMousePos = center;
            }
        }
    }
    */
    QOpenGLWidget::mouseMoveEvent(event);
}

void RenderWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        mousePressed = true;
        lastMousePos = event->pos();
        setCursor(Qt::BlankCursor);
    }
    QOpenGLWidget::mousePressEvent(event);
}

void RenderWidget::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().y() / 120.0f; // Standard wheel delta
    Camera::getInstance().handleMouseScroll(delta);
    QOpenGLWidget::wheelEvent(event);
}

void RenderWidget::focusInEvent(QFocusEvent *event)
{
    QOpenGLWidget::focusInEvent(event);
}

void RenderWidget::focusOutEvent(QFocusEvent *event)
{
    mousePressed = false;
    setCursor(Qt::ArrowCursor);
    QOpenGLWidget::focusOutEvent(event);
}
