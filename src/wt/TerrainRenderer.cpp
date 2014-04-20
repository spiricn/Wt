#include "wt/stdafx.h"

#include "wt/TerrainRenderer.h"
#include "wt/ShaderFactory.h"
#include "wt/Terrain.h"

#define  TD_TRACE_TAG "TerrainRenderer"

namespace wt
{

TerrainRenderer::TerrainRenderer(bool bounds) : mBoundsRenderer(bounds){
}

bool TerrainRenderer::isDeferred() const{
	return !mBoundsRenderer;
}

void TerrainRenderer::create(){
	if(!mBoundsRenderer){
		LOGV("Compiling terrain shader...");

		ShaderFactory::createShader(mShader,
			"terrain_deferred.vp",
			"terrain_deferred.fp");

		mShader.bindAttribLocation(0, "inPosition");
		mShader.bindAttribLocation(1, "inTexture");
		mShader.bindAttribLocation(2, "inTiledTexture");
		mShader.bindAttribLocation(3, "inNormal");

		mShader.link();

		mShader.use();
		mShader.setUniformVal("texArray", 0);
		mShader.setUniformVal("map", 1);
	}
}

uint32_t nodesVisible = 0;

void TerrainRenderer::render(Scene* scene, Camera* camera, PassType pass, Texture2D* shadowMap){
	if(scene->getActorSet(ASceneActor::eTYPE_TERRAIN).empty()){
		return;
	}

	// TODO only rendering the first one
	Terrain* terrain = dynamic_cast<Terrain*>( (*scene->getActorSet(ASceneActor::eTYPE_TERRAIN).begin()) );

	if(mBoundsRenderer){
		render(scene, terrain->getRootNode() );
	}
	else{
		//// Terrain is not a shadow caster
		//if(pass != ePASS_NORMAL){
		//	return;
		//}

		// Render terrain
		mShader.use();

		glm::mat4x4 view;
		camera->getCameraMatrix(view);

		glm::mat4 model;
		terrain->getTransformable()->getTransformMatrix(model);

		mShader.setUniformVal("uModelMat", model);
		mShader.setUniformVal("uViewMat",view);
		mShader.setUniformVal("uProjMat", camera->getProjectionMatrix());

		glActiveTexture(GL_TEXTURE0);
		terrain->getTerrainTexture()->bind();

		glActiveTexture(GL_TEXTURE1);
		terrain->getMapTexture()->bind();

		// Enable backface culling
		gl( Enable(GL_CULL_FACE) );
		gl( CullFace(GL_BACK) );

		uint32_t prevNodesVisible = nodesVisible;
		nodesVisible = 0;
		render(scene, terrain->getRootNode() );

		if(prevNodesVisible != nodesVisible){
			LOGI("Rendered %d nodes", nodesVisible);
		}

		prevNodesVisible = nodesVisible;
	}
}

gl::ShaderProgram* TerrainRenderer::setupSceneLighting(){
	return &mShader;
}

void TerrainRenderer::render(Scene* scene, const TerrainNode* node){
	// frustum cull test
	if( scene->getCamera().contains(node->getBounds()) || mBoundsRenderer){
		// have we reached a leaf?
		if(!node->isLeaf()){
			// render children
			for(uint32_t i=0; i<4; i++){
				render(scene, &node->getChildren()[i]);
			}
		}
		else{
			if(mBoundsRenderer){
				gl( UseProgram(0) );

				// projection
				gl( MatrixMode(GL_PROJECTION) );
				gl( LoadIdentity() );
				gl( LoadMatrixf( glm::value_ptr( scene->getCamera().getProjectionMatrix() ) ) );

				// moidelview
				gl( MatrixMode(GL_MODELVIEW) );
				gl( LoadIdentity() );
				glm::mat4 mv;
				scene->getCamera().getCameraMatrix(mv);
				gl( LoadMatrixf( glm::value_ptr(mv) ) );

				gl( Disable(GL_DEPTH_TEST) );
				gl( Disable(GL_BLEND) );

				PxBounds3 bounds = node->getBounds();

				PxVec3 center = bounds.getCenter();
				PxVec3 extents = bounds.getExtents();

				gl( PolygonMode(GL_FRONT_AND_BACK, GL_LINE) );
				//gl( Color4f(clr.red, clr.green, clr.blue, clr.alpha) );

				glBegin(GL_QUADS);
					// bottom
					glVertex3f( center.x - extents.x, center.y - extents.y, center.z - extents.z);
					glVertex3f( center.x - extents.x, center.y - extents.y, center.z + extents.z);
					glVertex3f( center.x + extents.x, center.y - extents.y, center.z + extents.z);
					glVertex3f( center.x + extents.x, center.y - extents.y, center.z - extents.z);

					// top
					glVertex3f( center.x - extents.x, center.y + extents.y, center.z - extents.z);
					glVertex3f( center.x - extents.x, center.y + extents.y, center.z + extents.z);
					glVertex3f( center.x + extents.x, center.y + extents.y, center.z + extents.z);
					glVertex3f( center.x + extents.x, center.y + extents.y, center.z - extents.z);
				glEnd();

				glBegin(GL_LINES);
					glVertex3f( center.x - extents.x, center.y - extents.y, center.z - extents.z);
					glVertex3f( center.x - extents.x, center.y + extents.y, center.z - extents.z);

					glVertex3f( center.x + extents.x, center.y - extents.y, center.z + extents.z);
					glVertex3f( center.x + extents.x, center.y + extents.y, center.z + extents.z);

					glVertex3f( center.x + extents.x, center.y - extents.y, center.z - extents.z);
					glVertex3f( center.x + extents.x, center.y + extents.y, center.z - extents.z);

					glVertex3f( center.x - extents.x, center.y - extents.y, center.z + extents.z);
					glVertex3f( center.x - extents.x, center.y + extents.y, center.z + extents.z);
				glEnd();

				gl( PolygonMode(GL_FRONT_AND_BACK, GL_FILL) );
			}
			else{
				gl( Disable(GL_TEXTURE_2D) );
				node->render();
				++nodesVisible;
			}
		}
	}
}

}; // </wt>
