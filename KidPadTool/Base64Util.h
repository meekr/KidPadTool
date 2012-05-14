#pragma warning(disable:4786 4244)
#include <string>
#include <iostream>
/////////////////////////////////////////////////////////////////////////////////////////
static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}
/////////////////////////////////////////////////////////////////////////////////////////
const unsigned char Base64Table[64]=
{
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
	'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
	'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
	'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
const unsigned char Base64RTable[128]=
{
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X3E,0XFF,0XFF,0XFF,0X3F,
	0X34,0X35,0X36,0X37,0X38,0X39,0X3A,0X3B,0X3C,0X3D,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0X00,0X01,0X02,0X03,0X04,0X05,0X06,0X07,0X08,0X09,0X0A,0X0B,0X0C,0X0D,0X0E,
	0X0F,0X10,0X11,0X12,0X13,0X14,0X15,0X16,0X17,0X18,0X19,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0X1A,0X1B,0X1C,0X1D,0X1E,0X1F,0X20,0X21,0X22,0X23,0X24,0X25,0X26,0X27,0X28,
	0X29,0X2A,0X2B,0X2C,0X2D,0X2E,0X2F,0X30,0X31,0X32,0X33,0XFF,0XFF,0XFF,0XFF,0XFF
};
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
static unsigned int Base64Encode(
	const unsigned char *psrc,
	unsigned int size,
	unsigned char *pdest,
	unsigned int *psize)
{
	if(psrc == NULL || size == 0)
		return 0;
	if( *psize == 0)
	{
		if(size % 3 == 0)
		{
			*psize=(size / 3)*4;
		}
		else
		{
			*psize=(size /3)*4 + 4;
		}
		return 1;
	}

	const unsigned char *ptable=Base64Table;
	const unsigned char *ps=psrc;
	unsigned char *pd=pdest;
	unsigned int mod=size % 3;

	if(mod == 0)
	{
		while( (unsigned int)(ps - psrc) < size )
		{
			*pd=*(ptable + ((*ps >> 2) & 0X3F));
			pd++;
			*pd=*(ptable + (((*ps << 4) + (*(ps+1) >> 4)) & 0X3F));
			pd++;
			*pd=*(ptable + (((*(ps+1) << 2) + (*(ps+2) >> 6)) & 0X3F));
			pd++;
			*pd=*(ptable + (*(ps+2) & 0X3F));
			pd++;

			ps += 3;
		}
	}
	else
	{
		while((unsigned int)(ps - psrc) < size - mod)
		{
			*pd=*(ptable + ((*ps >> 2) & 0X3F));
			pd++;
			*pd=*(ptable + (((*ps << 4) + (*(ps+1) >> 4)) & 0X3F));
			pd++;
			*pd=*(ptable + (((*(ps+1) << 2) + (*(ps+2) >> 6)) & 0X3F));
			pd++;
			*pd=*(ptable + (*(ps+2) & 0X3F));
			pd++;

			ps += 3;
		}

		unsigned char buff[16]={0};
		memcpy_s(buff,16,ps,mod);
		unsigned char *p=buff;

		if(mod == 1)
		{
			*pd=*(ptable + ((*p >> 2) & 0X3F)) ;
			pd++;
			*pd=*(ptable + (((*p << 4) + (*(p+1) >> 4)) & 0X3F));
			pd++;
			*pd='=';
			*pd++;
			*pd='=';
		}
		else if(mod == 2)
		{
			*pd=*(ptable + ((*p >> 2) & 0X3F));
			pd++;
			*pd=*(ptable + (((*p << 4) + (*(p+1) >> 4)) & 0X3F));
			pd++;
			*pd=*(ptable + (((*(p+1) << 2) + (*(p+2) >> 6)) & 0X3F));
			pd++;
			*pd='=';
		}
	}

	return 1;
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
static unsigned int Base64Decode(
	const char *psrc,
	unsigned int size,
	unsigned char *pdest,
	unsigned int *psize)
{
	if(psrc == NULL || size == 0 || ((size % 4) != 0) )
		return 0;

	if( *psize == 0)
	{
		*psize=(size/4)*3;
		//return 1;
	}
	const unsigned int ul4=4;

	const unsigned char *ptable=Base64RTable;
	const char *ps=psrc;
	unsigned char *pd=pdest;
	unsigned int realsize=0;
	unsigned int mod=0;

	const char *pequal=NULL;
	pequal=strchr(psrc,'=');
	if(pequal == NULL)
	{
		realsize=size;
	}
	else
	{
		realsize= (unsigned int)(pequal - psrc);
	}
	mod=realsize % 4;

	if(mod == 0)
	{
		while( (unsigned int)(ps - psrc ) < realsize)
		{
			*pd= ( *(ptable + *ps) << 2) + (*(ptable + *(ps+1)) >> 4);
			pd++;
			*pd= (*(ptable + *(ps+1)) << 4) + (*(ptable + *(ps+2)) >> 2);
			pd++;
			*pd= (*(ptable + *(ps+2)) << 6) + (*(ptable + *(ps+3)));
			pd++;

			ps += 4;
		}
	}
	else
	{
		while((unsigned int)(ps - psrc) < size - ul4)
		{
			*pd= ( *(ptable + *ps) << 2) + (*(ptable + *(ps+1)) >> 4);
			pd++;
			*pd= (*(ptable + *(ps+1)) << 4) + (*(ptable + *(ps+2)) >> 2);
			pd++;
			*pd= (*(ptable + *(ps+2)) << 6) + (*(ptable + *(ps+3)));
			pd++;

			ps += 4;
		}

		unsigned int uequal=size - realsize;

		char buff[16]={0};
		memcpy_s(buff,16,ps,ul4 - uequal);
		char *p=buff;


		if(uequal == 1)
		{
			*pd= (*(ptable + *p) << 2) + (*(ptable + *(p+1)) >> 4);
			pd++;
			*pd=(*(ptable + *(p+1)) << 4) +(*(ptable + *(p+2)) >> 2);

		}
		else if(uequal == 2)
		{
			*pd=(*(ptable + *p) << 2) + (*(ptable + *(p+1)) >> 4);

		}
	}

	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
static bool  __CString2AnsiStdString(CString src_str, void * output_buff, unsigned int * buff_len)
{
	BSTR bb = src_str.AllocSysString()   ;
	unsigned int l = ::SysStringLen(bb);
	char * cc = (char*) malloc(l + 1);
	memset(cc, 0, l + 1);
	for(unsigned int i=0;i<=l;++i)
	{
		memset( (void *)((unsigned int)cc + i), bb[i], 1);
	}
	
	::SysFreeString(bb); 
	bb = 0 ;

	memcpy(buff_len, &l, sizeof(unsigned int));
	memcpy(output_buff, &cc, sizeof(unsigned int));

	return  true ;
}
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
static unsigned int __Base64Decode(CString src_str, void ** output_ptr)
{
	char * c_src = 0;
	unsigned char * c_result = 0;
	unsigned int src_len = 0, result_len = 0 ;

	__CString2AnsiStdString(src_str, (void*)(&c_src), &src_len);

	c_result = (unsigned char*) malloc(src_len + 1);
	memset(c_result, 0, src_len + 1);

	Base64Decode(c_src, src_len, c_result, &result_len);
	free(c_src);
	(*output_ptr) = (void*)c_result;

	return result_len ;
}
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
static CString GetBase64DataFromTextFile(TCHAR * filename)
{
	HANDLE mhd = (HANDLE)::CreateFile(filename ,
			GENERIC_READ,	//GENERIC_READ| GENERIC_WRITE,
			FILE_SHARE_READ,		//FILE_SHARE_READ | FILE_SHARE_WRITE ,
			0,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,//|FILE_FLAG_OVERLAPPED,
			0
			);

	if(mhd == INVALID_HANDLE_VALUE) { return FALSE; }
	DWORD _file_size = ::GetFileSize(mhd , NULL) ;
	DWORD _read_size = 0 ;
	unsigned char * buff = (unsigned char*)malloc(_file_size + 1);
	memset(buff, 0, _file_size + 1);
	::ReadFile(mhd, buff, _file_size, &_read_size, 0);
	::CloseHandle(mhd);

	CString rtv ;
	for(unsigned int i=0; i<=_file_size; ++i)
	{
		if(is_base64(buff[i]))	{ rtv.AppendChar(buff[i]); }
	}

	return rtv ;
}