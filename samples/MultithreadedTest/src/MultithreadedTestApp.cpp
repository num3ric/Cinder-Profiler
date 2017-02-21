#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Thread.h"
#include "cinder/Rand.h"

#include "Profiler.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MultithreadedTestApp : public App {
  public:
	MultithreadedTestApp();
	~MultithreadedTestApp();
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
	
	void testThread( gl::ContextRef context );
	
	gl::Texture2dRef		mCurrentTexture;
	
	bool					mShouldQuit;
	shared_ptr<thread>		mThread;
};

MultithreadedTestApp::MultithreadedTestApp()
: mShouldQuit{ false }
, mCurrentTexture{ nullptr }
{
	getWindow()->getSignalPostDraw().connect( [this]() {
		perf::printProfiling();
	} );
	
	gl::ContextRef backgroundCtx = gl::Context::create( gl::context() );
	mThread = shared_ptr<thread>( new thread( bind( &MultithreadedTestApp::testThread, this, backgroundCtx ) ) );

}

MultithreadedTestApp::~MultithreadedTestApp()
{
	mShouldQuit = true;
	mThread->join();
}

void MultithreadedTestApp::testThread( gl::ContextRef context )
{
	ci::ThreadSetup threadSetup;
	context->makeCurrent();
	
	while( ! mShouldQuit ) {
		gl::Texture2dRef newTexture;
		{
			// FIXME: Call here breaks profiler!
			//CI_PROFILE( "Generating new texture" );
			Surface newSurface{ 600, 400, false };
			auto iter = newSurface.getIter();
			while( iter.line() ) {
				while( iter.pixel() ) {
					iter.r() = Rand::randInt(255);
					iter.g() = Rand::randInt(255);
					iter.b() = Rand::randInt(255);
				}
			}
			newTexture = gl::Texture2d::create( newSurface );
			// we need to wait on a fence before alerting the primary thread that the Texture is ready
			auto fence = gl::Sync::create();
			fence->clientWaitSync();
		}
		
		dispatchAsync( [this, newTexture]() {
			mCurrentTexture = newTexture;
		} );
	}
}

void MultithreadedTestApp::update()
{
}

void MultithreadedTestApp::draw()
{
	CI_PROFILE( "Draw" );
	
	gl::clear();
	
	if( mCurrentTexture )
		gl::draw( mCurrentTexture );
	
	CI_CHECK_GL();
}

void MultithreadedTestApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_ESCAPE ) {
		quit();
	}
}

CINDER_APP( MultithreadedTestApp, RendererGl )
