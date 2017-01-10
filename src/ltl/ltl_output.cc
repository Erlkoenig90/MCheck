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
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>

#include "ltl_output.hh"


class AutoRemover {
	const std::string& tmpdir;
	AutoRemover (const std::string& d) : tmpdir (d) {}
	~AutoRemover () {
		unlink ((tmpdir + "/mcheck.gv").c_str ());
		unlink ((tmpdir + "/mcheck.tex").c_str ());
		rmdir (tmpdir.c_str ());
	}
};

LTL::Output::Output (std::string outputFilename) : m_serial (0), m_outputFilename (outputFilename) {
	char tmpdirC [] = "/tmp/mcheckpdfXXXXXX";
	if (!mkdtemp (tmpdirC)) {
		throw std::runtime_error ("Couldn't create temporary directory for output!");
	}
	m_tmpdir = tmpdirC;

	m_lxPath = m_tmpdir + "/mcheck.tex";
	m_osLatex.open (m_lxPath);
	if (!m_osLatex)
		throw std::runtime_error ("Couldn't open LaTeX file:" + m_lxPath);

	m_osLatex <<	"\\title{LTL Output} \\date{\\today} \\documentclass[a4paper,12pt]{minimal} \\usepackage[a4paper, margin=15mm]{geometry}"
				"\\usepackage[T1]{fontenc} \\usepackage[utf8]{inputenc} \\usepackage{pdfpages} \\usepackage{amsmath} \\begin{document}\n";
}

LTL::Output::~Output () {
	m_osLatex.close ();
	system (("rm -r \"" + m_tmpdir + "\"").c_str ());
}

void LTL::Output::output (Algorithm& result) {
	std::string gvPath (m_tmpdir + "/mcheck_" + std::to_string(m_serial) + ".gv");
	std::ofstream osGV (gvPath);
	if (!osGV) throw std::runtime_error ("Couldn't open graphviz file: " + gvPath);


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

	std::string tblPath = m_tmpdir + "/tableau_" + std::to_string (m_serial) + ".pdf";
	if (std::system (("dot -Tpdf -o \"" + tblPath + "\" \"" + gvPath + "\"").c_str ()) != 0)
		throw std::runtime_error ("Error running GraphViz!");

	m_osLatex << "\\includegraphics{" << tblPath << "}\n\\begin{flalign*}\n";
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

	if (std::system (("pdflatex -output-directory \"" + m_tmpdir + "\" \"" + m_tmpdir + "/mcheck.tex\"").c_str ()) != 0)
		throw std::runtime_error ("Error running pdflatex!");

	std::string pdfPath = m_tmpdir + "/mcheck.pdf";
	if (system (("mv \"" + pdfPath + "\" \"" + m_outputFilename + "\"").c_str ()) != 0) {
		throw std::runtime_error ("Error moving \"" + pdfPath + "\" to \"" + m_outputFilename + "\"");
	}
}
