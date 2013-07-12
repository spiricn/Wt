#include "StdAfx.h"
#include "QImageView.h"


QImageView::QImageView(QWidget *parent) : QGLWidget(parent){
}


QImageView::~QImageView(void){
}

void QImageView::initializeGL() {
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 0);

	glGenTextures(1, &mTexture);
}

void QImageView::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void QImageView::setImage(const wt::Image& image){
	glBindTexture(GL_TEXTURE_2D, mTexture);

	
	glTexImage2D(GL_TEXTURE_2D, 0, image.getNumComponents(),
		image.getWidth(), image.getHeigth(), 0,
		image.getFormat(), GL_UNSIGNED_BYTE, image.getData());

	update();
}

void QImageView::setImage(QImage& image){
	glBindTexture(GL_TEXTURE_2D, mTexture);

	
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		image.width(), image.height(), 0,
		GL_RGB, GL_UNSIGNED_BYTE, image.bits());

	update();
}

void QImageView::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBegin(GL_QUADS);

		glVertex2f(0, 0);
		glTexCoord2f(0, 1);

		glVertex2f(width(), 0);
		glTexCoord2f(0, 0);

		glVertex2f(width(), height());
		glTexCoord2f(1, 0);

		glVertex2f(0, height());
		glTexCoord2f(1, 1);

	glEnd();
}