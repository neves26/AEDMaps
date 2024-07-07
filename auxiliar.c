#include "auxiliar.h"
#include <float.h>


/*funcao auxiliar para verificar a extensões*/
int compare_ext (char *a, char *b, int n)
{
	int result = 0;
	int i, j = n;

	for( i = strlen(a)-1 ; i>=strlen(a)-1-n; i--, j--)
	{
		if(a[i]!=b[j])
		{
			result = 1;
			return result;
		}
	}
	return result;
}

/*----------------------------------------------------------------------
NOME: le_um_prbs()
OBJECTIVO: Ler e guardar um problema/cabecalho
INPUTS: ponteiro para variavel do tipo PROBLEM (PROBLEM *P) e ponteiro para ficheiro onde esta o problema (FILE *fp)
OUTPUT: --

DESCRICAO: --
------------------------------------------------------------------------*/
int le_um_prbs(PROBLEM *P, FILE *fr)
{
	int num_conv = 0;
	int aux = 0;
	/*Ler o primeiro problema do ficheiro de problemas*/
		aux = fscanf(fr,"%s", P->var);
		switch (P->var[0])
		{
			case 'A':
				num_conv = fscanf(fr,"%d %d", &P->vi, &P->vj) + aux;
				break;
			case 'B':
				num_conv = fscanf(fr,"%d %d %c %lf", &P->vi, &P->vj, &P->c, &P->p) + aux;
				break;
			case 'C':
				num_conv = fscanf(fr,"%d %d %d", &P->vi, &P->vj, &P->k) + aux;
				break;
			case 'D':
				num_conv = fscanf(fr,"%d %d %d", &P->vi, &P->vj, &P->k) + aux;
				break;
		}
		/*printf("num conversoes = %d\n", num_conv);*/
		return num_conv;
}

/*----------------------------------------------------------------------
NOME: c_node()
OBJECTIVO: criar um no do tipo node, alocar mememoria para ele e preenche-lo
INPUTS: variavel do tipo int que sera o verice e uma variavel double com o custo associado
OUTPUT: um ponteiro para o no criado

DESCRICAO: --
------------------------------------------------------------------------*/
node* c_node (int x1, double cost)
{
	node* q = (node*) malloc(sizeof(node));
	q->data = x1;
	q->cost = cost;
	q->next = NULL;
	return q;
}

/*----------------------------------------------------------------------
NOME: create_graph()
OBJECTIVO: alocar memoria para uma estrutura do tipo grafo e inicializar as suas variveis
INPUTS: recebe uma variavel do tipo inteiro com o numero de vertices que o grafo ira ter
OUTPUT: um ponteiro para o grafo criado

DESCRICAO: --
------------------------------------------------------------------------*/
graph* create_graph (int V, int A)
{
	int i;

	graph* G = (graph*)malloc(sizeof(graph));
	G->numNodes = V;
	G->vector = (adjList*)malloc((V+1) * sizeof(adjList));
	G->visited = (int*) NULL;
	G->numEdges = A;

	for(i=0; i<=V; i++)
	{
		G->vector[i].head = NULL;
		G->vector[i].numAdj = 0;
		G->vector[i].tags = NULL;
	}
	return G;
}

/*----------------------------------------------------------------------
NOME: add_edge()
OBJECTIVO: criar uma aresta entre dois vertices
INPUTS: um ponteiro para o grafo, dois int que sao o vertice fonte e vertice destino e um double que e o custo associado a essa ligacao
OUTPUT: --

DESCRICAO: --
------------------------------------------------------------------------*/
void add_edge (graph* G, int src, int dest, double cost)
{
	node* tmp = c_node(dest, cost);
	tmp->next = G->vector[src].head;
	G->vector[src].head = tmp;
	G->vector[src].numAdj++;
	G->vector[dest].numAdj++;

	tmp = c_node(src,cost);
	tmp->next = G->vector[dest].head;
	G->vector[dest].head = tmp;

}


/*----------------------------------------------------------------------
NOME: freeGraph()
OBJECTIVO: libertar memoria criada por create_graph()
INPUTS: ponteiro para graph
OUTPUT: --

DESCRICAO: --
------------------------------------------------------------------------*/
void freeGraph(graph* G)
{
	node* aux1 = (node*) NULL;
	node* aux2 = (node*) NULL;

	int i = 0;

	for(i=0; i <= G->numNodes; i++) {
		aux1 = G->vector[i].head;
		if(G->vector[i].tags!=NULL)
			free(G->vector[i].tags);
		if(aux1 == (node*) NULL)
			continue;
		while(aux1->next != NULL) {
			aux2 = aux1;
			aux1 = aux1->next;
			free(aux2);
		}
		free(aux1);
	}
	free(G->visited);
	free(G->vector);
	free(G);
}

