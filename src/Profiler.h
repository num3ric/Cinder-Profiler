#pragma once

#include "cinder/Timer.h"
#include "cinder/Noncopyable.h"
#include "cinder/gl/Query.h"

#include <unordered_map>
#include <string>

//! set to 0 to disable profiling
#ifndef CI_PROFILING
#define CI_PROFILING 1
#endif

#if CI_PROFILING
#define CI_PROFILE_CPU( name )	CI_SCOPED_CPU( perf::detail::globalCpuProfiler(), name ) 
#define CI_PROFILE_GPU( name )	CI_SCOPED_GPU( perf::detail::globalGpuProfiler(), name ) 
#define CI_PROFILE( name )		do { CI_PROFILE_CPU( name ); CI_PROFILE_GPU( name ); } while( 0 )

#define CI_SCOPED_CPU( profiler, name ) perf::ScopedCpuProfiler __ci_cpu_profile{ profiler, name }
#define CI_SCOPED_GPU( profiler, name ) perf::ScopedGpuProfiler __ci_gpu_profile{ profiler, name }
#else
#define CI_PROFILE_CPU( name )
#define CI_PROFILE_GPU( name )
#define CI_PROFILE( name )

#define CI_SCOPED_CPU( profiler, name )
#define CI_SCOPED_GPU( profiler, name )
#endif

#if defined( CINDER_DLL )
	#if defined( PERF_EXPORT )
		#define PERF_API __declspec(dllexport)
	#else
		#define PERF_API __declspec(dllimport)
	#endif
#else
	#define PERF_API
#endif

namespace perf {

#if defined( CINDER_MSW )
	typedef std::shared_ptr<class GpuTimer> GpuTimerRef;

	/* Taken from https://github.com/NVIDIAGameWorks/OpenGLSamples/blob/master/extensions/include/NvGLUtils/NvTimers.h
	* Use once per frame.
	*/
	class PERF_API GpuTimer : public ci::Noncopyable {
	public:
		static GpuTimerRef create();

		virtual ~GpuTimer();

		/// Resets the elapsed time and count of start/stop calls to zero
		void reset();

		/// Starts the timer (the next OpenGL call will be the first timed).
		/// This must be called from a thread with the OpenGL context bound
		void begin();

		/// Starts the timer (the previous OpenGL call will be the last timed).
		/// This must be called from a thread with the OpenGL context bound
		void end();

		/// Get the number of start/stop cycles whose values have been accumulated
		/// since the last reset.  This value may be lower than the number of call
		/// pairs to start/stop, as this value does not take into account the start/stop
		/// cycles that are still "in flight" (awaiting results).
		/// \return the number of start/stop cycles represented in the current
		/// accumulated time
		int32_t getIntervalCount() const;

		/// Get the amount of time accumulated for start/stop cycles that have completed
		/// since the last reset.  This value may be lower than the time for all call
		/// pairs to start/stop since the last reset, as this value does not take into
		/// account the start/stop cycles that are still "in flight" (awaiting results).
		/// \return the accumulated time of completed start/stop cycles since the last reset
		double getElapsedTime();
	protected:
		/// Creates a stopped, zeroed timer
		GpuTimer();

		void getResults();

		const static unsigned int TIMER_COUNT = 4;
		enum {
			TIMESTAMP_QUERY_BEGIN = 0,
			TIMESTAMP_QUERY_END,
			TIMESTAMP_QUERY_COUNT
		};

		GLuint		mQueries[TIMER_COUNT][TIMESTAMP_QUERY_COUNT];
		bool		mIsQueryInFlight[TIMER_COUNT];
		uint8_t		mIndex;

		double		mElapsedTime;
		int32_t		mIntervalCount;
	};
#else
	typedef ci::gl::QueryTimeSwapped	GpuTimer;
	typedef ci::gl::QueryTimeSwappedRef	GpuTimerRef;
#endif

	class PERF_API CpuProfiler : public ci::Noncopyable {
	public:
		void start( const std::string& timerName );
		void stop( const std::string& timerName );

		std::unordered_map<std::string, double> getElapsedTimes();
	private:
		std::unordered_map<std::string, ci::Timer> mTimers;
	};

	class PERF_API GpuProfiler : public ci::Noncopyable {
	public:
		void start( const std::string& timerName );
		void stop( const std::string& timerName );

		std::unordered_map<std::string, double> getElapsedTimes();
	private:
		std::unordered_map<std::string, GpuTimerRef> mTimers;

#if ! defined( CINDER_MSW )
		static bool		sActiveTimer;
#endif
	};


	struct ScopedCpuProfiler {
		ScopedCpuProfiler( CpuProfiler& profiler, const std::string& timerName )
			: mProfiler( &profiler ), mTimerName( timerName )
		{
			mProfiler->start( mTimerName );
		}
		~ScopedCpuProfiler()
		{
			mProfiler->stop( mTimerName );
		}
	private:
		CpuProfiler *	mProfiler;
		std::string		mTimerName;
	};


	struct ScopedGpuProfiler {
		ScopedGpuProfiler( GpuProfiler& profiler, const std::string& timerName )
			: mProfiler( &profiler ), mTimerName( timerName )
		{
			mProfiler->start( mTimerName );
		}
		~ScopedGpuProfiler()
		{
			mProfiler->stop( mTimerName );
		}
	private:
		std::string		mTimerName;
		GpuProfiler *	mProfiler;
	};

	PERF_API void printProfiling( uint32_t skippedFrameCount = 20 );
	PERF_API void printProfiling( perf::CpuProfiler& cpuProfiler, perf::GpuProfiler& gpuProfiler, uint32_t skippedFrameCount = 20 );

	namespace detail {
		PERF_API perf::CpuProfiler&	globalCpuProfiler();
		PERF_API perf::GpuProfiler&	globalGpuProfiler();
	}
}

