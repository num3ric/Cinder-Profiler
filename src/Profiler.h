#pragma once

#include "cinder/gl/Query.h"
#include "cinder/Timer.h"

#define PERF_ENABLED

#ifdef PERF_ENABLED
#define CI_PROFILE( name ) perf::ScopedTimer timer{ name }
#else
#define CI_PROFILE( name )
#endif

#include <unordered_map>

//TODO: Allow for CPU and/or GPU selection

namespace perf {
	
	class ScopedTimer {
	public:
		ScopedTimer( const std::string& name );
		~ScopedTimer();
	private:
		std::string mName;
	};
	
	class Profiler : public ci::Noncopyable {
	public:
		~Profiler();
		static Profiler& instance();
		double getFrameTime() const { return mTotalFrameTime; }
		
		const std::unordered_map<std::string, glm::dvec2>& getAverageTimes() const { return mAverageTimes; }
		const std::unordered_map<std::string, glm::dvec2>& getCurrentTimes() const { return mCurrentTimes; }
	protected:
		Profiler();
		static std::once_flag				mOnceFlag;
		static std::unique_ptr<Profiler>	mInstance;
		
		class CpuGpuTimer {
		public:
			CpuGpuTimer();
			void		start( uint32_t frame );
			glm::dvec2	stop();
		private:
			ci::gl::QueryTimeSwappedRef	mTimerGpu;
			ci::Timer	mTimerCpu;
			uint32_t	mCurrentFrame = 0;
		};
		typedef std::unique_ptr<CpuGpuTimer> CpuGpuTimerPtr;
		
		void start( const std::string& name );
		void stop( const std::string& name );
		
		uint32_t	mCurrentFrame;
		bool		mActiveQuery;
		double		mTotalFrameTime;
		ci::Timer	mFrameTimer;
		
		std::unordered_map<std::string, CpuGpuTimerPtr> mTimers;
		
		std::unordered_map<std::string, glm::dvec2> mCurrentTimes;
		std::unordered_map<std::string, glm::dvec2> mAverageTimes;
		friend class ScopedTimer;		
	};
	
	const std::unordered_map<std::string, glm::dvec2>& getAverageTimes();
	const std::unordered_map<std::string, glm::dvec2>& getCurrentTimes();
	double getFrameTime();
	
	void draw( const glm::vec2& offset = glm::vec2(0) );
}

