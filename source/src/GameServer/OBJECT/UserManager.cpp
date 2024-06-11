#include "pch.h"
#include "UserManager.h"

#include "User.h"

namespace GLOBAL
{
	CUserManager USER_MANAGER;
}

CUserManager::~CUserManager()
{
	//CUser::_purge_memory();
}

eSERVER_RESULT CUserManager::Initialize(INT16 _capacity /*= NET::SESSION_MAX*/)
{
	m_active_users.reserve(_capacity);

	return SERVER_OK;
}

void CUserManager::Finalize()
{
}

eSERVER_RESULT CUserManager::Insert(CUser* _user)
{
	m_session_map[_user->SSNID()] = _user;

	return SERVER_OK;
}

CUser* CUserManager::FindActiveUserFromSSNID(INT16 _ssnid)
{
	auto it = m_session_map.find(_ssnid);
	if (it == m_session_map.end())
		return nullptr;

	return it->second;
}

eSERVER_RESULT CUserManager::Remove(CUser* _user)
{
	m_session_map.erase(_user->SSNID());

	return SERVER_OK;
}