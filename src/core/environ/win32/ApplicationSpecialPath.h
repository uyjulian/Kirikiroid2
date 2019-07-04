
#ifndef __APPLICATION_SPECIAL_PATH_H__
#define __APPLICATION_SPECIAL_PATH_H__

#if 0
#include <shlobj.h>
#endif
#include "FilePathUtil.h"
#include "StorageIntf.h"

class ApplicationSpecialPath {
public:
#if 0
	static tjs_string GetSpecialFolderPath(int csidl) {
		tjs_char path[MAX_PATH+1];
		if(!SHGetSpecialFolderPath(NULL, path, csidl, false))
			return tjs_string();
		return tjs_string(path);
	}
	static inline tjs_string GetPersonalPath() {
		tjs_string path = GetSpecialFolderPath(CSIDL_PERSONAL);
		if( path.empty() ) path = GetSpecialFolderPath(CSIDL_APPDATA);

		if(path != TJS_W("")) {
			return path;
		}
		return TJS_W("");
	}
	static inline tjs_string GetAppDataPath() {
		tjs_string path = GetSpecialFolderPath(CSIDL_APPDATA);
		if(path != TJS_W("") ) {
			return path;
		}
		return TJS_W("");
	}
	static inline tjs_string GetSavedGamesPath() {
		tjs_string result;
		PWSTR ppszPath = NULL;
		HRESULT hr = ::SHGetKnownFolderPath(FOLDERID_SavedGames, 0, NULL, &ppszPath);
		if( hr == S_OK ) {
			result = tjs_string( ppszPath );
			::CoTaskMemFree( ppszPath );
		}
		return result;
	}
#endif
#if 1
	static inline tjs_string ReplaceStringAll( tjs_string src, const tjs_string& target, const tjs_string& dest ) {
		tjs_string::size_type nPos = 0;
		while( (nPos = src.find(target, nPos)) != tjs_string::npos ) {
			src.replace( nPos, target.length(), dest );
		}
		return src;
	}
#endif
	static inline tjs_string GetConfigFileName( const tjs_string& exename ) {
		return ChangeFileExt(exename, TJS_W(".cf"));
	}
	static tjs_string GetDataPathDirectory( tjs_string datapath, const tjs_string& exename ) {
		if (datapath != TJS_W("")) return datapath;
		ttstr nativeDataPath = ttstr(TVPGetAppPath().AsStdString());
		TVPGetLocalName(nativeDataPath);
		nativeDataPath += TJS_W("/savedata/");
		return nativeDataPath.AsStdString();
#if 0
		if(datapath == TJS_W("") ) datapath = tjs_string(L"$(exepath)\\savedata");

		tjs_string exepath = ExcludeTrailingBackslash(ExtractFileDir(exename));
		tjs_string personalpath = ExcludeTrailingBackslash(GetPersonalPath());
		tjs_string appdatapath = ExcludeTrailingBackslash(GetAppDataPath());
		tjs_string savedgamespath = ExcludeTrailingBackslash(GetSavedGamesPath());
		if(personalpath == TJS_W("")) personalpath = exepath;
		if(appdatapath == TJS_W("")) appdatapath = exepath;
		if(savedgamespath == TJS_W("")) savedgamespath = exepath;

		datapath = ReplaceStringAll(datapath, TJS_W("$(exepath)"), exepath);
		datapath = ReplaceStringAll(datapath, TJS_W("$(personalpath)"), personalpath);
		datapath = ReplaceStringAll(datapath, TJS_W("$(appdatapath)"), appdatapath);
		datapath = ReplaceStringAll(datapath, TJS_W("$(vistapath)"), vistapath);
		datapath = ReplaceStringAll(datapath, TJS_W("$(savedgamespath)"), savedgamespath);
		return IncludeTrailingBackslash(ExpandUNCFileName(datapath));
#endif
	}
	static tjs_string GetUserConfigFileName( const tjs_string& datapath, const tjs_string& exename ) {
		// exepath, personalpath, appdatapath
		return GetDataPathDirectory(datapath, exename) + ExtractFileName(ChangeFileExt(exename, TJS_W(".cfu")));
	}
};


#endif // __APPLICATION_SPECIAL_PATH_H__
