#include<bits/stdc++.h>
#include<iostream>
#include<vector>

using namespace std;

enum status{
	SUCCESS = 0, 
	USER_ERROR = 1, 
	INTERNAL_ERROR = 2
};

enum ID{
	CYCLE_DETECTED = 10, 
	DB_FAILED = 20,
	PROTOCOL_FAILED = 30
};

typedef struct error{
	enum status st;
	enum ID errId;
}Error;

typedef struct task{
	int data;
	string taskTime;
}Task;

typedef struct edge{
	Task from;
	Task to;
}Edge;

typedef struct graph{
	vector<Edge> graph;
	unsigned short int numEdge;
	string lifeSpan;
	// Total lifespan of graph
}Graph;

typedef struct graphResult{
	Error err;
	unsigned int graphID;
	// If status is Successful then system return graphID
	// else return NULL and status -> error type (user of internal)
}GraphResult;

typedef struct execResult{
	Error err;
	unsigned int execID;
	// If status is Successful then system return execID
        // else return NULL and status -> error type (user of internal)
}ExecResult;

typedef struct taskResult{
	Error err;
	unsigned int taskID;
	int data;
	// If status is Successful then system return taskID
        // else return NULL and status -> error type (user of internal)
}TaskResult;

typedef struct graphStructuredDataResult{
        vector<int> uniqueNode;
	// Unique Node in Graph
        unordered_map<int,int> indegree;
	// Indgree of each Node
        unordered_map<int,string> nodeTime;
	// each Node time
}graphStructuredDataResult;



typedef struct checkCycleResult{
        int cycle;
}checkCycleResult;


//-------------------------METHODS---------------------------------------

GraphResult addGraph(Graph g);
// This Function take graph and check graph is Valid or Invalid.
// if Valid then add it's Info. else gives user(cyclicgraph) or internal error 
// This function generate all unique Node with nodeID, dependency, time 

ExecResult startExecution(unsigned int graphID);
// This function generate execID
// make replica of nodeID as taskID in task relation along with dependecy and default ready status

TaskResult getTask(unsigned int execID);
// This function take execID and give that task which has zero dependency and ready status

Error completeTask(unsigned int taskID);
// This function takeID and conform that this task is done
// after complete execution of that task we decrease dependency of dependent tasks

Error terminateExec(unsigned int execID);
// This function take execID and end the execution
// If once a execution terminated then delelte all records related to that execID

Error destroyGraph(unsigned int graphID);
// This function delete all records of that particular graph
