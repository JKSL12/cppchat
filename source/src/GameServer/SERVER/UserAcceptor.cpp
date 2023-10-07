#include "../pch.h"
#include "UserAcceptor.h"

CUserAcceptor::CUserAcceptor(INT16 _queue_index)
	: NET::CAcceptor(static_cast<INT8>(NET::USER_SESSION), _queue_index, true)
{
}

CUserAcceptor::~CUserAcceptor()
{
}