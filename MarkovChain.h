#pragma once
#include <vector>
#include "RandomChooser.h"

class MarkovNode {
private:
	std::vector<MarkovNode*> links;
	RandomChooser chooser;
	float max = 0.0f;
public:
	const char state;
	MarkovNode(char state) : state(state){
		
	}
	void AddLink(MarkovNode* node, float change) {
		links.push_back(node);
		chooser.AddChance(change);
		max += change;
	}
	MarkovNode* NextNode() {
		return links[chooser.Choose()];
	}
};

class MarkovChain {
private:
	char state;
	MarkovNode* currentNode;
	std::vector<MarkovNode> nodes;
public:
	MarkovChain() {
		state = 0;
		currentNode = NULL;
		srand(time(NULL));
	}
	char GetState() {
		return state;
	}
	char AdvanceState() {
		currentNode = currentNode->NextNode();
		state = currentNode->state;
		return state;
	}
	MarkovNode* GetNode(char itsstate) {
		std::vector<MarkovNode>::iterator it;
		for (it = nodes.begin(); it != nodes.end(); ++it) {
			if ((*it).state == itsstate)
				return &(*it);
		}
		return NULL;
	}
	void InitChain() {
		currentNode = &nodes[0];
	}
	void InitChain(char itsstate) {
		currentNode = GetNode(itsstate);
	}
	void AddNode(char itsstate) {
		nodes.push_back(MarkovNode(itsstate));
	}
	void AddLink(char fromstate, char tostate, float change) {
		MarkovNode* fromnode = GetNode(fromstate);
		MarkovNode* tonode = GetNode(tostate);
		if (fromnode != NULL && fromnode != NULL) {
			fromnode->AddLink(tonode, change);
		}
	}
	void AddLinkToAll(char fromstate, float change) {
		MarkovNode* fromnode = GetNode(fromstate);
		std::vector<MarkovNode>::iterator it;
		for (it = nodes.begin(); it != nodes.end(); ++it) {
			fromnode->AddLink(&(*it), change);
		}
	}
};