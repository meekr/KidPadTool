#pragma once

#ifndef _LOW_LEVEL_HIGH_PERFORMANCE_NBASE_LIB_
#define _LOW_LEVEL_HIGH_PERFORMANCE_NBASE_LIB_

/**********************************************************************************************************/
/**********************************************************************************************************/

namespace bs
{
	const size_t MEMMGR_CLSID = 0x00000001;
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
			size_t pv ;
			_MemPtrListNode * prev  ;
			_MemPtrListNode * next ;
		}MemPtrListNode;

		void __cdecl InsertPtrNode(size_t pv);
		bool __cdecl RemovePtrNode(size_t pv);
		bool __cdecl IsPtrInList(size_t pv);

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
		TCHAR * __cdecl StrTrimEx(const TCHAR * src_val,const TCHAR * _flag, size_t _flag_len);
		TCHAR * __cdecl StrLeftTrim(const TCHAR * src_val,const TCHAR * _flag = 0);
		TCHAR * __cdecl StrLeftTrimEx(const TCHAR * src_val,const TCHAR * _flag, size_t _flag_len);
		TCHAR * __cdecl StrRightTrim(const TCHAR * src_val,const TCHAR * _flag = 0);
		TCHAR * __cdecl StrRightTrimEx(const TCHAR * src_val,const TCHAR * _flag, size_t _flag_len);
		long __cdecl StrIndexOf(const TCHAR * src_ptr , const TCHAR * val_ptr,bool is_case_sensitive = false);
		long __cdecl StrCount(const TCHAR * src_ptr , const TCHAR * val_ptr,bool is_case_sensitive = false);
		long __cdecl StrLastIndexOf(const TCHAR * src_ptr , const TCHAR * val_ptr,bool is_case_sensitive = false);
		TCHAR * __cdecl StrUCase(const TCHAR * src_ptr) ;
		TCHAR * __cdecl StrLCase(const TCHAR * src_ptr) ;
		TCHAR * __cdecl StrReverse(const TCHAR * src_ptr) ;
		void __cdecl UpperCase(TCHAR * src_ptr) ;
		void __cdecl LowerCase(TCHAR * src_ptr) ;

		bool __cdecl StrSplit(const TCHAR * src_ptr, const TCHAR * f_val, void ** _output_ptr, size_t * _elm_len_ptr, bool is_removed_empty = false);


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
		size_t	__cdecl  size();
		const TCHAR * 	__cdecl  c_str();
		int		__cdecl  c_str(void * _output_buff);  //return length of string  in character.
		void		__cdecl  clear(bool is_free_all = true);
		
		void		__cdecl  Append(void * block, size_t block_size);
		void		__cdecl  AppendLine(const TCHAR * val); 
		void		__cdecl  Trim(const TCHAR * _flag = 0);
		void		__cdecl  TrimEx(const TCHAR * _flag, size_t _flag_len);
		void		__cdecl  Replace(const TCHAR * old_val, const TCHAR * new_val, bool is_case_sensitive = false);
		void		__cdecl  Dispel(const TCHAR * _flag, bool is_case_sensitive = false);
		
	private:

		typedef struct _strBuffNode
		{
			size_t	address;
			size_t 	used_size ;
			size_t 	free_size ;
			_strBuffNode * 	prev ;
			_strBuffNode * 	next ;
		}strBuffNode;

		void * _cstr_ptr ;
		short volatile _self_state ;
		bs::MemMgr * volatile _myBase ;
		size_t _buffer_size ;
		strBuffNode * phead, * plast ;

		void __cdecl NewBufferNode(size_t buff_size);
	
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
#ifndef _LIST_UTIL_FUNCS
#define _LIST_UTIL_FUNCS 1
#ifdef  _LIST_UTIL_FUNCS
	typedef int (__cdecl * ListCompareFunc)(const void *, const void *);
	typedef int (__cdecl * ListDeleteFilterFunc)(const void *);
