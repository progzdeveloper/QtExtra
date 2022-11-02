#include "glblurfunctions.h"
#include <QVector3D>

namespace
{
    class Vertex
    {
    public:
        // Constructors
        Q_DECL_CONSTEXPR Vertex();
        Q_DECL_CONSTEXPR explicit Vertex(const QVector3D &position);

        // Accessors / Mutators
        Q_DECL_CONSTEXPR const QVector3D& position() const;
        void setPosition(const QVector3D& position);

        // OpenGL Helpers
        static const int PositionTupleSize = 3;
        static Q_DECL_CONSTEXPR int positionOffset();
        static Q_DECL_CONSTEXPR int stride();

    private:
        QVector3D m_position;
    };

}

/*******************************************************************************
 * Inline Implementation
 ******************************************************************************/

// Note: Q_MOVABLE_TYPE means it can be memcpy'd.
Q_DECLARE_TYPEINFO(Vertex, Q_MOVABLE_TYPE);

// Constructors
Q_DECL_CONSTEXPR inline Vertex::Vertex() {}
Q_DECL_CONSTEXPR inline Vertex::Vertex(const QVector3D &position) : m_position(position) {}

// Accessors / Mutators
Q_DECL_CONSTEXPR inline const QVector3D& Vertex::position() const { return m_position; }
void inline Vertex::setPosition(const QVector3D& position) { m_position = position; }

// OpenGL Helpers
Q_DECL_CONSTEXPR inline int Vertex::positionOffset() { return offsetof(Vertex, m_position); }
Q_DECL_CONSTEXPR inline int Vertex::stride() { return sizeof(Vertex); }


namespace
{
    static const Vertex sg_vertexes[] =
    {
      Vertex( QVector3D( 1.0f,  1.0f, 1.0f)),
      Vertex( QVector3D(-1.0f,  1.0f, 1.0f)),
      Vertex( QVector3D(-1.0f, -1.0f, 1.0f)),
      Vertex( QVector3D( 1.0f, -1.0f, 1.0f))
    };
}

GLBlurFunctions::GLBlurFunctions()
{
    m_Context = new QOpenGLContext();
    m_Context->setFormat(QSurfaceFormat::defaultFormat());
    m_Context->create();

    m_Surface = new QOffscreenSurface();
    m_Surface->create();

    QSurfaceFormat surfaceFormat;
    surfaceFormat.setVersion(3, 3);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    m_Surface->setFormat(surfaceFormat);

    m_Context->makeCurrent(m_Surface);

    initializeOpenGLFunctions();

    m_ShaderProgram_kawase_up = new QOpenGLShaderProgram();
    m_ShaderProgram_kawase_up->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
    m_ShaderProgram_kawase_up->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/dual_kawase_up.frag");
    m_ShaderProgram_kawase_up->link();

    m_ShaderProgram_kawase_down = new QOpenGLShaderProgram();
    m_ShaderProgram_kawase_down->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
    m_ShaderProgram_kawase_down->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/dual_kawase_down.frag");
    m_ShaderProgram_kawase_down->link();

    m_VertexBuffer.create();
    m_VertexBuffer.bind();
    m_VertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_VertexBuffer.allocate(sg_vertexes, sizeof(sg_vertexes));

    m_VertexArrayObject.create();
    m_VertexArrayObject.bind();

    m_ShaderProgram_kawase_up->enableAttributeArray(0);
    m_ShaderProgram_kawase_up->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());

    m_ShaderProgram_kawase_down->enableAttributeArray(0);
    m_ShaderProgram_kawase_down->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());

    m_textureToBlur = new QOpenGLTexture(QImage());
    m_textureToBlur->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_textureToBlur->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

    m_iterations = -1;
    m_imageToBlur = QImage();
}

GLBlurFunctions::~GLBlurFunctions()
{
    delete m_ShaderProgram_kawase_up;
    delete m_ShaderProgram_kawase_down;

    for (int i = 0; i < m_FBO_vector.size(); i++) {
        delete m_FBO_vector[i];
    }

    delete m_textureToBlur;
}

