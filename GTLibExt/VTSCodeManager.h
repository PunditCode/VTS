#pragma once

#include "GTCodeManager.h"

class VTSCodeManager : public GTCodeManager
{
public:
	VTSCodeManager(void);
	~VTSCodeManager(void);

	BOOL Init(DKBuffer& dbuf);
private:
	BOOL	m_bInit;
};
