
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
#include "vkdefine.h"
#include "Platform.h"
#include "Application.h"
#include "ScriptMgnIntf.h"
#include "win32/TVPWindow.h"
#include "VelocityTracker.h"
#include "SystemImpl.h"
#include "RenderManager.h"
#include "VideoOvlIntf.h"
#include "Exception.h"
#include "win32/MenuItemImpl.h"
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include <SDL.h>

#include <unistd.h>
#include <stdio.h>
#include <limits.h>



#include <wchar.h>

#include <string> 
#include <locale> 
#include <codecvt> 


class TVPWindowLayer;

// static TVPWindowManagerOverlay *_windowMgrOverlay = nullptr;
// static TVPConsoleWindow* _consoleWin = nullptr;
static float _touchMoveThresholdSq;
static float _mouseCursorScale;
// static Vec2 _mouseTouchPoint, _mouseBeginPoint;
// static std::set<Touch*> _mouseTouches;
static tTVPMouseButton _mouseBtn;
static int _touchBeginTick;
static bool _virutalMouseMode = false;
static bool _mouseMoved, _mouseClickedDown;
static tjs_uint8 _scancode[0x200];
static tjs_uint16 _keymap[0x200];
static TVPWindowLayer *_lastWindowLayer, *_currentWindowLayer;

bool sdlProcessEvents();
bool sdlProcessEventsForFrames(int frames);

#include <iostream>

class TVPWindowLayer : public iWindowLayer {
protected:
	tTVPMouseCursorState MouseCursorState = mcsVisible;
	tjs_int HintDelay = 500;
	tjs_int ZoomDenom = 1; // Zooming factor denominator (setting)
	tjs_int ZoomNumer = 1; // Zooming factor numerator (setting)
	double TouchScaleThreshold = 5, TouchRotateThreshold = 5;
	SDL_Window *window;

	
	tjs_int ActualZoomDenom; // Zooming factor denominator (actual)
	tjs_int ActualZoomNumer; // Zooming factor numerator (actual)
	// Sprite *DrawSprite = nullptr;
	// Node *PrimaryLayerArea = nullptr;
	int LayerWidth = 0, LayerHeight = 0;
	//iTVPTexture2D *DrawTexture = nullptr;
	TVPWindowLayer *_prevWindow, *_nextWindow;
	friend class TVPWindowManagerOverlay;
	friend class TVPMainScene;
	int _LastMouseX = 0, _LastMouseY = 0;
	std::string _caption;
//	std::map<tTJSNI_BaseVideoOverlay*, Sprite*> _AllOverlay;
	float _drawSpriteScaleX = 1.0f, _drawSpriteScaleY = 1.0f;
	float _drawTextureScaleX = 1.f, _drawTextureScaleY = 1.f;
	bool UseMouseKey = false, MouseLeftButtonEmulatedPushed = false, MouseRightButtonEmulatedPushed = false;
	bool LastMouseMoved = false, Visible = false;
	tjs_uint32 LastMouseKeyTick = 0;
	tjs_int MouseKeyXAccel = 0;
	tjs_int MouseKeyYAccel = 0;
	int LastMouseDownX = 0, LastMouseDownY = 0;
	VelocityTrackers TouchVelocityTracker;
	VelocityTracker MouseVelocityTracker;
	static const int TVP_MOUSE_MAX_ACCEL = 30;
	static const int TVP_MOUSE_SHIFT_ACCEL = 40;
	static const int TVP_TOOLTIP_SHOW_DELAY = 500;
	SDL_Texture* framebuffer;
	// SDL_Texture* framebuffer;
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
		// ActualZoomDenom = 1;
		// ActualZoomNumer = 1;
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
		
		window = SDL_CreateWindow("An SDL2 window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 640, 480);
		SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
		// SDL_ShowWindow(window);
	}

	virtual ~TVPWindowLayer() {
		if (_lastWindowLayer == this) _lastWindowLayer = _prevWindow;
		if (_nextWindow) _nextWindow->_prevWindow = _prevWindow;
		if (_prevWindow) _prevWindow->_nextWindow = _nextWindow;

		if (_currentWindowLayer == this) {
			// TVPWindowLayer *anotherWin = _lastWindowLayer;
			// while (anotherWin && !anotherWin->isVisible()) {
			// 	anotherWin = anotherWin->_prevWindow;
			// }
			// if (anotherWin && anotherWin->isVisible()) {
			// 	anotherWin->setPosition(0, 0);
			// 	//anotherWin->setVisible(true);
			// }
			_currentWindowLayer = _lastWindowLayer;
		}
		SDL_DestroyTexture(framebuffer);
		framebuffer = NULL;
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
		SDL_DestroyWindow(window);
		window = NULL;

	}

