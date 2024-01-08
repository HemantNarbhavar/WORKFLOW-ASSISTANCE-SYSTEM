#include <iostream>
#include <pqxx/pqxx>
#include "newInterface.hpp"

using namespace pqxx;

//----------------------------Internal Database Methods ----------------------------------------

GraphResult storeGraphInfo(Graph g, graphStructuredDataResult graphStrcData);

GraphResult storeGraph(pqxx::connection &C, string lifeSpan);

Error storeNodes(pqxx::connection &C, unsigned int graphID, graphStructuredDataResult graphStrcData);

Error storeEdges(pqxx::connection &C, unsigned int graphID, vector<Edge> edges);

ExecResult storeExecutionInfo(unsigned int graphID);

Error storeTask(pqxx::connection &C, unsigned int execID);

TaskResult fetchTask(unsigned int execID);

Error updateDependency(pqxx::connection &C, unsigned int nodeID, unsigned int execID);

GraphResult storeGraphInfo(Graph g, graphStructuredDataResult graphStrcData)
{
	GraphResult gr;
        try {
		connection C("dbname = graphsystem user = hemant password = 3025  hostaddr = 127.0.0.1 port = 5432");

		if (C.is_open()) {
                        cout << "Opened database successfully: " << C.dbname() << endl;
		}

                else {
                        cout << "Can't open database" << endl;
			gr.graphID = -1;
			gr.err.st = INTERNAL_ERROR;
		        gr.err.errId = DB_FAILED;	
                        return gr;
                }

		gr = storeGraph(C, g.lifeSpan);
		// store lifespan of graph in Graph relation and gives graphID

		if(gr.err.st == SUCCESS) {
			gr.err = storeNodes(C, gr.graphID, graphStrcData);
		       // store unique nodes in Node relation and return Error status
		}      		
		else {
			cout<< "Error" << endl;
			return gr;
		}

		if(gr.err.st == SUCCESS) {
			gr.err = storeEdges(C, gr.graphID, g.graph);
			// store edges in Edge relation and return Error stauts

			if(gr.err.st != SUCCESS) {
				cout << "Error" << endl;
				return gr;
			}
		}
		else {
			cout<< "Error" << endl;
			return gr;
		}					
		    
        C.disconnect();

        }

        catch (const std::exception &e) {
                cerr << e.what() << std::endl;
		gr.graphID = -1;
		gr.err.st = INTERNAL_ERROR;
		gr.err.errId = DB_FAILED;	
                return gr;
        }

	return gr;
}



GraphResult storeGraph(pqxx::connection &C, string lifeSpan)
{
	GraphResult gr;

	work w(C);

	string insert = "INSERT INTO graph(lifespan) VALUES('" + w.esc(lifeSpan) + "')";
	w.exec(insert);

	w.commit();
	cout<< "Record created" << endl;

	nontransaction n(C);

	string select = "SELECT LASTVAL()";

	result r(n.exec(select));

	result::const_iterator g_id = r.begin();

	gr.graphID = g_id[0].as<int>();
	gr.err.st = SUCCESS;

	return gr;
}

Error storeNodes(pqxx::connection &C, unsigned int graphID, graphStructuredDataResult graphStrcData)
{
	Error e;
	work w(C);

	int n = graphStrcData.uniqueNode.size();
	int i = 0;
	int uN;
	int inde;
	string time;
	while(i<n)
	{
		uN = graphStrcData.uniqueNode[i];
		time = graphStrcData.nodeTime.at(uN);
		inde = graphStrcData.indegree.at(uN);

		w.exec("INSERT INTO node(graph_id, node, execution_time, indegree) VALUES(" + to_string(graphID) + ", " + to_string(uN) + ", " + " ' " +  time +  " ' " + ",  '" + to_string(graphStrcData.indegree.at(uN)) + "')");					
		i++;
	}

	w.commit();	
	e.st = SUCCESS;

	return e;
}

Error storeEdges(pqxx::connection &C, unsigned int graphID, vector<Edge> ed)
{
	Error e;

	int i = 0;
	int len = ed.size();
	string select;
	int eidfrom;
	int eidto;
	

	while(i<len)
	{

		nontransaction n(C);

		select = "SELECT node_id FROM node WHERE node IN(" + to_string(ed[i].from.data) + ", " + to_string(ed[i].to.data) + ") AND graph_id IN(" + to_string(graphID) + ")";

		result r(n.exec(select));
		result::const_iterator e_id = r.begin();
		eidfrom = e_id[0].as<int>();
		e_id++;
		eidto = e_id[0].as<int>();
		
		n.commit();

		work w(C);

		w.exec("INSERT INTO edge(from_node, to_node, graph_id) VALUES(" + to_string(eidfrom) + ", " + to_string(eidto) + ", '" + to_string(graphID) + "')");

		w.commit();

		i++;
	}

	e.st = SUCCESS;
	return e;
}


