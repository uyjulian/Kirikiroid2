
##### OPTIONS #####
CC = clang
CXX = clang++
AR = 
ASM = 
CFLAGS += -Ofast -march=native -g
CXXFLAGS += $(CFLAGS) -std=c++11
# CFLAGS += -O0 -g
# GIT_HASH = $(shell git rev-parse --short HEAD)
GIT_HASH = nothing
CUR_TIME = $(shell date +%s)
CFLAGS += -Wall
CFLAGS += -DTJS_TEXT_OUT_CRLF -DTVP_TEXT_READ_ANSI_MBCS 
CFLAGS += -Isrc/core
CFLAGS += -Isrc/core/base
CFLAGS += -Isrc/core/base/android
CFLAGS += -Isrc/core/base/win32
CFLAGS += -Isrc/core/environ
CFLAGS += -Isrc/core/environ/win32
CFLAGS += -Isrc/core/environ/android
CFLAGS += -Isrc/core/environ/sdl
CFLAGS += -Isrc/core/msg
CFLAGS += -Isrc/core/msg/win32
CFLAGS += -Isrc/core/extension
CFLAGS += -Isrc/core/sound
CFLAGS += -Isrc/core/sound/win32
CFLAGS += -Isrc/core/tjs2
CFLAGS += -Isrc/core/utils
CFLAGS += -Isrc/core/utils/win32
CFLAGS += -Isrc/core/visual
CFLAGS += -Isrc/core/visual/gl
CFLAGS += -Isrc/core/visual/win32
CFLAGS += -Isrc/plugins

CFLAGS += -I/usr/local/opt/zlib/include
LDFLAGS += -L/usr/local/opt/zlib/lib
LDLIBS += -lz

CFLAGS += -I/usr/local/opt/sdl2/include/SDL2
LDFLAGS += -L/usr/local/opt/sdl2/lib
LDLIBS += -lSDL2

CFLAGS += -I/usr/local/opt/oniguruma/include
LDFLAGS += -L/usr/local/opt/oniguruma/lib
LDLIBS += -lonig

CFLAGS += -I/usr/local/opt/freetype/include -I/usr/local/opt/freetype/include/freetype2
LDFLAGS += -L/usr/local/opt/freetype/lib
LDLIBS += -lfreetype

%.o: %.c
	@echo -e "\tCC  $<"
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.cpp
	@echo -e "\tCXX  $<"
	$(CXX) -c $(CXXFLAGS) -o $@ $<

