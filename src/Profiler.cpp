#include "Profiler.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"

using namespace perf;
using namespace ci;

ScopedTimer::ScopedTimer( const std::string& name )
: mName( name )
{
	Profiler::instance().start( mName );
}

ScopedTimer::~ScopedTimer()
{
	Profiler::instance().stop( mName );
}

std::unique_ptr<Profiler> Profiler::mInstance = nullptr;
std::once_flag Profiler::mOnceFlag;

Profiler& Profiler::instance()
{
	std::call_once( mOnceFlag,
				   [] {
					   mInstance.reset( new Profiler );
				   } );
	return *mInstance.get();
}

Profiler::Profiler()
: mCurrentFrame( 0 )
, mActiveQuery( false )
, mTotalFrameTime( 0 )
{
	app::App::get()->getSignalUpdate().connect( [this]()
	{
		mFrameTimer.stop();
		mTotalFrameTime = 1000.0 * mFrameTimer.getSeconds();
		mFrameTimer.start();
		
		auto fid = app::getElapsedFrames();
		
		for( auto kv : mCurrentTimes ) {
			auto& name = kv.first;
			if( ! mAverageTimes.count( name) ) {
				mAverageTimes[ name ] = kv.second;
			}
			//cumulative running averages
			mAverageTimes[ name ] = ( kv.second + double(fid) * mAverageTimes[ name ] ) / double( fid + 1 );
		}
	} );
}

Profiler::~Profiler()
{
}

Profiler::CpuGpuTimer::CpuGpuTimer()
: mCurrentFrame( 0 )
, mTimerGpu( gl::QueryTimeSwapped::create() )
{
	
}

void Profiler::CpuGpuTimer::start( uint32_t frame )
{
	if( mCurrentFrame == frame ) {
		CI_LOG_E( "Cannot reuse the same timer on the same frame." );
	}
	else {
		mTimerCpu.start();
		mTimerGpu->begin();
	}
	
	mCurrentFrame = frame;
}

glm::dvec2 Profiler::CpuGpuTimer::stop()
{
	mTimerCpu.stop();
	mTimerGpu->end();
	
	return dvec2( 1000.0 * mTimerCpu.getSeconds(), mTimerGpu->getElapsedMilliseconds() );
}

void Profiler::start( const std::string& name )
{
	CI_ASSERT_MSG( ! mActiveQuery, "Cannot profile inner scope since glBeginQuery can only be called once per active target." );
	mActiveQuery = true;
	
	auto frame = app::getElapsedFrames();
	if( ! mTimers.count( name ) ) {
		mTimers[name] = CpuGpuTimerPtr( new CpuGpuTimer );
	}
	mTimers.at( name )->start( frame );
}

void Profiler::stop( const std::string& name )
{
	if( mTimers.count( name ) ) {
		mCurrentTimes[ name ] = mTimers.at( name )->stop();
	}
	else {
		CI_ASSERT_MSG( false, "Scoped timer not contained in profile map at destruction." );
	}
	mActiveQuery = false;
}

const std::unordered_map<std::string, glm::dvec2>& perf::getAverageTimes()
{
	return Profiler::instance().getAverageTimes();
}

const std::unordered_map<std::string, glm::dvec2>& perf::getCurrentTimes()
{
	return Profiler::instance().getCurrentTimes();
}

double perf::getFrameTime()
{
	return Profiler::instance().getFrameTime();
}

#include "cinder/Utilities.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"

void perf::draw( const glm::vec2& offset )
{
#ifdef PERF_ENABLED
	TextLayout simple;
	simple.setColor( Color::white() );
	simple.addLine( "[ cpu time, gpu time ]" );
	vec2 total;
	for( auto kv : perf::getCurrentTimes() ) {
		simple.addLine( toString( kv.second ) + " : " + kv.first );
		total += kv.second;
	}
	simple.addLine( "----------------------------" );
	simple.addLine( toString( total ) );
	simple.addLine( "Frame total: " + toString( perf::getFrameTime() ) + " ms" );
	gl::draw( gl::Texture2d::create( simple.render( true, true ) ), offset );
#endif
}

