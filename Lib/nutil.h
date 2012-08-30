#pragma once

#ifndef _NUTIL_LIB_
#define _NUTIL_LIB_

/**********************************************************************************************************/
/**********************************************************************************************************/
bool __cdecl SetWindowTransparentForColor(HWND wnd_hwnd, unsigned long color);
/**********************************************************************************************************/
typedef  void (__cdecl * CmdAppCallBackFN)(void *, void *, void *) ;

typedef struct _tag_cmd_app_param_node
{
	TCHAR   * cmd_str ;
	void	* fn ;
}CmdAppParam;

bool __cdecl RunCmdApp(CmdAppParam * lp);
bool __cdecl RunCmdAppForFFMpeg(CmdAppParam * lp);
/**********************************************************************************************************/


	#if defined(_NUTIL_LIB_REFERENCE)
	
		#ifdef _DEBUG
			#pragma comment(lib, "nutil_dbg.lib")
		
		#else	//_DEBUG
			#pragma comment(lib, "nutil.lib")
		#endif	//_DEBUG
	
	#endif  //_N_WIN32UI_LIB_REFERENCE
/**********************************************************************************************************/
/**********************************************************************************************************/

#endif

