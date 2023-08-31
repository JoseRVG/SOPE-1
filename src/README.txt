Grupo: T4G05

Elementos: Gonçalo da Mota Laranjeira Torres Leão (up201406036@fe.up.pt)
		   José Ricardo Vieira Gomes (up201308242@fe.up.pt)
		   
Módulos:
	compfiles: compara dois ficheiros byte a byte para determinar se têm o mesmo conteúdo (semelhante ao comando diff)
	listrf: lista, recursivamente, todos "rfents" de todos os sub-diretórios no ficheiro binário "files"
	lsdir: programa que lista todos os "rfents" do diretório que recebe como argumento em "files"
	readrf: gera o ficheiro de texto "files.txt" a partir de "files"
	rfent: contém a estrutura de dados usada no programa para guardar toda a informação necessária sobre um ficheiro regular
	rmdup: programa principal
	sizelim: define certos limites do programa que devem ser respeitados (ver mais abaixo)
	sortrf: ordena as "rfents" em "files" por nome, permissões, tamanho e última data de modificação
	
Notas adicionais:
	- os programas rmdup e lsdir devem ser chamados da pasta onde está o código fonte (e o Makefile)
	- os três ficheiros gerados pelo programa são: files, files.txt e hlinks.txt. 
	Os ficheiros são guardados no mesmo diretório que o código fonte.
	- para evitar ter de processar linhas inteiras do ficheiro files.txt, optou-se por
	guardar a informação obtida por lsdir num ficheiro binário para facilitar a sua
	posterior consulta. Em troca, foi necessário criar o módulo sortrf, em vez de se
	usar o utilitário "sort" (mas acreditamos que foi uma boa troca :) ).
	- relativamente aos limites do programa (definidos em sizelim.h):
		o nome de um ficheiro não pode ultrapassar 128 chars (incluindo \0);
		o nome completo (nome do ficheiro + path) ão pode ultrapassar 256 chars (incluindo \0);
		o número total de ficheiros regulares (incluindo os dos sub-directórios) não deve ultrapassar os 1000.
	
	
	