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

	void open(const fs::path &inPath);
	void save(const fs::path &outPath);


private:
	void recursiveTreeLoop(GDM::Group *group, ImGuiTreeNodeFlags nodeFlags);
	void treeViewWindow(void);

	void detailWindow(void);

	void addObject(GDM::Group *group);


private:
	bool
		view_imguidemo = false,
		view_implotdemo = false,
		view_data = false,
		close_file = false;

	struct
	{
		GDM::Group *group = nullptr;
		bool view = false;
	} addObj;

	GDM::Object *current = nullptr;
	GDM::File *arq = nullptr;
};