SOURCES := 
SOURCES += src/core/base/BinaryStream.cpp
SOURCES += src/core/base/CharacterSet.cpp
SOURCES += src/core/base/EventIntf.cpp
SOURCES += src/core/base/PluginIntf.cpp
SOURCES += src/core/base/ScriptMgnIntf.cpp
SOURCES += src/core/base/StorageIntf.cpp
SOURCES += src/core/base/SysInitIntf.cpp
SOURCES += src/core/base/SystemIntf.cpp
SOURCES += src/core/base/TextStream.cpp
SOURCES += src/core/base/UtilStreams.cpp
SOURCES += src/core/base/XP3Archive.cpp
SOURCES += src/core/base/android/GetLocalFileListAt.cpp
SOURCES += src/core/base/android/StorageImpl.cpp
SOURCES += src/core/base/win32/EventImpl.cpp
SOURCES += src/core/base/win32/FileSelector.cpp
SOURCES += src/core/base/win32/NativeEventQueue.cpp
SOURCES += src/core/base/win32/PluginImpl.cpp
SOURCES += src/core/base/win32/ScriptMgnImpl.cpp
SOURCES += src/core/base/win32/SysInitImpl.cpp
SOURCES += src/core/base/win32/SystemImpl.cpp
SOURCES += src/core/environ/win32/Application.cpp
SOURCES += src/core/environ/android/SystemControl.cpp
SOURCES += src/core/extension/Extension.cpp
SOURCES += src/core/msg/MsgIntf.cpp
SOURCES += src/core/msg/win32/MsgImpl.cpp
SOURCES += src/core/sdl2/SDLApplication.cpp
SOURCES += src/core/sound/MathAlgorithms.cpp
SOURCES += src/core/sound/PhaseVocoderDSP.cpp
SOURCES += src/core/sound/PhaseVocoderFilter.cpp
SOURCES += src/core/sound/RealFFT.cpp
SOURCES += src/core/sound/SoundBufferBaseImpl.cpp
SOURCES += src/core/sound/SoundBufferBaseIntf.cpp
SOURCES += src/core/sound/WaveFormatConverter.cpp
SOURCES += src/core/sound/WaveIntf.cpp
SOURCES += src/core/sound/WaveLoopManager.cpp
SOURCES += src/core/sound/WaveSegmentQueue.cpp
SOURCES += src/core/sound/win32/WaveImpl.cpp
SOURCES += src/core/sound/win32/WaveMixer.cpp
SOURCES += src/core/tjs2/tjs.cpp
SOURCES += src/core/tjs2/tjs.tab.cpp
SOURCES += src/core/tjs2/tjsArray.cpp
SOURCES += src/core/tjs2/tjsBinarySerializer.cpp
SOURCES += src/core/tjs2/tjsByteCodeLoader.cpp
SOURCES += src/core/tjs2/tjsCompileControl.cpp
SOURCES += src/core/tjs2/tjsConfig.cpp
SOURCES += src/core/tjs2/tjsConstArrayData.cpp
SOURCES += src/core/tjs2/tjsDate.cpp
SOURCES += src/core/tjs2/tjsDateParser.cpp
SOURCES += src/core/tjs2/tjsDebug.cpp
SOURCES += src/core/tjs2/tjsDictionary.cpp
SOURCES += src/core/tjs2/tjsDisassemble.cpp
SOURCES += src/core/tjs2/tjsError.cpp
SOURCES += src/core/tjs2/tjsException.cpp
SOURCES += src/core/tjs2/tjsGlobalStringMap.cpp
SOURCES += src/core/tjs2/tjsInterCodeExec.cpp
SOURCES += src/core/tjs2/tjsInterCodeGen.cpp
SOURCES += src/core/tjs2/tjsInterface.cpp
SOURCES += src/core/tjs2/tjsLex.cpp
SOURCES += src/core/tjs2/tjsMT19937ar-cok.cpp
SOURCES += src/core/tjs2/tjsMath.cpp
SOURCES += src/core/tjs2/tjsMessage.cpp
SOURCES += src/core/tjs2/tjsNamespace.cpp
SOURCES += src/core/tjs2/tjsNative.cpp
SOURCES += src/core/tjs2/tjsObject.cpp
SOURCES += src/core/tjs2/tjsObjectExtendable.cpp
SOURCES += src/core/tjs2/tjsOctPack.cpp
SOURCES += src/core/tjs2/tjsRandomGenerator.cpp
SOURCES += src/core/tjs2/tjsRegExp.cpp
SOURCES += src/core/tjs2/tjsScriptBlock.cpp
SOURCES += src/core/tjs2/tjsScriptCache.cpp
SOURCES += src/core/tjs2/tjsSnprintf.cpp
SOURCES += src/core/tjs2/tjsString.cpp
SOURCES += src/core/tjs2/tjsUtils.cpp
SOURCES += src/core/tjs2/tjsVariant.cpp
SOURCES += src/core/tjs2/tjsVariantString.cpp
SOURCES += src/core/tjs2/tjsdate.tab.cpp
SOURCES += src/core/tjs2/tjspp.tab.cpp
SOURCES += src/core/utils/ClipboardIntf.cpp
SOURCES += src/core/utils/DebugIntf.cpp
SOURCES += src/core/utils/MiscUtility.cpp
SOURCES += src/core/utils/Random.cpp
SOURCES += src/core/utils/TVPTimer.cpp
SOURCES += src/core/utils/ThreadIntf.cpp
SOURCES += src/core/utils/TickCount.cpp
SOURCES += src/core/utils/TimerIntf.cpp
SOURCES += src/core/utils/TimerThread.cpp
SOURCES += src/core/utils/VelocityTracker.cpp
SOURCES += src/core/utils/cp932_uni.cpp
SOURCES += src/core/utils/encoding/gbk2unicode.c
SOURCES += src/core/utils/encoding/jis2unicode.c
SOURCES += src/core/utils/md5.c
SOURCES += src/core/utils/uni_cp932.cpp
SOURCES += src/core/utils/win32/ClipboardImpl.cpp
SOURCES += src/core/visual/BitmapIntf.cpp
SOURCES += src/core/visual/BitmapLayerTreeOwner.cpp
SOURCES += src/core/visual/CharacterData.cpp
SOURCES += src/core/visual/ComplexRect.cpp
SOURCES += src/core/visual/FontSystem.cpp
SOURCES += src/core/visual/FreeType.cpp
SOURCES += src/core/visual/FreeTypeFontRasterizer.cpp
SOURCES += src/core/visual/GraphicsLoadThread.cpp
SOURCES += src/core/visual/GraphicsLoaderIntf.cpp
SOURCES += src/core/visual/ImageFunction.cpp
SOURCES += src/core/visual/LayerBitmapIntf.cpp
SOURCES += src/core/visual/LayerIntf.cpp
SOURCES += src/core/visual/LayerManager.cpp
SOURCES += src/core/visual/LayerTreeOwnerImpl.cpp
SOURCES += src/core/visual/PrerenderedFont.cpp
SOURCES += src/core/visual/RectItf.cpp
SOURCES += src/core/visual/TransIntf.cpp
SOURCES += src/core/visual/VideoOvlIntf.cpp
SOURCES += src/core/visual/WindowIntf.cpp
SOURCES += src/core/visual/gl/ResampleImage.cpp
SOURCES += src/core/visual/gl/WeightFunctor.cpp
SOURCES += src/core/visual/tvpgl.c
SOURCES += src/core/visual/win32/BasicDrawDevice.cpp
SOURCES += src/core/visual/win32/BitmapBitsAlloc.cpp
SOURCES += src/core/visual/win32/BitmapInfomation.cpp
SOURCES += src/core/visual/win32/DrawDevice.cpp
SOURCES += src/core/visual/win32/GraphicsLoaderImpl.cpp
SOURCES += src/core/visual/win32/LayerBitmapImpl.cpp
SOURCES += src/core/visual/win32/LayerImpl.cpp
SOURCES += src/core/visual/win32/NullDrawDevice.cpp
SOURCES += src/core/visual/win32/TVPScreen.cpp
SOURCES += src/core/visual/win32/VideoOvlImpl.cpp
SOURCES += src/core/visual/win32/WindowImpl.cpp
SOURCES += src/plugins/InternalPlugins.cpp
SOURCES += src/plugins/csvParser.cpp
SOURCES += src/plugins/dirlist.cpp
SOURCES += src/plugins/fftgraph.cpp
SOURCES += src/plugins/getSample.cpp
SOURCES += src/plugins/ncbind/ncbind.cpp
SOURCES += src/plugins/saveStruct.cpp
SOURCES += src/plugins/varfile.cpp
SOURCES += src/plugins/win32dialog.cpp
SOURCES += src/plugins/wutcwf.cpp
OBJECTS := $(SOURCES:.c=.o)
OBJECTS := $(OBJECTS:.cpp=.o)
OBJECTS := $(OBJECTS:.asm=.o)
OBJECTS := $(OBJECTS:.rc=.o)

BINARY ?= kirikiroid2
ARCHIVE ?= kirikiroid2.$(GIT_HASH).7z

all: $(BINARY)

archive: $(ARCHIVE)

clean:
	rm -f $(OBJECTS) $(BINARY) $(ARCHIVE)

$(ARCHIVE): $(BINARY)
	rm -f $(ARCHIVE)
	7z a $@ $^

$(BINARY): $(OBJECTS) 
	@echo -e "\tLNK $@"
	$(CXX) $(CFLAGS) $(LDFLAGS)  -o $@ $^  $(LDLIBS)
