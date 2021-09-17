/* Hello Transforms - c�digo inicial fornecido em https://learnopengl.com/#!Getting-started/Hello-Triangle 
 * 
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de F�sica para Jogos Digitais - Jogos Digitais - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 13/03/2019
 *
 */

using namespace std;

#include "SceneManager.h"


// The MAIN function, from here we start the application and run the game loop
int main()
{
	SceneManager *scene = new SceneManager;
	scene->initialize(800, 600);

	scene->run();

	scene->finish();
	
	return 0;
}


