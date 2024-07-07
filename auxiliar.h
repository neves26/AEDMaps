#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#ifndef functionsHeader
#define functionsHeader

typedef struct _problem{
	char var[3];
	int vi;
	int vj;
	int k;
	double p;
	char c;
}PROBLEM;

typedef struct node {
 	int data;
  double cost;
  struct node* next;
} node;

typedef struct adjList {
	int numAdj;
	char* tags;
  node* head;
} adjList;

typedef struct graph {
  	adjList* vector;
  	int numNodes;
		int numEdges;
  	int* visited;
} graph;

typedef struct link_info{
	int v1;
	int v2;
	double custo;
}link_info;

typedef struct links{
	int v1;
	int v2;
	double custo;
	struct links * next;
}links;

/*estrutura que guarda caminhos para varianteB*/
typedef struct path {
	int v;
	int steps;
	double custo;
	links* head;
} path;

int compare_ext (char *a, char *b, int n);
int le_um_prbs(PROBLEM *P,FILE *fr);

node* c_node (int , double);
graph* create_graph (int , int);
void add_edge (graph* , int , int, double );
void freeGraph(graph* );

void varianteA1(FILE*, graph*, int, int, char*, int, int, double*);
void varianteB1(FILE*, graph*, PROBLEM);
void varianteC1(FILE*, graph*, PROBLEM , double*);
void varianteD1(FILE*, graph*, PROBLEM , double*);

link_info* Dijkstra(graph* , int , int, int*, double*);
path* DijkstraB1(graph* g, int v, int src, char c, int* tag);
path* DijkstraB2(graph*, int, char);
link_info* DijkstraC1(graph* g, PROBLEM p, int*steps, double*custo, int* xPov, int* v2, double* c1);
double DijkstraC2(graph* g, int xPov, int v, int src);
link_info* DijkstraD1(graph* g, PROBLEM p, int*steps, double*custo, int* xPov, int* v2, double* c1);
double DijkstraD2(graph* g, int xPov, int v, PROBLEM);

double costNodes(graph* , int , int);
double costNodesD(graph* , int , int, int , int);
int checkTag(graph*, int , char );
void printInversePath(FILE*, links*);
void freePaths(links* );

#endif
