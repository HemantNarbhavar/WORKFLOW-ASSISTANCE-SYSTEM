#include<iostream>
#include"internalMethods.hpp"

using namespace std;

GraphResult addGraph(Graph g)
{
	GraphResult gr; 
	
	graphStructuredDataResult graphStrcData = graphStructuredData(g);	
	// make array of unique nodes 
	
	gr  = storeGraphInfo(g, graphStrcData);
	// Store All graph Inforamtion in DB	
	
	return gr;
}

ExecResult startExecution(unsigned int graphID)
{
	ExecResult exec;

	exec = storeExecutionInfo(graphID);
	// Store in Execution relation and gives execID
	// Store graph nodes and their info in Task relation and return Error status
	 
	return exec;
}

TaskResult getTask(unsigned int execID)
{
	TaskResult task;

	task = fetchTask(execID);
	// fetch taskID from Task relation with 0 dependency and ready status
	
	return task;	
}

Error completeTask(TaskResult task, unsigned int execID)
{
	Error er;
	
	if(task.err.st != SUCCESS)
	{
		task.err.st = USER_ERROR;
		task.err.errId = PROTOCOL_FAILED;
	}

	er = updateDependency(task, execID);
	//decrease dependcy of dependent task by 1 and complete status of completed taskID
	
	return er;	
}

int main(void)
{
///*
	Task t1,t2,t3;
        t1.data = 33430;
        t1.taskTime = "10:10:10";

        t2.data = 478374;
        t2.taskTime = "09:10:10";

        t3.data = 34340;
        t3.taskTime = "08:10:10";

        Edge e1,e2,e3;
        e1.from = t1;
        e1.to = t2;

        e2.from = t1;
        e2.to = t3;
	
	e3.from = t2;
	e3.to = t3;

        Graph g;
        g.graph = {e1, e2, e3};
        g.lifeSpan = "12-12-24 10:10:10";

	GraphResult gr = addGraph(g);
        cout<<gr.graphID<<" "<<gr.err.st<<endl;
/*
	ExecResult er = startExecution(1);
   	cout<<er.execID<<" "<<er.err.st<<endl;

	TaskResult tr = getTask(4);
	cout<<tr.taskID<< " "<<tr.err.st<<endl;

	TaskResult tr;
	tr.taskID = 5;
	tr.data = 33430;
	tr.err.st = SUCCESS;

	Error e = completeTask(tr,4);
	cout<<e.st<<" "<<e.errId<<endl;
*/
}




