#pragma once

#ifndef _LOW_LEVEL_HIGH_PERFORMANCE_NBASE_LIB_
#define _LOW_LEVEL_HIGH_PERFORMANCE_NBASE_LIB_

/**********************************************************************************************************/
/**********************************************************************************************************/

namespace bs
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const unsigned int MEMMGR_CLSID = 0x00000001;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class  MemMgr
	{

	public:
		MemMgr(void);
		~MemMgr(void);

		void __cdecl Init();
		void __cdecl Release();
		void * __cdecl _malloc(size_t block_size);
		void __cdecl _free(void * block_ptr);
		void __cdecl _free_all();
		void * __cdecl _memdup(const void * block_ptr,size_t block_size);

	private:

		typedef struct _MemPtrListNode
		{
			unsigned int pv ;
			_MemPtrListNode * prev  ;
			_MemPtrListNode * next ;
		}MemPtrListNode;

		void __cdecl InsertPtrNode(unsigned int pv);
		bool __cdecl RemovePtrNode(unsigned int pv);
		bool __cdecl IsPtrInList(unsigned int pv);

		MemPtrListNode * volatile phead , * volatile plast ;
		int volatile _ref_block_num ;

		short volatile _self_state ;

	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class BFS:public bs::MemMgr
	{
	public:
		BFS(void);
		~BFS(void);
		void __cdecl Init();
		void __cdecl Release();
		void __cdecl ResetMemBuff() ;
		TCHAR * __cdecl StrCopy(const TCHAR * src_val);
		TCHAR * __cdecl StrUnion(const TCHAR * v1 , const TCHAR * v2) ;
		TCHAR * __cdecl StrReplace(const TCHAR * src_val,const TCHAR * f_val,const TCHAR * n_val,bool is_case_sensitive = false/*true:sensitive;   false:insensitive*/);
		TCHAR * __cdecl StrDispel(const TCHAR * src_val,const TCHAR * _flag,bool is_case_sensitive = false/*true:sensitive;   false:insensitive*/);
		TCHAR * __cdecl StrTrim(const TCHAR * src_val,const TCHAR * _flag = 0);
		TCHAR * __cdecl StrTrimEx(const TCHAR * src_val,const TCHAR * _flag, unsigned int _flag_len);
		TCHAR * __cdecl StrLeftTrim(const TCHAR * src_val,const TCHAR * _flag = 0);
		TCHAR * __cdecl StrLeftTrimEx(const TCHAR * src_val,const TCHAR * _flag, unsigned int _flag_len);
		TCHAR * __cdecl StrRightTrim(const TCHAR * src_val,const TCHAR * _flag = 0);
		TCHAR * __cdecl StrRightTrimEx(const TCHAR * src_val,const TCHAR * _flag, unsigned int _flag_len);
		long __cdecl StrIndexOf(const TCHAR * src_ptr , const TCHAR * val_ptr,bool is_case_sensitive = false);
		long __cdecl StrCount(const TCHAR * src_ptr , const TCHAR * val_ptr,bool is_case_sensitive = false);
		long __cdecl StrLastIndexOf(const TCHAR * src_ptr , const TCHAR * val_ptr,bool is_case_sensitive = false);
		TCHAR * __cdecl StrUCase(const TCHAR * src_ptr) ;
		TCHAR * __cdecl StrLCase(const TCHAR * src_ptr) ;
		TCHAR * __cdecl StrReverse(const TCHAR * src_ptr) ;
		void __cdecl UpperCase(TCHAR * src_ptr) ;
		void __cdecl LowerCase(TCHAR * src_ptr) ;

		bool __cdecl StrSplit(const TCHAR * src_ptr, const TCHAR * f_val, void ** _output_ptr, unsigned int * _elm_len_ptr, bool is_removed_empty = false);


#if defined(_MSC_VER) || defined(_WINDOWS) || defined(WIN32) || defined(WINCE) || defined(_WIN32_WCE) 
		void  *   __cdecl CurCharToBSTR(const TCHAR * t) ;
		TCHAR * __cdecl BSTRToCurChar(const void * b) ;

		wchar_t * __cdecl UTF8ToWstr(const void * buff_src);
		char * __cdecl UTF8ToAnsi(const void * buff_src);
		char * __cdecl WstrToAnsi(const void * buff_src);
		wchar_t * __cdecl AnsiToWstr(const void * buff_src);
		char * __cdecl WstrToUTF8(const void * buff_src);		
		char * __cdecl AnsiToUTF8(const void * buff_src);
#endif

		static TCHAR * Float2TCHAR(float f, int precision = 10);

	private:


		short volatile _self_state ;
		bs::MemMgr * volatile _myBase ;
	
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class StringBuffer:public bs::MemMgr
	{
	public:
		StringBuffer(void);
		~StringBuffer(void);
		void __cdecl Init();
		void __cdecl Release();
		
		void  		__cdecl  append(const TCHAR * val);
		int		__cdecl  length();
		unsigned int	__cdecl  size();
		const TCHAR * 	__cdecl  c_str();
		int		__cdecl  c_str(void * _output_buff);  //return length of string  in character.
		void		__cdecl  clear(bool is_free_all = true);
		
		void		__cdecl  Append(void * block, unsigned int block_size);
		void		__cdecl  AppendLine(const TCHAR * val); 
		void		__cdecl  Trim(const TCHAR * _flag = 0);
		void		__cdecl  TrimEx(const TCHAR * _flag, unsigned int _flag_len);
		void		__cdecl  Replace(const TCHAR * old_val, const TCHAR * new_val, bool is_case_sensitive = false);
		void		__cdecl  Dispel(const TCHAR * _flag, bool is_case_sensitive = false);
		
	private:

		typedef struct _strBuffNode
		{
			unsigned int	address;
			unsigned int 	used_size ;
			unsigned int 	free_size ;
			_strBuffNode * 	prev ;
			_strBuffNode * 	next ;
		}strBuffNode;

		void * _cstr_ptr ;
		short volatile _self_state ;
		bs::MemMgr * volatile _myBase ;
		unsigned int _buffer_size ;
		strBuffNode * phead, * plast ;

		void __cdecl NewBufferNode(unsigned int buff_size);
	
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	#define CREATE_NEW          1
	#define CREATE_ALWAYS       2
	#define OPEN_EXISTING       3
	#define OPEN_ALWAYS         4
	#define TRUNCATE_EXISTING   5
	*/

	class WinFileCls : public MemMgr
	{
	public:
		WinFileCls(void);
		~WinFileCls(void);

		void __cdecl Init();
		void __cdecl Release();
		void __cdecl SetErrorInfor(const TCHAR * v, bool is_reset = false) ;

		bool __cdecl OpenDiskFile(const TCHAR * file_name , unsigned long open_type = OPEN_EXISTING, bool check_is_text_file = false);
		bool __cdecl OpenDiskFile(const TCHAR * file_name ,unsigned long read_write_mode, unsigned long shared_mode ,unsigned long open_mode ,bool check_is_text_file = false);
		bool __cdecl CloseDiskFile();
		bool __cdecl IsDiskFileExists(const TCHAR * file_name) ;
		bool __cdecl SetPtrBof();
		bool __cdecl SetPtrEof();
		bool __cdecl IsOpen();
		bool __cdecl IsBof();
		bool __cdecl IsEof();

		unsigned long __cdecl FileSize();
		unsigned short __cdecl GetFileFormat();

		TCHAR * __cdecl ShortFileName(TCHAR * buff = 0 ,size_t buff_size = 0);//add function : FileName
		TCHAR * __cdecl FilePath(TCHAR * buff = 0 ,size_t buff_size = 0);//add function : FilePath
		TCHAR * __cdecl FullFileName(TCHAR * buff = 0 ,size_t buff_size = 0);//add function : FullFileName


		void * __cdecl ReadAll(void * buff = 0 ,size_t buff_size = 0);
		unsigned long  __cdecl ReadBlock(void * buff ,unsigned long buff_size,unsigned long start_pos);
		unsigned long  __cdecl WriteBlock(void * buff ,unsigned long buff_size,unsigned long start_pos);

		unsigned long  __cdecl ReadBlock(void * buff ,unsigned long buff_size);
		unsigned long  __cdecl WriteBlock(void * buff ,unsigned long buff_size);

		unsigned long __cdecl SeekCharPos(int _char , unsigned long start_pos = 0);
		//unsigned long __cdecl WriteLine(void * buff = 0 ,size_t buff_size = 0); // just for text files

		void  __cdecl SetFilePtrPos(unsigned long pos = 0) ;
		unsigned long  __cdecl GetFilePtrPos() ;
		void __cdecl SetFileBofPos(unsigned short pos=0);

	private:
		HANDLE mhd;
		unsigned long volatile _file_size;
		bool volatile _is_open;
		unsigned short _file_format_flag ; // 00000000 0000000 
		TCHAR * _full_file_name , * _short_file_name , * _file_path ;
		TCHAR  * _err_msg ;
		static const unsigned long _ERR_BUFF_LEN = 16 * 1024 ;
		OVERLAPPED olp;
		bool __cdecl ProcessFileInfo(const TCHAR * _src_file_val);
		void __cdecl CheckIsBinaryFile();
		bool __cdecl VerifyFile(bool check_is_text_file);
		short volatile _self_state ;
		unsigned short _BOF_POS ;
		static const unsigned long _READ_BUFF_LEN = 8 * 1024 ;
		bool volatile _is_file_can_be_read ;
		bool volatile _is_file_can_be_write ;
		MemMgr * volatile _myBase ;
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	Supported text file : utf-8(0x4) , utf-16(0x2) , ansi(0x1)
	*/
	enum FILETYPE
	{
		ANSI = 1,
		UTF16 = 2,
		UTF8 = 4
	};

	class WinTxtFile : public WinFileCls
	{
	public:
		WinTxtFile(void);
		~WinTxtFile(void);

		void __cdecl Init();
		void __cdecl Release();

		bool __cdecl OpenTextFileForRead(const TCHAR * file_name);
		bool __cdecl OpenTextFileForWrite(const TCHAR * file_name, bool is_append_mode = true);
		bool __cdecl CreateTextFile(const TCHAR * file_name, unsigned short char_size = sizeof(TCHAR));
		bool __cdecl CloseTextFile();

		void *  __cdecl ReadAll(unsigned short char_size = sizeof(TCHAR));/*0:auto convert; 1: ansi format;2:unicode format; 4:utf-8 format;*/
		void *  __cdecl ReadText(unsigned long text_len, unsigned short char_size = sizeof(TCHAR));
		void *  __cdecl ReadUTF8Text(unsigned long text_len, unsigned short char_size = sizeof(TCHAR));
		void *  __cdecl ReadLine(unsigned short char_size = sizeof(TCHAR));  //just for text files

		unsigned long  __cdecl WriteText(void * buff,  unsigned short char_size = sizeof(TCHAR));/*0:auto convert; 1: ansi format;2:unicode format; 4:utf-8 format;*/
		unsigned long  __cdecl WriteLine(void * buff = 0, unsigned short char_size = sizeof(TCHAR)); // just for text files/*0:auto convert; 1: ansi format;2:unicode format; 4:utf-8 format;*/
		
		
		wchar_t * __cdecl __UTF8ToWstr(const void * buff_src);
		char * __cdecl __UTF8ToAnsi(const void * buff_src);
		char * __cdecl __WstrToAnsi(const void * buff_src);
		wchar_t * __cdecl __AnsiToWstr(const void * buff_src);
		char * __cdecl __WstrToUTF8(const void * buff_src);		
		char * __cdecl __AnsiToUTF8(const void * buff_src);

	private:
		WinFileCls * volatile _myBase ;
		bool volatile _is_valid_text_file ;
		unsigned short volatile _file_format ;
		short volatile _self_state ;

		static const unsigned char OEM_CHAR_ENTER_FALG = 0xD ;
		static const unsigned char OEM_CHAR_NEWLINE_FALG = 0xA ;
		unsigned char OEM_UNICODE_FALG[2] ;
		unsigned char OEM_UTF8_FALG[3] ;
		bool volatile _is_file_can_be_read ;
		bool volatile _is_file_can_be_write ;
		
		void * __cdecl __BuildOutputStream(void * buff_src, unsigned short char_size);
		
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

};
/**********************************************************************************************************/
/**********************************************************************************************************/
bs::BFS * CreateBFSPtr();
void FreeBFSPtr(void * p);

bs::WinFileCls * CreateWinFileClsPtr();
void FreeWinFileClsPtr(void * p);

bs::WinTxtFile * CreateWinTxtFilePtr();
void FreeWinTxtFilePtr(void * p);
/**********************************************************************************************************/
/**********************************************************************************************************/
#if defined(_NABASE_LIB_REFERENCE)
#if defined _DEBUG
	#pragma comment(lib, "nbase9d.lib")
#else
	#pragma comment(lib, "nbase9.lib")
#endif
#endif  //_NABASE_LIB_REFERENCE
/**********************************************************************************************************/
/**********************************************************************************************************/

#endif