/*---------------------------------------------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------
NOME: varianteA1()
OBJECTIVO: resolver os problemas do tipo A1, ou seja, determinar o caminho de menor custo entre 2 vertices
INPUTS:
OUTPUT:

DESCRICAO: --
------------------------------------------------------------------------*/
void varianteA1(FILE* write, graph* g, int vertices, int arestas, char* var, int v, int src, double* cost) {
	double custo = 0.0;
	int steps, i;
	link_info *outcome;

	if(v > g->numNodes || v <= 0 || src > g->numNodes || src <= 0) {
		fprintf(write,"%d %d %s %d %d -1\n\n", vertices, arestas, var, v, src);
		return;
	}

	if(v == src) {
		fprintf(write,"%d %d %s %d %d 0 0.00\n\n", vertices, arestas, var, v, src);
		return;
	}

	/*aplica o algoritmos para encontrar o caminho*/
	outcome = Dijkstra(g, v, src, &steps, &custo);

	/*caso não exista caminho imprime no ficheiro o cabeçlho seguido de (-1)*/
	if(custo == FLT_MAX) {
		fprintf(write,"%d %d %s %d %d -1\n\n", vertices, arestas, var, v, src);
		free(outcome);
		return;
	}

	/*se nº passos>o (se existe caminho) imprime no ficheiro o cabecalho + passos + custo da solucao e abaixo o caminho*/
	if(steps>0)
	{
		fprintf(write,"%d %d %s %d %d %d %0.2f\n", vertices, arestas, var, v, src, steps, custo);
		for(i=steps-1;i>=0;i--)
			fprintf(write,"%d %d %0.2f\n",outcome[i].v1, outcome[i].v2, outcome[i].custo);
		fprintf(write,"\n");
	}

	free(outcome);
	return;
}

