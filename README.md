# MCheck - Simple implementation of a model checking algorithm using CTL formulae
This is a simple C++ based application that parses CTL formulae, descriptions of transition systems, and checks whether the systems satisfy the formulae. Both the formulae and the transision systems are parsed using the [Boost.Spirit](http://boost-spirit.com) Parser library. It is written in Standard C++ and runs on both Windows and Linux.
## Syntax
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

CTL Formulae look like this:
```
∀ ⬜ (y → (∀ X g))
∃ ⬜ ∃(r U (∀X y))
```
Unicode UTF-8 characters are used for the operators. ASCII symbols are available as alternatives. Here, r, g and y are atomic propositions that correspond to the ones defined in the transition system. The following syntax elements are defined (Φ,Ψ are placeholders for sub-formulae):

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

Formulae are input via simple text files, one formula per line. Empty lines are ignored, as are comments starting with #. If the unicode operators are used, the files must be encoded as UTF-8, without a byte order mark. Therefore, Windows notepad can not be used. Whitespace is generally ignored except within atomic propositions.

## Usage
MCheck is a console application, run it as:
```shell
MCheck TransitionSystem.txt Formulae.txt
```
Where TransitionSystem.txt contains a description of a transition system, and Formulae.txt one or more CTL formulae. Since the Windows Console does not support UTF-8 output, output can be redirected to a file which can then be displayed by a text editor to view the unicode characters:
```shell
MCheck TransitionSystem.txt Formulae.txt > Result.txt
```

An example output is
```
∀ ⬜ (y → (∀ X g))
Is satisfied
Sat (∀ ⬜ (y → (∀ X g))) = {S0, S1, S2}
	Sat (y → (∀ X g)) = {S0, S1, S2}
		Sat (y) = {S1}
		Sat (∀ X g) = {S1}
			Sat (g) = {S2}
```
Each computed formulae are printed, followed by the computation result, and its syntax tree. Each sub-formula is annotated with the set of states satisfying it.

## Download
The [Releases](https://github.com/Erlkoenig90/MCheck/releases) page provides binaries for Windows and Linux. You can also download the source and compile it using eclipse and GCC or MSVC.

## License
This is an open source project licensed under the terms of the BSD license. See the [LICENSE file](LICENSE) for details.
