#include "string"
class CMyClass
{
public:
	CMyClass();
	~CMyClass();
	void SetSid(int nid);
	void SetName(char *strname);
	const int GetSid();
	const char*  GetName();
private:
	int m_nsid;
	std::string m_strname;
};

