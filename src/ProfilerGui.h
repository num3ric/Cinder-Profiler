#pragma once

#include "cinder/Timer.h"
#include "cinder/app/App.h"
#include "cinder/CinderImGui.h"

#include "Profiler.h"
#include <list>
#include <map>

namespace perf {
	class ProfilerGui {
	public:
		void update( float averageFps )
		{
#ifdef IMGUI_API
#if CI_PROFILING
			for( auto kv : perf::detail::globalCpuProfiler().getElapsedTimes() ) {
				mCpuTimes[kv.first] = kv.second;
			}

			for( auto kv : perf::detail::globalGpuProfiler().getElapsedTimes() ) {
				mGpuTimes[kv.first] = kv.second;
			}
#endif

			mTimer.stop();
			float t = 1000.0f * (float)mTimer.getSeconds();
			mTimer.start();

			mFrameTimes.push_back( t );
			if( mFrameTimes.size() > 50 )
				mFrameTimes.pop_front();

			float average = 0.0f;
			float max = 16.0f;
			std::vector<float> times;
			for( auto& t : mFrameTimes ) {
				average += t;
				if( t > max )
					max = t;
				times.push_back( t );
			}
			average /= (float)mFrameTimes.size();

			ImGui::PlotLines( "Frame Times", times.data(), (int)times.size(), 0, NULL, 0.0f, 100.0f, ImVec2( 300, 100 ) );
			ImGui::Text( " FPS: %.2f", averageFps );
			ImGui::SameLine();
			ImGui::Text( " Time: %.2f", average );
			ImGui::SameLine();
			ImGui::Text( " Max: %.2f", max );

#if CI_PROFILING
			ImGui::Text( "CPU: " );
			ImGui::Indent( 10.0f );
			for( auto kv : mCpuTimes ) {
				ImGui::Text( ( kv.first + " : %.2f" ).c_str(), kv.second );
				//ImGui::InputFloat( kv.first.c_str(), &kv.second );
			}

			ImGui::Indent( -10.0f );
			ImGui::Text( "GPU: " );
			ImGui::Indent( 10.0f );
			for( auto kv : mGpuTimes ) {
				ImGui::Text( ( kv.first + " : %.2f" ).c_str(), kv.second );
				//ImGui::InputFloat( kv.first.c_str(), &kv.second );
			}
#endif
#endif
		}
	private:
		ci::Timer						mTimer;
		std::list<float>				mFrameTimes;
		std::map<std::string, float>	mCpuTimes;
		std::map<std::string, float>	mGpuTimes;
	};
}