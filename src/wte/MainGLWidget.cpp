#include "stdafx.h"

#include "wte/MainGLWidget.h"
#include <wt/RenderBuffer.h>

MainGLWidget::MainGLWidget(QWidget* parent, QGLContext* context) : QGLWidget(parent), mInitialized(false), mContext(context){
}

MainGLWidget* MainGLWidget::instantiate(QWidget* parent){
	MainGLWidget::instance = new MainGLWidget(parent, 0);

	return MainGLWidget::instance;
}
MainGLWidget::SubGLWidget* MainGLWidget::acquireSubGLWidget(){
	makeCurrent();

	wt::RenderTarget* target = new wt::RenderTarget;

	wt::gl::RenderBuffer* depthBuffer = new wt::gl::RenderBuffer;


	depthBuffer->create();
	depthBuffer->setStorage(GL_DEPTH_COMPONENT,
			// TODO
			640, 480);

	wt::Texture2D* texture = new wt::Texture2D;
	texture->create();
	texture->setData(
		640, 480,
		GL_RGB, GL_RGB, NULL, GL_UNSIGNED_BYTE, true);

	target->getFrameBuffer().create();

	
	target->getFrameBuffer().addAttachment(GL_COLOR_ATTACHMENT0, texture);
	target->getFrameBuffer().addAttachment(GL_DEPTH_ATTACHMENT, *depthBuffer);

	target->addBuffer(GL_COLOR_ATTACHMENT0);

	WT_ASSERT(target->getFrameBuffer().isComplete(), "Incomplete framebuffer");

	SubGLWidget* res = new SubGLWidget;
	res->depthBuffer = depthBuffer;
	res->texture = texture;
	res->target = target;

	target->unbind();
	texture->unbind();

	return res;
}

void MainGLWidget::initializeGL(){
	makeCurrent();

	GLenum r = glewInit();
	if(r != GLEW_OK){
		WT_THROW("Error initializing glew \"%s\"",
			(const char*)glewGetErrorString(r));
	}

	mRenderer.init(640, 480);
	mRenderer.setClearColor(wt::Color::gray());
	mRenderer.setRenderBoundingBoxes(true);
	
	mInitialized = true;

	emit initialized();
}

QGLContext* MainGLWidget::getContext(){
	return mContext;
}

MainGLWidget* MainGLWidget::getInstance(){
	return MainGLWidget::instance;
}

bool MainGLWidget::isInitialized() const{
	return mInitialized;
}

wt::Renderer* MainGLWidget::getRenderer(){
	return &mRenderer;
}

MainGLWidget* MainGLWidget::instance = NULL;