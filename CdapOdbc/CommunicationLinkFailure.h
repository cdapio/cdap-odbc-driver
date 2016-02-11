#pragma once
namespace Cask {
	namespace CdapOdbc {
		class CommunicationLinkFailure :
			public std::logic_error
		{
		public:
			CommunicationLinkFailure(const char* what);
			~CommunicationLinkFailure() = default;
		};
	}
}

