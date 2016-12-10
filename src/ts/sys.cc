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

#include "sys.hh"
#include "grammar.hh"

namespace TS {

TranSys::TranSys (const E_Graph& src) {
	for (const auto& stmt : src.statements) {
		if (const E_Transition* tran = boost::get<E_Transition> (&stmt)) {
			if (statesMap.find (tran->from) == statesMap.end ()) statesMap.emplace (std::make_pair (tran->from, State { tran->from }));
			if (statesMap.find (tran->to) == statesMap.end ()) statesMap.emplace (std::make_pair (tran->to, State { tran->to }));
		} else if (const E_AttrDecl* ad = boost::get<E_AttrDecl> (&stmt)) {
			if (statesMap.find (ad->node) == statesMap.end ()) statesMap.emplace (std::make_pair (ad->node, State { ad->node }));

			for (const auto& attr : ad->attributes) {
				if (const E_AttrLabel* la = boost::get<E_AttrLabel> (&attr)) {
					for (const auto& ap : la->atomicPropositions) {
						if (labels.find (ap) == labels.end ())
							labels.emplace (std::make_pair (ap, Label { ap }));
					}
				}
			}
		}
	}
	for (auto& s : statesMap) {
		statesSet.insert (&s.second);
	}
	for (const auto& stmt : src.statements) {
		if (const E_Transition* tran = boost::get<E_Transition> (&stmt)) {
			State* from = &statesMap.at (tran->from);
			State* to = &statesMap.at (tran->to);
			from->successors.insert (to);
			to->predecessors.insert (from);
		} else if (const E_AttrDecl* ad = boost::get<E_AttrDecl> (&stmt)) {
			State* state = &statesMap.at (ad->node);
			for (const auto& attr : ad->attributes) {
				if (const E_AttrShape* sa = boost::get<E_AttrShape> (&attr)) {
					if (sa->shape == "box")
						init.insert (state);
				} else if (const E_AttrLabel* la = boost::get<E_AttrLabel> (&attr)) {
					for (const auto& ap : la->atomicPropositions) {
						state->atomicPropositions.insert (&labels.at (ap));
					}
				}
			}
		}
	}
}


}
