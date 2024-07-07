#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auxiliar.h"

int main(int argc, char *argv[])
{
	FILE *file_prbs, *file_maps, *file_sol;
	char op1[] = "-oo", op2[] = "-oa", op3[] = "-ao", op4[] = "-aa";
	char *prbsExt, *mapsExt, *filename_out;
	int i = 0, src = 0, dest = 0, num_arg = 0, num_arg_cabecalho=0;
	double cost;
	graph* G = (graph*) NULL;
	PROBLEM PRB;
	int num_conv = 0;
	int num_vertices = 0;
	int num_arestas = 0;
	int vert_lixo = 0;
	char tags[27];

/*------ Verificacao se o numero de argumentos de entrada está correto.---------------------------------------------------*/
	if(argc != 4)
	{
		return 0;
	}
/*------ Verificacao da entrada OPCAO (argv [1]) -------------------------------------------------------------------------*/
	if(strcmp(argv[1],op1)!= 0 && strcmp(argv[1],op2)!= 0 && strcmp(argv[1],op3)!= 0 && strcmp(argv[1],op4)!= 0)
	{
		return 0;
	}
/*------ Verificacao das extensões da entrada PROBLEMS (argv[2]) e da entrada MAPS (argv [3]) ----------------------------*/
	prbsExt = (char*) malloc(6 * sizeof(char));
	strcpy (prbsExt, ".prbs");
	mapsExt = (char*) malloc(6 * sizeof(char));
	strcpy (mapsExt, ".maps");

	if (strlen(argv[2]) < 5 || strlen(argv[3]) < 5)
	{
		return 0;
	}

	if (compare_ext(argv[2], prbsExt, 4) == 1 || compare_ext(argv[3], mapsExt, 4) == 1)
	{
		free(prbsExt); free(mapsExt);
		return 0;
	}

/*------ Nome para o ficheiro de saída -----------------------------------------------------------------------------------*/
	filename_out = (char*) malloc((strlen(argv[3])+ 4) * sizeof(char));
	for(i=0; i< strlen(argv[3])- 5; i++)
	{
		filename_out[i] = argv[3][i];
	}
	filename_out[i] = '\0';
	strcat(filename_out, ".routes");


/*------ Abertura de ficheiros de entrada, prbs e maps -------------------------------------------------------------------*/
	if((file_prbs = fopen(argv[2], "r")) == 0) {
		free(prbsExt); free(mapsExt); free(filename_out);
		return 0;
	}

	if((file_maps = fopen(argv[3], "r")) == 0) {
		free(prbsExt); free(mapsExt); free(filename_out);
		return 0;
	}

	if((file_sol = fopen(filename_out, "w")) == 0) {
		free(prbsExt); free(mapsExt); free(filename_out);
		return 0;
	}

/*------ Leitura, armazenamento e resolucao conforme a opcao inserida ----------------------------------------------------*/

/*-------- OPCAO 1 (-oo): Resolver o primeiro problema para o primeiro mapa ----------------------------------------------*/
	if(strcmp(argv[1],op1)== 0)
	{
		/*Ler e guardar o primeiro problema do ficheiro de problemas*/
		num_conv = le_um_prbs(&PRB,file_prbs);

		num_arg_cabecalho = fscanf(file_maps, "%d %d", &num_vertices, &num_arestas);
		if(num_arg_cabecalho != 2) {
			free(prbsExt); free(mapsExt);
			return 0;
		}

		G = create_graph(num_vertices, num_arestas);

		/*Ler e armazenar no grafo o primeiro mapa do ficheiro mapas*/
		for(i=0; i < num_vertices; i++)
		{
			num_arg = fscanf(file_maps, "%d %s", &vert_lixo, tags);
			if(num_arg != 2) {
				return 0;
			}
			if(tags[0] == '-')
				continue;
			G->vector[i].tags = (char*) malloc(sizeof(char) * (strlen(tags)+1));
			strcpy(G->vector[i].tags, tags);
		}


		for(i=0; i < num_arestas; i++)
		{
			num_arg = fscanf(file_maps,"%d %d %lf", &src, &dest, &cost);
			if(num_arg != 3)
				return 0;
			add_edge(G, src, dest, cost);
		}

		/*Resolver o Probema consoante a variante e imprimir solucao no ficheiro de saida*/
		fprintf(file_sol,"%s %s %s\n\n", argv[1],argv[2],argv[3]);
		switch (PRB.var[0])
		{
			case 'A':
				varianteA1(file_sol, G, num_vertices, num_arestas, PRB.var, PRB.vi, PRB.vj, &cost);
				break;
			case 'B':
				varianteB1(file_sol, G, PRB);
				break;
			case 'C':
				varianteC1(file_sol, G, PRB, &cost);
				break;

			case 'D':
				varianteD1(file_sol, G, PRB, &cost);
				break;
		}
	}

/*-------- OPCAO 2 (-oa): Resolver o primeiro problema para todos os mapas do ficheiro maps ------------------------------*/
	if(strcmp(argv[1],op2)== 0)
	{
		/*Ler e guardar o primeiro problema do ficheiro de problemas*/
		num_conv = le_um_prbs(&PRB,file_prbs);

		/*ciclo para percorrer todos os mapas do ficheiro maps ----*/
		fprintf(file_sol,"%s %s %s\n\n", argv[1],argv[2],argv[3]);
		while(1) {
			/*ler e armazenar no grafo o mapa*/
			num_arg_cabecalho = fscanf(file_maps, "%d %d", &num_vertices, &num_arestas);
			if(num_arg_cabecalho != 2)
				break;
			if(G != (graph*) NULL)
				freeGraph(G);

			G = create_graph(num_vertices, num_arestas);

			/*Ler e armazenar no grafo o primeiro mapa do ficheiro mapas*/
			for(i=0; i < num_vertices; i++)
			{
				num_arg = fscanf(file_maps, "%d %s", &vert_lixo, tags);
				if(num_arg != 2) {
					return 0;
				}
				if(tags[0] == '-')
					continue;
				G->vector[i].tags = (char*) malloc(sizeof(char) * (strlen(tags)+1));
				strcpy(G->vector[i].tags, tags);
			}

			for(i=0; i < num_arestas; i++)
			{
				num_arg = fscanf(file_maps,"%d %d %lf", &src, &dest, &cost);
				if(num_arg != 3)
					return 0;
				add_edge(G, src, dest, cost);
			}

			/*Resolver o Probema consoante a variante e imprimir solucao no ficheiro de saida*/
			switch (PRB.var[0])
			{
				case 'A':
					varianteA1(file_sol, G, num_vertices, num_arestas, PRB.var, PRB.vi, PRB.vj, &cost);
					break;
				case 'B':
					varianteB1(file_sol, G, PRB);
					break;
				case 'C':
					varianteC1(file_sol, G, PRB, &cost);
					break;
				case 'D':
					varianteD1(file_sol, G, PRB, &cost);
					break;
			}
		}
	}

/*-------- OPCAO 3 (-ao): Resolver todos os problemas do ficheiro problemas para o primeiro mapa do ficheiro maps --------*/

	if(strcmp(argv[1],op3)== 0)
	{
		/*Ler e guardar o primeiro mapa do ficheiro maps*/
		num_arg_cabecalho = fscanf(file_maps, "%d %d", &num_vertices, &num_arestas);
		if(num_arg_cabecalho != 2)
			return 0;
		if(G != (graph*) NULL) {
			freeGraph(G);
		}

		G = create_graph(num_vertices, num_arestas);

		/*Ler e armazenar no grafo o primeiro mapa do ficheiro mapas*/
		for(i=0; i < num_vertices; i++)
		{
			num_arg = fscanf(file_maps, "%d %s", &vert_lixo, tags);
			if(num_arg != 2) {
				return 0;
			}
			if(tags[0] == '-')
				continue;
			G->vector[i].tags = (char*) malloc(sizeof(char) * (strlen(tags)+1));
			strcpy(G->vector[i].tags, tags);
		}

		for(i=0; i < num_arestas; i++)
		{
			num_arg = fscanf(file_maps,"%d %d %lf", &src, &dest, &cost);
			if(num_arg != 3)
				return 0;
			add_edge(G, src, dest, cost);
		}

		/*ciclo para percorrer todos os problemas do ficheiro problemas*/
		fprintf(file_sol,"%s %s %s\n\n", argv[1],argv[2],argv[3]);
		while(1)
		{
			/*le um prblema*/
			num_conv = le_um_prbs(&PRB,file_prbs);
			if(num_conv < 3)
				break;

			/*resolve-o consoante a variante*/
			switch (PRB.var[0])
			{
				case 'A':
					varianteA1(file_sol, G, num_vertices, num_arestas, PRB.var, PRB.vi, PRB.vj, &cost);
					break;
				case 'B':
					varianteB1(file_sol, G, PRB);
					break;
				case 'C':
					varianteC1(file_sol, G, PRB, &cost);
					break;
				case 'D':
					varianteD1(file_sol, G, PRB, &cost);
					break;
			}
		}
	}

/*-------- OPCAO 4 (-aa): Resolver todos os problemas do ficheiro problemas para o todos os mapas do ficheiro maps --------*/

	if(strcmp(argv[1],op4)== 0)
	{
		fprintf(file_sol,"%s %s %s\n\n", argv[1],argv[2],argv[3]);

		/*ciclo para percorrer todos os mapas*/
		while(1)
		{
			/*le e armazena o mapa*/
			num_arg_cabecalho = fscanf(file_maps, "%d %d", &num_vertices, &num_arestas);
			if(num_arg_cabecalho != 2)
				break;

			if(G != (graph*) NULL) {
				freeGraph(G);
			}

			G = create_graph(num_vertices, num_arestas);

			/*Ler e armazenar no grafo*/
			for(i=0; i < num_vertices; i++)
			{
				num_arg = fscanf(file_maps, "%d %s", &vert_lixo, tags);
				if(num_arg != 2) {
					return 0;
				}
				if(tags[0] == '-')
					continue;
				G->vector[i].tags = (char*) malloc(sizeof(char) * (strlen(tags)+1));
				strcpy(G->vector[i].tags, tags);
			}

			for(i=0; i < num_arestas; i++)
			{
				num_arg = fscanf(file_maps,"%d %d %lf", &src, &dest, &cost);
				if(num_arg != 3)
					return 0;
				add_edge(G, src, dest, cost);
			}
			/*ciclo para percorrer os problemas*/
			while(1)
			{
				/*le um prblema*/
				num_conv = le_um_prbs(&PRB,file_prbs);
				if(num_conv < 2) {
					fclose(file_prbs);
					if((file_prbs = fopen(argv[2], "r")) == 0)
						return 0;
					break;
				}

				/*resolve-o consoante a variante*/
				switch (PRB.var[0])
				{
					case 'A':
						varianteA1(file_sol, G, num_vertices, num_arestas, PRB.var, PRB.vi, PRB.vj, &cost);
						break;
					case 'B':
						varianteB1(file_sol, G, PRB);
						break;
					case 'C':
						varianteC1(file_sol, G, PRB, &cost);
						break;
					case 'D':
						varianteD1(file_sol, G, PRB, &cost);
						break;
				}
			}
		}
	}

/*------Libertacao memoria: graphs, files, extensions----------------------------------------------------------------------*/

	fclose(file_prbs);
	fclose(file_maps);
	fclose(file_sol);
	freeGraph(G);
	free(prbsExt);
	free(mapsExt);
	free(filename_out);

	return 0;
}
