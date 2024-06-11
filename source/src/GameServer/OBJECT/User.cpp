#include "pch.h"
#include "User.h"

CUser::CUser() : m_SSNID(0), account_seq(0)
{
	Clear();
}

CUser::CUser(INT16 ssnid)
{
	Clear();

	m_SSNID = ssnid;
}

CUser::~CUser()
{

}

void CUser::Clear()
{
	m_SSNID = 0;
	account_seq = 0;

}