
#include "tjsCommHead.h"
#include "StorageIntf.h"
#include "EventIntf.h"
#include "SysInitImpl.h"
#include "WindowImpl.h"
#include "LayerBitmapIntf.h"
#include "LayerBitmapImpl.h"
#include "TickCount.h"
#include "Random.h"
#include "UtilStreams.h"
#ifndef _WIN32
#include "VirtualKey.h"
#endif
#include "Platform.h"
#include "Application.h"
#include "ScriptMgnIntf.h"
#include "WindowIntf.h"
#include "TVPWindow.h"
#include "VelocityTracker.h"
#include "SystemImpl.h"
#include "VideoOvlIntf.h"
#include "Exception.h"
#include <SDL.h>
#include <stdio.h>
#include <limits.h>
#include <wchar.h>
#include <string>
#include <locale>
#include <iostream>

class TVPWindowLayer;
static TVPWindowLayer *_lastWindowLayer, *_currentWindowLayer;

bool sdlProcessEvents();
bool sdlProcessEventsForFrames(int frames);

class TVPWindowLayer : public TTVPWindowForm {
protected:
	SDL_Window *window;

	TVPWindowLayer *_prevWindow, *_nextWindow;
	SDL_Texture* framebuffer;
	SDL_Renderer* renderer;
	tTJSNI_Window *TJSNativeInstance;
	bool hasDrawn = false;
	bool isBeingDeleted = false;

public:
	TVPWindowLayer(tTJSNI_Window *w)
	{
		_nextWindow = nullptr;
		_prevWindow = _lastWindowLayer;
		_lastWindowLayer = this;
		if (_prevWindow) {
			_prevWindow->_nextWindow = this;
		}
		if (!_currentWindowLayer) {
			_currentWindowLayer = this;
		}
		if (w) {
			TJSNativeInstance = w;
		}
		else {
			TJSNativeInstance = nullptr;
		}
		
		window = SDL_CreateWindow("Kirikiroid2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 640, 480);
		SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
	}

	virtual ~TVPWindowLayer() {
		if (_lastWindowLayer == this) _lastWindowLayer = _prevWindow;
		if (_nextWindow) _nextWindow->_prevWindow = _prevWindow;
		if (_prevWindow) _prevWindow->_nextWindow = _nextWindow;
		if (_currentWindowLayer == this) {
			_currentWindowLayer = _lastWindowLayer;
		}
		SDL_DestroyTexture(framebuffer);
		framebuffer = NULL;
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
		SDL_DestroyWindow(window);
		window = NULL;
	}

	static TVPWindowLayer *create(tTJSNI_Window *w) {
		TVPWindowLayer *ret = new TVPWindowLayer (w);
		return ret;
	}

	virtual void SetPaintBoxSize(tjs_int w, tjs_int h) override {
	}
	virtual bool GetFormEnabled() override {
		return SDL_GetWindowFlags(window) & SDL_WINDOW_SHOWN;

	}
	virtual void SetDefaultMouseCursor() override {
	}
	virtual void GetCursorPos(tjs_int &x, tjs_int &y) override {
		SDL_GetMouseState(&x, &y);
	}
	virtual void SetCursorPos(tjs_int x, tjs_int y) override {
		SDL_WarpMouseInWindow(window, x, y);
	}
	virtual void SetHintText(const ttstr &text) override {
	}
	virtual void SetAttentionPoint(tjs_int left, tjs_int top, const struct tTVPFont * font) override {
	}
	virtual void ZoomRectangle(
		tjs_int & left, tjs_int & top,
		tjs_int & right, tjs_int & bottom) override {
	}
	virtual void BringToFront() override {
		if (_currentWindowLayer != this) {
			if (_currentWindowLayer) {
				_currentWindowLayer->TJSNativeInstance->OnReleaseCapture();
			}
			_currentWindowLayer = this;
		}
		SDL_RaiseWindow(window);
	}
	virtual void ShowWindowAsModal() override {
		in_mode_ = true;
		BringToFront();
		modal_result_ = 0;
		while (this == _currentWindowLayer && !modal_result_) {
			sdlProcessEvents();
			if (::Application->IsTarminate()) {
				modal_result_ = mrCancel;
			} else if (modal_result_ != 0) {
				break;
			}
		}
		in_mode_ = false;
	}
	virtual bool GetVisible() override {
		return SDL_GetWindowFlags(window) & SDL_WINDOW_SHOWN;
	}
	virtual void SetVisible(bool bVisible) override {
		if (bVisible) {
			SDL_ShowWindow(window);
			BringToFront();
		}
		else {
			SDL_HideWindow(window);
			if (_currentWindowLayer == this) {
				_currentWindowLayer = _prevWindow ? _prevWindow : _nextWindow;
			}
		}
	}
	virtual const char *GetCaption() override {
		return SDL_GetWindowTitle(window);
	}
	virtual void SetCaption(const tjs_string & ws) override {
		SDL_SetWindowTitle(window, ttstr(ws).AsNarrowStdString().c_str());
	}
	virtual void SetWidth(tjs_int w) override {
		int h;
		SDL_GetWindowSize(window, NULL, &h);
		SDL_SetWindowSize(window, w, h);
		if (framebuffer) {
			SDL_DestroyTexture(framebuffer);
			framebuffer = NULL;
		}
		framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
	}
	virtual void SetHeight(tjs_int h) override {
		int w;
		SDL_GetWindowSize(window, &w, NULL);
		SDL_SetWindowSize(window, w, h);
		if (framebuffer) {
			SDL_DestroyTexture(framebuffer);
			framebuffer = NULL;
		}
		framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
	}
	virtual void SetSize(tjs_int w, tjs_int h) override {
		SDL_SetWindowSize(window, w, h);
		if (framebuffer) {
			SDL_DestroyTexture(framebuffer);
			framebuffer = NULL;
		}
		framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
	}
	virtual void GetSize(tjs_int &w, tjs_int &h) override {
		SDL_GetWindowSize(window, &w, &h);
	}
	virtual tjs_int GetWidth() const override {
		int w;
		SDL_GetWindowSize(window, &w, NULL);
		return w;
	}
	virtual tjs_int GetHeight() const override {
		int h;
		SDL_GetWindowSize(window, NULL, &h);
		return h;
	}
	virtual void GetWinSize(tjs_int &w, tjs_int &h) override {
		SDL_GetWindowSize(window, &w, &h);
	}
	virtual void SetZoom(tjs_int numer, tjs_int denom) override {
	}
	virtual void NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const class BitmapInfomation * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity) override {
		SDL_UpdateTexture(framebuffer, NULL, bits, bitmapinfo->GetBITMAPINFO()->bmiHeader.biWidth * 4);
	}
	virtual void Show() override {
		SDL_RenderCopyEx(renderer, framebuffer, NULL, NULL, 0, 0, SDL_FLIP_VERTICAL);
		SDL_RenderPresent(renderer);
		hasDrawn = true;
	}
	virtual void InvalidateClose() override {
		isBeingDeleted = true;
	}
	virtual bool GetWindowActive() override {
		return _currentWindowLayer == this;
	}
	bool Closing = false, ProgramClosing = false, CanCloseWork = false;
	bool in_mode_ = false; // is modal
	int modal_result_ = 0;
	enum CloseAction {
		caNone,
		caHide,
		caFree,
		caMinimize
	};
	void OnClose(CloseAction& action) {
		if (modal_result_ == 0)
			action = caNone;
		else
			action = caHide;

		if (ProgramClosing) {
			if (TJSNativeInstance) {
				if (TJSNativeInstance->IsMainWindow()) {
					// this is the main window
				} else 			{
					// not the main window
					action = caFree;
				}
				iTJSDispatch2 * obj = TJSNativeInstance->GetOwnerNoAddRef();
				TJSNativeInstance->NotifyWindowClose();
				obj->Invalidate(0, NULL, NULL, obj);
				TJSNativeInstance = NULL;
				SetVisible(false);
				isBeingDeleted = true;
			}
		}
	}
	bool OnCloseQuery() {
		// closing actions are 3 patterns;
		// 1. closing action by the user
		// 2. "close" method
		// 3. object invalidation

		if (TVPGetBreathing()) {
			return false;
		}

		// the default event handler will invalidate this object when an onCloseQuery
		// event reaches the handler.
		if (TJSNativeInstance && (modal_result_ == 0 ||
			modal_result_ == mrCancel/* mrCancel=when close button is pushed in modal window */)) {
			iTJSDispatch2 * obj = TJSNativeInstance->GetOwnerNoAddRef();
			if (obj) {
				tTJSVariant arg[1] = { true };
				static ttstr eventname(TJS_W("onCloseQuery"));

				if (!ProgramClosing) {
					// close action does not happen immediately
					if (TJSNativeInstance) {
						TVPPostInputEvent(new tTVPOnCloseInputEvent(TJSNativeInstance));
					}

					Closing = true; // waiting closing...
				//	TVPSystemControl->NotifyCloseClicked();
					return false;
				} else {
					CanCloseWork = true;
					TVPPostEvent(obj, obj, eventname, 0, TVP_EPT_IMMEDIATE, 1, arg);
					sdlProcessEvents(); // for post event
					// this event happens immediately
					// and does not return until done
					return CanCloseWork; // CanCloseWork is set by the event handler
				}
			} else {
				return true;
			}
		} else {
			return true;
		}
	}
	virtual void Close() override {
		// closing action by "close" method
		if (Closing) return; // already waiting closing...

		ProgramClosing = true;
		try {
			//tTVPWindow::Close();
			if (in_mode_) {
				modal_result_ = mrCancel;
			} 
			else if (OnCloseQuery()) {
				CloseAction action = caFree;
				OnClose(action);
				switch (action) {
				case caNone:
					break;
				case caHide:
					SetVisible(false);
					break;
				case caMinimize:
					//::ShowWindow(GetHandle(), SW_MINIMIZE);
					break;
				case caFree:
				default:
					isBeingDeleted = true;
					//::PostMessage(GetHandle(), TVP_EV_WINDOW_RELEASE, 0, 0);
					break;
				}
			}
		}
		catch (...) {
			ProgramClosing = false;
			throw;
		}
		ProgramClosing = false;
	}
	virtual void OnCloseQueryCalled(bool b) override {
		// closing is allowed by onCloseQuery event handler
		if (!ProgramClosing) {
			// closing action by the user
			if (b) {
				if (in_mode_)
					modal_result_ = 1; // when modal
				else
					SetVisible(false);  // just hide

				Closing = false;
				if (TJSNativeInstance) {
					if (TJSNativeInstance->IsMainWindow()) {
						// this is the main window
						iTJSDispatch2 * obj = TJSNativeInstance->GetOwnerNoAddRef();
						obj->Invalidate(0, NULL, NULL, obj);
					}
				} else {
					delete this;
				}
			} else {
				Closing = false;
			}
		} else {
			// closing action by the program
			CanCloseWork = b;
		}
	}
	virtual void InternalKeyDown(tjs_uint16 key, tjs_uint32 shift) override {
	}
	virtual void OnKeyUp(tjs_uint16 vk, int shift) override {
	}
	virtual void OnKeyPress(tjs_uint16 vk, int repeat, bool prevkeystate, bool convertkey) override {
	}
	virtual tTVPImeMode GetDefaultImeMode() const override {
		return ::imDisable;
	}
	virtual void SetImeMode(tTVPImeMode mode) override {
		switch (mode) {
		case ::imDisable:
		case ::imClose:
			break;
		case ::imOpen:

		default:
			break;
		}
	}
	virtual void ResetImeMode() override {
	}
	virtual void UpdateWindow(tTVPUpdateType type) override {
		if (TJSNativeInstance) {
			tTVPRect r;
			r.left = 0;
			r.top = 0;
			SDL_GetWindowSize(window, &r.right, &r.bottom);
			TJSNativeInstance->NotifyWindowExposureToLayer(r);
			TVPDeliverWindowUpdateEvents();
		}
	}
	virtual void SetVisibleFromScript(bool b) override {
		SetVisible(b);
	}
	virtual void SetUseMouseKey(bool b) override {
	}
	virtual bool GetUseMouseKey() const override {
		return false;
	}
	virtual void ResetMouseVelocity() override {
	}
	virtual void ResetTouchVelocity(tjs_int id) override {
	}
	virtual bool GetMouseVelocity(float& x, float& y, float& speed) const override {
		return false;
	}
	virtual void TickBeat() override {
	}
	Sint32 mouseLastX = 0;
	Sint32 mouseLastY = 0;
	void sdlRecvEvent(SDL_Event event) {
		if (isBeingDeleted) {
			delete this;
			return;
		}
		if (window && _prevWindow) {
			uint32_t windowID = SDL_GetWindowID(window);
			bool tryParentWindow = false;
			switch (event.type) {
				case SDL_DROPFILE:
				case SDL_DROPTEXT:
				case SDL_DROPBEGIN:
				case SDL_DROPCOMPLETE:
					tryParentWindow = event.drop.windowID != windowID;
					break;
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					tryParentWindow = event.key.windowID != windowID;
					break;
				case SDL_MOUSEMOTION:
					tryParentWindow = event.motion.windowID != windowID;
					break;
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					tryParentWindow = event.button.windowID != windowID;
					break;
				case SDL_MOUSEWHEEL:
					tryParentWindow = event.wheel.windowID != windowID;
					break;
				case SDL_TEXTEDITING:
					tryParentWindow = event.edit.windowID != windowID;
					break;
				case SDL_TEXTINPUT:
					tryParentWindow = event.text.windowID != windowID;
					break;
			}
			if (tryParentWindow) {
				if (!in_mode_) {
					_prevWindow->sdlRecvEvent(event);
				}
				return;
			}
		}
		if (window && hasDrawn) {
			if (TJSNativeInstance->CanDeliverEvents()) {
				switch (event.type) { 
					case SDL_MOUSEMOTION: {
						mouseLastX = event.motion.x;
						mouseLastY = event.motion.y;
						TVPPostInputEvent(new tTVPOnMouseMoveInputEvent(TJSNativeInstance, event.motion.x, event.motion.y, 0));
						break;
					}
					case SDL_MOUSEBUTTONDOWN: {
						tTVPMouseButton btn;
						bool hasbtn = false;
						switch(event.button.button) {
							case SDL_BUTTON_RIGHT:
								btn = tTVPMouseButton::mbRight;
								hasbtn = true;
								break;
							case SDL_BUTTON_MIDDLE:
								btn = tTVPMouseButton::mbMiddle;
								hasbtn = true;
								break;
							case SDL_BUTTON_LEFT:
								btn = tTVPMouseButton::mbLeft;
								hasbtn = true;
								break;
						}
						if (hasbtn) {
							mouseLastX = event.button.x;
							mouseLastY = event.button.y;
							TVPPostInputEvent(new tTVPOnMouseDownInputEvent(TJSNativeInstance, event.button.x, event.button.y, btn, 0));
						}
						break;
					}
					case SDL_MOUSEBUTTONUP: {
						tTVPMouseButton btn;
						bool hasbtn = false;
						switch(event.button.button) {
							case SDL_BUTTON_RIGHT:
								btn = tTVPMouseButton::mbRight;
								hasbtn = true;
								break;
							case SDL_BUTTON_MIDDLE:
								btn = tTVPMouseButton::mbMiddle;
								hasbtn = true;
								break;
							case SDL_BUTTON_LEFT:
								btn = tTVPMouseButton::mbLeft;
								hasbtn = true;
								break;
						}
						if (hasbtn) {
							mouseLastX = event.button.x;
							mouseLastY = event.button.y;
							TVPPostInputEvent(new tTVPOnClickInputEvent(TJSNativeInstance, event.button.x, event.button.y));
							TVPPostInputEvent(new tTVPOnMouseUpInputEvent(TJSNativeInstance, event.button.x, event.button.y, btn, 0));
						}
						break;
					}
					case SDL_KEYDOWN: {
						TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, event.key.keysym.sym, 0));
						break;
					}
					case SDL_KEYUP: {
						TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, event.key.keysym.sym, 0));
						break;
					}
					case SDL_MOUSEWHEEL: {
						TVPPostInputEvent(new tTVPOnMouseWheelInputEvent(TJSNativeInstance, 0, event.wheel.y, mouseLastX, mouseLastY));
						break;
					}
					case SDL_WINDOWEVENT_CLOSE:
					case SDL_QUIT: {
						TVPPostInputEvent(new tTVPOnCloseInputEvent(TJSNativeInstance));
					}
				}
			}
		}
	}
};

