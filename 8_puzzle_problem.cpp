// Program: 8 Puzzle Solver; Author: Param chokshi, Kejvi Cupa, Lahari Poluparti
// The Program finds the cheapest solution for 8-Puzzle using Dijkstra Algorithm, BFS, and DFS
// The solution assumes that instance of puzzle is solvable
#include <iostream>
#include <queue>
#include <vector>
#include <fstream>
#include <algorithm>
#include <list>
#include <cstring>
using namespace std;
#define N 3
#define INT_MAX 2147483647

// state space tree nodes
struct Node
{
	// stores the parent node of the current node
	// helps in tracing path when the answer is found
	Node* parent;
	// explored is used in Dijkstra Algorithm to mark a vertex explored, so no further relaxation to perform.
	bool explored = false;
	// visited is used in BFS
	bool visited = false;
	// stores matrix
	int mat[N][N];
	// stores blank tile coordinates
	int x, y;
	// stores the number of misplaced tiles
	int cost;
	// stores the number on the moved tile 
	int level;
	// stores the distance in Dijkstra and BFS, and depth in DFS
	int dist;
	// overloaded operator to compare two nodes
	bool operator==(Node rhs) {
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
				if (this->mat[i][j] != rhs.mat[i][j])
					return false;
		}
		return true;
	}
};

// Function to print N x N matrix
void printMatrix(int mat[N][N])
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			cout << mat[i][j];
		cout << endl;
	}
}

// Function to allocate a new node
Node* newNode(int mat[N][N], int x, int y, int newX,
	int newY, int level, int dist, Node* parent)
{
	Node* node = new Node;
	// set pointer for path to root
	node->parent = parent;
	// copy data from parent node to current node
	memcpy(node->mat, mat, sizeof node->mat);
	// move tile by 1 position
	swap(node->mat[x][y], node->mat[newX][newY]);
	// set number of misplaced tiles
	node->cost = INT_MAX-1;
	// set number on the tile to be moved
	node->level = level;
	// set the distance from the intial point to this vertex
	node->dist = dist;
	// update new blank tile cordinates
	node->x = newX;
	node->y = newY;

	return node;
}

// bottom, left, top, right
int row[] = { 1, 0, -1, 0 };
int col[] = { 0, -1, 0, 1 };

// Function to calculate the number of misplaced tiles
// ie. number of non-blank tiles not in their goal position
int calculateCost(int initial[N][N], int final[N][N])
{
	int count = 0;
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (initial[i][j] && initial[i][j] != final[i][j])
				count++;
	return count;
}

// Function to check if (x, y) is a valid matrix cordinate
int isSafe(int x, int y)
{
	return (x >= 0 && x < N&& y >= 0 && y < N);
}

// print path from root node to destination node
void printPath(Node* root)
{
	if (root == NULL)
		return;
	printPath(root->parent);
	printMatrix(root->mat);

	cout << endl;
}

//Binary predicate to compare two node pointers
bool comp( Node* lhs,  Node* rhs) 
{
	return (*lhs == *rhs);
}

//Binary predicate used to help min_element algorithm
bool nodecomp(const Node* lhs, const Node* rhs)
{
	return (lhs->dist) < (rhs->dist);
}

// Function to solve 8 puzzle algorithm using
// Dijkstra's Algorithm . x and y are blank tile coordinates
// in initial state
void solve_Dijkstra(int initial[N][N], int x, int y,
	int final[N][N])
{
	// Stores created vertices of the graph
	vector<Node*> pqv;

	// Create root node with initial state
	Node* root1 = newNode(initial, x, y, x, y, 0,0, NULL);
	root1->cost = calculateCost(initial, final);

	pqv.push_back(root1);

	while (!pqv.empty())
	{
		//Extract vertex with min distance
		Node* min = *min_element(pqv.begin(), pqv.end(), nodecomp);

		//Set explred to true, so that we don't further relax this vertex
		min->explored = true;
		 
		if (min->cost == 0)
		{
			// print the path from root to destination
			printPath(min);
			cout << "Cost: " << min->dist << endl;
			return;
		}
		
		// Visit each child, or vist each next possible steps from the current state of the puzzle
		for (int i = 0; i < 4; i++)
		{
			if (isSafe(min->x + row[i], min->y + col[i])) //checks if a move is possible
			{
				Node* child = newNode(min->mat, min->x,
					min->y, min->x + row[i],
					min->y + col[i],
					min->mat[min->x + row[i]][min->y + col[i]], 0, min);
				child->cost = calculateCost(child->mat, final);
				child->dist = min->dist + child->level;

				//Relaxation of the distance
				unsigned int j = 0;
				while (j < pqv.size())
				{
					if (*child == *pqv[j] && pqv[j]->explored != true)
						break;
					j++;
				}
			
				if (j != pqv.size()) // If the vertex is already in the vector
				{
					if (child->dist < pqv[j]->dist)// If the new distance is less than the previous one
					{
						pqv[j]->dist = child->dist;
					}
				}
				else // if not in the vector, add the child to the vector
				{
					pqv.push_back(child);
				}

			}
		}
		min->dist = INT_MAX; // This helps to make sure that the min_elemnt doesn't visit an already explored vertex again.
	}
}

