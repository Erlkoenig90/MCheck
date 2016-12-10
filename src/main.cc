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

#include "ts/grammar.hh"
#include "ts/sys.hh"
#include "ctl/grammar.hh"
#include "ctl/ctl.hh"

int main (int argc, char* argv []) {
	try {
		if (argc != 3) {
			throw std::runtime_error ("Usage: " + std::string(argv [0]) + " TS-File Formula-File");
		} else {
			std::string filenameTS = argv [1];
			std::string filenameFormula = argv [2];

			std::ifstream fileTS (filenameTS);
			if (!fileTS)
				throw std::runtime_error ("Failed to open file: \"" + filenameTS + "\"");

			std::string strTS ((std::istreambuf_iterator<char> (fileTS)), std::istreambuf_iterator<char> ());
			fileTS.close ();

			TS::E_Graph graph;
			if (!TS::parse (strTS, graph))
				throw std::runtime_error ("Failed to parse transition system from file \"" + filenameTS + "\"");

			TS::TranSys ts (graph);

			std::ifstream fileFormula (filenameFormula);
			if (!fileFormula)
				throw std::runtime_error ("Failed to open file: \"" + filenameFormula + "\"");

			for (std::string line; std::getline (fileFormula, line); ) {
				CTL::Expression formula;
				if (!CTL::parse (line, formula))
					throw std::runtime_error ("Failed to parse formula \"" + line + "\" from file \"" + filenameFormula + "\"");

				auto sat = CTL::computeSat (formula, ts);
				std::cout << line << std::endl;
				if (sat.first)
					std::cout << "Is satisfied" << std::endl;
				else
					std::cout << "Is not satisfied" << std::endl;

				sat.second->print (std::cout, 0);
			}
		}
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what () << std::endl;
	}
}