ExecResult storeExecutionInfo(unsigned int graphID)
{
	ExecResult er;
	try {
		connection C("dbname = graphsystem user = hemant password = 3025  hostaddr = 127.0.0.1 port = 5432");

		if (C.is_open()) {
                        cout << "Opened database successfully: " << C.dbname() << endl;
		}

                else {
                        cout << "Can't open database" << endl;
			er.execID = -1;
			er.err.st = INTERNAL_ERROR;
		        er.err.errId = DB_FAILED;	
                        return er;
                }


		work w(C);

		w.exec("INSERT INTO execution(graph_id, start_time) VALUES(" + to_string(graphID) + ", now())" ); 
		w.commit();

		nontransaction n(C);

		result r(n.exec("SELECT LASTVAL()"));
		result::const_iterator exe_id = r.begin();

		er.execID = exe_id[0].as<int>();
		n.commit();

		er.err.st = SUCCESS;

		if(er.err.st == SUCCESS)
		{
			er.err = storeTask(C, er.execID);
			if(er.err.st != SUCCESS)
				return er;
		}
		else
		{
			cout << "Error " << endl;
			return er;
		}

		C.disconnect();

        }

        catch (const std::exception &e) {
                cerr << e.what() << std::endl;
		er.execID = -1;
		er.err.st = INTERNAL_ERROR;
		er.err.errId = DB_FAILED;	
                return er;
        }

	return er;
}

	
Error storeTask(pqxx::connection &C, unsigned int execID)
{
	Error e;

	string select;
	string insert;
	int size;

	vector<int> task;
	vector<int> dep;

	nontransaction n(C);

	select = "select node_id, indegree from node where graph_id IN (select graph_id from execution where exec_id IN(" + to_string(execID) + "))";
	
	result r(n.exec(select));
	size = r.size();

	for(result::const_iterator c = r.begin(); c != r.end(); ++c)
	{
		task.push_back(c[0].as<int>());
		dep.push_back(c[1].as<int>());
	}
	n.commit();

	int i = 0;

	work w(C);
	while(i<size)
	{

		insert = "INSERT INTO task(node_id, exec_id, current_dependency) VALUES(" + to_string(task[i]) + ", " + to_string(execID) + ", " + to_string(dep[i]) + ")";

		w.exec(insert);
		i++;
	}

	w.commit();
	e.st = SUCCESS;

	return e;
}


TaskResult fetchTask(unsigned int execID)
{
	TaskResult task;
	
        try {
                connection C("dbname = graphsystem user = hemant password = 3025  hostaddr = 127.0.0.1 port = 5432");

                if (C.is_open()) {
                        cout << "Opened database successfully: " << C.dbname() << endl;
                }

                else {
                        cout << "Can't open database" << endl;
                        task.taskID = -1;
                        task.err.st = INTERNAL_ERROR;
                        task.err.errId = DB_FAILED;
                        return task;
                }
		
		string select;

		nontransaction n(C);
		select = "SELECT task_id FROM task WHERE exec_id = " + to_string(execID) + " AND current_dependency = 0 AND status = 'ready' ";

		result r(n.exec(select));
		result::const_iterator task_id = r.begin();
	
		if(to_string(task_id[0]) == "")
		{
			task.taskID = 0;
			task.err.st = USER_ERROR;
			task.err.errId = DB_FAILED;
			return task;
		}	

		task.taskID = task_id[0].as<int>();

		n.commit();
		task.err.st = SUCCESS;

	}
	
	catch (const std::exception &e) {
                cerr << e.what() << std::endl;
                task.taskID = -1;
                task.err.st = INTERNAL_ERROR;
                task.err.errId = DB_FAILED;
                return task;
        }

	return task;

}

Error updateDependency(TaskResult task, unsigned int execID)
{
	Error e = task.err;

	string select;
	string update;

	if(e.st != SUCCESS)
		return e;
	
        try {
                connection C("dbname = graphsystem user = hemant password = 3025  hostaddr = 127.0.0.1 port = 5432");

                if (C.is_open()) {
                        cout << "Opened database successfully: " << C.dbname() << endl;
                }

                else {
                        cout << "Can't open database" << endl;
                        e.st = INTERNAL_ERROR;
                        e.errId = DB_FAILED;
                        return e;
                }

		if(e.st != SUCCESS)
			return e;
			
		work w1(C);

		string update = "UPDATE task SET status = 'complete' WHERE task_id = " + to_string(task.taskID) + "";
		w1.exec(update);
		w1.commit();
		
		nontransaction n(C);
		
		select = "SELECT node_id FROM task WHERE task_id = " + to_string(task.taskID) +"";

		result r1(n.exec(select));
		result::const_iterator node_id = r1.begin();

		unsigned int nodeID = node_id[0].as<int>();

		select = "SELECT to_node FROM edge WHERE from_node = " + to_string(nodeID) +"";

		result r2(n.exec(select));

        	int size = r2.size();

		vector<int> to_node;

        	for(result::const_iterator c = r2.begin(); c != r2.end(); ++c)
        	{
                	to_node.push_back(c[0].as<int>());
        	}
        	n.commit();

		work w2(C);

		int i = 0;

		while(i<size)
		{
			update = "UPDATE task SET current_dependency = current_dependency-1 WHERE node_id = " + to_string(to_node[i]) + " AND exec_id =  " + to_string(execID) + "";
			w2.exec(update);
			i++;
		}

		w2.commit();
		e.st = SUCCESS;
	}

	catch (const std::exception &er) {
                cerr << er.what() << std::endl;
                e.st = INTERNAL_ERROR;
                e.errId = DB_FAILED;
                return e;
       	}

	return e;
}



