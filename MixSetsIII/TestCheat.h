#pragma once
#include "string.h"

bool TestCheat(const char* cheat)
{
	char *c = (char *)0x00885B90;
	char buf[30];
	strcpy(buf, cheat);
	char *s = _strrev(buf);
	if (_strnicmp(s, c, strlen(s))) return false;
	c[0] = 0;
	return true;
}