	bool init() {
		// bool ret = inherit::init();
		// setClippingToBounds(false);
		// DrawSprite = Sprite::create();
		// DrawSprite->setAnchorPoint(Vec2(0, 1)); // top-left
		// PrimaryLayerArea = Node::create();
		// addChild(PrimaryLayerArea);
		// PrimaryLayerArea->addChild(DrawSprite);
		// setAnchorPoint(Size::ZERO);
		// EventListenerMouse *evmouse = EventListenerMouse::create();
		// evmouse->onMouseScroll = std::bind(&TVPWindowLayer::onMouseScroll, this, std::placeholders::_1);
		// evmouse->onMouseDown = std::bind(&TVPWindowLayer::onMouseDownEvent, this, std::placeholders::_1);
		// evmouse->onMouseUp = std::bind(&TVPWindowLayer::onMouseUpEvent, this, std::placeholders::_1);
		// evmouse->onMouseMove = std::bind(&TVPWindowLayer::onMouseMoveEvent, this, std::placeholders::_1);
		// _eventDispatcher->addEventListenerWithSceneGraphPriority(evmouse, this);
		// setTouchEnabled(false);
		// //_touchListener->setSwallowTouches(true);
		// setVisible(false);
		

		return true;
	}

	static TVPWindowLayer *create(tTJSNI_Window *w) {
		TVPWindowLayer *ret = new TVPWindowLayer (w);
		ret->init();
		// ret->autorelease();
		return ret;
	}

