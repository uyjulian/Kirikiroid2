//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Thread base class
//---------------------------------------------------------------------------
#ifndef ThreadIntfH
#define ThreadIntfH
#include "tjsNative.h"

#if 0
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#endif
#include <functional>
#include <SDL.h>

//---------------------------------------------------------------------------
// tTVPThreadPriority
//---------------------------------------------------------------------------
enum tTVPThreadPriority
{
	ttpIdle, ttpLowest, ttpLower, ttpNormal, ttpHigher, ttpHighest, ttpTimeCritical
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// tTVPThread
//---------------------------------------------------------------------------
class tTVPThread
{
protected:
#if 0
	std::thread* Thread;
#endif
	SDL_Thread* Thread;
	SDL_threadID ThreadId;
private:
	bool Terminated;

#if 0
	std::mutex Mtx;
	std::condition_variable Cond;
#endif
	SDL_mutex *Mtx;
	SDL_cond *Cond;
	bool ThreadStarting;

#if 0
	void StartProc();
#endif
	static int StartProc(void * arg);

public:
	tTVPThread();
	virtual ~tTVPThread();

	bool GetTerminated() const { return Terminated; }
	void SetTerminated(bool s) { Terminated = s; }
	void Terminate() { Terminated = true; }

protected:
	virtual void Execute() {}

public:
	void StartTread();
#if 0
	void WaitFor() { if (Thread && Thread->joinable()) { Thread->join(); } }
#endif
	void WaitFor() { if (Thread) SDL_WaitThread(Thread, nullptr); Thread = nullptr; }

	SDL_ThreadPriority _Priority;
	tTVPThreadPriority GetPriority();
	void SetPriority(tTVPThreadPriority pri);

#if 0
	std::thread::native_handle_type GetHandle() { if(Thread) return Thread->native_handle(); else return reinterpret_cast<std::thread::native_handle_type>(nullptr); }
	std::thread::id GetThreadId() { if(Thread) return Thread->get_id(); else return std::thread::id(); }
#endif
	SDL_Thread* GetHandle() const { return Thread; } 	
	SDL_threadID GetThreadId() const { if (ThreadId) return ThreadId; else return SDL_ThreadID(); }  
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// tTVPThreadEvent
//---------------------------------------------------------------------------
class tTVPThreadEvent
{
#if 0
	std::mutex Mtx;
	std::condition_variable Cond;
#endif
	SDL_mutex *Mtx;
	SDL_cond *Cond;
	bool IsReady;

public:
#if 0
	tTVPThreadEvent() : IsReady(false) {}
	virtual ~tTVPThreadEvent() {}
#endif
	tTVPThreadEvent() : IsReady(false) { Mtx = SDL_CreateMutex(); Cond = SDL_CreateCond(); }
	virtual ~tTVPThreadEvent() { SDL_DestroyCond(Cond); SDL_DestroyMutex(Mtx); }

	void Set() {
#if 0
		{
			std::lock_guard<std::mutex> lock(Mtx);
			IsReady = true;
		}
		Cond.notify_all();
#endif
		SDL_LockMutex(Mtx);
		IsReady = true;
		SDL_CondBroadcast(Cond);
		SDL_UnlockMutex(Mtx);
	}
	/*
	void Reset() {
		std::lock_guard<std::mutex> lock(Mtx);
		IsReady = false;
	}
	*/
	bool WaitFor( tjs_uint timeout ) {
#if 0
		std::unique_lock<std::mutex> lk( Mtx );
		if( timeout == 0 ) {
			Cond.wait( lk, [this]{ return IsReady;} );
			IsReady = false;
			return true;
		} else {
			//std::cv_status result = Cond.wait_for( lk, std::chrono::milliseconds( timeout ) );
			//return result == std::cv_status::no_timeout;
			bool result = Cond.wait_for( lk, std::chrono::milliseconds( timeout ), [this]{ return IsReady;} );
			IsReady = false;
			return result;
		}
#endif
		SDL_LockMutex(Mtx);
		if( timeout == 0 ) {
			while (!IsReady) {
				SDL_CondWait(Cond, Mtx);
			}
			IsReady = false;
			SDL_UnlockMutex(Mtx);
			return true;
		} else {
			bool result = false;
			while (!IsReady) {
				int tmResult = SDL_CondWaitTimeout(Cond, Mtx, timeout);
				if (tmResult == SDL_MUTEX_TIMEDOUT) {
					result = IsReady;
					break;
				}
			}
			IsReady = false;
			SDL_UnlockMutex(Mtx);
			return result;
		}
	}
};
//---------------------------------------------------------------------------


/*[*/
const tjs_int TVPMaxThreadNum = 8;
#if 0
typedef void (TJS_USERENTRY *TVP_THREAD_TASK_FUNC)(void *);
typedef void * TVP_THREAD_PARAM;
#endif
typedef const std::function<void(int)> &TVP_THREAD_TASK_FUNC;
/*]*/

TJS_EXP_FUNC_DEF(tjs_int, TVPGetProcessorNum, ());
TJS_EXP_FUNC_DEF(tjs_int, TVPGetThreadNum, ());
TJS_EXP_FUNC_DEF(void, TVPBeginThreadTask, (tjs_int num));
#if 0
TJS_EXP_FUNC_DEF(void, TVPExecThreadTask, (TVP_THREAD_TASK_FUNC func, TVP_THREAD_PARAM param));
#endif
TJS_EXP_FUNC_DEF(void, TVPExecThreadTask, (int numThreads, TVP_THREAD_TASK_FUNC func));
TJS_EXP_FUNC_DEF(void, TVPEndThreadTask, ());

#endif
