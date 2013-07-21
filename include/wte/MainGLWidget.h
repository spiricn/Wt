#ifndef WTE_MAINGLWIDGET_H
#define WTE_MAINGLWIDGET_H

#include <wt/Scene.h>
#include <wt/Renderer.h>
#include <wt/RenderBuffer.h>
#include <wt/RenderTarget.h>

class MainGLWidget : public QGLWidget{
Q_OBJECT
public:
	wt::Renderer* getRenderer();

	QGLContext* getContext();

	static MainGLWidget* instantiate(QWidget* parent);

	bool isInitialized() const;

	static MainGLWidget* getInstance();

	
	struct SubGLWidget{
		wt::Texture2D* texture;
		wt::gl::RenderBuffer* depthBuffer;
		wt::RenderTarget* target;
	};

	SubGLWidget* acquireSubGLWidget();

protected:
	void initializeGL();

signals:
	void initialized();

private:
	

	MainGLWidget(QWidget* parent, QGLContext* context);

	MainGLWidget(const MainGLWidget&);

	QGLContext* mContext;
	wt::Renderer mRenderer;
	bool mInitialized;
	static MainGLWidget* instance;

}; // </MainGLWidget>

#endif // </WTE_MAINGLWIDGET_H>