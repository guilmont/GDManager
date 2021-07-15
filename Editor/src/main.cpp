#include "gdeditor.h"

int main()
{
	GDEditor* editor = new GDEditor();
	editor->run();

	delete editor;

	return EXIT_SUCCESS;
}