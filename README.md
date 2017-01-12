# MCheck - Simple implementation of a model checking algorithm using CTL and LTL formulae
This is a simple C++ based implementation of the CTL and LTL model checking algorithms. It parses the formulae and descriptions of transition systems, and...
* for CTL, calculates the SAT sets of the subformulae and decides whether the system satisfies the formula.
* for LTL, calculates the closure, valid atoms and generates a tableau.

Both the formulae and the transision systems are parsed using the [Boost.Spirit](http://boost-spirit.com) Parser library. It is written in Standard C++ and runs on both Windows and Linux.

## Syntax for transition systems
The syntax for transistion system descriptions is a simple subset of the language used by the [Graphviz dot](http://www.graphviz.org/) graph plotting application, which allows to easily plot the system. An example transition system is:
```
digraph G {
	S0 -> S1
	S1 -> S2
	S2 -> S0
	
	S0 [label = "S0: r", shape="box"]
	S1 [label = "r,y"]
	S2 [label = "S2: g"]
}
```
The lines 2-4 define some states and transitions. The lines 6-8 define additional attributes for the states. The label is used by graphviz for displaying, and used by MCheck to define the atomic propositions. The part up to and including the colon is optional and ignored by MCheck. The remainder should be a comma-seperated list of propositions, each being a C-like identifier. Nodes whose shape is defined as "box" are assumed as initial states.
Transistion systems descriptions are saved in ordinary text files and passed to MCheck.

## Syntax for formulae
Formulae are input via simple text files, one formula per line. Unicode UTF-8 characters are used for the operators. ASCII symbols are available as alternatives. Empty lines are ignored, as are comments starting with #. If the unicode operators are used, the files must be encoded as UTF-8, without a byte order mark. Therefore, Windows notepad can not be used. Whitespace is generally ignored except within atomic propositions. In the following sections, r, g and y are atomic propositions that correspond to the ones defined in the transition system, and Φ,Ψ are placeholders for sub-formulae.

### CTL Syntax
CTL Formulae look like this:
```
∀ ⬜ (y → (∀ X g))
∃ ⬜ ∃(r U (∀X y))
```
The following syntax elements are defined for CTL formulae:

Form | ASCII Alternative | Description
-----|-------------------|------------
⊤ / ⊥ | true/false | Boolean literals
¬Φ | ! | Negation
Φ∧Ψ | & | Logical And
Φ∨Ψ | \| | Logical Or
Φ→Ψ | -> | Logical implication
∃XΦ | E X | Exists next
∃(Φ U Ψ) | E(Φ U Ψ) | Exists until
∃⬜Φ | E W | Exists always
∀XΦ | A X Φ | For all next
∀(Φ U Ψ) | A(Φ U Ψ) | For all until
∀⬜Φ | A W Φ | For all always

### LTL Syntax
LTL Formulae look like this:
```
⊤ U (y ∧ (X g))
!(⊤ U (r ∧ ¬(X g)))
```
The following syntax elements are defined for LTL formulae:

Form | ASCII Alternative | Description
-----|-------------------|------------
⊤ / ⊥ | true/false | Boolean literals
¬Φ | ! | Negation
Φ∧Ψ | & | Logical And
Φ∨Ψ | \| | Logical Or
Φ→Ψ | -> | Logical implication
XΦ | X | Next
(Φ U Ψ) |  | Until

## Usage
MCheck is a console application, run it as:
```shell
MCheck CTL TransitionSystem.txt Formulae.txt
```
or
```shell
MCheck LTL TransitionSystem.txt Formulae.txt Output.pdf
```
Where TransitionSystem.txt contains a description of a transition system and Formulae.txt one or more CTL/LTL formulae. The output of the LTL algorithm is written to Output.pdf, and the output of the CTL algorithm is printed to the standard output. Since the Windows Console does not support all of the Unicode operators, output can be redirected to a file which can then be displayed by a text editor to view the unicode characters:
```shell
MCheck CTL TransitionSystem.txt Formulae.txt > Result.txt
```

An example output for CTL is
```
∀ ⬜ (y → (∀ X g))
Is satisfied
Sat (∀ ⬜ (y → (∀ X g))) = {S0, S1, S2}
	Sat (y → (∀ X g)) = {S0, S1, S2}
		Sat (y) = {S1}
		Sat (∀ X g) = {S1}
			Sat (g) = {S2}
```
All computed formulae are printed, followed by the computation result, and its syntax tree. Each sub-formula is annotated with the set of states satisfying it.

The LTL algorithm prints the closure, and consistent atoms, and the resulting tableau into the PDF file.

## Download
The [Releases](https://github.com/Erlkoenig90/MCheck/releases) page provides binaries for Windows and Linux. If you want to compile MCheck yourself, you can download the source code from GitHub.

## Installation
For the CTL algorithm, no steps other than obtaining the binary are neccessary. For LTL, [Graphviz](http://www.graphviz.org/) (specifically, its "dot" application) and pdflatex (for example via the [TeX Live](http://www.tug.org/texlive/) distribution) have to be installed and reachable via the "Path" environment variable.

## Building
This project uses CMake and is known to work at least with MSVC and GCC on Linux. To compile it on Linux, run:
```shell
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release .
make -j 4
```
To compile on Windows, run
```shell
cmake -G "Visual Studio 14 2015 Win64" .
```
and open the generated project in MSVC and compile it.

## License
This is an open source project licensed under the terms of the BSD license. See the [LICENSE file](LICENSE) for details.
