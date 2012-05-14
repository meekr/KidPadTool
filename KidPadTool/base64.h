#include <string>

/*std::string*/void base64_encode(std::string &, unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

int DecodeBase64(const char* pSrc, unsigned char* pDst, int nSrcLen);