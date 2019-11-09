#include <bits/stdc++.h>
using namespace std;

#define MESH_SIZE 6

//For getting the neighbors
int row[] = {1, 0, -1, 0};
int col[] = {0, 1, 0, -1};

vector <vector <int>> mesh2D(MESH_SIZE, vector <int>(MESH_SIZE, 0));
vector <vector <int>> finalMapping(MESH_SIZE, vector <int>(MESH_SIZE, 0));

unordered_map <int, pair <int, int>> taskToTileMapping;

// getTopologicalOrder() function to get a topological order of the directed acyclic graph
vector <int> getTopologicalOrder(vector <unordered_map <int, int>> &adjancencyList, int numberOfTasks)
{
	queue <int> q;
	int indegree[numberOfTasks + 1];
	memset(indegree, 0, sizeof(indegree));

	for(int curNode=1; curNode<=numberOfTasks; ++curNode)
	{
		for(auto child: adjancencyList[curNode])
			indegree[child.first]++;
	}

	//Pushing vertices with indegree 0.
	for(int curNode=1; curNode<=numberOfTasks; ++curNode)
	{
		if(indegree[curNode] == 0)
		{
			q.push(curNode);
		}
	}

	vector <int> topologicalOrder;
	while(!q.empty())
	{
		int curNode = q.front();
		topologicalOrder.push_back(curNode);
		q.pop();

		for(auto child: adjancencyList[curNode])
		{
			indegree[child.first]--;
			if(indegree[child.first] == 0)
				q.push(child.first);
		}
	}

	return topologicalOrder;
}

bool isValidTile(int x, int y)
{
	return (x >= 0 && x < MESH_SIZE && y >= 0 && y < MESH_SIZE);
}

int neighboringUnoccupiedTiles(pair <int, int> curTile)
{

	int degree = 0;
	for(int k=0; k<4; ++k)
	{
		int x = curTile.first + row[k], y = curTile.second + col[k];
		if(isValidTile(x, y) && mesh2D[x][y] == 0)
			degree++;
	}
	return degree;
}

pair <int, int> getTile(set <pair <int, int>> &boundaryPoints, vector <unordered_map <int, int>> &adjancencyList, int curVertex, vector <int> &topologicalOrder, int index)
{
	int unallocatedAdjacentTasks = adjancencyList[curVertex].size();

	pair <int, int> tile;

	//Closeness Criteria
	int closeness = INT_MAX;
	set <pair <int,int>> candidates  = boundaryPoints;


	for(auto point : boundaryPoints)
	{
		for(int i=0; i< index; ++i)
		{
			int parent = topologicalOrder[i];
			pair <int, int> curTile = taskToTileMapping[parent];
			if(adjancencyList[parent].count(curVertex))
			{
				int val = adjancencyList[parent][curVertex]*(abs(curTile.first - point.first) + abs(curTile.second - point.second));
				if(closeness > val)
				{
					closeness = val;
					candidates.clear();
					candidates.insert(point);
				}
				else if (val == closeness)
					candidates.insert(point);
			}
		}
	}
	int opt = INT_MAX;
	for(auto point: candidates)
	{
		int unallocatedAdjacentTiles = neighboringUnoccupiedTiles(point);
		if(opt > abs(unallocatedAdjacentTasks - unallocatedAdjacentTiles))
		{
			opt = abs(unallocatedAdjacentTasks - unallocatedAdjacentTiles);
			tile = point;
		}
	}

	return tile;
}

void printMesh()
{
	for(int i=0; i<MESH_SIZE; ++i)
	{
		for(int j=0; j<MESH_SIZE; ++j)
		{
			cout<<finalMapping[i][j]<<" ";
		}
		cout<<endl;
	}
    cout<<endl;
	return;
}

void getMapping(vector <unordered_map <int, int>> &adjancencyList, vector <int> &topologicalOrder, int x, int y)
{
	//resetting the 2D-mesh
	for(int i=0; i < MESH_SIZE; ++i)
		for(int j=0; j < MESH_SIZE; ++j)
			mesh2D[i][j] = 0;
	taskToTileMapping.clear();

	set <pair <int, int>> boundaryPoints;
	boundaryPoints.insert({x, y});

	for(int i=0;i<topologicalOrder.size();i++)
	{
		int curVertex = topologicalOrder[i];
		pair <int, int> tile = getTile(boundaryPoints, adjancencyList, curVertex,topologicalOrder,i);
		int x = tile.first, y = tile.second;
		mesh2D[x][y] = curVertex;
		taskToTileMapping[curVertex] = {x, y};
		boundaryPoints.erase(tile);

		//Updating boundary tiles
		for(int k=0; k<4; ++k)
		{
			int x1 = x + row[k], y1 = y + col[k];
			if(isValidTile(x1, y1) && mesh2D[x1][y1] == 0)
				boundaryPoints.insert({x1, y1});
		}
	}
}

double getStretchFactor(vector <unordered_map <int, int>> &adjancencyList, int numberOfEdges)
{
	double sumOfManhattanDistances = 0;
	for(int i=1; i < adjancencyList.size(); ++i)
	{
		int parent = i;
		pair <int, int> parentTile = taskToTileMapping[parent];
		for(auto child: adjancencyList[i])
		{
			pair <int, int> childTile = taskToTileMapping[child.first];
			sumOfManhattanDistances += abs(parentTile.first - childTile.first) + abs(parentTile.second - childTile.second);
		}
	}
	return sumOfManhattanDistances/ (double)numberOfEdges;
}

double optimalStretchFactor(vector <unordered_map <int, int>> &adjancencyList, vector <int> &topologicalOrder, int numberOfEdges)
{
	double stretchFactor = INT_MAX;
	//Checking Starting-vertex for all possibilities
	for(int i=0; i <= MESH_SIZE/2; ++i)
	{
		for(int j=0 ; j<= i; ++j)
		{
			getMapping(adjancencyList, topologicalOrder, i, j);
            double currentStretchFactor = getStretchFactor(adjancencyList, numberOfEdges);
            if(stretchFactor > currentStretchFactor)
            {
            	stretchFactor = currentStretchFactor;
				finalMapping = mesh2D;            	
            }
		}
	}
	return stretchFactor;
}


int main()
{
	int numberOfTasks, numberOfEdges;
	cin>>numberOfTasks>>numberOfEdges;

	//1-indexed
	vector <unordered_map <int, int>> adjancencyList(numberOfTasks+1);

	for(int i=0;i<numberOfEdges;++i)
	{
		int u, v, w;
		cin>>u>>v>>w;
		adjancencyList[u][v] = w;
	}

	vector <int> topologicalOrder = getTopologicalOrder(adjancencyList, numberOfTasks);
    cout<<"Topological Ordering: ";
	for(auto l: topologicalOrder)
		cout<<l<<" ";
	cout<<endl<<endl;
	double stretchFactor = optimalStretchFactor(adjancencyList, topologicalOrder, numberOfEdges);
    cout<<"Final Mapping:\n";
	printMesh();
    cout<<"Stretch Factor: ";
    cout<<stretchFactor<<endl;
}