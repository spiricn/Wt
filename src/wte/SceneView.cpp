#include "stdafx.h"

#include <qcursor.h>

#include <wt/Assets.h>
#include <wt/FrameBuffer.h>
#include <wt/AGameInput.h>

#include "wte/SceneView.h"

SceneView::SceneView(QWidget* parent) : QGLWidget(parent), mTimer(this), mInputGrabbed(false),
	mIsLooking(false), mScene(NULL), mFocused(false), mRenderer(NULL){

	setFocusPolicy(Qt::StrongFocus);

	installEventFilter(this);
}

void SceneView::setRenderer(wt::Renderer* renderer){
	mRenderer = renderer;
}

wt::Renderer* SceneView::getRenderer(){
	return mRenderer;
}

void SceneView::setScene(wt::Scene* scene){
	mScene = scene;
}

wt::Scene* SceneView::getScene(){
	return mScene;
}

bool SceneView::eventFilter(QObject *object, QEvent *event){
	if(event->type() == QEvent::FocusOut){
		mFocused = false;
	}
	else if(event->type() == QEvent::FocusIn){
		mFocused = true;
	}

	return false;
}


void SceneView::update(float dt){
	if(mFocused){
		// Camera movement
		glm::vec3 disp(
			wt::AGameInput::isKeyDown('A') ? 1.0f : wt::AGameInput::isKeyDown('D') ? -1.0f : 0.0f,
			wt::AGameInput::isKeyDown(32) ? 1.0f : wt::AGameInput::isKeyDown('C') ? -1.0f : 0.0f,
			wt::AGameInput::isKeyDown('W') ? 1.0f : wt::AGameInput::isKeyDown('S') ? -1.0f : 0.0f
			);

		float speed = 50;

		mScene->getCamera().move(disp*speed*dt);
	}

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

void SceneView::initializeGL() {
    makeCurrent();

	GLenum r = glewInit();
	if(r != GLEW_OK){
		WT_THROW("Error initializing glew \"%s\"",
			(const char*)glewGetErrorString(r));
	}

	mRenderer->init(640, 480);

	mRenderer->setClearColor(wt::Color::Gray());
	mRenderer->setRenderBoundingBoxes(true);


	emit initialized();
}

void SceneView::grabInput(bool state){
}

void SceneView::resizeGL(int w, int h) {
	makeCurrent();
	if(mRenderer){
		
		mRenderer->setViewPort(w <= 0 ? 1 : w, h <= 0 ? 1 : h);
	}	
}

void SceneView::paintGL(){
	if(mRenderer && mScene){
		makeCurrent();
		mRenderer->render(*mScene);
	}
}

void SceneView::mouseMoveEvent(QMouseEvent* evt){
	if(mIsLooking){
		float dx = evt->x()-width()/2.0f;
		float dy = evt->y()-height()/2.0f;
		QCursor::setPos(mapToGlobal(QPoint(width()/2, height()/2)));

		if(mScene){
			mScene->getCamera().yaw(-dx);
			mScene->getCamera().pitch(dy);
		}
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