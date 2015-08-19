#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#define CI_PROFILING // enables the scoped profiling calls
#include "Profiler.h"

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

	perf::CpuProfiler			mCpuProfiler;
	perf::GpuProfiler			mGpuProfiler;
};

ProfilerTestApp::ProfilerTestApp()
{
	mNoiseShader = gl::GlslProg::create( loadAsset( "pass.vert" ), loadAsset( "noise.frag" ) );

	getWindow()->getSignalPostDraw().connect( [this]() {
		if( app::getElapsedFrames() % 20 == 1 ) {

			app::console() << "[FPS] " << getAverageFps() << std::endl;

			app::console() << "[GPU times]" << std::endl;
			for( auto kv : mGpuProfiler.getElapsedTimes() ) {
				app::console() << "	" << kv.first << " : " << kv.second << std::endl;
			}

			app::console() << "[CPU times]" << std::endl;
			for( auto kv : mCpuProfiler.getElapsedTimes() ) {
				app::console() << "	" << kv.first << " : " << kv.second << std::endl;
			}
		}
	} );

	gl::getStockShader( gl::ShaderDef().color() )->bind();
}
void ProfilerTestApp::update()
{
}

void ProfilerTestApp::draw()
{
	CI_SCOPED_CPU( mCpuProfiler, "Draw" );
	CI_SCOPED_GPU( mGpuProfiler, "Draw" );

	gl::clear();

	// Expensive CPU operation
	for( size_t i = 0; i < 20000; ++i ) {
		Rand::randVec3();
	}

	// Expensive GPU pass
	{
		CI_SCOPED_GPU( mGpuProfiler, "Noise pass" );
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


