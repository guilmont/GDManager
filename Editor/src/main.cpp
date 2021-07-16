#include "gdeditor.h"

int main(int argc, char *argv[])
{
	GDEditor* editor = new GDEditor();

	if(argc > 1)
		editor->open(argv[1]);

	editor->run();

	delete editor;

	return EXIT_SUCCESS;
}