bool sdlProcessEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (_currentWindowLayer) {
			_currentWindowLayer->sdlRecvEvent(event);
		}
	}
	::Application->Run();
	return true;
}

bool sdlProcessEventsForFrames(int frames) {
	for (int i = 0; i < frames; ++i) {
		sdlProcessEvents();
	}
}

int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_EVERYTHING);

	_argc = argc;
	_wargv = new tjs_char*[argc];

	for (int i = 0; i < argc; i += 1) {
		const tjs_char* warg;
		if (!i)
			warg = ttstr(realpath(argv[i], NULL)).c_str();
		else
			warg = ttstr(argv[i]).c_str();

		tjs_char* warg_copy = new tjs_char[strlen(argv[i]) + 1];
		memcpy(warg_copy, warg, sizeof(tjs_char) * (strlen(argv[i]) + 1));
		_wargv[i] = warg_copy;
	}

	::Application = new tTVPApplication();
	::Application->StartApplication( _argc, _wargv );

    Uint32 startTime = 0;
    Uint32 endTime = 0;
    Uint32 delta = 0;
    short timePerFrame = 16; // miliseconds
    
	while (sdlProcessEvents()) {
		if (!startTime) {
            startTime = SDL_GetTicks(); 
        } else {
            delta = endTime - startTime; // how many ms for a frame
        }
        
        if (delta < timePerFrame) {
            SDL_Delay(timePerFrame - delta);
        }
        
        startTime = endTime;
        endTime = SDL_GetTicks();
	}
	SDL_Quit();
	return 0;
}

