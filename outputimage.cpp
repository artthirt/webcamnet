#include "outputimage.h"

#include <QMouseEvent>

OutputImage::OutputImage(QWidget *w)
    : QOpenGLWidget(w)
    , OpenGLFunctions()
{
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    mTimer.start(16);

    setAutoFillBackground(false);
}

void OutputImage::setImage(const cv::Mat &m)
{
    mImage = m;
    mIsUpdate = true;
    mIsTexUpdate = true;
}

void OutputImage::onTimeout()
{
    if(mIsTexUpdate && mRender){
        mIsTexUpdate = false;
        generateTexture();
    }

    if(mIsUpdate && mRender){
        mIsUpdate = false;
        update();
    }
}

void OutputImage::drawTexture()
{
    mModel.setToIdentity();
    mModel.translate(mTranslate.x(), -mTranslate.y(), -2);

    if(!mImage.empty()){
        float ar = 1. * width() / height();
        float arim = 1. * mImage.cols / mImage.rows;

        if(ar > arim){
            mModel.scale(mScale * arim, mScale * 1, 0);
        }else{
            mModel.scale(mScale * ar, mScale * ar/arim, 0);
        }
    }

    mMVP = mProj * mModel;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1, 0.1, 0.1, 1);

    setViewport(width(), height());

    mProg.bind();

    const float *mat = mMVP.data();
    glUniformMatrix4fv(mMvpInt, 1, false, mat);

    glUniform4fv(mRGBInt, 1, mRgb);

    glUniform1i(mGammaInt, mGamma);

    //showError(0);

    if(mTexGen){
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, mTexGen);
    }
    glEnableVertexAttribArray(mVecAttrib);
    glVertexAttribPointer(mVecAttrib, 3, GL_FLOAT, false, 0, mVecA.data());
    //showError(1);
    glEnableVertexAttribArray(mTexAttrib);
    glVertexAttribPointer(mTexAttrib, 2, GL_FLOAT, false, 0, mTexA.data());
    //showError(2);
    glDrawElements(GL_TRIANGLES, mIdx.size(), GL_UNSIGNED_INT, mIdx.data());
    //glDrawArrays(GL_LINE_STRIP, 0, mVecA.size()/3);
    //showError(3);
    glDisableVertexAttribArray(mVecAttrib);
    glDisableVertexAttribArray(mTexAttrib);
    glDisable(GL_TEXTURE_2D);

    mProg.release();
}

void OutputImage::generateTexture()
{
    if(mImage.empty())
        return;

    if(!mTexGen){
        glGenTextures(1, &mTexGen);
    }

    glBindTexture(GL_TEXTURE_2D, mTexGen);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    int ifmt = GL_R8;
    int fmt = GL_RED;
    int tp = GL_UNSIGNED_BYTE;

    int d = mImage.depth();
    int ch = mImage.channels();

    if(mImage.depth() == CV_8U && mImage.channels() == 3){
        ifmt = GL_RGB;
        fmt = GL_RGB;
        tp = GL_UNSIGNED_BYTE;
    }
    if(mImage.depth() == CV_16U && mImage.channels() == 1){
        ifmt = GL_R16;
        fmt = GL_RED;
        tp = GL_UNSIGNED_SHORT;
    }
    if(mImage.depth() == CV_16U && mImage.channels() == 3){
        ifmt = GL_RGB16;
        fmt = GL_RGB;
        tp = GL_UNSIGNED_SHORT;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, ifmt, mImage.cols, mImage.rows, 0, fmt, tp, mImage.data);
}

void OutputImage::setViewport(float w, float h)
{
    glViewport(0, 0, w, h);
    float ar = w/h;
    mProj.setToIdentity();
    mProj.frustum(-ar/2., ar/2., -1./2, 1./2, 1., 50.);
}

void OutputImage::initializeGL()
{
    QOpenGLWidget::initializeGL();
    OpenGLFunctions::initializeOpenGLFunctions();

    mVecA = {
        -1, -1, 0,
        -1, 1, 0,
        1, 1, 0,
        1, -1, 0
    };
    mTexA = {
        0, 1,
        0, 0,
        1, 0,
        1, 1,
    };
    mIdx = {
        0, 1, 2,
        2, 3, 0,
    };

    mProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/res/obj.vert");
    mProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/res/obj.frag");
    mProg.link();
    qDebug("Log:\n<<<\n%s\n>>>", mProg.log().toLatin1().data());

    mProg.bind();
    mVecAttrib = mProg.attributeLocation("aPos");
    mTexAttrib = mProg.attributeLocation("aTex");
    mMvpInt = mProg.uniformLocation("mvp");
    mTexSampler = mProg.uniformLocation("Tex");
    mGammaInt = mProg.uniformLocation("Gamma");

    mRGBInt = mProg.uniformLocation("uRgb");

    mIsInit = true;
}

void OutputImage::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);

    setViewport(w, h);
}

void OutputImage::paintGL()
{
    drawTexture();
}

void OutputImage::mousePressEvent(QMouseEvent *event)
{
    mLBDown = event->button() == Qt::LeftButton;
    mIsUpdate = true;
    mMPos = event->pos();
}

void OutputImage::mouseReleaseEvent(QMouseEvent *event)
{
    mLBDown = false;
    mIsUpdate = true;
    mMPos = event->pos();
}

void OutputImage::mouseMoveEvent(QMouseEvent *event)
{
    if(mLBDown){
        QPointF dlt = event->pos() - mMPos;
        mTranslate += dlt * 2./height();
        mIsUpdate = true;
        mMPos = event->pos();
    }
}

void OutputImage::wheelEvent(QWheelEvent *event)
{
    float dlt = event->delta();
    mScale += dlt/100;
    if(mScale < 1){
        mScale = 1;
        mTranslate = QPointF();
        mIsUpdate = true;
    }
}
