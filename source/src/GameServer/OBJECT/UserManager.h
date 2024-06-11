#pragma once

class CUserManager
{
public:
	~CUserManager();

	eSERVER_RESULT	Initialize(INT16 _capacity = NET::SESSION_MAX);
	void	Finalize();

	eSERVER_RESULT	Insert(CUser* _user);

	CUser* FindActiveUserFromSSNID(INT16 _ssnid);

	eSERVER_RESULT Remove(CUser* _user);

	template <typename T>
	void ForAllUsers(T&& lambda)
	{
		for (auto session : m_session_map)
		{
			lambda(session.second);
		}
	}

private:
	std::unordered_map<INT64/*userid*/, CUser*> m_active_users;
	std::unordered_map<INT16/*ssnid*/, CUser*> m_session_map;
};

namespace GLOBAL
{
	extern CUserManager USER_MANAGER;
}