	virtual void SetPaintBoxSize(tjs_int w, tjs_int h) override {
		// LayerWidth = w; LayerHeight = h;
		// RecalcPaintBox();
	}
	virtual bool GetFormEnabled() override {
		return SDL_GetWindowFlags(window) & SDL_WINDOW_SHOWN;

	}
	virtual void SetDefaultMouseCursor() override {

	}
	virtual void GetCursorPos(tjs_int &x, tjs_int &y) override {
		SDL_GetMouseState(&x, &y);
		// x = _LastMouseX;
		// y = _LastMouseY;
	}
	virtual void SetCursorPos(tjs_int x, tjs_int y) override {
		SDL_WarpMouseInWindow(window, x, y);
		// _LastMouseX = x;
		// _LastMouseY = y;
	}
	virtual void SetHintText(const ttstr &text) override {

	}
	// tjs_int _textInputPosY;
	virtual void SetAttentionPoint(tjs_int left, tjs_int top, const struct tTVPFont * font) override {
		// _textInputPosY = top;
	}
	virtual void ZoomRectangle(
		tjs_int & left, tjs_int & top,
		tjs_int & right, tjs_int & bottom) override {

	}
	virtual void BringToFront() override {
		// left = tjs_int64(left) * ActualZoomNumer / ActualZoomDenom;
		// top = tjs_int64(top) * ActualZoomNumer / ActualZoomDenom;
		// right = tjs_int64(right) * ActualZoomNumer / ActualZoomDenom;
		// bottom = tjs_int64(bottom) * ActualZoomNumer / ActualZoomDenom;
		if (_currentWindowLayer != this) {
			if (_currentWindowLayer) {
				// const Size &size = _currentWindowLayer->getViewSize();
				// _currentWindowLayer->setPosition(Vec2(size.width, 0));
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
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert; 
		std::string s = convert.to_bytes(ws.c_str());  
		SDL_SetWindowTitle(window, s.c_str());
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
	virtual void UpdateDrawBuffer(iTVPTexture2D *tex) override {
		if (!tex) return;
		//TODO: Update and swap SDL
		// SDL_LockTexture(framebuffer, NULL, NULL, NULL);
		SDL_UpdateTexture(framebuffer, NULL, tex->GetPixelData(), tex->GetPitch());
		// SDL_UnlockTexture(framebuffer);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
		SDL_RenderPresent(renderer);
		hasDrawn = true;
	}
#if 0
	virtual void AddOverlay(tTJSNI_BaseVideoOverlay *ovl) = 0;
	virtual void RemoveOverlay(tTJSNI_BaseVideoOverlay *ovl) = 0;
	virtual void UpdateOverlay() = 0;
#endif
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
				//if (TVPFullScreenedWindow != this) {
					// if this is not a fullscreened window
				//	SetVisible(false);
				//}
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
	tTVPImeMode LastSetImeMode = ::imDisable;
	tTVPImeMode DefaultImeMode = ::imDisable;
	virtual tTVPImeMode GetDefaultImeMode() const override {
		return DefaultImeMode;
	}
	virtual void SetImeMode(tTVPImeMode mode) override {
		switch (mode) {
		case ::imDisable:
		case ::imClose:
			break;
		case ::imOpen:
			//TVPMainScene::GetInstance()->attachWithIME();
			//break;
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

	// void SetZoomNumer(tjs_int n) { SetZoom(n, ZoomDenom); }
	// tjs_int GetZoomNumer() const { return ZoomNumer; }
	// void SetZoomDenom(tjs_int d) { SetZoom(ZoomNumer, d); }
	// tjs_int GetZoomDenom() const { return ZoomDenom; }

	// dummy function
	// TODO
};




//TODO: Frame update use tTVPApplication::Run, then iTVPTexture2D::RecycleProcess
//TODO: Rendering: On every frame copy from GetAdapterTexture
//TODO: Startup game at path ::Application->StartApplication(path);

bool sdlProcessEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (_currentWindowLayer) {
			_currentWindowLayer->sdlRecvEvent(event);
		}
	}
	::Application->Run();
	iTVPTexture2D::RecycleProcess();
	return true;
}

bool sdlProcessEventsForFrames(int frames) {
	for (int i = 0; i < frames; ++i) {
		sdlProcessEvents();
	}
}

int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
	char cwd[PATH_MAX];

	tjs_char** wargv = reinterpret_cast<tjs_char**>(malloc(sizeof(tjs_char*) * argc));

	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		strncat(cwd, "/", PATH_MAX);
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
		for (int i = 0; i < argc; i += 1) {
			tjs_char* warg;
			if (!i)
				warg = const_cast<tjs_char*>(convert.from_bytes(realpath(argv[i], NULL)).c_str());
			else
				warg = const_cast<tjs_char*>(convert.from_bytes(argv[i]).c_str());
			tjs_char* warg_copy = reinterpret_cast<tjs_char*>(malloc(sizeof(tjs_char) * (strlen(argv[i]) + 1)));
			memcpy(warg_copy, warg, sizeof(tjs_char) * (strlen(argv[i]) + 1));
			wargv[i] = warg_copy;
		}
		_argc = argc;
		_wargv = wargv;
		::Application = new tTVPApplication();
		::Application->StartApplication( _argc, _wargv );
	}

    Uint32 startTime = 0;
    Uint32 endTime = 0;
    Uint32 delta = 0;
    short timePerFrame = 16; // miliseconds
    
	while (sdlProcessEvents()) {
		// TVPRelinquishCPU();
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
	// if (keycode >= sizeof(_scancode) / sizeof(_scancode[0])) return false;
	// tjs_uint8 code = _scancode[keycode];
	// _scancode[keycode] &= 1;
	// return code & (getcurrent ? 1 : 0x10);
	return false;
}



#undef st_atime
#undef st_ctime
#undef st_mtime

//int stat64(const char* __path, struct stat64* __buf) __INTRODUCED_IN(21); // force link it !
bool TVP_stat(const char *name, tTVP_stat &s) {
	struct stat t;
	// static_assert(sizeof(t.st_size) == 4, "");
	// static_assert(sizeof(t.st_size) == 8, "");
	bool ret = !stat(name, &t);
	s.st_mode = t.st_mode;
	s.st_size = t.st_size;
	s.st_atime = t.st_atimespec.tv_sec;
	s.st_mtime = t.st_mtimespec.tv_sec;
	s.st_ctime = t.st_ctimespec.tv_sec;
	return ret;
}


bool TVP_stat(const tjs_char *name, tTVP_stat &s) {
	tTJSNarrowStringHolder holder(name);
	return TVP_stat(holder, s);
}

// tjs_uint32 TVPGetRoughTickCount32()
// {
// 	// tjs_uint32 uptime = 0;
// 	// struct timespec on;
// 	// if (clock_gettime(CLOCK_MONOTONIC, &on) == 0)
// 	// 	uptime = on.tv_sec * 1000 + on.tv_nsec / 1000000;
// 	// return uptime;
// 	return SDL_GetTicks();
// }

bool TVPGetJoyPadAsyncState(tjs_uint keycode, bool getcurrent)
{
	// if (keycode >= sizeof(_scancode) / sizeof(_scancode[0])) return false;
	// tjs_uint8 code = _scancode[keycode];
	// _scancode[keycode] &= 1;
	return false;
}

std::string TVPShowFileSelector(const std::string &title, const std::string &initfilename, std::string initdir, bool issave)
{
	// if (initdir.empty()) {
	// 	ttstr dir = TVPGetAppPath();
	// 	TVPGetLocalName(dir);
	// 	initdir = dir.AsStdString();
	// }
	// std::string _fileSelectorResult;
	// TVPFileSelectorForm* _fileSelector = TVPFileSelectorForm::create(initfilename, initdir, issave);
	// _fileSelector->setOnClose([&](const std::string &result) {
	// 	_fileSelectorResult = result;
	// 	_fileSelector = nullptr;
	// });
	// TVPMainScene::GetInstance()->pushUIForm(_fileSelector);
	// Director* director = Director::getInstance();
	// while (_fileSelector) {
	// 	TVPProcessInputEvents();
	// 	int remain = TVPDrawSceneOnce(30); // 30 fps
	// 	if (remain > 0) {
 //            std::this_thread::sleep_for(std::chrono::milliseconds(remain));
	// 	}
	// }
	return "";
}

// bool TVPWriteDataToFile(const ttstr &filepath, const void *data, unsigned int size) {
// 	tjs_string wfilename(filepath.AsStdString());
// 	std::string nfilename;
// 	if (TVPUtf16ToUtf8(nfilename, wfilename)) {
// 		FILE *fp = fopen(nfilename.c_str(), "wb");
// 		if (fp) {
// 			// file api is OK
// 			int writed = fwrite(data, 1, size, fp);
// 			fclose(fp);
// 			return writed == size;
// 		}
// 	}
// 	return false;
// }

void TVPExitApplication(int code) {
	// TVPDeliverCompactEvent(TVP_COMPACT_LEVEL_MAX);
	// if (!TVPIsSoftwareRenderManager())
	// 	iTVPTexture2D::RecycleProcess();
	// JniMethodInfo t;
	// if (JniHelper::getStaticMethodInfo(t, "org/tvp/kirikiri2/KR2Activity", "exit", "()V")) {
	// 	t.env->CallStaticVoidMethod(t.classID, t.methodID);
	// 	t.env->DeleteLocalRef(t.classID);
	// }
	SDL_Quit();
	exit(code);
}

// #include <sched.h>
#include <unistd.h>
void TVPRelinquishCPU() {
	// sched_yield();
	// nanosleep(15);
	SDL_Delay(0);
}

// void TVP_utime(const char *name, time_t modtime) {
// 	timeval mt[2];
// 	mt[0].tv_sec = modtime;
// 	mt[0].tv_usec = 0;
// 	mt[1].tv_sec = modtime;
// 	mt[1].tv_usec = 0;
// 	utimes(name, mt);
// }

#include <string.h>

// bool TVPCreateFolders(const ttstr &folder)
// {
// 	tTJSNarrowStringHolder path(folder.c_str());

//     const size_t len = strlen(path);
//     char _path[PATH_MAX];
//     char *p; 

//     errno = 0;

//     /* Copy string so its mutable */
//     if (len > sizeof(_path)-1) {
//         errno = ENAMETOOLONG;
//         return false; 
//     }   
//     strcpy(_path, path);

//     /* Iterate the string */
//     for (p = _path + 1; *p; p++) {
//         if (*p == '/') {
//             /* Temporarily truncate */
//             *p = '\0';

//             if (mkdir(_path, S_IRWXU) != 0) {
//                 if (errno != EEXIST)
//                     return false; 
//             }

//             *p = '/';
//         }
//     }   

//     if (mkdir(_path, S_IRWXU) != 0) {
//         if (errno != EEXIST)
//             return false; 
//     }   

// 	return true;
// }

iWindowLayer *TVPCreateAndAddWindow(tTJSNI_Window *w) {
//Start the rendering here
	TVPWindowLayer* ret = TVPWindowLayer::create(w);
	//TODO: Check iWindowLayer and implement it, then add it using below function
	// TVPMainScene::GetInstance()->addLayer(ret);
	// if (_consoleWin) ret->setVisible(false);
	return ret;
	// return ret;
}

// void TVPRemoveWindowLayer(iWindowLayer *lay) {
// 	// static_cast<TVPWindowLayer*>(lay)->removeFromParent();
// }


void TVPConsoleLog(const ttstr &l, bool important) {

	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert; 
	std::string dest = convert.to_bytes(l.c_str());  
	fprintf(stderr, "%s", dest.c_str());
}

namespace TJS {
	static const int MAX_LOG_LENGTH = 16 * 1024;
	void TVPConsoleLog(const tjs_char *l) {
		// std::string utf8;
		// assert(sizeof(tjs_char) == sizeof(wchar_t));
		// std::u16string buf((const wchar_t*)l);
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert; 
		std::string dest = convert.to_bytes(l);  
		fprintf(stderr, "%s", dest.c_str());
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

bool TVPGetScreenSize(tjs_int idx, tjs_int &w, tjs_int &h) {
	return true;
}

tjs_uint32 TVPGetCurrentShiftKeyState()
{
	tjs_uint32 f = 0;

	// if (_scancode[VK_SHIFT] & 1) f |= ssShift;
	// if (_scancode[VK_MENU] & 1) f |= ssAlt;
	// if (_scancode[VK_CONTROL] & 1) f |= ssCtrl;
	// if (_scancode[VK_LBUTTON] & 1) f |= ssLeft;
	// if (_scancode[VK_RBUTTON] & 1) f |= ssRight;
	//if (_scancode[VK_MBUTTON] & 1) f |= TVP_SS_MIDDLE;

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

void TVPShowPopMenu(tTJSNI_MenuItem* menu) {
	// TODO: STUB
};
