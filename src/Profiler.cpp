#include "Profiler.h"

#include "cinder/app/App.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"

using namespace perf;
using namespace ci;

#if defined( CINDER_MSW )
GpuTimerRef GpuTimer::create()
{
	return GpuTimerRef( new GpuTimer );
}

GpuTimer::~GpuTimer()
{
	if( mQueries[0] ) {
		glDeleteQueries( TIMER_COUNT * TIMESTAMP_QUERY_COUNT, mQueries[0] );
	}
}

void GpuTimer::reset() {
	mElapsedTime = 0.0;
	mIntervalCount = 0;
}

void GpuTimer::begin()
{
	getResults();  // add timings from previous start/stop if pending
	glQueryCounter( mQueries[mIndex][TIMESTAMP_QUERY_BEGIN], GL_TIMESTAMP );
}

void GpuTimer::end()
{
	glQueryCounter( mQueries[mIndex][TIMESTAMP_QUERY_END], GL_TIMESTAMP );
	mIsQueryInFlight[mIndex] = true;
}

int32_t GpuTimer::getIntervalCount() const
{
	return mIntervalCount;
}

double GpuTimer::getElapsedTime()
{
	getResults();

	return mElapsedTime;
}

GpuTimer::GpuTimer()
	: mElapsedTime( 0.f ), mIntervalCount( 0 ), mIndex( 0 )
{
	for( unsigned int i = 0; i < TIMER_COUNT; i++ ) {
		mIsQueryInFlight[i] = false;
	}

	glGenQueries( TIMER_COUNT*TIMESTAMP_QUERY_COUNT, mQueries[0] );
}

void GpuTimer::getResults() {
	// Make a pass over all timers - if any are pending results ("in flight"), then
	// grab the the time diff and add to the accumulator.  Then, mark the timer as
	// not in flight and record it as a possible next timer to use (since it is
	// now unused)
	int32_t freeQuery = -1;
	for( unsigned int i = 0; i < TIMER_COUNT; i++ ) {
		// Are we awaiting a result?
		if( mIsQueryInFlight[i] ) {
			GLuint available = false;
			glGetQueryObjectuiv( mQueries[i][TIMESTAMP_QUERY_END], GL_QUERY_RESULT_AVAILABLE, &available );

			// Is the result ready?
			if( available ) {
				uint64_t timeStart, timeEnd;
				glGetQueryObjectui64v( mQueries[i][TIMESTAMP_QUERY_BEGIN], GL_QUERY_RESULT, &timeStart );
				glGetQueryObjectui64v( mQueries[i][TIMESTAMP_QUERY_END], GL_QUERY_RESULT, &timeEnd );

				// add the time to the accumulator
				mElapsedTime += double( timeEnd - timeStart ) * 1.e-6;
				freeQuery = i;
				mIsQueryInFlight[i] = false;

				// Increment the counter of start/stop cycles
				mIntervalCount++;
			}
		}
	}
	// Use the newly-freed counters if one exists
	// otherwise, swap to the "next" counter
	if( freeQuery >= 0 ) {
		mIndex = freeQuery;
	}
	else {
		mIndex++;
		mIndex = (mIndex >= TIMER_COUNT) ? 0 : mIndex;
	}
}
#endif

void CpuProfiler::start( const std::string& timerName )
{
	if( !mTimers.count( timerName ) ) {
		mTimers.emplace( timerName, Timer( true ) );
	}
	else {
		CI_ASSERT( mTimers.at( timerName ).isStopped() );
		mTimers.at( timerName ).start();
	}
}


void CpuProfiler::stop( const std::string& timerName )
{
	CI_ASSERT( mTimers.count( timerName ) );
	CI_ASSERT( !mTimers.at( timerName ).isStopped() );
	mTimers.at( timerName ).stop();
}

std::unordered_map<std::string, double> CpuProfiler::getElapsedTimes()
{
	std::unordered_map<std::string, double> elapsedTimes;
	for( auto& kv : mTimers ) {
		auto& timer = kv.second;
		CI_ASSERT( timer.isStopped() );
		elapsedTimes[kv.first] = timer.getSeconds() * 1.e3;
	}
	return elapsedTimes;
}

#if ! defined( CINDER_MSW )
bool GpuProfiler::sActiveTimer = false;
#endif

void GpuProfiler::start( const std::string& timerName )
{
#if ! defined( CINDER_MSW )
	if( sActiveTimer ) {
		throw std::runtime_error( "Cannot use inner-scope gpu time queries." );
	}
	sActiveTimer = true;
#endif
	if( !mTimers.count( timerName ) ) {
		mTimers.emplace( timerName, GpuTimer::create() );
	}
	mTimers.at( timerName )->begin();
}

void GpuProfiler::stop( const std::string& timerName )
{
	CI_ASSERT( mTimers.count( timerName ) );
	mTimers.at( timerName )->end();
#if ! defined( CINDER_MSW )
	sActiveTimer = false;
#endif
}

std::unordered_map<std::string, double> GpuProfiler::getElapsedTimes()
{
	std::unordered_map<std::string, double> elapsedTimes;
	for( auto& kv : mTimers ) {
		auto& timer = kv.second;
#if defined( CINDER_MSW )
		if( timer->getIntervalCount() > 0 ) {
			auto time = timer->getElapsedTime() / timer->getIntervalCount();
			elapsedTimes[kv.first] = time;
			timer->reset();
		}
#else
		elapsedTimes[kv.first] = timer->getElapsedMilliseconds();
#endif
	}
	return elapsedTimes;
}

perf::CpuProfiler&	perf::detail::globalCpuProfiler()
{
	static perf::CpuProfiler sInstance;
	return sInstance;
}

perf::GpuProfiler&	perf::detail::globalGpuProfiler()
{
	static perf::GpuProfiler sInstance;
	return sInstance;
}

void perf::printProfiling( uint32_t skippedFrameCount )
{
#if CI_PROFILING
	perf::printProfiling( perf::detail::globalCpuProfiler(), perf::detail::globalGpuProfiler(), skippedFrameCount );
#endif // CI_PROFILING
}

void perf::printProfiling( perf::CpuProfiler& cpuProfiler, perf::GpuProfiler& gpuProfiler, uint32_t skippedFrameCount )
{
#if CI_PROFILING
	bool print = (skippedFrameCount) ? (app::getElapsedFrames() % skippedFrameCount == 1) : true;
	if( print ) {
		app::console() << "[FPS] " << app::AppBase::get()->getAverageFps() << std::endl;
		cinder::app::console() << "[GPU times]" << std::endl;
		for( auto kv : gpuProfiler.getElapsedTimes() ) {
			cinder::app::console() << "	" << kv.first << " : " << kv.second << std::endl;
		}
		cinder::app::console() << "[CPU times]" << std::endl;
		for( auto kv : cpuProfiler.getElapsedTimes() ) {
			cinder::app::console() << "	" << kv.first << " : " << kv.second << std::endl;
		}
	}
#endif // CI_PROFILING
}


