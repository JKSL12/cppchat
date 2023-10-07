#pragma once

#include "Net/Acceptor.h"

class CUserAcceptor : public NET::CAcceptor
{
public:
	CUserAcceptor(INT16 _queue_index);
	virtual ~CUserAcceptor();
};