bool TVPGetKeyMouseAsyncState(tjs_uint keycode, bool getcurrent)
{
	if(keycode >= VK_LBUTTON && keycode <= VK_XBUTTON2 && keycode != VK_CANCEL)
	{
		Uint32 state = SDL_GetMouseState(NULL, NULL);
		switch (keycode) {
			case VK_LBUTTON:
				return state & SDL_BUTTON(SDL_BUTTON_LEFT);
			case VK_RBUTTON:
				return state & SDL_BUTTON(SDL_BUTTON_RIGHT);
			case VK_MBUTTON:
				return state & SDL_BUTTON(SDL_BUTTON_MIDDLE);
			case VK_XBUTTON1:
				return state & SDL_BUTTON(SDL_BUTTON_X1);
			case VK_XBUTTON2:
				return state & SDL_BUTTON(SDL_BUTTON_X2);
			default:
				return false;
		}
	}
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	return state[SDL_GetScancodeFromKey(keycode)];
}

bool TVPGetJoyPadAsyncState(tjs_uint keycode, bool getcurrent)
{
	return false;
}

void TVPRelinquishCPU() {
	SDL_Delay(0);
}

#include <string.h>

TTVPWindowForm *TVPCreateAndAddWindow(tTJSNI_Window *w) {
	TVPWindowLayer* ret = TVPWindowLayer::create(w);
	return ret;
}

