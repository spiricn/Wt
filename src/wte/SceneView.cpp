#include "stdafx.h"

#include <qcursor.h>

#include <wt/Assets.h>
#include <wt/FrameBuffer.h>
#include <wt/AGameInput.h>

#include "wte/SceneView.h"

SceneView::SceneView(QWidget* parent) : QGLWidget(parent), mTimer(this), mInputGrabbed(false),
	mIsLooking(false), mScene(NULL){
	connect(&mTimer, SIGNAL(timeout()),
		this, SLOT(onUpdateTimeout()));

	//mTimer.start(16);
	setFocusPolicy(Qt::StrongFocus);
}

void SceneView::update(float dt){
	glm::vec3 disp
		(
		wt::AGameInput::isKeyDown('A') ? 1.0f : wt::AGameInput::isKeyDown('D') ? -1.0f : 0.0f,
		wt::AGameInput::isKeyDown(32) ? 1.0f : wt::AGameInput::isKeyDown('C') ? -1.0f : 0.0f,
		wt::AGameInput::isKeyDown('W') ? 1.0f : wt::AGameInput::isKeyDown('S') ? -1.0f : 0.0f
		);

	

	float speed = 50;

	mScene->getCamera().move(disp*speed*dt);
	repaint();
}

void SceneView::keyPressEvent(QKeyEvent* evt){
#if 0
	glm::vec3 disp;

	if(evt->key() == Qt::Key_S){
		disp.z = -10.0f;
	}
	else if(evt->key() == Qt::Key_W){
		disp.z = 10.0f;
	}
	else if(evt->key() == Qt::Key_A){
		disp.x = -10.0f;
	}
	else if(evt->key() == Qt::Key_D){
		disp.x = 10.0f;
	}
	else if(evt->key() == Qt::Key_Space){
		disp.y = 10.0f;
	}
	else if(evt->key() == Qt::Key_C){
		disp.y = -10.0f;
	}
	
	mCamera.move( disp);
	update();
#endif
}

void SceneView::onUpdateTimeout(){
}

void SceneView::initializeGL() {
    makeCurrent();

	GLenum r = glewInit();
	if(r != GLEW_OK){
		WT_THROW("Error initializing glew \"%s\"",
			(const char*)glewGetErrorString(r));
	}

	//mRenderer.init(width(), height());
	mRenderer.init(1920, 1080);

	mRenderer.setClearColor(wt::Color::gray());
	mRenderer.setRenderBoundingBoxes(true);

	mScene->getCamera().setPosition(glm::vec3(20, 150, 20));
	mScene->getCamera().lookAt(glm::vec3(100, 30, 100));

	emit initialized();
}

void SceneView::grabInput(bool state){
	
}

void SceneView::resizeGL(int w, int h) {
	makeCurrent();
	mRenderer.setViewPort(w, h);
}

void SceneView::paintGL(){
	//wt::Gl::FrameBuffer::unbind(wt::Gl::FrameBuffer::DRAW);
	//static const GLenum defaultBfrs[] = {GL_BACK};
	//glDrawBuffers(1, defaultBfrs);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	mRenderer.render(*mScene);
}

void SceneView::mouseMoveEvent(QMouseEvent* evt){
	if(mIsLooking){
		float dx = evt->x()-width()/2.0f;
		float dy = evt->y()-height()/2.0f;
		QCursor::setPos(mapToGlobal(QPoint(width()/2, height()/2)));


		mScene->getCamera().yaw(-dx);
		mScene->getCamera().pitch(dy);
	}

	float dx = evt->x() - mLastX;
	float dy = evt->y() - mLastY;

	mLastX = evt->x();
	mLastY = evt->y();
	
	emit onMouseDrag(evt->x(), evt->y(), mMouseClicked);

	emit onMouseDrag(MouseDragEvent(evt->x(), evt->y(), dx, dy, mMouseClicked));
}

void SceneView::mousePressEvent(QMouseEvent* evt){
	mMouseClicked = evt->button();

	mLastX = evt->x();
	mLastY = evt->y();

	if(evt->button() == Qt::RightButton){
		// move the cursor to the middle of the widget
		QCursor::setPos(mapToGlobal(QPoint(width()/2, height()/2)));

		// hide cursor
		setCursor(QCursor(Qt::BlankCursor));

		mIsLooking = true;
	}

	emit onMouseDown(evt);
	
}

void SceneView::mouseReleaseEvent(QMouseEvent* evt){
	// unhide cursor
	setCursor(QCursor(Qt::ArrowCursor));
	mIsLooking = false;
}