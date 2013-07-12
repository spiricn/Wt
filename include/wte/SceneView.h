#ifndef WTE_SCENEVIEW_H
#define WTE_SCENEVIEW_H

#include "stdafx.h"

#include <QtOpenGL/QGLWidget>
#include <qtimer.h>

#include <wt/Scene.h>
#include <wt/Renderer.h>

struct MouseDragEvent{
	float x;
	float y;
	float dx;
	float dy;
	Qt::MouseButton button;

	MouseDragEvent(float x, float y, float dx, float dy, Qt::MouseButton button) : 
		x(x), y(y), dx(dx), dy(dy), button(button){
	}

}; // </MouseDragEvent>


class SceneView : public QGLWidget{
	Q_OBJECT;

private:
	bool mInputGrabbed;
	wt::Scene* mScene;
	wt::Renderer mRenderer;
	QTimer mTimer;
	bool mIsLooking;
	Qt::MouseButton mMouseClicked;

	float mLastX, mLastY;

public:

	SceneView(QWidget *parent);

	wt::Renderer& getRenderer(){
		return mRenderer;
	}

	void setScene(wt::Scene* scene){
		mScene = scene;
	}

	wt::Scene* getScene(){
		return mScene;
	}

	void grabInput(bool state);

	void update(float dt=0.0f);


signals:
	void initialized();

	void onMouseDown(QMouseEvent*);

	void onMouseDrag(float,float,Qt::MouseButton);

	void onMouseDrag(MouseDragEvent evt);

protected:
	void initializeGL();

    void resizeGL(int w, int h);

    void paintGL();

	void mouseMoveEvent(QMouseEvent* evt);

	void mousePressEvent(QMouseEvent* evt);

	void mouseReleaseEvent(QMouseEvent* evt);

	void keyPressEvent(QKeyEvent * evt);

protected slots:
	void onUpdateTimeout();

}; // </SceneView>

#endif // </WTE_SCENEVIEW_H>