#pragma once

#ifndef _LOW_LEVEL_HIGH_PERFORMANCE_NBASE_LIB_
#define _LOW_LEVEL_HIGH_PERFORMANCE_NBASE_LIB_

/**********************************************************************************************************/
/**********************************************************************************************************/

namespace bs
{
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
#endif

	private:


		short volatile _self_state ;
		bs::MemMgr * volatile _myBase ;
	
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class WinFileCls : public MemMgr
	{
	public:
		WinFileCls(void);
		~WinFileCls(void);

		void __cdecl Init();
		void __cdecl Release();

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
		OVERLAPPED olp;
		bool __cdecl ProcessFileInfo(const TCHAR * _src_file_val);
		void __cdecl CheckIsBinaryFile();
		bool __cdecl VerifyFile(const TCHAR * file_name,bool check_is_text_file);
		short volatile _self_state ;
		unsigned short _BOF_POS ;
		static const unsigned long _READ_BUFF_LEN = 4096 ;
		bool volatile _is_file_can_be_read ;
		bool volatile _is_file_can_be_write ;
		MemMgr * volatile _myBase ;
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class WinTxtFile : public WinFileCls
	{
	public:
		WinTxtFile(void);
		~WinTxtFile(void);

		void __cdecl Init();
		void __cdecl Release();

		bool __cdecl OpenTextFile(const TCHAR * file_name , unsigned long open_type = OPEN_EXISTING);
		bool __cdecl OpenTextFileForRead(const TCHAR * file_name);
		bool __cdecl OpenTextFileForWrite(const TCHAR * file_name);
		bool __cdecl CreateTextFile(const TCHAR * file_name);
		bool __cdecl CloseTextFile();
		bool __cdecl IsOpen();
		bool __cdecl IsEof();

		unsigned long __cdecl FileSize();

		TCHAR * __cdecl ShortFileName(TCHAR * buff = 0 ,size_t buff_size = 0);//add function : FileName
		TCHAR * __cdecl FilePath(TCHAR * buff = 0 ,size_t buff_size = 0);//add function : FilePath
		TCHAR * __cdecl FullFileName(TCHAR * buff = 0 ,size_t buff_size = 0);//add function : FullFileName


		void * __cdecl ReadAll(unsigned int _buff_format = 0,void * buff = 0 ,size_t buff_size = 0);/*0:auto convert; 1: ansi format;2:unicode format*/
		void *  __cdecl ReadText(unsigned int _output_format ,unsigned long text_num,unsigned long start_pos);
		void *  __cdecl ReadText(unsigned int _output_format ,unsigned long text_num);
		void *  __cdecl ReadText(unsigned long text_num);

		unsigned long  __cdecl WriteText(void * buff ,unsigned long start_pos,unsigned int _buff_format = 0);/*0:auto convert; 1: ansi format;2:unicode format*/
		unsigned long  __cdecl WriteText(void * buff,unsigned int _buff_format = 0);/*0:auto convert; 1: ansi format;2:unicode format*/

		//below 3 functions use current file ptr pos
		void * __cdecl ReadLine(unsigned int _output_format = 0);  //just for text files
		unsigned long __cdecl WriteLine(void * buff = 0,unsigned int _buff_format = 0); // just for text files/*0:auto convert; 1: ansi format;2:unicode format*/
		void  __cdecl SetFilePtrPos(unsigned long pos = 0) ;


	private:
		WinFileCls * volatile _myBase ;
		bool volatile _is_valid_text_file ;
		bool volatile _is_file_unicode_format ;//_is_file_unicode_format:format of openned file , not application character settings ;
		short volatile _self_state ;

		static const unsigned char OEM_CHAR_ENTER_FALG = 0xD ;
		static const unsigned char OEM_CHAR_NEWLINE_FALG = 0xA ;
		unsigned char OEM_UNICODE_FALG[2] ;
		bool volatile _is_file_can_be_read ;
		bool volatile _is_file_can_be_write ;

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
#pragma comment(lib, "nbase.lib")
#endif  //_NABASE_LIB_REFERENCE
/**********************************************************************************************************/
/**********************************************************************************************************/

#endif