void TVPConsoleLog(const ttstr &l, bool important) {
	fprintf(stderr, "%s", l.AsNarrowStdString().c_str());
}

namespace TJS {
	static const int MAX_LOG_LENGTH = 16 * 1024;
	void TVPConsoleLog(const tjs_char *l) {
		fprintf(stderr, "%s", ttstr(l).AsNarrowStdString().c_str());
	}

	void TVPConsoleLog(const tjs_nchar *format, ...) {
		va_list args;
		va_start(args, format);
		char buf[MAX_LOG_LENGTH];
		vsnprintf(buf, MAX_LOG_LENGTH - 3, format, args);
		fprintf(stderr, "%s", buf);
		va_end(args);
	}
}

tjs_uint32 TVPGetCurrentShiftKeyState()
{
	tjs_uint32 f = 0;

	return f;
}

ttstr TVPGetPlatformName()
{
	return "MacOS";
}

ttstr TVPGetOSName()
{
	return TVPGetPlatformName();
}

#define MK_SHIFT 4
#define MK_CONTROL 8
#define MK_ALT (0x20)
tjs_uint32 TVP_TShiftState_To_uint32(TShiftState state) {
	tjs_uint32 result = 0;
	if (state & MK_SHIFT) {
		result |= ssShift;
	}
	if (state & MK_CONTROL) {
		result |= ssCtrl;
	}
	if (state & MK_ALT) {
		result |= ssAlt;
	}
	return result;
}
TShiftState TVP_TShiftState_From_uint32(tjs_uint32 state){
	TShiftState result = 0;
	if (state & ssShift) {
		result |= MK_SHIFT;
	}
	if (state & ssCtrl) {
		result |= MK_CONTROL;
	}
	if (state & ssAlt) {
		result |= MK_ALT;
	}
	return result;
}

