#pragma once

#include "cinder/CinderImGui.h"
#include "cinder/Timer.h"
#include "cinder/app/App.h"

#include "Profiler.h"
#include <list>
#include <map>

namespace perf {
class ProfilerGui {
  public:
	ProfilerGui( size_t windowSize = 50 )
		: mWindowSize{ windowSize }
		, mFrameTimes{ this }
	{
	}

	void update( float averageFps )
	{
#ifdef IMGUI_API
#if CI_PROFILING
		for( const auto& kv : perf::detail::globalCpuProfiler().getElapsedTimes() ) {
			if( mCpuTimes.count( kv.first ) ) {
				mCpuTimes[kv.first].add( kv.second );
			}
			else {
				mCpuTimes[kv.first] = RecordHistory{ this };
			}
		}

		for( const auto& kv : perf::detail::globalGpuProfiler().getElapsedTimes() ) {
			if( mGpuTimes.count( kv.first ) ) {
				mGpuTimes[kv.first].add( kv.second );
			}
			else {
				mGpuTimes[kv.first] = RecordHistory{ this };
			}
		}
#endif
		mTimer.stop();
		mFrameTimes.add( 1000.0f * (float)mTimer.getSeconds() );
		mTimer.start();

		std::vector<float> times   = mFrameTimes.getVector();
		ImGui::PlotLines( "Frame Times", times.data(), (int)times.size(), 0, NULL, 0.0f, 100.0f, ImVec2( 300, 100 ) );
		ImGui::Text( " FPS: %.2f", averageFps );
		ImGui::Text( "%.2f [%.2f max] (ms)", mFrameTimes.getAverage(), mFrameTimes.getMax() );

#if CI_PROFILING
		ImGui::Text( "CPU: " );
		ImGui::Indent( 10.0f );
		for( auto kv : mCpuTimes ) {
			ImGui::Text( ( "%.2f [%.2f max] " + kv.first ).c_str(), kv.second.getAverage(), kv.second.getMax() );
		}

		ImGui::Indent( -10.0f );
		ImGui::Text( "GPU: " );
		ImGui::Indent( 10.0f );
		for( auto kv : mGpuTimes ) {
			ImGui::Text( ( "%.2f [%.2f max] " + kv.first ).c_str(), kv.second.getAverage(), kv.second.getMax() );
		}
#endif
#endif
	}

  private:
	struct RecordHistory {
		RecordHistory() = default;
		RecordHistory( ProfilerGui* parent )
			: mParent{ parent }
			, mAverage{ 0.0f }
			, mMax{ 0.0f }
			, mActive{ true }
		{
		}

		void add( float value )
		{
			mHistory.push_back( value );
			if( mHistory.size() > mParent->mWindowSize )
				mHistory.pop_front();

			mAverage = 0.0f;
			mMax	 = 0.0f;
			for( const auto& value : mHistory ) {
				mAverage += value;
				if( value > mMax )
					mMax = value;
			}
			mAverage /= (float)mHistory.size();
			mActive = true;
		}

		std::vector<float> getVector() const
		{
			std::vector<float> result;
			result.reserve( mHistory.size() );
			for( auto& t : mHistory ) {
				result.push_back( t );
			}
			return result;
		}

		void  setIsActive( bool active ) { mActive = active; }
		bool  getIsActive() const { return mActive; }

		float getCurrent() const { return mHistory.back(); }
		float getAverage() const { return mAverage; }
		float getMax() const { return mMax; }

		ProfilerGui*	 mParent;
		float			 mAverage, mMax;
		std::list<float> mHistory;
		bool			 mActive;
	};

	ci::Timer							 mTimer;
	size_t								 mWindowSize;
	RecordHistory						 mFrameTimes;
	std::map<std::string, RecordHistory> mCpuTimes;
	std::map<std::string, RecordHistory> mGpuTimes;
};
} // namespace perf