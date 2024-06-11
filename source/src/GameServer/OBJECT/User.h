#pragma once

#include "Allocator.hpp"

class CUser : public Object
{
public:
	CUser();
	CUser(INT16 ssnid);
	~CUser();

	void Clear();

	void SetSSNID(INT16 ssnId) { m_SSNID = ssnId; }
	INT16	SSNID()const { return m_SSNID; }

public:
	INT64			account_seq;

private:
	INT16			m_SSNID;	
};