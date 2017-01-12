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

#ifndef SYSSTRING_HH_
#define SYSSTRING_HH_

#include <string>
#include <boost/filesystem.hpp>
#include <iostream>


#if defined(_WIN32)
#define SYSLIT(lit) L##lit
using syschar = wchar_t;
using sysstring = std::wstring;
#else
#define SYSLIT(lit) lit
using syschar = char;
using sysstring = std::string;
#endif

int auto_system (const char* cmd);
int auto_system (const wchar_t* cmd);

sysstring to_sys (const std::string& str);
std::string to_int (const sysstring& str);
sysstring escape (const boost::filesystem::path& src);

#ifdef _WIN32

inline std::wostream& sysout () { return std::wcout; }
inline std::wostream& syserr () { return std::wcerr; }
template <typename T>
auto to_sys(T&& obj) -> decltype (std::to_wstring(std::forward<T>(obj))) {
	return std::to_wstring(std::forward<T>(obj));
}

#else

inline std::ostream& sysout () { return std::cout; }
inline std::ostream& syserr () { return std::cerr; }

template <typename T>
auto to_sys(T&& obj) -> decltype (std::to_string(std::forward<T>(obj))) {
	return std::to_string(std::forward<T>(obj));
}

#endif

#endif /* SYSSTRING_HH_ */
