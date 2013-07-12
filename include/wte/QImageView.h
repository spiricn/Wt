#ifndef _H_IMAGE_VIEW
#define _H_IMAGE_VIEW

#include "stdafx.h"
#include <QtOpenGL/QGLWidget>
#include <wt/Image.h>


class QImageView : public QGLWidget{
	Q_OBJECT
public:
	QImageView(QWidget *parent);

	~QImageView(void);

	void setImage(QImage& image);

	void setImage(const wt::Image& image);
private:
	GLuint mTexture;

protected:
	void initializeGL();

    void resizeGL(int w, int h);

    void paintGL();
};

#endif