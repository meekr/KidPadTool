#pragma once

#ifndef _USER_BASE64_UTILS_
#define _USER_BASE64_UTILS_

#define _NABASE_LIB_REFERENCE 1
#pragma warning(disable:4786 4244 4996)
#include <tchar.h>
#include <nbase.h>
/////////////////////////////////////////////////////////////////////////////////////////
namespace b64
{
	static char basis_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";	
#define CHAR64(c)  (((c) < 0 || (c) > 127) ? -1 : index_64[(c)])

	static unsigned char index_64[128] = {	
		0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X3E,0XFF,0XFF,0XFF,0X3F,
		0X34,0X35,0X36,0X37,0X38,0X39,0X3A,0X3B,0X3C,0X3D,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0X00,0X01,0X02,0X03,0X04,0X05,0X06,0X07,0X08,0X09,0X0A,0X0B,0X0C,0X0D,0X0E,
		0X0F,0X10,0X11,0X12,0X13,0X14,0X15,0X16,0X17,0X18,0X19,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0X1A,0X1B,0X1C,0X1D,0X1E,0X1F,0X20,0X21,0X22,0X23,0X24,0X25,0X26,0X27,0X28,
		0X29,0X2A,0X2B,0X2C,0X2D,0X2E,0X2F,0X30,0X31,0X32,0X33,0XFF,0XFF,0XFF,0XFF,0XFF
	} ;
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	static unsigned int __base64_encode(const unsigned char *value, int vlen, char * output_buff)
	{
		unsigned char oval = 0 ; 
		char * out = output_buff ;
		unsigned int ret = 0 ;

		while (vlen >= 3)
		{	
			*out++ = basis_64[value[0] >> 2];	
			*out++ = basis_64[((value[0] << 4) & 0x30) | (value[1] >> 4)];	
			*out++ = basis_64[((value[1] << 2) & 0x3C) | (value[2] >> 6)];	
			*out++ = basis_64[value[2] & 0x3F];	
			ret += 4 ;
			value += 3;	
			vlen -= 3;	
		}
		if (vlen > 0)
		{	
			*out++ = basis_64[value[0] >> 2];	
			ret ++;
			oval = (value[0] << 4) & 0x30 ;	
			if (vlen > 1) oval |= value[1] >> 4;	
			*out++ = basis_64[oval];	
			ret ++;
			*out++ = (vlen < 2) ? '=' : basis_64[(value[1] << 2) & 0x3C];	
			ret ++;
			*out++ = '=';	
			ret ++;
		}	
		*out = '\0';		
		return ret;	
	}	
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	static unsigned int __base64_decode(const char *value, unsigned int vlen, unsigned char * output_buff)	
	{		
		int c1, c2, c3, c4;	        	
		unsigned int ret = 0;
		unsigned char * out = output_buff ;

		while (vlen>0)
		{	
			c1 = value[0];	
			if (CHAR64(c1) == -1)	{ goto lb_exit; }
			c2 = value[1];	
			if (CHAR64(c2) == -1)	{ goto lb_exit; }
			c3 = value[2];	
			if ((c3 != '=') && (CHAR64(c3) == -1))	{ goto lb_exit; }
			c4 = value[3];	
			if ((c4 != '=') && (CHAR64(c4) == -1))	{ goto lb_exit; }
			value += 4 ;
			vlen -= 4 ;
			*out++ = (CHAR64(c1) << 2) | (CHAR64(c2) >> 4);	
			ret += 1;	
			if (c3 != '=')
			{	
				*out++ = ((CHAR64(c2) << 4) & 0xf0) | (CHAR64(c3) >> 2);	
				ret += 1;	
				if (c4 != '=')
				{	
					*out++ = ((CHAR64(c3) << 6) & 0xc0) | CHAR64(c4);	
					ret += 1;	
				}
			}	
		}

lb_exit:	


		return ret;	
	}


	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	static bool  __CString2AnsiStdString(CString src_str, void * output_buff, unsigned int * buff_len)
	{
		BSTR bb = src_str.AllocSysString()   ;
		unsigned int l = ::SysStringLen(bb);
		char * cc = (char*) malloc(l + 1);
		unsigned int ptr = (unsigned int)cc ;
		memset(cc, 0, l + 1);
		for(unsigned int i=0;i<=l;++i)
		{
			memset( (void *)ptr, bb[i], 1);
			ptr ++ ;
		}

		::SysFreeString(bb); 
		bb = 0 ;

		memcpy(buff_len, &l, sizeof(unsigned int));
		memcpy(output_buff, &cc, sizeof(unsigned int));

		return  true ;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	static unsigned int Base64Decode(const char * src_str, unsigned int src_len, void ** output_ptr)
	{
		unsigned char * c_result = 0;
		unsigned int result_len = 0 ;
		
		c_result = (unsigned char*) malloc(src_len + 8);
		memset(c_result, 0, src_len + 8);

		result_len = __base64_decode((const char*)src_str, src_len, c_result);
		(*output_ptr) = (void*)c_result;
	
		return result_len ;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	static unsigned int Base64Decode(CString src_str, void ** output_ptr)
	{
		unsigned char * c_src = 0;
		unsigned int src_len = 0, result_len = 0 ;
	
		__CString2AnsiStdString(src_str, (void*)(&c_src), &src_len);
	
		result_len = Base64Decode((const char *)c_src, src_len, output_ptr);

		free(c_src);
		
		return result_len ;
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	static CString GetBase64DataFromTextFile(TCHAR * filename)
	{
		CString rtv ;

		bs::WinTxtFile * wfs = new bs::WinTxtFile();
		wfs->Init();
		
		if(wfs->OpenTextFileForRead(filename) == false)		{ goto lb_exit ; }

		void * buff = wfs->ReadAll(sizeof(TCHAR)); 
		if(buff == 0)	{ goto lb_exit ; }
		rtv.Append((TCHAR*)buff); 
		wfs->_free(buff); 
		
lb_exit:

		if(wfs !=0)	{ wfs->Release(); delete wfs; wfs = 0 ; } 

		return rtv ;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	static TCHAR * GetBase64BufferFromTextFile(TCHAR * filename)
	{
		bs::WinTxtFile * wfs = new bs::WinTxtFile();
		wfs->Init();

		if(wfs->OpenTextFileForRead(filename) == false)		{ goto lb_exit ; }

		void * buff = wfs->ReadAll(sizeof(TCHAR)); 
		if(buff == 0)	{ goto lb_exit ; }
		wfs->_free(buff); 

lb_exit:
		return (TCHAR*)buff ;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	static bool ConvertFileToBase64(TCHAR * filename, const TCHAR * new_file_name = 0)
	{
		bool rtv = false ;

		bs::WinFileCls * wfs1 = 0; 
		bs::WinFileCls * wfs2 = 0; 
		char * buff_read = 0, * buff_b64 = 0 ;
		TCHAR output_filename[512] = {0} ;
		unsigned long buff_size = 4096, len_read = 0, len_write = 0;

		wfs1 = new bs::WinFileCls(); 
		wfs1->Init(); 
		if(wfs1->OpenDiskFile(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING) == false)	{ goto lb_exit ; }
		if(wfs1->FileSize() < 1)	{ goto lb_exit ; }

		
		wfs2 = new bs::WinFileCls(); 
		wfs2->Init();
		if(new_file_name == 0)
		{
			_tcscpy(output_filename, filename); 
			_tcscat(output_filename, _T(".b64.txt"));
			if(wfs2->OpenDiskFile(output_filename,  GENERIC_WRITE, 0, CREATE_ALWAYS) == false)	{ goto lb_exit ; }
		}
		else
		{
			if(wfs2->OpenDiskFile(new_file_name,  GENERIC_WRITE, 0, CREATE_ALWAYS) == false)	{ goto lb_exit ; }
		}

		buff_read = (char*) (wfs1->_malloc(buff_size + 4)); 
		buff_b64 = (char*) (wfs2->_malloc(buff_size*2 + 4)); 
		while(1)
		{
			memset(buff_read, 0, buff_size + 4);
			memset(buff_b64, 0, buff_size*2 + 4);
			len_read = wfs1->ReadBlock(buff_read, buff_size);
			
			len_write = __base64_encode((const unsigned char *)buff_read, len_read, buff_b64);
			if(len_write > 0)	{ wfs2->WriteBlock(buff_b64, len_write); }
			if(len_read < buff_size)	{ break ; }

		}

		wfs1->_free(buff_read); 
		wfs2->_free(buff_b64);
		rtv = true ;

lb_exit:

		if(wfs1 != 0)	{ wfs1->Release(); delete wfs1 ; wfs1 = 0 ; }
		if(wfs2 != 0)	{ wfs2->Release(); delete wfs2 ; wfs2 = 0 ; }

		return rtv ;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	static bool ConvertFileFromBase64(TCHAR * filename, const TCHAR * new_file_name = 0)
	{
		bool rtv = false ;
		bs::BFS * bfs = 0 ;
		bs::WinFileCls * wfs1 = 0; 
		bs::WinFileCls * wfs2 = 0; 
		char * buff_read = 0, * buff_b64 = 0 ;
		TCHAR * output_filename = 0 ;
		unsigned long buff_size = 4096, len_read = 0, len_write = 0;

		wfs1 = new bs::WinFileCls(); 
		wfs1->Init(); 
		if(wfs1->OpenDiskFile(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING) == false)	{ goto lb_exit ; }
		if(wfs1->FileSize() < 1)	{ goto lb_exit ; }

		wfs2 = new bs::WinFileCls(); 
		wfs2->Init();

		if(new_file_name == 0)
		{
			bfs = new bs::BFS();
			bfs->Init();
			output_filename = bfs->StrReplace(filename, _T(".b64.txt"), _T(".dat")) ;
			if(wfs2->OpenDiskFile(output_filename,  GENERIC_WRITE, 0, CREATE_ALWAYS) == false)	{ goto lb_exit ; }
		}
		else
		{
			if(wfs2->OpenDiskFile(new_file_name,  GENERIC_WRITE, 0, CREATE_ALWAYS) == false)	{ goto lb_exit ; }
		}

		

		buff_read = (char*) (wfs1->_malloc(buff_size + 4)); 
		buff_b64 = (char*) (wfs2->_malloc(buff_size*2 + 4)); 
		while(1)
		{
			memset(buff_read, 0, buff_size + 4);
			memset(buff_b64, 0, buff_size*2 + 4);
			len_read = wfs1->ReadBlock(buff_read, buff_size);
			len_write = __base64_decode(buff_read, len_read, (unsigned char *)buff_b64);
			if(len_write > 0)	{ wfs2->WriteBlock(buff_b64, len_write); }
			if(len_read < buff_size)	{ break ; }
		}

		wfs1->_free(buff_read); 
		wfs2->_free(buff_b64);

		rtv = true ;

lb_exit:

		if(bfs != 0)	{ bfs->Release(); delete bfs ; bfs = 0 ; }
		if(wfs1 != 0)	{ wfs1->Release(); delete wfs1 ; wfs1 = 0 ; }
		if(wfs2 != 0)	{ wfs2->Release(); delete wfs2 ; wfs2 = 0 ; }

		return rtv ;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	static CString UTF8ToCString(const char * buff, unsigned int buff_len)
	{
		CString rtv ;
		wchar_t * wbuff = 0 ;
		int l1 = ::MultiByteToWideChar(CP_UTF8, NULL, buff, -1, wbuff, 0);
		if(l1>0)
		{
			l1 *= 2 ;
			wbuff = (wchar_t*)malloc(l1);
			memset(wbuff, 0, l1);
			if(::MultiByteToWideChar(CP_UTF8,NULL, buff, -1, wbuff, l1) > 0)
			{
				rtv = wbuff ;
			}
		}
		if(wbuff != 0)	{ free(wbuff); }
		return rtv ;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	static CString __ReadAllFromTextFileUTF8(TCHAR * filename)
	{
		CString rtv ;
		bs::WinFileCls * wfs = 0; 
		void * buff = 0;
		wchar_t * wbuff = 0 ;
		unsigned int file_size = 0 ;

		wfs = new bs::WinFileCls(); 
		wfs->Init(); 
		if(wfs->OpenDiskFile(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING) == false)	{ goto lb_exit ; }
		file_size = wfs->FileSize(); 
		if(file_size < 1)	{ goto lb_exit ; }

		buff = wfs->ReadAll(); 

		int l1 = ::MultiByteToWideChar(CP_UTF8, NULL, (char*)((unsigned int)buff + 3), -1, wbuff, 0);
		if(l1>0)
		{
			l1 *= 2 ;
			wbuff = (wchar_t*)wfs->_malloc(l1);
			if(::MultiByteToWideChar(CP_UTF8,NULL,(char*)((unsigned int)buff + 3),-1,wbuff, l1) > 0)
			{
				rtv = wbuff ;
			}
		}

lb_exit:

		if(wfs != 0)	{ wfs->Release(); delete wfs ; wfs = 0 ; }
		

		return rtv ;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	static CString ReadAllFromTextFile(TCHAR * filename)
	{
		bool is_try_utf8 = false ;
		CString rtv ;

		bs::WinTxtFile * wfs = new bs::WinTxtFile();
		wfs->Init();
		if(wfs->OpenTextFileForRead(filename) == true)
{
		void * buff = wfs->ReadAll(sizeof(TCHAR)); 
		if(buff == 0)	{ goto lb_exit ; }
		rtv.Append((TCHAR*)buff); 
		wfs->_free(buff); 
}
		
lb_exit:

		if(wfs !=0)	{ wfs->Release(); delete wfs; wfs = 0 ; } 

		return rtv ;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	static CString ReadAllFromTextFile(CString filename)
	{
		return ReadAllFromTextFile(filename.GetBuffer(filename.GetLength()+1));
	}
} ///////end namespace

#endif