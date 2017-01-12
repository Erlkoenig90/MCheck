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

#include <fstream>
#include <cstdlib>
#include <locale>
#include <codecvt>
#include <utility>

#include <boost/algorithm/string/replace.hpp>

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __unix__
#include <stdio.h>
#include <errno.h>
#endif

#include "ltl_output.hh"

LTL::Output::Output (const sysstring& outputPath) : m_serial (0), m_outputPath (outputPath) {
	do {
		m_tmpPath = boost::filesystem::temp_directory_path () / boost::filesystem::unique_path ();
	} while (!boost::filesystem::create_directory (m_tmpPath));

	m_lxPath = m_tmpPath / SYSLIT("mcheck.tex");
	m_osLatex.open (m_lxPath.native (), std::ios::binary);
	if (!m_osLatex)
		throw std::runtime_error ("Couldn't open LaTeX file:" + to_int (m_lxPath.native ()));

	m_osLatex <<	"\\title{LTL Output} \\date{\\today} \\documentclass[a4paper,12pt]{minimal} \\usepackage[a4paper, top=15mm, bottom=30mm, left=15mm, right=15mm]{geometry}"
				"\\usepackage{graphicx}\\usepackage[space]{grffile}\\usepackage[T1]{fontenc} \\usepackage[utf8]{inputenc} \\usepackage{pdfpages} \\usepackage{amsmath} \\begin{document}\n";
}

LTL::Output::~Output () {
	m_osLatex.close ();
	boost::filesystem::remove_all (m_tmpPath);
}

void LTL::Output::output (Algorithm& result) {
	boost::filesystem::path gvPath = m_tmpPath / (SYSLIT("mcheck_") + to_sys (m_serial) + SYSLIT(".gv"));
	std::ofstream osGV (gvPath.native (), std::ios::binary);
	if (!osGV) throw std::runtime_error ("Couldn't open graphviz file: " + to_int (gvPath.native ()));


	osGV << "digraph G {\n\tmargin=0;\n\tgraph [rankdir=LR]\n";
	for (auto& a : result.atoms) {
		osGV << "\t{rank=same;";
		for (size_t i : a.second) {
			osGV << ' ' << a.first->name << '_' << i;
		}
		osGV << "; }\n";
	}
	for (const TableauEdge& edge : result.edges) {
		osGV << '\t' << edge.startState->name << '_' << edge.startAtom << " -> " << edge.endState->name << '_' << edge.endAtom << '\n';
	}
	osGV << "}\n";
	osGV.close ();

	boost::filesystem::path tblPath = m_tmpPath / (SYSLIT ("tableau_") + to_sys (m_serial) + SYSLIT (".pdf"));
	if (auto_system ((SYSLIT ("dot -Tpdf -o ") + escape (tblPath) + SYSLIT(" ") + escape (gvPath)).c_str ()) != 0)
		throw std::runtime_error ("Error running GraphViz!");

	std::string tblPathLatex = tblPath.generic_string ();
	boost::replace_all (tblPathLatex, "}", "\\}");

	m_osLatex << "\\includegraphics{" << tblPathLatex << "}\n\\begin{flalign*}\n";
	m_osLatex << "& \\mathrm{Formula:} " << Formula::latexprint << result.formula << " & \\\\\n";
	m_osLatex << "& \\mathrm{Closure\\left[" << result.closure.size () << "\\right]:} " << Formula::latexprint << result.closure << " & \\\\\n";

	for (auto& a : result.atoms) {
		for (size_t i : a.second) {
			m_osLatex << "& \\mathrm{" << a.first->name << "\\_" << i << ":}" << Formula::latexprint << result.sharedAtoms[i] << " & \\\\\n";
		}
	}
	m_osLatex << "\\end{flalign*}\n";
	++m_serial;
}

void LTL::Output::finish () {
	m_osLatex << "\\end{document}\n\n";
	m_osLatex.close ();

	if (auto_system ((SYSLIT ("pdflatex -output-directory ") + escape (m_tmpPath) + SYSLIT(" ") + escape (m_lxPath)).c_str ()) != 0)
		throw std::runtime_error ("Error running pdflatex!");

	boost::filesystem::path pdfPath = m_tmpPath / + SYSLIT ("mcheck.pdf");

#ifdef _WIN32
	if (!MoveFileExW (pdfPath.c_str (), m_outputPath.c_str (), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) {
		throw std::runtime_error ("Couldn't move PDF: " + to_int (escape (pdfPath.native ())) + " to " + to_int (escape (m_outputPath.native ())));
	}
#endif
#ifdef __unix__
	if (rename (pdfPath.c_str (), m_outputPath.c_str ()) != 0) {
		if (errno == EXDEV) {
			boost::filesystem::copy_file (pdfPath, m_outputPath, boost::filesystem::copy_option::overwrite_if_exists);
			boost::filesystem::remove (pdfPath);
		} else {
			throw std::runtime_error("Couldn't move PDF: " + to_int(escape(pdfPath.native())) + " to " + to_int(escape(m_outputPath.native())));
		}
	}
#endif

}
