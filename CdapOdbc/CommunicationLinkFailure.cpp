#include "stdafx.h"
#include "CommunicationLinkFailure.h"

Cask::CdapOdbc::CommunicationLinkFailure::CommunicationLinkFailure(const char* what)
	: std::logic_error(what) {
}