#endif
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class SLists:public MemMgr
	{

	public:
		SLists(void);
		SLists(size_t memblock_size);
		~SLists(void);
		void	__cdecl Init();
		void	__cdecl Release();
		void	__cdecl SetMemBlockSize(size_t memblock_size);

		void *		__cdecl FirstEntry();
		void *		__cdecl LastEntry();
		void *		__cdecl GetEntryByNo(size_t entry_no);
		void *		__cdecl GetEntry(size_t * ptr);
		size_t	__cdecl GetEntryCount();

		void *  __cdecl NewEntry(); 
		void *  __cdecl InsertEntry(void * entry = 0); 
		bool	__cdecl DeleteEntryByNo(size_t entry_no);
		bool	__cdecl DeleteEntry(void * entry , size_t * ptr = 0);
		bool	__cdecl DeleteEntryByFilter(void * filter_func_ptr);
		bool	__cdecl DeleteAllEntries();

		bool __cdecl SwapEntry(const void * entry1 , const void * entry2);
		bool __cdecl Collocate(void * cmp_func_ptr, size_t entry_ptr_offset = 0);
		bool __cdecl Truncate(const void * entry);

	private:
		typedef struct _simple_list_t
		{
			void * entry ;
			_simple_list_t * next ;
		}SimpleListInfo;


		MemMgr * volatile _myBase ;
	
		short volatile _self_state ;
		size_t _memblock_size ;
		size_t volatile _entry_count ;
		SimpleListInfo * volatile _phead ;
		SimpleListInfo * volatile _plast;
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class DynLists:public MemMgr
	{
	public:
		DynLists(void);
		DynLists(size_t memblock_size);
		~DynLists(void);
		void	__cdecl Init();
		void	__cdecl Release();
		void	__cdecl SetMemBlockSize(size_t memblock_size);

		void *		__cdecl FirstEntry();
		void *		__cdecl LastEntry();
		void *		__cdecl GetEntryByNo(size_t entry_no);
		void *		__cdecl GetEntry(size_t * ptr);
		size_t	__cdecl GetEntryCount();

		void *  __cdecl NewEntry(); 
		void *  __cdecl InsertEntry(void * entry = 0); 
		bool	__cdecl DeleteEntryByNo(size_t entry_no);
		bool	__cdecl DeleteEntry(void * entry , size_t * ptr = 0);
		bool	__cdecl DeleteEntryByFilter(void * filter_func_ptr);
		bool	__cdecl DeleteAllEntries();

		bool __cdecl SwapEntry(const void * entry1 , const void * entry2);
		bool __cdecl Collocate(void * cmp_func_ptr, size_t entry_ptr_offset = 0);
		bool __cdecl Truncate(const void * entry);

	private:
		typedef struct _dynamic_list_t
		{
			void * entry ;
			_dynamic_list_t * prev ;
			_dynamic_list_t * next ;
		}DynamicListInfo;

		MemMgr * volatile _myBase ;

		short volatile _self_state ;
		size_t _memblock_size ;
		size_t volatile _entry_count ;
		DynamicListInfo * volatile _phead , * volatile _plast ;

		bool __cdecl _RemoveNode(DynamicListInfo * _node);
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class SimpleByteEncrypt
	{
	public:
		SimpleByteEncrypt(void);
		~SimpleByteEncrypt(void);
		
		void __cdecl Init();
		void __cdecl Release();
		
		bool __cdecl SetKey(const void * key_buff, size_t key_buff_len);
		bool __cdecl Encrypt(void * buff, size_t buff_len, bool _ignore_zero = false);
		bool __cdecl Encrypt(const void * key_buff, size_t key_buff_len, void * buff, size_t buff_len,bool _ignore_zero = false);
		

	private:

		short _self_state ;
		
		unsigned char * _enc_key_buff ;
		size_t    _enc_key_buff_len ;
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class Queue:public MemMgr
	{

	public:
		Queue(void);
		~Queue(void);
		
		void	__cdecl Init();
		void	__cdecl Release();
		
		void 	__cdecl Push(void * data);
		void *	__cdecl Pop();
		int	__cdecl GetEntryCount();

		
	private:
		
		typedef struct _simple_queue_t
		{
			_simple_queue_t * prev ;
			void * data ;
			_simple_queue_t * next ;
		}QueueNodeInfo;

		MemMgr * volatile _myBase ;
	
		short volatile _self_state ;
		
		
		int volatile _entry_count ;
		QueueNodeInfo * volatile _head, * volatile _last ;
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class StdFileReader : public MemMgr
	{
	public:
		StdFileReader(void);
		~StdFileReader(void);

		void __cdecl Init();
		void __cdecl Release();

		bool __cdecl OpenDiskFileForRead(const TCHAR * file_name , bool check_is_text_file = false);
		bool __cdecl CloseDiskFile();
		bool __cdecl IsDiskFileExists(const TCHAR * file_name) ;

		bool __cdecl IsOpen();
		bool __cdecl IsBof();
		bool __cdecl IsEof();
		bool __cdecl SetPtrBof();
		bool __cdecl SetPtrEof();
		void  __cdecl SetFilePtrPos(unsigned long pos = 0) ;
		unsigned long  __cdecl GetFilePtrPos() ;
		void __cdecl SetFileBofPos(unsigned short pos=0);

		unsigned long __cdecl FileSize();
		unsigned short __cdecl GetFileFormat();

		TCHAR * __cdecl ShortFileName(TCHAR * buff = 0 ,size_t buff_size = 0);//add function : FileName
		TCHAR * __cdecl FilePath(TCHAR * buff = 0 ,size_t buff_size = 0);//add function : FilePath
		TCHAR * __cdecl FullFileName(TCHAR * buff = 0 ,size_t buff_size = 0);//add function : FullFileName


		void * __cdecl ReadAll(void * buff = 0 ,size_t buff_size = 0);
		unsigned long  __cdecl ReadBlock(void * buff ,unsigned long buff_size,unsigned long start_pos);
		unsigned long  __cdecl ReadBlock(void * buff ,unsigned long buff_size);


	private:
		int mhd;

		typedef struct _std_file_ext_info
		{
			unsigned long _size;
			unsigned long _ptr_pos;
			unsigned short _BOF_POS ;
			unsigned short _file_format_flag ; // 00000000 0000000 
			bool _is_open;

		}STDFINFO;

		STDFINFO _finfo ;
		short _self_state ;
		static const size_t _READ_BUFF_LEN = 8 * 1024 ;
		MemMgr * _myBase ;
		TCHAR * _full_file_name , * _short_file_name , * _file_path ;

		bool __cdecl ProcessFileInfo(const TCHAR * _src_file_val);
		void __cdecl CheckIsBinaryFile();
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class StdFileWriter : public MemMgr
	{
	public:
		StdFileWriter(void);
		~StdFileWriter(void);

		void __cdecl Init();
		void __cdecl Release();

		bool __cdecl CreateDiskFileForWrite(const TCHAR * file_name);
		bool __cdecl OpenDiskFileForWrite(const TCHAR * file_name , bool check_is_text_file = false);
		bool __cdecl CloseDiskFile();
		bool __cdecl IsDiskFileExists(const TCHAR * file_name) ;

		bool __cdecl IsOpen();
		bool __cdecl IsBof();
		bool __cdecl IsEof();
		bool __cdecl SetPtrBof();
		bool __cdecl SetPtrEof();
		void  __cdecl SetFilePtrPos(unsigned long pos = 0) ;
		unsigned long  __cdecl GetFilePtrPos() ;
		void __cdecl SetFileBofPos(unsigned short pos=0);

		unsigned long __cdecl FileSize();
		unsigned short __cdecl GetFileFormat();

		TCHAR * __cdecl ShortFileName(TCHAR * buff = 0 ,size_t buff_size = 0);//add function : FileName
		TCHAR * __cdecl FilePath(TCHAR * buff = 0 ,size_t buff_size = 0);//add function : FilePath
		TCHAR * __cdecl FullFileName(TCHAR * buff = 0 ,size_t buff_size = 0);//add function : FullFileName


		unsigned long  __cdecl WriteBlock(void * buff ,unsigned long buff_size,unsigned long start_pos);
		unsigned long  __cdecl WriteBlock(void * buff ,unsigned long buff_size);


	private:
		int mhd;

		typedef struct _std_file_ext_info
		{
			unsigned long _size;
			unsigned long _ptr_pos;
			unsigned short _BOF_POS ;
			unsigned short _file_format_flag ; // 00000000 0000000 
			bool _is_open;

		}STDFINFO;

		STDFINFO _finfo ;
		short _self_state ;
		static const size_t _READ_BUFF_LEN = 8 * 1024 ;
		MemMgr * _myBase ;
		TCHAR * _full_file_name , * _short_file_name , * _file_path ;

		bool __cdecl ProcessFileInfo(const TCHAR * _src_file_val);
		void __cdecl CheckIsBinaryFile();
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class b64util
	{
	public:
		static size_t __cdecl  __base64_encode(const unsigned char *value, int vlen, char * output_buff);
		static size_t __cdecl __base64_decode(const char *value, size_t vlen, unsigned char * output_buff);
		static bool __cdecl  ConvertFileToBase64(TCHAR * filename, const TCHAR * new_file_name = 0);
		static bool __cdecl  ConvertFileFromBase64(TCHAR * filename, const TCHAR * new_file_name = 0);
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
/**********************************************************************************************************/
/**********************************************************************************************************/
namespace nds
{
	typedef void (__cdecl * STreeNodeDeallocateFunc)(void *, const void *);
	typedef int  (__cdecl * STreeNodeSeekFunc)(void * , const void * , const void * );

	typedef struct _ds_tree_node
	{
		_ds_tree_node *	parent_node ;
		_ds_tree_node *	prev_sibling_node ;
		_ds_tree_node *	next_sibling_node ;
		_ds_tree_node *	first_child_node ;
		void * 		data ;
	}STreeNode ;
	typedef struct _ds_tree_index_node
	{
		size_t node_ptr;
		void * path ;
		unsigned short level;			//root level is 0.
		unsigned short status_bit;		//1:marked will be deleted.

	}TreeIndexNode;

	class STree:public bs::MemMgr
	{

	public:
		STree(void);
		STree(size_t memblock_size);
		~STree(void);
		void	__cdecl SetMemBlockSize(size_t memblock_size);
		void	__cdecl Init();
		void	__cdecl Release();
		
		void __cdecl SetNodeDeallocateFunc(void * _ptr);

		STreeNode *	__cdecl CreateRootNode();
		STreeNode *	__cdecl GetRootNode();
		STreeNode *	__cdecl GetNodeExt(void * _ptr, const void * _param_ptr_1=0,const void * _param_ptr_2=0);
		STreeNode *	__cdecl NewSiblingNode(STreeNode * _target_node, size_t _insert_pos = 0); 			// 0: insert after;		1: insert before. 
		STreeNode *	__cdecl NewSubNode(STreeNode * _target_node, size_t _insert_pos = 0);			// 0: last child node;		1: first child node. 
		bool		__cdecl DeleteNode(STreeNode * _target_node);
		bool		__cdecl DeleteAllNodes();
		
		
		/*void __cdecl LockStree();
		void __cdecl UnLockStree();*/

	private:
		

		
		short volatile _self_state ;
		size_t NODESIZE, _memblock_size; 
		size_t volatile _tree_node_num;
		STreeNode * volatile _root_node ;
		void  * _index_list , * _node_deallocate_func ;
		bool volatile _is_locked ;
		

		bs::MemMgr * volatile _myBase ;
		
		void __cdecl RecordSTreeNodeBehavior(const STreeNode * _node_ptr);
		void __cdecl RemoveSTreeNodeBehavior(const STreeNode *, bool _is_free_data = false);
		bool __cdecl IsNodeValidly(const void * _node_ptr);

	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class STable:public bs::MemMgr
	{

	public:
		STable(void);
		~STable(void);
		void __cdecl Init();
		void __cdecl Release();
		bool __cdecl BuildTable(size_t column_num);
		void * __cdecl InsertRow(void * hdl = 0);
		void * __cdecl  GetRowByNo(size_t num);
		void * __cdecl  GetRow(size_t * pos);
		bool __cdecl SetCell(void * row_hdl, size_t col_num, const void * data, size_t data_len);
		bool __cdecl GetCell(void * row_hdl, size_t col_num, void ** output_buff, size_t * lp_dlen);

		void * __cdecl DeleteRow(void * row_hdl);

		bool __cdecl SwapRow(const void * row_hdl_1, const void * row_hdl_2);

		bool __cdecl CollocateByColumn(size_t col_num, void * cmp_func_ptr);

	private:
		short volatile _self_state ;
		size_t volatile _rows_count ;
		size_t volatile _column_count ;
		void * volatile _rows_ptr ;

		size_t ROWSIZE, CELLSIZE ; 
		
		bs::MemMgr * volatile _myBase ;

		typedef struct _ds_data_cell
		{
			void * 		data ;
			size_t	data_len ;
		}TableCell ;

		void __cdecl ReleaseRowsList();
	};
}
/**********************************************************************************************************/
/**********************************************************************************************************/
class BigNumMut:public bs::MemMgr
{

public:
	BigNumMut(void);
	~BigNumMut(void);
	void __stdcall Init();
	void __stdcall Release();
	//int __stdcall Mutiplicative(unsigned char * _number1, size_t l1, unsigned char * _number2 , size_t l2, unsigned char * _result);
	int __stdcall Mutiplicative(unsigned char * _number1, unsigned char * _number2 ,unsigned char * _result);
	void __stdcall SetMutiplierBufferSize(size_t _size);
	void __stdcall SetResultBufferSize(size_t _size);
	
	unsigned char * __stdcall Integer2Bytes(int _number);
	char * __stdcall Bytes2String(unsigned char * buff, char * buff_output);
	
private:

	static bool  __stdcall MutiplicativeCalculater(unsigned char  multiplier, unsigned char multiplicand, int bit_pos, unsigned char * _result_buff);
	short _self_state;
	bs::MemMgr * _myBase ;
	size_t _buff_size , _result_buff_size ;
	unsigned char  * multiplier_buff, * multiplicand_buff;
};


/**********************************************************************************************************/
/**********************************************************************************************************/
extern "C"  __declspec(noinline) void __cdecl InitLLHPEnv(int user_page_size=0, int user_max_block_size = 0, bool auto_shrink_page = false); 
extern "C"  __declspec(noinline) void __cdecl ExitLLHPEnv(); 

bs::MemMgr * CreateMemMgrClsPtr();
void FreeMemMgrClsPtr(void * p);

bs::BFS * CreateBFSPtr();
void FreeBFSPtr(void * p);

bs::StringBuffer * CreateStringBufferPtr();
void FreeStringBufferPtr(void * p);

bs::WinFileCls * CreateWinFileClsPtr();
void FreeWinFileClsPtr(void * p);

bs::WinTxtFile * CreateWinTxtFilePtr();
void FreeWinTxtFilePtr(void * p);

bs::SLists * CreateSListsPtr();
void FreeSListsPtr(void * p);

bs::DynLists * CreateDynListsPtr();
void FreeDynListsPtr(void * p);

nds::STree * CreateSTreePtr();
void FreeSTreePtr(void * p);

nds::STable * CreateSTablePtr();
void FreeSTablePtr(void * p);

bs::SimpleByteEncrypt * CreateSimpleByteEncryptPtr();
void FreeSimpleByteEncryptPtr(void * p);

bs::StdFileReader * CreateStdFileReaderPtr();
void FreeStdFileReaderPtr(void * p);

bs::StdFileWriter * CreateStdFileWriterPtr();
void FreeStdFileWriterPtr(void * p);

bs::Queue * CreateQueuePtr();
void FreeQueuePtr(void * p);

BigNumMut * CreateBigNumMutPtr();
void FreeBigNumMutPtr(void * p);
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

