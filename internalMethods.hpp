#include<iostream>
#include "internalDBMethods.hpp"

using namespace std;

graphStructuredDataResult graphStructuredData(Graph g)
{
	graphStructuredDataResult r;
	vector<int> uniqueNode;
	unordered_map<int,int> indegree;
	unordered_map<int,string> nodeTime;

	for(int i = 0;i < g.graph.size();i++)
	{
		Task t;
		t = g.graph[i].from;
		if(indegree.find(t.data) == indegree.end())
		{
			indegree[t.data] = 0;
			nodeTime[t.data] = t.taskTime;
			uniqueNode.push_back(t.data);
		}
		t = g.graph[i].to;
		if(indegree.find(t.data) == indegree.end())
		{
			indegree[t.data] = 0;
			nodeTime[t.data] = t.taskTime;
			uniqueNode.push_back(t.data);
		}
	}

	for(int i = 0; i<g.graph.size(); i++)
	{
		Edge e;
		e = g.graph[i];
		indegree[e.to.data] ++;
	}

	r.uniqueNode = uniqueNode;
	r.indegree = indegree;
	r.nodeTime = nodeTime;

	return r;
}

/*

checkCycleResult checkCycle();

*/

