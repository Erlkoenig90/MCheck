/*
 * Copyright (c) 2016, Niklas Gürtler
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

 
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <streambuf>
#include <stdexcept>
#include <sstream>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include "ts/grammar.hh"
#include "ts/sys.hh"
#include "ctl/ctl_grammar.hh"
#include "ctl/ctl.hh"
#include "ltl/ltl_grammar.hh"
#include "ltl/ltl.hh"
#include "ltl/ltl_output.hh"
#include "sysstring.hh"

void usage (const std::vector<sysstring>& args) {
	syserr () << SYSLIT("Usage:\n") << args [0] << SYSLIT(" CTL TS-File Formula-File\n") << args [0] << SYSLIT(" LTL TS-File Formula-File PDFOutput\n");
}

#ifdef _WIN32
int wmain (int argc, wchar_t* argv []) {
	_setmode (_fileno (stdout), _O_U16TEXT);
#else
int main (int argc, char* argv []) {
#endif
	std::vector<sysstring> args (argv, argv + argc);

	try {
		if (args.size () < 2) {
			usage (args);
			return 1;
		} else {
			const bool ctl = args[1] == SYSLIT("CTL");
			if ((ctl && args.size () < 4)
			||	(!ctl && args.size () < 5)) {
				usage (args);
				return 1;
			}

			const sysstring& filenameTS = args [2];
			const sysstring& filenameFormula = args [3];

			std::ifstream fileTS (filenameTS, std::ios::binary);
			if (!fileTS)
				throw std::runtime_error ("Failed to open file: " + to_int (escape (filenameTS)));

			std::string strTS ((std::istreambuf_iterator<char> (fileTS)), std::istreambuf_iterator<char> ());
			fileTS.close ();

			TS::E_Graph graph;
			if (!TS::parse (strTS, graph))
				throw std::runtime_error ("Failed to parse transition system from file " + to_int (escape (filenameTS)));

			TS::TranSys ts (graph);

			std::ifstream fileFormula (filenameFormula, std::ios::binary);
			if (!fileFormula)
				throw std::runtime_error ("Failed to open file: " + to_int(escape(filenameFormula)));

			if (ctl) {
				for (std::string line; std::getline (fileFormula, line); ) {
					Formula::Expression formula;
					if (!BaseGrammar::isEmpty(line)) {
						sysout () << "Parsing line" << std::endl;

						if (!CTL::parse (line, formula))
							throw std::runtime_error ("Failed to parse CTL formula \"" + line + "\" from file " + to_int (escape (filenameFormula)) + "");

						sysout () << "Computing SAT" << std::endl;

						auto sat = CTL::computeSat (formula, ts);
						sysout () << "Printing formula" << std::endl;
						sysout () << to_sys (line) << std::endl;
						sysout () << "Printing result" << std::endl;
						if (sat.first)
							sysout () << SYSLIT ("Is satisfied") << std::endl;
						else
							sysout () << SYSLIT ("Is not satisfied") << std::endl;
#ifdef _WIN32
						std::stringstream ss;
						sat.second->print (ss, 0);
						sysout () << "Printing SAT sets" << std::endl;
						sysout () << to_sys (ss.str ());
#else
						sat.second->print (sysout (), 0);
#endif
					}
				}
			} else {
				LTL::Output op (args [4]);
				for (std::string line; std::getline (fileFormula, line); ) {
					Formula::Expression formula;
					if (!BaseGrammar::isEmpty(line)) {
						if (!LTL::parse (line, formula))
							throw std::runtime_error ("Failed to parse LTL formula \"" + line + "\" from file \"" + to_int (filenameFormula) + "\"");

						LTL::Algorithm a (formula, ts);
						op.output (a);
					}
				}
				op.finish ();
			}
		}
	} catch (std::exception& e) {
		syserr () << SYSLIT("Error: ") << to_sys (e.what ()) << std::endl;
	}
}