// Function to solve 8 puzzle algorithm using
// BFS . x and y are blank tile coordinates
// in initial state
void solve_BFS(int initial[N][N], int x, int y,
	int final[N][N])
{
	// Stores created vertices of the graph
	vector<Node*> pqv;

	// Create root node with initial state
	Node* root1 = newNode(initial, x, y, x, y, 0, 0, NULL);
	root1->visited = true;

	pqv.push_back(root1);

	while (!pqv.empty())
	{
		//Here the vector is implemented to act as a queue.
		Node* min = pqv.front();
		for (auto p : pqv) { //Extract the first unexplored vertex from the front
			if (p->explored == false)
			{
				min = p;
				break;
			}
				
		}
		//Explored is marked true to virtually delete the vertex form the Queue.
		//Its not actually removed as it is used in printing out the solution
		min->explored = true;

		if (min->cost == 0)
		{
			// print the path from root to destination
			printPath(min);
			cout << "Cost: " << min->dist << endl;
			return;
		}

		// Visit each child, or vist each next possible steps from the current state of the puzzle
		for (int i = 0; i < 4; i++)
		{
			if (isSafe(min->x + row[i], min->y + col[i]))
			{				
				Node* child = newNode(min->mat, min->x,
					min->y, min->x + row[i],
					min->y + col[i],
					0, 0, min);
				child->cost = calculateCost(child->mat, final);
				child->dist = min->dist + 1; //cost is added

				unsigned int j = 0;
				while (j < pqv.size())
				{
					if (*child == *pqv[j] && pqv[j]->explored != true)
						break;
					j++;
				}

				if (j != pqv.size()) // If the child is already in the Queue, distance is update
				{
						pqv[j]->dist = child->dist;		
				}
				else// If the child is not in the queue it si added to the queue
				{
					pqv.push_back(child);
					child->visited = true;
				}

			}
		}
		
	}

}

//Helper DFS function used for solving 8 Puzzle
void DFS(Node* min, int final[N][N], Node* ans)
{
	if (min->cost == 0)
	{ //performs min(solutions found)
		if (ans->dist > min->dist)
		{
			ans->parent = min->parent;
			ans->dist = min->dist;
      memcpy(ans->mat,min->mat , sizeof ans->mat);
		}

	}
	else
	{
		//Visit Each child
		for (int i = 0; i < 4; i++)
		{
			if (isSafe(min->x + row[i], min->y + col[i]))
			{
				Node* child = newNode(min->mat, min->x,
					min->y, min->x + row[i],
					min->y + col[i],
					0, 0, min);
				child->cost = calculateCost(child->mat, final);
				child->dist = min->dist + 1;

				if ( child->dist < 15)
				{
					DFS(child, final, ans);
				}
			}
		}
	}
}

// Function to solve 8 puzzle algorithm using
// BFS . x and y are blank tile coordinates
// in initial state
void solve_DFS(int initial[N][N], int x, int y, 
	int final[N][N])
{
	Node* root1 = newNode(initial, x, y, x, y, 0, 0, NULL);
	Node* temp = newNode(initial, x, y, x, y, 0, INT_MAX, NULL);
	root1->cost = calculateCost(initial, final);
	DFS(root1, final,  temp);
	printPath(temp);
	cout << "Cost: " << temp->dist << endl;
}

// Driver code
int main()
{
	int initial[N][N] =
	{
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	};

	// Initial configuration
    // Value 0 is used for empty space
	ifstream input;
	input.open("1.txt");
	if (input) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				input >> initial[i][j];
			}
		}
	}
	input.close();

	// Solvable Final configuration
	// Value 0 is used for empty space
	int final[N][N] =
	{
		{1, 2, 3},
		{8, 0, 4},
		{7, 6, 5}
	};

	// Blank tile coordinates in initial
	// configuration
	int x = 1, y = 1;
	cout << "------8 Puzzle Solver-----" << endl << endl;
	cout << "Solution using Dijkstra's Algortihm" << endl << endl;
	solve_Dijkstra(initial, x, y, final);
	cout << endl << "Solution using BFS" << endl << endl;
	solve_BFS(initial, x, y, final);
	cout << endl << "Solution using DFS" << endl << endl;
	solve_DFS(initial, x, y, final);

	return 0;
}
