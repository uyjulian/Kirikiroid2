
CC = clang
CXX = clang++
AR = 
ASM = 
CFLAGS += -O0 -g
# GIT_HASH = $(shell git rev-parse --short HEAD)
GIT_HASH = nothing
CUR_TIME = $(shell date +%s)
CFLAGS += -Wall
CFLAGS += -DTJS_TEXT_OUT_CRLF -D__STDC_CONSTANT_MACROS -DUSE_UNICODE_FSTRING -D_7ZIP_ST
CFLAGS += -Isrc/core -Isrc/core/base -Isrc/core/base/win32 -Isrc/core/environ -Isrc/core/environ/win32 -Isrc/core/environ/android -Isrc/core/environ/sdl -Isrc/core/msg -Isrc/core/msg/win32 -Isrc/core/extension -Isrc/core/sound -Isrc/core/sound/win32 -Isrc/core/tjs2 -Isrc/core/utils -Isrc/core/utils/win32 -Isrc/core/visual -Isrc/core/visual/ARM -Isrc/core/visual/win32 -Isrc/core/visual/RenderScript/rs
CFLAGS += -Isrc/plugins
CFLAGS += -I/usr/local/opt/xxhash/include -I/usr/local/opt/webp/include -I/usr/local/opt/libpng/include -I/usr/local/opt/ffmpeg/include -I/usr/local/opt/libarchive/include -I/usr/local/opt/libbpg/include -I/usr/local/opt/lz4/include -I/usr/local/opt/opencv/include/opencv4 -I/usr/local/opt/mono-libgdiplus/include -I/usr/local/opt/opus/include/opus -I/usr/local/opt/sdl2/include/SDL2 -I/usr/local/opt/libiconv/include -I/usr/local/opt/libjpeg-turbo/include -I/usr/local/opt/oniguruma/include -I/usr/local/opt/jxrlib/include -I/usr/local/opt/jxrlib/include/glue -I/usr/local/opt/freetype/include -I/usr/local/opt/freetype/include/freetype2 -I/usr/local/opt/opusfile/include/opus 
LDFLAGS += -L/usr/local/opt/xxhash/lib -L/usr/local/opt/webp/lib -L/usr/local/opt/libpng/lib -L/usr/local/opt/ffmpeg/lib -L/usr/local/opt/libarchive/lib -L/usr/local/opt/lz4/lib -L/usr/local/opt/opencv/lib -L/usr/local/opt/mono-libgdiplus/lib -L/usr/local/opt/opus/lib -L/usr/local/opt/sdl2/lib -L/usr/local/opt/libiconv/lib -L/usr/local/opt/libjpeg-turbo/lib -L/usr/local/opt/oniguruma/lib -L/usr/local/opt/jxrlib/lib -L/usr/local/opt/jxrlib/lib -L/usr/local/opt/freetype/lib -L/usr/local/opt/freetype/lib -L/usr/local/opt/opusfile/lib
LDLIBS += -framework OpenAL -lxxhash -lwebp -lz -lpng -lavcodec -lavdevice -lavfilter -lavformat -lavresample -lavutil -lpostproc -lswresample -lswscale -lvorbis -lvorbisfile -larchive -llz4 -lopencv_core -lopencv_imgproc -lgdiplus -lopus -lSDL2 -liconv -lturbojpeg -lonig -ljpegxr -ljxrglue -lfreetype -lopusfile

%.o: %.c
	echo -e "\tCC  $<"
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.cpp
	echo -e "\tCXX  $<"
	$(CXX) -c $(CFLAGS) -o $@ $<