QImage GLBlurFunctions::blurImage_DualKawase(QImage imageToBlur, int offset, int iterations)
{
    // Check to avoid unnecessary texture reallocation
    if (iterations != m_iterations || imageToBlur != m_imageToBlur) {
        m_iterations = iterations;
        m_imageToBlur = imageToBlur.convertToFormat(QImage::Format_RGBA8888);

        initFBOTextures();
    }


    //Don't record the texture and FBO allocation time


    //Start the GPU timer
    glGenQueries(1, GPUTimerQueries);
    glBeginQuery(GL_TIME_ELAPSED, GPUTimerQueries[0]);

    //Start the CPU timer
    CPUTimer.start();

    // --------------- blur start ---------------

    m_ShaderProgram_kawase_down->setUniformValue("offset", QVector2D(offset, offset));
    m_ShaderProgram_kawase_up->setUniformValue("offset", QVector2D(offset, offset));

    //Initial downsample
    //We only need this helper texture because we can't put a QImage into the texture of a QOpenGLFramebufferObject
    //Otherwise we would skip this and start the downsampling from m_FBO_vector[0] instead of m_FBO_vector[1]
    renderToFBO(m_FBO_vector[1], m_textureToBlur->textureId(), m_ShaderProgram_kawase_down);

    //Downsample
    for (int i = 1; i < iterations; i++) {
        renderToFBO(m_FBO_vector[i + 1], m_FBO_vector[i]->texture(), m_ShaderProgram_kawase_down);
    }

    //Upsample
    for (int i = iterations; i > 0; i--) {
        renderToFBO(m_FBO_vector[i - 1], m_FBO_vector[i]->texture(), m_ShaderProgram_kawase_up);
    }

    // --------------- blur end ---------------

    //Get the CPU timer result
    CPUTimerElapsedTime = CPUTimer.nsecsElapsed();

    //Get the GPU timer result
    glEndQuery(GL_TIME_ELAPSED);
    GPUTimerAvailable = 0;

    while (!GPUTimerAvailable) {
        glGetQueryObjectiv(GPUTimerQueries[0], GL_QUERY_RESULT_AVAILABLE, &GPUTimerAvailable);
    }

    glGetQueryObjectui64v(GPUTimerQueries[0], GL_QUERY_RESULT, &GPUtimerElapsedTime);
    glDeleteQueries(1, GPUTimerQueries);

    return m_FBO_vector[0]->toImage();
}

void GLBlurFunctions::renderToFBO(QOpenGLFramebufferObject *targetFBO, GLuint sourceTexture, QOpenGLShaderProgram *shader)
{
    targetFBO->bind();
    glBindTexture(GL_TEXTURE_2D, sourceTexture);
    shader->bind();

    shader->setUniformValue("iResolution", QVector2D(targetFBO->size().width(), targetFBO->size().height()));
    shader->setUniformValue("halfpixel", QVector2D(0.5 / targetFBO->size().width(), 0.5 / targetFBO->size().height()));

    glViewport(0, 0, targetFBO->size().width(), targetFBO->size().height());
    glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(sg_vertexes) / sizeof(sg_vertexes[0]));
}

void GLBlurFunctions::initFBOTextures()
{
    for (int i = 0; i < m_FBO_vector.size(); i++) {
        delete m_FBO_vector[i];
    }

    m_FBO_vector.clear();
    m_FBO_vector.append(new QOpenGLFramebufferObject(m_imageToBlur.size(), QOpenGLFramebufferObject::CombinedDepthStencil, GL_TEXTURE_2D));

    for (int i = 1; i <= m_iterations; i++) {
        m_FBO_vector.append(new QOpenGLFramebufferObject(m_imageToBlur.size() / qPow(2, i), QOpenGLFramebufferObject::CombinedDepthStencil, GL_TEXTURE_2D));

        glBindTexture(GL_TEXTURE_2D, m_FBO_vector.last()->texture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    delete m_textureToBlur;

    m_textureToBlur = new QOpenGLTexture(m_imageToBlur.mirrored(), QOpenGLTexture::DontGenerateMipMaps);
    m_textureToBlur->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_textureToBlur->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
}

float GLBlurFunctions::getGPUTime()
{
    float gpuTime = GPUtimerElapsedTime / 1000000.;
    return roundf(gpuTime * 1000) / 1000;
}

float GLBlurFunctions::getCPUTime()
{
    float cpuTime = CPUTimerElapsedTime / 1000000.;
    return roundf(cpuTime * 1000) / 1000;
}
