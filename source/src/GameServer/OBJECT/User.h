#pragma once

class CUser
{
public:
	CUser(INT16 ssnid);
	~CUser();

	INT16	SSNID()const { return m_SSNID; }

public:
	INT64			account_seq;

private:
	INT16			m_SSNID;	
};