SOURCES := 
SOURCES += src/core/sdl2/stubs.cpp
SOURCES += src/core/visual/BitmapIntf.cpp
SOURCES += src/core/visual/BitmapLayerTreeOwner.cpp
SOURCES += src/core/visual/CharacterData.cpp
SOURCES += src/core/visual/ComplexRect.cpp
SOURCES += src/core/visual/FontImpl.cpp
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
#SOURCES += src/core/visual/LoadBPG.cpp
SOURCES += src/core/visual/LoadJPEG.cpp
#SOURCES += src/core/visual/LoadJXR.cpp
SOURCES += src/core/visual/LoadPNG.cpp
# SOURCES += src/core/visual/LoadPVRv3.cpp
SOURCES += src/core/visual/LoadTLG.cpp
SOURCES += src/core/visual/LoadWEBP.cpp
SOURCES += src/core/visual/MenuItemIntf.cpp
SOURCES += src/core/visual/PrerenderedFont.cpp
SOURCES += src/core/visual/RectItf.cpp
SOURCES += src/core/visual/RenderManager.cpp
SOURCES += src/core/visual/SaveTLG5.cpp
SOURCES += src/core/visual/SaveTLG6.cpp
SOURCES += src/core/visual/TransIntf.cpp
SOURCES += src/core/visual/VideoOvlIntf.cpp
SOURCES += src/core/visual/WindowIntf.cpp
SOURCES += src/core/visual/argb.cpp
SOURCES += src/core/visual/tvpgl.cpp
# SOURCES += src/core/base/7zArchive.cpp
SOURCES += src/core/base/BinaryStream.cpp
SOURCES += src/core/base/CharacterSet.cpp
SOURCES += src/core/base/EventIntf.cpp
SOURCES += src/core/base/PluginIntf.cpp
SOURCES += src/core/base/ScriptMgnIntf.cpp
SOURCES += src/core/base/StorageIntf.cpp
SOURCES += src/core/base/SysInitIntf.cpp
SOURCES += src/core/base/SystemIntf.cpp
SOURCES += src/core/base/TARArchive.cpp
SOURCES += src/core/base/TextStream.cpp
SOURCES += src/core/base/UtilStreams.cpp
SOURCES += src/core/base/XP3Archive.cpp
# SOURCES += src/core/base/ZIPArchive.cpp
SOURCES += src/core/base/win32/EventImpl.cpp
SOURCES += src/core/base/win32/FileSelector.cpp
SOURCES += src/core/base/win32/NativeEventQueue.cpp
SOURCES += src/core/base/win32/PluginImpl.cpp
SOURCES += src/core/base/win32/ScriptMgnImpl.cpp
SOURCES += src/core/base/win32/StorageImpl.cpp
SOURCES += src/core/base/win32/SysInitImpl.cpp
SOURCES += src/core/base/win32/SystemImpl.cpp
SOURCES += src/core/environ/Application.cpp
SOURCES += src/core/environ/DetectCPU.cpp
# SOURCES += src/core/environ/DumpSend.cpp
# SOURCES += src/core/environ/XP3ArchiveRepack.cpp
# SOURCES += src/core/environ/cocos2d/AppDelegate.cpp
# SOURCES += src/core/environ/cocos2d/CCKeyCodeConv.cpp
# SOURCES += src/core/environ/cocos2d/CustomFileUtils.cpp
# SOURCES += src/core/environ/cocos2d/MainScene.cpp
# SOURCES += src/core/environ/cocos2d/YUVSprite.cpp
# SOURCES += src/core/environ/android/AndroidUtils.cpp
# SOURCES += src/core/environ/linux/Platform.cpp
# SOURCES += src/core/environ/ui/BaseForm.cpp
# SOURCES += src/core/environ/ui/ConsoleWindow.cpp
# SOURCES += src/core/environ/ui/DebugViewLayerForm.cpp
# SOURCES += src/core/environ/ui/FileSelectorForm.cpp
# SOURCES += src/core/environ/ui/GameMainMenu.cpp
# SOURCES += src/core/environ/ui/GlobalPreferenceForm.cpp
# SOURCES += src/core/environ/ui/InGameMenuForm.cpp
# SOURCES += src/core/environ/ui/IndividualPreferenceForm.cpp
# SOURCES += src/core/environ/ui/MainFileSelectorForm.cpp
# SOURCES += src/core/environ/ui/MessageBox.cpp
# SOURCES += src/core/environ/ui/PreferenceForm.cpp
# SOURCES += src/core/environ/ui/SeletListForm.cpp
# SOURCES += src/core/environ/ui/SimpleMediaFilePlayer.cpp
# SOURCES += src/core/environ/ui/TipsHelpForm.cpp
# SOURCES += src/core/environ/ui/XP3RepackForm.cpp
# SOURCES += src/core/environ/ui/extension/ActionExtension.cpp
# SOURCES += src/core/environ/ui/extension/UIExtension.cpp
SOURCES += src/core/environ/win32/SystemControl.cpp
SOURCES += src/core/extension/Extension.cpp
SOURCES += src/core/movie/krmovie.cpp
# SOURCES += src/core/movie/ffmpeg/AEChannelInfo.cpp
# SOURCES += src/core/movie/ffmpeg/AEFactory.cpp
# SOURCES += src/core/movie/ffmpeg/AEStreamInfo.cpp
# SOURCES += src/core/movie/ffmpeg/AEUtil.cpp
# SOURCES += src/core/movie/ffmpeg/AudioCodecFFmpeg.cpp
# SOURCES += src/core/movie/ffmpeg/AudioCodecPassthrough.cpp
# SOURCES += src/core/movie/ffmpeg/AudioDevice.cpp
# SOURCES += src/core/movie/ffmpeg/BaseRenderer.cpp
# SOURCES += src/core/movie/ffmpeg/BitstreamStats.cpp
# SOURCES += src/core/movie/ffmpeg/Clock.cpp
# SOURCES += src/core/movie/ffmpeg/CodecUtils.cpp
# SOURCES += src/core/movie/ffmpeg/Demux.cpp
# SOURCES += src/core/movie/ffmpeg/DemuxFFmpeg.cpp
# SOURCES += src/core/movie/ffmpeg/DemuxPacket.cpp
# SOURCES += src/core/movie/ffmpeg/FactoryCodec.cpp
# SOURCES += src/core/movie/ffmpeg/InputStream.cpp
# SOURCES += src/core/movie/ffmpeg/KRMovieLayer.cpp
# SOURCES += src/core/movie/ffmpeg/KRMoviePlayer.cpp
# SOURCES += src/core/movie/ffmpeg/Message.cpp
# SOURCES += src/core/movie/ffmpeg/MessageQueue.cpp
# SOURCES += src/core/movie/ffmpeg/ProcessInfo.cpp
# SOURCES += src/core/movie/ffmpeg/RenderFlags.cpp
# SOURCES += src/core/movie/ffmpeg/StreamInfo.cpp
# SOURCES += src/core/movie/ffmpeg/TVPMediaDemux.cpp
# SOURCES += src/core/movie/ffmpeg/Thread.cpp
# SOURCES += src/core/movie/ffmpeg/TimeUtils.cpp
# SOURCES += src/core/movie/ffmpeg/Timer.cpp
# SOURCES += src/core/movie/ffmpeg/VideoCodec.cpp
# SOURCES += src/core/movie/ffmpeg/VideoCodecFFmpeg.cpp
# SOURCES += src/core/movie/ffmpeg/VideoPlayer.cpp
# SOURCES += src/core/movie/ffmpeg/VideoPlayerAudio.cpp
# SOURCES += src/core/movie/ffmpeg/VideoPlayerVideo.cpp
# SOURCES += src/core/movie/ffmpeg/VideoReferenceClock.cpp
# SOURCES += src/core/movie/ffmpeg/VideoRenderer.cpp
# SOURCES += src/core/movie/ffmpeg/krffmpeg.cpp
SOURCES += src/core/msg/MsgIntf.cpp
SOURCES += src/core/msg/win32/MsgImpl.cpp
SOURCES += src/core/msg/win32/OptionsDesc.cpp
SOURCES += src/core/sound/CDDAIntf.cpp
# SOURCES += src/core/sound/FFWaveDecoder.cpp
SOURCES += src/core/sound/MIDIIntf.cpp
# SOURCES += src/core/sound/MathAlgorithms.cpp
SOURCES += src/core/sound/PhaseVocoderDSP.cpp
SOURCES += src/core/sound/PhaseVocoderFilter.cpp
SOURCES += src/core/sound/SoundBufferBaseIntf.cpp
SOURCES += src/core/sound/VorbisWaveDecoder.cpp
SOURCES += src/core/sound/WaveFormatConverter.cpp
SOURCES += src/core/sound/WaveIntf.cpp
SOURCES += src/core/sound/WaveLoopManager.cpp
SOURCES += src/core/sound/WaveSegmentQueue.cpp
SOURCES += src/core/sound/win32/CDDAImpl.cpp
SOURCES += src/core/sound/win32/MIDIImpl.cpp
SOURCES += src/core/sound/win32/SoundBufferBaseImpl.cpp
SOURCES += src/core/sound/win32/WaveImpl.cpp
SOURCES += src/core/sound/win32/WaveMixer.cpp
SOURCES += src/core/sound/win32/tvpsnd.cpp
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
SOURCES += src/core/utils/md5.c
SOURCES += src/core/utils/ClipboardIntf.cpp
SOURCES += src/core/utils/DebugIntf.cpp
SOURCES += src/core/utils/KAGParser.cpp
SOURCES += src/core/utils/MathAlgorithms_Default.cpp
SOURCES += src/core/utils/MiscUtility.cpp
SOURCES += src/core/utils/PadIntf.cpp
SOURCES += src/core/utils/Random.cpp
SOURCES += src/core/utils/RealFFT_Default.cpp
SOURCES += src/core/utils/ThreadIntf.cpp
SOURCES += src/core/utils/TickCount.cpp
SOURCES += src/core/utils/TimerIntf.cpp
SOURCES += src/core/utils/VelocityTracker.cpp
SOURCES += src/core/utils/encoding/gbk2unicode.c
SOURCES += src/core/utils/encoding/jis2unicode.c
SOURCES += src/core/utils/minizip/unzip.c
SOURCES += src/core/utils/minizip/zip.c
SOURCES += src/core/utils/minizip/ioapi.cpp
SOURCES += src/core/utils/win32/ClipboardImpl.cpp
SOURCES += src/core/utils/win32/DebugImpl.cpp
SOURCES += src/core/utils/win32/PadImpl.cpp
SOURCES += src/core/utils/win32/TVPTimer.cpp
SOURCES += src/core/utils/win32/ThreadImpl.cpp
SOURCES += src/core/utils/win32/TimerImpl.cpp
SOURCES += src/core/visual/gl/ResampleImage.cpp
SOURCES += src/core/visual/gl/WeightFunctor.cpp
SOURCES += src/core/visual/gl/blend_function.cpp
# SOURCES += src/core/visual/ogl/RenderManager_ogl.cpp
# SOURCES += src/core/visual/ogl/astcrt.cpp
# SOURCES += src/core/visual/ogl/etcpak.cpp
# SOURCES += src/core/visual/ogl/imagepacker.cpp
# SOURCES += src/core/visual/ogl/pvrtc.cpp
SOURCES += src/core/visual/win32/BasicDrawDevice.cpp
SOURCES += src/core/visual/win32/BitmapBitsAlloc.cpp
SOURCES += src/core/visual/win32/BitmapInfomation.cpp
SOURCES += src/core/visual/win32/DInputMgn.cpp
SOURCES += src/core/visual/win32/DrawDevice.cpp
SOURCES += src/core/visual/win32/GraphicsLoaderImpl.cpp
SOURCES += src/core/visual/win32/LayerBitmapImpl.cpp
SOURCES += src/core/visual/win32/LayerImpl.cpp
SOURCES += src/core/visual/win32/MenuItemImpl.cpp
SOURCES += src/core/visual/win32/PassThroughDrawDevice.cpp
SOURCES += src/core/visual/win32/TVPScreen.cpp
SOURCES += src/core/visual/win32/VideoOvlImpl.cpp
SOURCES += src/core/visual/win32/WindowImpl.cpp
SOURCES += src/plugins/InternalPlugins.cpp
SOURCES += src/plugins/LayerExBase.cpp
SOURCES += src/plugins/addFont.cpp
SOURCES += src/plugins/csvParser.cpp
SOURCES += src/plugins/dirlist.cpp
SOURCES += src/plugins/fftgraph.cpp
SOURCES += src/plugins/getSample.cpp
SOURCES += src/plugins/getabout.cpp
# SOURCES += src/plugins/layerExMovie.cpp
SOURCES += src/plugins/layerExPerspective.cpp
SOURCES += src/plugins/saveStruct.cpp
SOURCES += src/plugins/varfile.cpp
SOURCES += src/plugins/win32dialog.cpp
SOURCES += src/plugins/wutcwf.cpp
SOURCES += src/plugins/xp3filter.cpp
SOURCES += src/plugins/ncbind/ncbind.cpp
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
