#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "Profiler.h"
#include "ProfilerGui.h" 

using namespace ci;
using namespace ci::app;
using namespace std;

class ProfilerTestApp : public App {
public:
	ProfilerTestApp();
	void update() override;
	void draw() override;
private:
	gl::GlslProgRef			mNoiseShader;
};

ProfilerTestApp::ProfilerTestApp()
{
	mNoiseShader = gl::GlslProg::create( loadAsset( "pass.vert" ), loadAsset( "noise.frag" ) );

	getWindow()->getSignalPostDraw().connect( [this]() {
		perf::printProfiling();
	} );

	gl::getStockShader( gl::ShaderDef().color() )->bind();
}
void ProfilerTestApp::update()
{
}

void ProfilerTestApp::draw()
{
	CI_PROFILE( "Draw" );

	gl::clear();

	// Expensive CPU operation
	for( size_t i = 0; i < 20000; ++i ) {
		Rand::randVec3();
	}

	// Expensive GPU pass
	{
#if ! defined( CINDER_COCOA )
		CI_PROFILE_GPU( "Noise pass" );
#endif
		gl::ScopedGlslProg s( mNoiseShader );
		mNoiseShader->uniform( "uTime", (float)app::getElapsedSeconds() );
		gl::drawSolidRect( app::getWindowBounds() );
	}

	// More gpu work...
	gl::drawSolidCircle( app::getWindowSize()/2, 100.0f, 50000 );

	CI_CHECK_GL();
}

CINDER_APP( ProfilerTestApp, RendererGl, []( App::Settings * settings )
{
	settings->setWindowSize( ivec2( 1500, 900 ) );
} );


