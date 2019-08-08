//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Definition of Messages and Message Related Utilities
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include "MsgIntf.h"
#include "MsgImpl.h"
#include "PluginImpl.h"

#include "Application.h"
#include "CharacterSet.h"
#if 0
#include "resource.h"
#endif

//---------------------------------------------------------------------------
// version retrieving
//---------------------------------------------------------------------------
void TVPGetVersion(void)
{
	static bool DoGet=true;
	if(DoGet)
	{
		DoGet = false;

		TVPVersionMajor = 0;
		TVPVersionMinor = 0;
		TVPVersionRelease = 0;
		TVPVersionBuild = 0;
#if 0
		TVPGetFileVersionOf(ExePath().c_str(), TVPVersionMajor, TVPVersionMinor,
			TVPVersionRelease, TVPVersionBuild);
#endif
	}
}

