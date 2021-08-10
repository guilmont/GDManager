#include "GDManager.h"
#include "GRender.h"

class GDEditor : public GRender::Application
{
public:
	GDEditor(void);
	~GDEditor(void);

	void onUserUpdate(float deltaTime) override;
	void ImGuiLayer(void) override;
	void ImGuiMenuLayer(void) override;

	void openFile(const fs::path& inPath); // So we can open files from the command line
	

private:
	void saveFile(void);


private:
	void recursiveTreeLoop(GDM::Group *group, ImGuiTreeNodeFlags nodeFlags);
	void treeViewWindow(void);

	void detailWindow(void);

	void plotHeatmap(void);
	void plotLines(void);

	void releaseMemory(GDM::Group* group);

	void addObject(GDM::Group *group);


private:
	bool
		view_imguidemo = false,
		view_implotdemo = false;

		GDM::Data* plotPointer = nullptr;
		void (GDEditor::*plotWindow)(void) = nullptr;

private:
	std::string	close_file = "";

	struct
	{
		GDM::Group *group = nullptr;
		bool view = false;
	} addObj;

	GDM::Object* currentObj = nullptr;
	GDM::File* currentFile = nullptr;

	std::map<fs::path, GDM::File> vFile;
};