void varianteB1(FILE* write, graph* g, PROBLEM p) {
	double custo = FLT_MAX, auxC = 0.0;
	int N = g->numNodes + 1, i, auxV = 0, auxS = 0, tag = 0;
	path *s1 = NULL, *s2 = NULL;
	links* auxN = NULL;

	if(p.vi > g->numNodes || p.vi <= 0 || p.vj > g->numNodes || p.vj <= 0) {
		if(p.p == -1.0) {
			fprintf(write,"%d %d %s %d %d %c -1 -1\n\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.c);
		} else
			fprintf(write,"%d %d %s %d %d %c %.2lf -1\n\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.c, p.p);
		return;
	}

	if(p.vi == p.vj && checkTag(g,p.vi,p.c)) {
		if(p.p == -1.0) {
			fprintf(write,"%d %d %s %d %d %c -1 %d %.2lf\n\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.c, 0, 0.00);
		} else {
			fprintf(write,"%d %d %s %d %d %c %.2lf %d %.2lf\n\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.c, p.p, 0, 0.00);
		}
		return;
	}

  /*s1 contem todos os caminhos em listas de Vi para vertices do grafico com a tag (se existirem)*/
	s1 = DijkstraB1(g, p.vi, p.vj, p.c, &tag);
	if(s1[0].v == 0) { /*nao ha vertices no grafo com a tag */
		if(p.p == -1.0) {
			fprintf(write,"%d %d %s %d %d %c -1 -1\n\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.c);
		} else
			fprintf(write,"%d %d %s %d %d %c %.2lf -1\n\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.c, p.p);

		for(i = 1; i < N; i++) {
			if(s1[i].steps != -1) {
				freePaths(s1[i].head);
			}
		}
		free(s1);
		return;
	}
	/*ha vertices com a tag e nenhum deles pertence ao caminho de menor custo (tag = 0) entao meter em s2 os caminhos de Vj para tais vertices com tag*/
	if(s1[0].v > 0 && tag == 0) {
		s2 = DijkstraB2(g, p.vj, p.c);
	}

	/*imprimir resultado quando o menor caminho ja contem a tag*/
	if(tag == 1) {
		if(p.p == -1.0) {
			fprintf(write,"%d %d %s %d %d %c -1 %d %0.2f\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.c, s1[p.vj].steps, s1[p.vj].custo);
		} else {
			fprintf(write,"%d %d %s %d %d %c %.2lf %d %0.2f\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.c, p.p, s1[p.vj].steps, s1[p.vj].custo);
		}

		for(auxN = s1[p.vj].head; auxN->next != NULL ;  auxN = auxN->next) {
			fprintf(write,"%d %d %0.2f\n", auxN->v1, auxN->v2, auxN->custo);
		}
		fprintf(write,"%d %d %0.2f\n\n", auxN->v1, auxN->v2, auxN->custo);


		for(i = 1; i < N; i++) {
			if(s1[i].steps != -1) {
				freePaths(s1[i].head);
			}
		}
		free(s1);
		return;
	}
	/*loop para encontrar o vertice Vx que contem a tag e e caminho mais curto e depois imprimir esse resultado*/
	if(s2 != NULL) {
		for(i = 1; i < N; i++) {

			if(s1[i].steps == -1 || s2[i].steps == -1) {
				continue;
			}

			auxC = s1[i].custo + s2[i].custo;
			if(auxC < custo) {
				custo = auxC;
				auxV = i;
				auxS = s1[i].steps + s2[i].steps;
			}
		}
		/*custo maior que a percentagem defenida no problema?*/
		if(custo > ((p.p * s1[p.vj].custo) + s1[p.vj].custo) && p.p != -1.0) {
			if(p.p == -1.0) {
				fprintf(write,"%d %d %s %d %d %c -1 -1\n\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.c);
			} else {
				fprintf(write,"%d %d %s %d %d %c %.2lf -1\n\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.c, p.p);
			}

			if(s2[0].v > 0) {
				for(i = 1; i < N; i++) {
					if(s1[i].steps != -1) {
						freePaths(s1[i].head);
					}

					if(s2[i].steps != -1) {
						freePaths(s2[i].head);
					}
				}
			}

			free(s1); free(s2);
			return;
		}
	}

	/*quando bate tudo certo: ha vertice que contem tag e o custo n excede o pedido e e o de menor custo e imprime o resultado*/
	if(s1[auxV].steps != -1 && s2[auxV].steps != -1) {
		if(p.p == -1.0) {
			fprintf(write,"%d %d %s %d %d %c -1 %d %0.2f\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.c, auxS, custo);
		} else {
			fprintf(write,"%d %d %s %d %d %c %.2lf %d %0.2f\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.c, p.p, auxS, custo);
		}

		for(auxN = s1[auxV].head; auxN->next != NULL ;  auxN = auxN->next) {
			fprintf(write,"%d %d %0.2f\n", auxN->v1, auxN->v2, auxN->custo);
		}
		fprintf(write,"%d %d %0.2f\n", auxN->v1, auxN->v2, auxN->custo);

		printInversePath(write, s2[auxV].head);
		fprintf(write,"\n");
	}
	/*liberta os caminhos*/
	if(s2[0].v > 0) {
		for(i = 1; i < N; i++) {
			if(s1[i].steps != -1) {
				freePaths(s1[i].head);
			}

			if(s2[i].steps != -1) {
				freePaths(s2[i].head);
			}
		}
	}
	/*liberta a estrutura que continha ponteiros para os caminhos*/
	free(s1); free(s2);
	return;
}

void varianteC1(FILE* write, graph* g, PROBLEM p, double* cost) {
	double custo = 0.0, custo1 = 0.0, custo2 = 0.0;
	int steps, i, vX, vi2;
	link_info *outcome;

	if(p.vi > g->numNodes || p.vi <= 0 || p.vj > g->numNodes || p.vj <= 0) {
		fprintf(write,"%d %d %s %d %d %d %d\n\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.k, -1);
		return;
	}

	if(p.vi == p.vj) {
		fprintf(write,"%d %d %s %d %d %d %d %.2lf %d\n\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.k, 0, 0.00, -1);
		return;
	}

  /*aplica o algoritmos para encontrar o caminho mais curto entre Vi e Vj e descobre o vertice q n pode ser usado*/
	outcome = DijkstraC1(g, p, &steps, &custo, &vi2, &vX, &custo1);

	/*ha caminho*/
	if(steps>0)
	{
		if(p.k < 2 || p.k > steps) { /*se no indiponivel e Vi ou Vj ou fora do caminho*/
			fprintf(write,"%d %d %s %d %d %d %d %0.2f %d\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.k, steps, custo, -1);
			for(i=steps-1;i>=0;i--)
				fprintf(write,"%d %d %0.2f\n",outcome[i].v1, outcome[i].v2, outcome[i].custo);
			fprintf(write,"\n");
			free(outcome);
			return;
		}
		custo2 = DijkstraC2(g, vX, p.vi, p.vj); /*procura caminho sem o vertice impossibilitado*/
		if(custo2 == FLT_MAX)
			fprintf(write,"%d %d %s %d %d %d %d %0.2f %d\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.k, steps, custo, -1);
		else {
			custo2 = custo2 - custo;
			fprintf(write,"%d %d %s %d %d %d %d %0.2f %0.2f\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.k, steps, custo, custo2);
		}
		for(i=steps-1;i>=0;i--)
			fprintf(write,"%d %d %0.2f\n",outcome[i].v1, outcome[i].v2, outcome[i].custo);
	}
	/*caso não exista caminho imprime no ficheiro o cabeçlho seguido de (-1)*/
	else
		fprintf(write,"%d %d %s %d %d %d %d\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.k, -1);


	fprintf(write,"\n");

	free(outcome);
	return;
}

void varianteD1(FILE* write, graph* g, PROBLEM p, double* cost) {
	double custo = 0.0, custo1 = 0.0, custo2 = 0.0;
	int steps, i, vX, vi2;
	link_info *outcome;

	if(p.vi > g->numNodes || p.vi <= 0 || p.vj > g->numNodes || p.vj <= 0) {
		fprintf(write,"%d %d %s %d %d %d %d\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.k, -1);
		fprintf(write,"\n");
		return;
	}

	if(p.vi == p.vj) {
		fprintf(write,"%d %d %s %d %d %d %d %.2lf %d\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.k, 0, 0.00, -1);
		fprintf(write,"\n");
		return;
	}

  /*aplica o algoritmos para encontrar o caminho mais curto entre Vi e Vj e descobre a aresta impossibilitada*/
	outcome = DijkstraD1(g, p, &steps, &custo, &vi2, &vX, &custo1);

	/*se nº passos>o (se existe caminho) imprime no ficheiro o cabecalho + passos + custo da solucao e abaixo o caminho*/

	if(steps>0)
	{
		if(p.k < 1 || p.k > steps) { /*aresta pedida esta fora do caminho*/
			fprintf(write,"%d %d %s %d %d %d %d %0.2f %d\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.k, steps, custo, -1);
			for(i=steps-1;i>=0;i--)
				fprintf(write,"%d %d %0.2f\n",outcome[i].v1, outcome[i].v2, outcome[i].custo);
			fprintf(write,"\n");
			free(outcome);
			return;
		}
		custo2 = DijkstraD2(g, vi2, vX, p); /*procura caminho de menor custo sem a aresta indisponivel*/
		if(custo2 == FLT_MAX)
			fprintf(write,"%d %d %s %d %d %d %d %0.2f %d\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.k, steps, custo, -1);
		else {
			custo2 = custo2 - custo;
			fprintf(write,"%d %d %s %d %d %d %d %0.2f %0.2f\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.k, steps, custo, custo2);
		}
		for(i=steps-1;i>=0;i--)
			fprintf(write,"%d %d %0.2f\n",outcome[i].v1, outcome[i].v2, outcome[i].custo);
	}
	/*caso não exista caminho imprime no ficheiro o cabeçlho seguido de (-1)*/
	else
		fprintf(write,"%d %d %s %d %d %d %d\n", g->numNodes, g->numEdges, p.var, p.vi, p.vj, p.k, -1);



	fprintf(write,"\n");

	free(outcome);
	return;
}
/*como nas listas os caminhos ficam ao contrario queremos imprimir o caminho no ficheiro de saida invertido*/
void printInversePath(FILE* w, links* h) {

	if(h->next != NULL) {
		printInversePath(w, h->next);
	}
	fprintf(w,"%d %d %0.2f\n", h->v2, h->v1, h->custo);

	return;
}

void freePaths(links* s) {
	links* aux = NULL;

	if(s == NULL)
		return;

	while(s->next != NULL) {
		aux = s->next;
		free(s);
		s = aux;
	}

	free(s);

	return;
}

/*Dijkstra simples, procura apenas caminho menor custo*/
link_info* Dijkstra(graph* g, int v, int src, int* steps, double* custo) {

	int *pred, *visited, nextnode = 1, i,j,k,aux = 0, count = 0;
	double* distance, mindistance, auxC = 0.0;
	int N;
	link_info *path;
	node * p;
	N = g->numNodes	+ 1;

	distance = malloc(sizeof(double) * (N));
	pred = malloc(sizeof(int) * N);
	visited = malloc(sizeof(int) * N);


	for(i=0; i < N; i++) {
		distance[i] = FLT_MAX;
		pred[i] = v;
		visited[i] = 0;
	}
	p = g->vector[v].head;
	while(p != NULL) {
		distance[p->data] = p->cost;
		p = p->next;
	}

	distance[v] = 0.0;
	visited[v] = 1;
	count = 1;
	while( count < N - 2  && !visited[src]) {
		mindistance = FLT_MAX;

		for(i=0; i < N; i++) {
			if(distance[i] < mindistance && !visited[i]) {
				mindistance = distance[i];
				nextnode = i;
			}
		}

		visited[nextnode] = 1;
		p = g->vector[nextnode].head;

		while(p != NULL) {
			if(!visited[p->data]) {
				auxC = p->cost;
				if(mindistance + auxC < distance[p->data]) {
					distance[p->data] = mindistance + auxC;
					pred[p->data] = nextnode;
				}
			}
			p = p->next;
		}
		count++;
	}

/*ciclo para determinar o numero de passos do caminho*/
	k=0;
	if(src != v) {
		j=src;
		do{
			j = pred[j];
			k++;
		} while(j != v);
	}


/*aloca memoria para guardar o caminho de menor custo com o tamanho do numero de passos*/
	path = (link_info*)malloc(sizeof(link_info)*k);

/*ciclo para preencher o vetor como os dados das arestas pertencentes ao caminho de menor custo*/
	if(src != v) {
		j=src;
		do{
			for(aux=0;aux<k;aux++){
			path[aux].v1 = pred[j];
			path[aux].v2 = j;
			path[aux].custo = distance[j] - distance[pred[j]];

			j=pred[j];
			}
		}while(j!=v);
	}

	*steps = k;
	*custo = distance[src];
	free(distance); free(pred); free(visited);
	return path;
}

double costNodes(graph* g, int src, int dest) {
	double cost = FLT_MAX;
	node* aux = NULL;
	int auxN = 0;

	auxN = g->vector[src].numAdj;

	if(auxN > g->vector[dest].numAdj) {
		aux = g->vector[dest].head;
		if(aux!=NULL) {
			for(;aux!=NULL;aux = aux->next) {
				if(aux->data == src)
					cost = aux->cost;
			}
		}
	} else {
		aux = g->vector[src].head;
		if(aux!=NULL) {
			for(;aux!=NULL;aux = aux->next) {
				if(aux->data == dest)
					cost = aux->cost;
			}
		}
	}

	return cost;
}

/*Dijkstra procura caminho menor custo de Vi para todos vertices e guarda caminhos para vertices com tag e Vj*/
path* DijkstraB1(graph* g, int v, int src, char c, int* tag) {
	int *pred, *visited, count = 0, nextnode =1, i, j, N;
	double* distance, mindistance, edge_cost, auxC = 0.0;
	path* s;
	links* aux1, *aux2;
	node* p;
	N = g->numNodes	+ 1;

	distance = malloc(sizeof(double) * N);
	pred = malloc(sizeof(int) * N);
	visited = malloc(sizeof(int) * N);
	s = (path*) malloc(sizeof(path) * N);

	for(i=1; i<N; i++) {
		distance[i] = FLT_MAX;
		pred[i] = v;
		visited[i] = 0;
		s[i].v = i;
		s[i].steps = -1;
		s[i].custo = 0.0;
		s[i].head = (links*) NULL;
		if(checkTag(g,i,c)) {
			count++;
			/*armazena n de vertices que tem a tag*/
			s[i].steps = 0;
		}
		if(i == src)
			s[i].steps = 0;
	}
	s[0].v = count;
	p = g->vector[v].head;
	while(p != NULL) {
		distance[p->data] = p->cost;
		p = p->next;
	}

	distance[v] = 0.0;
	visited[v] = 1;
	count = 1;

	while(count < g->numNodes) {
		mindistance = FLT_MAX;

		for(i=1; i < N; i++) {
			if(distance[i] < mindistance && !visited[i]) {
				mindistance = distance[i];
				nextnode = i;
			}
		}

		visited[nextnode] = 1;
		p = g->vector[nextnode].head;
		while(p != NULL) {
			if(!visited[p->data]) {
				auxC = p->cost;
				if(mindistance + auxC < distance[p->data]) {
					distance[p->data] = mindistance + auxC;
					pred[p->data] = nextnode;
				}
			}
			p = p->next;
		}
		count++;
	}

/*ciclo para determinar o caminho e quais vertices tem a tag*/
	for(i = 1; i < N; i++) {
		/*apenas os vertices que tinham tag vao ter os seus caminhos guardados em listas ligadas*/
		if(s[i].steps == 0) {
			if(i != v) {
				j = i;
				do {
					edge_cost = distance[j] - distance[pred[j]];
					s[i].steps++;
					s[i].custo += edge_cost;
					aux1 = (links*) malloc(sizeof(links)*1);
					aux1->v1 = pred[j];
					aux1->v2 = j;
					aux1->custo = edge_cost;
					aux1->next = NULL;
					if(s[i].head == NULL) {
						s[i].head = aux1;
					} else {
						aux2 = s[i].head;
						s[i].head = aux1;
						aux1->next = aux2;
					}
					/*vertice j (do caminho mais curto) tem a tag?*/
					if(i == src && checkTag(g,j,c))
						*tag = 1;

					j = pred[j];
				} while(j != v);
			}
		}
	}

	/*vertice inicial Vi ja tem a tag?*/
	if(checkTag(g,v,c))
		*tag = 1;

	free(distance); free(pred); free(visited);
	return s;
}

/*Dijkstra procurar caminhos de menor custo de Vj para todos os vertices com tag e Vi*/
path* DijkstraB2(graph* g, int v, char c) {
	int *pred, *visited, count = 0, nextnode =1, i, j;
	double* distance, mindistance, edge_cost, auxC;
	int N;
	node* p;
	path* s;
	links* aux1, *aux2;
	N = g->numNodes	+ 1;

	distance = malloc(sizeof(double) * N);
	pred = malloc(sizeof(int) * N);
	visited = malloc(sizeof(int) * N);
	s = (path*) malloc(sizeof(path)*N);

	for(i=1; i<N; i++) {
		distance[i] = FLT_MAX;
		pred[i] = v;
		visited[i] = 0;
		s[i].v = i;
		s[i].steps = -1;
		s[i].custo = 0.0;
		s[i].head = (links*) NULL;
		if(checkTag(g,i,c)) {
			count++;
			s[i].steps = 0;
		}
	}
	s[0].v = count;
	p = g->vector[v].head;
	while(p != NULL) {
		distance[p->data] = p->cost;
		p = p->next;
	}

	distance[v] = 0.0;
	visited[v] = 1;
	count = 1;

	while(count < g->numNodes - 1) {
		mindistance = FLT_MAX;

		for(i=1; i < N; i++) {
			if(distance[i] < mindistance && !visited[i]) {
				mindistance = distance[i];
				nextnode = i;
			}
		}

		visited[nextnode] = 1;
		p = g->vector[nextnode].head;

		while(p != NULL) {
			if(!visited[p->data]) {
				auxC = p->cost;
				if(mindistance + auxC < distance[p->data]) {
					distance[p->data] = mindistance + auxC;
					pred[p->data] = nextnode;
				}
			}
			p = p->next;
		}
		count++;
	}

/*ciclo para determinar o numero de passos do caminho*/
	for(i = 1; i < N; i++) {
		if(s[i].steps == 0) {
			if(i != v) {
				j = i;
				do {
					edge_cost = distance[j] - distance[pred[j]];
					s[i].steps++;
					s[i].custo += edge_cost;
					aux1 = (links*) malloc(sizeof(links)*1);
					aux1->v1 = pred[j];
					aux1->v2 = j;
					aux1->custo = edge_cost;
					aux1->next = NULL;
					if(s[i].head == NULL) {
						s[i].head = aux1;
					} else {
						aux2 = s[i].head;
						s[i].head = aux1;
						aux1->next = aux2;
					}
					j = pred[j];
				} while(j != v);
			}
		}
	}


	free(distance); free(pred); free(visited);
	return s;
}

int checkTag(graph* g, int v, char c) {
	int val = 0, i = 0, length = 0;
	if(g->vector[v-1].tags == NULL)
		return val;
	char* tags = g->vector[v-1].tags;
	length = strlen(tags);

	for(i =0; i < length; i++) {
		if(tags[i] == c) {
			val = 1;
			return val;
		}
	}
	return val;
}

/*Dijkstra procura caminho de menor custo entre Vi e Vj e encotra vertice indiponivel*/
link_info* DijkstraC1(graph* g, PROBLEM P1, int*steps, double*custo, int* v2, int* xPov, double* c1) {
	int *pred, *visited, count, nextnode=1, i,j,k=1,aux;
	double* distance, mindistance, auxC = 0.0;
	int N, v = P1.vi, src = P1.vj, pov = P1.k;
	link_info *path;
	node* p;
	N = g->numNodes	+ 1;
	*c1 = 0.0;

	distance = malloc(sizeof(double) * (N+1));
	pred = malloc(sizeof(int) * N);
	visited = malloc(sizeof(int) * N);

	for(i=0; i < N; i++) {
		distance[i] = FLT_MAX;
		pred[i] = v;
		visited[i] = 0;
	}
	p = g->vector[v].head;
	while(p != NULL) {
		distance[p->data] = p->cost;
		p = p->next;
	}

	distance[v] = 0.0;
	visited[v] = 1;
	count = 1;


	while(count < g->numNodes - 1 && !visited[src]) {
		mindistance = FLT_MAX;


		for(i=0; i < N; i++) {
			if(distance[i] < mindistance && !visited[i]) {
				mindistance = distance[i];
				nextnode = i;
			}
		}

		visited[nextnode] = 1;
		p = g->vector[nextnode].head;

		while(p != NULL) {
			if(!visited[p->data]) {
				auxC = p->cost;
				if(mindistance + auxC < distance[p->data]) {
					distance[p->data] = mindistance + auxC;
					pred[p->data] = nextnode;
				}
			}
			p = p->next;
		}
		count++;
	}

/*ciclo para determinar o numero de passos do caminho*/
	k=0;


			if(src!=v){
				j=src;
				do{
					j=pred[j];
					k++;
				}while(j!=v);
			}


	*steps = k;

/*aloca memoria para guardar o caminho de menor custo com o tamanho do numero de passos*/
	path = (link_info*)malloc(sizeof(link_info)*k);

/*ciclo para preencher o vetor como os dados das arestas pertencentes ao caminho de menor custo*/

			if(src!=v){
				j=src;
				do{
					for(aux=0;aux<k;aux++){
						path[aux].v1 = pred[j];
						path[aux].v2 = j;
						path[aux].custo = distance[j] - distance[pred[j]];;

						if(aux == k - pov + 1) {
							*xPov = j;
							*v2 = pred[j];
							*c1 = distance[*v2];
						}

						j=pred[j];
					}
				}while(j!=v);
			}



	*custo = distance[src];
	free(distance); free(pred); free(visited);
	return path;
}

/*Dijkstra encontra caminho menor custo entre Vi e Vj sem usar vertice indisponivel*/
double DijkstraC2(graph* g, int xPov, int v, int src) {
	int *pred, *visited, count, nextnode = -1, i;
	double* distance, mindistance, custo = 0.0, auxC =0.0;
	node* p;
	int N;

	N = g->numNodes	+ 1;
	distance = malloc(sizeof(double) * (N+1));
	pred = malloc(sizeof(int) * N);
	visited = malloc(sizeof(int) * N);

	for(i=0; i < N; i++) {
		if(i == xPov)
			continue;
		distance[i] = FLT_MAX;
		pred[i] = v;
		visited[i] = 0;
	}
	p = g->vector[v].head;
	while(p != NULL) {
		if(p->data == xPov) {
			p = p->next;
			continue;
		}
		distance[p->data] = p->cost;
		p = p->next;
	}

	distance[v] = 0.0;
	visited[v] = 1;
	count = 1;

	while(count < g->numNodes && !visited[src]) {
		mindistance = FLT_MAX;
		/*finds node with least cost adj to previous vertice*/
		for(i=0; i < N; i++) {
			if(i == xPov)
				continue;
			if(distance[i] < mindistance && !visited[i]) {
				mindistance = distance[i];
				nextnode = i;
			}
		}

		if(nextnode == -1) {
			break;
		}

		visited[nextnode] = 1;
		p = g->vector[nextnode].head;
		while(p != NULL) {
			if(p->data == xPov) {
				p = p->next;
				continue;
			}
			if(!visited[p->data]) {
				auxC = p->cost;
				if(mindistance + auxC < distance[p->data]) {
					distance[p->data] = mindistance + auxC;
					pred[p->data] = nextnode;
				}
			}
			p = p->next;
		}
		count++;
	}

	custo = distance[src];
	free(distance); free(pred); free(visited);
	return custo;
}

/*Dijkstra procura caminho menor custo entre Vi e Vj e aresta indisponivel*/
link_info* DijkstraD1(graph* g, PROBLEM p, int*steps, double*custo, int* v2, int* xPov, double* c1) {
	int *pred, *visited, count, nextnode=1, i,j,k=1,aux;
	double* distance, mindistance, edge_cost, auxC = 0.0;
	int N, v = p.vi, src = p.vj, pov = p.k;
	link_info *path;
	node* no;
	N = g->numNodes	+ 1;
	*c1 = 0.0;

	distance = malloc(sizeof(double) * (N+1));
	pred = malloc(sizeof(int) * N);
	visited = malloc(sizeof(int) * N);

	for(i=0; i < N; i++) {
		distance[i] = FLT_MAX;
		pred[i] = v;
		visited[i] = 0;
	}
	no = g->vector[v].head;
	while(no != NULL) {
		distance[no->data] = no->cost;
		no = no->next;
	}

	distance[v] = 0.0;
	visited[v] = 1;
	count = 1;


	while(count < g->numNodes && !visited[src]) {
		mindistance = FLT_MAX;


		for(i=0; i < N; i++) {
			if(distance[i] < mindistance && !visited[i]) {
				mindistance = distance[i];
				nextnode = i;
			}
		}

		visited[nextnode] = 1;
		no = g->vector[nextnode].head;

		while(no != NULL) {
			if(!visited[no->data]) {
				auxC = no->cost;
				if(mindistance + auxC < distance[no->data]) {
					distance[no->data] = mindistance + auxC;
					pred[no->data] = nextnode;
				}
			}
			no = no->next;
		}
		count++;
	}

/*ciclo para determinar o numero de passos do caminho*/
	k=0;

			if(src!=v){
				j=src;
				do{
					j=pred[j];
					k++;
				}while(j!=v);
			}

	*steps = k;

/*aloca memoria para guardar o caminho de menor custo com o tamanho do numero de passos*/
	path = (link_info*)malloc(sizeof(link_info)*k);

/*ciclo para preencher o vetor como os dados das arestas pertencentes ao caminho de menor custo*/

			if(src!=v){
				j=src;
				do{
					for(aux=0;aux<k;aux++){
						edge_cost = distance[j] - distance[pred[j]];

						path[aux].v1 = pred[j];
						path[aux].v2 = j;
						path[aux].custo = edge_cost;

						if(aux == k - pov) {
							*xPov = j;
							*v2 = pred[j];
							*c1 = distance[*v2];
						}

						j=pred[j];
					}
				}while(j!=v);
			}


	*custo = distance[src];
	free(distance); free(pred); free(visited);
	return path;
}

/*Dijkstra encontra caminho de menor custo entre Vi e Vj sem usar aresta indisponivel*/
double DijkstraD2(graph* g, int v, int xPov, PROBLEM P1) {
	int *pred, *visited, count, nextnode = -1, i, src = P1.vj, s = P1.vi;
	double* distance, mindistance, custo = 0.0, auxC = 0.0;
	int N;
	node* p;

	N = g->numNodes	+ 1;
	distance = malloc(sizeof(double) * (N+1));
	pred = malloc(sizeof(int) * N);
	visited = malloc(sizeof(int) * N);

	for(i=0; i < N; i++) {
		distance[i] = FLT_MAX;
		pred[i] = s;
		visited[i] = 0;
	}
	p = g->vector[s].head;
	while(p != NULL) {
		if(s == v && p->data == xPov) {
			distance[p->data] = FLT_MAX;
			p = p->next;
			continue;
		}
		distance[p->data] = p->cost;
		p = p->next;
	}

	distance[s] = 0.0;
	visited[s] = 1;
	count = 1;


	while(count < g->numNodes && !visited[src]) {
		mindistance = FLT_MAX;
		/*finds node with least cost adj to previous vertice*/
		for(i=0; i < N; i++) {
			if(distance[i] < mindistance && !visited[i]) {
				mindistance = distance[i];
				nextnode = i;
			}
		}

		if(nextnode == -1)
			break;

		visited[nextnode] = 1;
		p = g->vector[nextnode].head;
		while(p != NULL) {
			if(!visited[p->data]) {
				if((nextnode == v && p->data == xPov) || (nextnode == xPov && p->data == v)) {
					auxC = FLT_MAX;
				} else {
					auxC = p->cost;
				}
				if(mindistance + auxC < distance[p->data]) {
					distance[p->data] = mindistance + auxC;
					pred[p->data] = nextnode;
				}
			}
			p = p->next;
		}
		count++;
	}

	custo = distance[src];
	free(distance); free(pred); free(visited);
	return custo;
}

double costNodesD(graph* g, int src, int dest, int vi, int vj) {
	double cost = FLT_MAX;
	node* aux;
	aux = g->vector[src].head;

	if(src == vi && dest == vj) {
		return cost;
	}

	if(aux!=NULL) {
		for(;aux!=NULL;aux = aux->next) {
			if(aux->data == dest)
				cost = aux->cost;
		}
	}
	return cost;
}
