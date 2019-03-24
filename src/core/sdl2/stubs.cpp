
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
// #include "ConfigManager/IndividualConfigManager.h"
#include "Platform.h"
#include "Application.h"
#include "ScriptMgnIntf.h"
#include "win32/TVPWindow.h"
#include "VelocityTracker.h"
#include "SystemImpl.h"
#include "RenderManager.h"
#include "VideoOvlIntf.h"
#include "Exception.h"
// #include "win32/SystemControl.h"
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
						// TJSNativeInstance = NULL; // ¤³¤Î¶ÎëA¤Ç¤Ï¼È¤Ëthis¤¬Ï÷³ý¤µ¤ì¤Æ¤¤¤ë¤¿¤á¡¢\á\ó\Ð©`¤Ø\¢\¯\»\¹¤·¤Æ¤Ï¤¤¤±¤Ê¤¤
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
// #if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
// 			TVPHideIME();
// #else
// //#ifdef _MSC_VER
// 			TVPMainScene::GetInstance()->detachWithIME();
// #endif
			break;
		case ::imOpen:
			//TVPMainScene::GetInstance()->attachWithIME();
			//break;
		default:
// #if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
// 		{
// 			Size screenSize = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
// 			TVPShowIME(0, _textInputPosY, screenSize.width, screenSize.height / 4);
// 		}
// #else
// //#ifdef _MSC_VER
// 			TVPMainScene::GetInstance()->attachWithIME();
// #endif
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
	// virtual cocos2d::Node *GetPrimaryArea() override {
	// 	return NULL;
	// }
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
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		strncat(cwd, "/", PATH_MAX);
		::Application->StartApplication(cwd); //nice
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

