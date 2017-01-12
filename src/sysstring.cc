/*
* Copyright (c) 2017, Niklas Gürtler
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
* following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*    disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
*    following disclaimer in the documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <locale>
#include <codecvt>

#include <boost/algorithm/string/replace.hpp>

#include "sysstring.hh"


int auto_system (const char* cmd) {
	return ::system (cmd);
}

#ifdef _WIN32

int auto_system (const wchar_t* cmd) {
	return ::_wsystem (cmd);
}

sysstring to_sys(const std::string& str) {
	return std::wstring_convert <std::codecvt_utf8 <wchar_t>>().from_bytes(str);
}

std::string to_int(const sysstring& str) {
	return std::wstring_convert <std::codecvt_utf8 <wchar_t>>().to_bytes(str);
}

#else

sysstring to_sys(const std::string& str) {
	return str;
}

std::string to_int(const std::string& str) {
	return str;
}


#endif

sysstring escape (const boost::filesystem::path& src) {
	sysstring str = src.native ();
	boost::replace_all (str, SYSLIT ("\\"), SYSLIT ("\\\\"));
	boost::replace_all (str, SYSLIT ("\""), SYSLIT ("\\\""));
	return SYSLIT ("\"") + str + SYSLIT ("\"");
}
