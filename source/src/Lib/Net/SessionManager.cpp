#include "pch.h"
#include "SessionManager.h"

#include "Session.h"

namespace NET
{
	CSessionManager::_QUEUE_SSNID::_QUEUE_SSNID()
		: m_base(0)
		, m_capacity(0)
	{
	}

	CSessionManager::_QUEUE_SSNID::~_QUEUE_SSNID()
	{
		m_set_ssnids.clear();
	}

	// 멤버 함수 구현
	void CSessionManager::_QUEUE_SSNID::reserve(const INT16 _capacity, const INT16 _base /*= 0*/)
	{
		m_base = _base;
		m_capacity = _capacity;

		INT16 _max = _base + _capacity;

		for (INT16 i = _base; _max > i; ++i)
		{
			m_set_ssnids.insert(i);
		}
	}

	bool CSessionManager::_QUEUE_SSNID::push(const INT16 _ssnid)
	{
		if ((m_base <= _ssnid) && ((m_base + m_capacity) > _ssnid))
		{
			auto	iterS = std::find(m_set_ssnids.begin(), m_set_ssnids.end(), _ssnid);
			auto	iterE = m_set_ssnids.end();

			//	존재하지 않으면
			if (iterS == iterE)
				m_set_ssnids.insert(_ssnid);

			return true;
		}

		return false;
	}

	INT16 CSessionManager::_QUEUE_SSNID::pop()
	{
		if (!m_set_ssnids.empty())
		{
			auto begin = m_set_ssnids.begin();
			INT16 _ssnid = *begin;

			m_set_ssnids.erase(begin);

			return _ssnid;
		}

		return INVALID_SSNID;
	}

	CSessionManager::_ARRAY_SESSION::_ARRAY_SESSION()
		: m_active(0)
	{
	}

	CSessionManager::_ARRAY_SESSION::~_ARRAY_SESSION()
	{
		m_sessions.clear();
	}

	// 멤버 함수
	void CSessionManager::_ARRAY_SESSION::reserve(const INT16 _capacity)
	{
		m_sessions.resize(_capacity, NULL);
	}

	void CSessionManager::_ARRAY_SESSION::insert(const INT16 _index, std::shared_ptr<CSession> _session)
	{
		if (nullptr == _session)
			return;

		m_active += 1;
		m_sessions[_index] = _session;
	}

	void CSessionManager::_ARRAY_SESSION::remove(const INT16 _index)
	{
		m_active -= 1;
		m_sessions[_index] = nullptr;
	}

	INT16 CSessionManager::_ARRAY_SESSION::findemptyssnid()
	{
		auto size = capacity();
		for (int i = 0; i < size; ++i)
		{
			if (m_sessions[i] == nullptr) return i;
		}

		return INVALID_SSNID;
	}

	std::shared_ptr<CSession> CSessionManager::_ARRAY_SESSION::get(const INT16 _index)
	{
		if ((0 > _index) || (m_sessions.capacity() <= _index))
		{
			// 			_ERROR_LOG( "The session is out of range (Range: 0~%d, Request: %d)",
			// 				m_sessions.capacity() - 1, _index );

			return NULL;
		}

		return m_sessions[_index];
	}

	bool CSessionManager::Initialize(INT16 _capacity /*= SESSION_MAX*/,
		INT16 _server_capacity /*= SERVER_SESSION_MAX*/)
	{
		_ASSERTE((0 == m_active_sessions.capacity()) && (0 == m_inactive_ssnids.capacity()));

		m_active_sessions.reserve(_capacity + _server_capacity);
		m_inactive_ssnids.reserve(_capacity);
		//m_inactive_server_ssnids.reserve(_server_capacity, _capacity);

		return true;
	}

	void CSessionManager::Finalize()
	{
		INT16 _capacity = m_active_sessions.capacity();

		for (INT16 i = 0; _capacity > i; ++i)
		{
			std::shared_ptr<CSession> pSession = m_active_sessions.get(i);

			if (pSession)
			{
				pSession->_close_handler();
				pSession->_close();
			}
		}
	}

	INT16 CSessionManager::Insert(std::shared_ptr<CSession> _session)
	{
		boost::unique_lock<boost::shared_mutex>(m_lock);

		INT16 _ssnid = (USER_SESSION == _session->Type())
			? m_inactive_ssnids.pop() : INVALID_SSNID;//m_inactive_server_ssnids.pop();

		if (INVALID_SSNID != _ssnid)
		{
			_session->Activate(_ssnid);
			m_active_sessions.insert(_ssnid, _session);
		}
		else
		{
			printf("Can't add more sessions (max sessions exceeded)");
		}

		return _ssnid;
	}

	bool CSessionManager::Remove(INT8 uType, INT16 _ssnid)
	{
		boost::unique_lock<boost::shared_mutex>(m_lock);

		if (m_active_sessions.get(_ssnid))
		{
			m_active_sessions.remove(_ssnid);

			//	TODO: 최대하
			//	타입으로 push를 결정
			if (uType == USER_SESSION)
				m_inactive_ssnids.push(_ssnid);
			else
				//m_inactive_server_ssnids.push(_ssnid);

			return true;
		}

		return false;
	}

	std::shared_ptr<CSession> CSessionManager::Find(INT16 _ssnid)
	{
		boost::unique_lock<boost::shared_mutex>(m_lock);

		return m_active_sessions.get(_ssnid);
	}

	std::shared_ptr<CSession> CSessionManager::make_ptr(INT8 type, INT16 _queue_index, bool  bEncrypt)
	{
		return std::shared_ptr<CSession>(new CSession(type, _queue_index, bEncrypt), boost::bind(&CSessionManager::destroy, this, _1));
	}

	void CSessionManager::destroy(CSession* i_obj)
	{
		if (nullptr == i_obj)
			return;

		delete i_obj;
		i_obj = nullptr;
	}
}