std::string TVPGetPackageVersionString() {
	return "sdl2";
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

int TVPShowSimpleInputBox(ttstr &text, const ttstr &caption, const ttstr &prompt, const std::vector<ttstr> &vecButtons) {
	// JniMethodInfo methodInfo;
	// if (JniHelper::getStaticMethodInfo(methodInfo, "org/tvp/kirikiri2/KR2Activity", "ShowInputBox", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;)V"))
	// {
	// 	jstring jstrTitle = methodInfo.env->NewStringUTF(caption.AsStdString().c_str());
	// 	jstring jstrText = methodInfo.env->NewStringUTF(text.AsStdString().c_str());
	// 	jstring jstrPrompt = methodInfo.env->NewStringUTF(prompt.AsStdString().c_str());
	// 	jclass strcls = methodInfo.env->FindClass("java/lang/String");
	// 	jobjectArray btns = methodInfo.env->NewObjectArray(vecButtons.size(), strcls, nullptr);
	// 	for (unsigned int i = 0; i < vecButtons.size(); ++i) {
	// 		jstring jstrBtn = methodInfo.env->NewStringUTF(vecButtons[i].AsStdString().c_str());
	// 		methodInfo.env->SetObjectArrayElement(btns, i, jstrBtn);
	// 		methodInfo.env->DeleteLocalRef(jstrBtn);
	// 	}

	// 	MsgBoxRet = -2;
	// 	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jstrTitle, jstrPrompt, jstrText, btns);

	// 	methodInfo.env->DeleteLocalRef(jstrTitle);
	// 	methodInfo.env->DeleteLocalRef(jstrText);
	// 	methodInfo.env->DeleteLocalRef(jstrPrompt);
	// 	methodInfo.env->DeleteLocalRef(btns);
	// 	methodInfo.env->DeleteLocalRef(methodInfo.classID);

	// 	std::unique_lock<std::mutex> lk(MessageBoxLock);
	// 	while (MsgBoxRet == -2) {
	// 		MessageBoxCond.wait_for(lk, std::chrono::milliseconds(200));
	// 		if (MsgBoxRet == -2) {
	// 			TVPForceSwapBuffer(); // update opengl events
	// 		}
	// 	}
	// 	text = MessageBoxRetText;
	// 	return MsgBoxRet;
	// }
	return -1;
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

void TVPGetMemoryInfo(TVPMemoryInfo &m)
{
 //    /* to read /proc/meminfo */
 //    FILE* meminfo;
 //    char buffer[100] = {0};
 //    char* end;
 //    int found = 0;

 //    /* Try to read /proc/meminfo, bail out if fails */
	// meminfo = fopen("/proc/meminfo", "r");

 //    static const char
 //        pszMemFree[] = "MemFree:",
 //        pszMemTotal[] = "MemTotal:",
 //        pszSwapTotal[] = "SwapTotal:",
 //        pszSwapFree[] = "SwapFree:",
 //        pszVmallocTotal[] = "VmallocTotal:",
 //        pszVmallocUsed[] = "VmallocUsed:";

 //    /* Read each line untill we got all we ned */
 //    while( fgets( buffer, sizeof( buffer ), meminfo ) )
 //    {
 //        if( strstr( buffer, pszMemFree ) == buffer )
 //        {
 //            m.MemFree = strtol( buffer + sizeof(pszMemFree), &end, 10 );
 //            found++;
 //        }
 //        else if( strstr( buffer, pszMemTotal ) == buffer )
 //        {
 //            m.MemTotal = strtol( buffer + sizeof(pszMemTotal), &end, 10 );
 //            found++;
 //        }
 //        else if( strstr( buffer, pszSwapTotal ) == buffer )
 //        {
 //            m.SwapTotal = strtol( buffer + sizeof(pszSwapTotal), &end, 10 );
 //            found++;
 //            break;
 //        }
 //        else if( strstr( buffer, pszSwapFree ) == buffer )
 //        {
 //            m.SwapFree = strtol( buffer + sizeof(pszSwapFree), &end, 10 );
 //            found++;
 //            break;
 //        }
 //        else if( strstr( buffer, pszVmallocTotal ) == buffer )
 //        {
 //            m.VirtualTotal = strtol( buffer + sizeof(pszVmallocTotal), &end, 10 );
 //            found++;
 //            break;
 //        }
 //        else if( strstr( buffer, pszVmallocUsed ) == buffer )
 //        {
 //            m.VirtualUsed = strtol( buffer + sizeof(pszVmallocUsed), &end, 10 );
 //            found++;
 //            break;
 //        }
 //    }
 //    fclose(meminfo);
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

// int TVPShowSimpleMessageBoxYesNo(const ttstr & text, const ttstr & caption) {
// 	return 0;
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
// 	static bool TVPLoggingToConsole = IndividualConfigManager::GetInstance()->GetValue<bool>("outputlog", true);
// 	if (!TVPLoggingToConsole) return;
// 	if (_consoleWin) {
// 		_consoleWin->addLine(l, important ? Color3B::YELLOW : Color3B::GRAY);
// 		TVPDrawSceneOnce(100); // force update in 10fps
// 	}
// #ifdef _WIN32
// 	//cocos2d::log("%s", utf8.c_str());
// 	char buf[16384] = { 0 };
// 	WideCharToMultiByte(CP_ACP, 0, l.c_str(), -1, buf, sizeof(buf), nullptr, FALSE);
// 	puts(buf);
// #else
// 	std::string utf8;
// 	if (StringUtils::UTF16ToUTF8(l.c_str(), utf8))
// 		cocos2d::log("%s", utf8.c_str());
// #endif

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
	// if (idx != 0) return false;
	// const cocos2d::Size &size = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
	// //w = size.height; h = size.width;
	// w = 2048;
	// h = w * (size.height / size.width);
	return true;
}


int TVPShowSimpleMessageBox(const char *pszText, const char *pszTitle, unsigned int nButton, const char **btnText) {
	// JniMethodInfo methodInfo;
	// if (JniHelper::getStaticMethodInfo(methodInfo, "org/tvp/kirikiri2/KR2Activity", "ShowMessageBox", "(Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;)V"))
	// {
	// 	jstring jstrTitle = methodInfo.env->NewStringUTF(pszTitle);
	// 	jstring jstrText = methodInfo.env->NewStringUTF(pszText);
	// 	jclass strcls = methodInfo.env->FindClass("java/lang/String");
	// 	jobjectArray btns = methodInfo.env->NewObjectArray(nButton, strcls, nullptr);
	// 	for (unsigned int i = 0; i < nButton; ++i) {
	// 		jstring jstrBtn = methodInfo.env->NewStringUTF(btnText[i]);
	// 		methodInfo.env->SetObjectArrayElement(btns, i, jstrBtn);
	// 		methodInfo.env->DeleteLocalRef(jstrBtn);
	// 	}

	// 	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jstrTitle, jstrText, btns);

	// 	methodInfo.env->DeleteLocalRef(jstrTitle);
	// 	methodInfo.env->DeleteLocalRef(jstrText);
	// 	methodInfo.env->DeleteLocalRef(btns);
	// 	methodInfo.env->DeleteLocalRef(methodInfo.classID);

	// 	std::unique_lock<std::mutex> lk(MessageBoxLock);
	// 	while (MsgBoxRet == -2) {
	// 		MessageBoxCond.wait_for(lk, std::chrono::milliseconds(200));
	// 		if (MsgBoxRet == -2) {
	// 			TVPForceSwapBuffer(); // update opengl events
	// 		}
	// 	}
	// 	return MsgBoxRet;
	// }
	TVPConsoleLog(pszText);
	return -1;
}

int TVPShowSimpleMessageBox(const ttstr & text, const ttstr & caption, const std::vector<ttstr> &vecButtons) {
	// tTJSNarrowStringHolder pszText(text.c_str());
	// tTJSNarrowStringHolder pszTitle(caption.c_str());
	// std::vector<const char *> btnText; btnText.reserve(vecButtons.size());
	// std::vector<std::string> btnTextHold; btnTextHold.reserve(vecButtons.size());
	// for (const ttstr &btn : vecButtons) {
	// 	tjs_string wbtn(btn.AsStdString());
	// 	std::string nbtn;
	// 	if (TVPUtf16ToUtf8(nbtn, wbtn)) {
	// 		btnTextHold.emplace_back(nbtn);
	// 	}
	// 	btnText.emplace_back(btnTextHold.back().c_str());
	// }
	// return TVPShowSimpleMessageBox(pszText, pszTitle, btnText.size(), &btnText[0]);
	TVPConsoleLog(text, true);
	return -1;
}


// ttstr TVPGetDataPath() {
// 	// std::string path = cocos2d::FileUtils::getInstance()->getWritablePath();
// 	return "pa";
// }

// #include "StorageImpl.h"
// static std::string _TVPGetInternalPreferencePath() {
// 	// std::string path = cocos2d::FileUtils::getInstance()->getWritablePath();
// 	// path += ".preference";
// 	// if (!TVPCheckExistentLocalFolder(path)) {
// 	// 	TVPCreateFolders(path);
// 	// }
// 	// path += "/";
// 	return "pah";
// }

// const std::string &TVPGetInternalPreferencePath() {
// 	// static std::string ret = _TVPGetInternalPreferencePath();
// 	return "pah";
// }

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
	// switch (cocos2d::Application::getInstance()->getTargetPlatform()) {
	// case ApplicationProtocol::Platform::OS_WINDOWS:
	// 	return "Win32";
	// case ApplicationProtocol::Platform::OS_LINUX:
	// 	return "Linux";
	// case ApplicationProtocol::Platform::OS_MAC:
	// 	return "MacOS";
	// case ApplicationProtocol::Platform::OS_ANDROID:
	// 	return "Android";
	// case ApplicationProtocol::Platform::OS_IPHONE:
	// 	return "iPhone";
	// case ApplicationProtocol::Platform::OS_IPAD:
	// 	return "iPad";
	// case ApplicationProtocol::Platform::OS_BLACKBERRY:
	// 	return "BlackBerry";
	// case ApplicationProtocol::Platform::OS_NACL:
	// 	return "Nacl";
	// case ApplicationProtocol::Platform::OS_TIZEN:
	// 	return "Tizen";
	// case ApplicationProtocol::Platform::OS_WINRT:
	// 	return "WinRT";
	// case ApplicationProtocol::Platform::OS_WP8:
	// 	return "WinPhone8";
	// default:
	// 	return "Unknown";
	// }
	return "MacOS";
}

ttstr TVPGetOSName()
{
	return TVPGetPlatformName();
}

void TVPShowPopMenu(tTJSNI_MenuItem* menu) {
	// TODO: STUB
};
