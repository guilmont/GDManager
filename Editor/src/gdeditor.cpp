#include "gdeditor.h"

static std::string type2Label(GDM::Type type)
{
	switch (type)
	{
	case GDM::Type::GROUP:
		return "GROUP";
	case GDM::Type::INT32:
		return "INT32";
	case GDM::Type::INT64:
		return "INT64";
	case GDM::Type::UINT8:
		return "UINT8";
	case GDM::Type::UINT16:
		return "UINT16";
	case GDM::Type::UINT32:
		return "UINT32";
	case GDM::Type::UINT64:
		return "UINT64";
	case GDM::Type::FLOAT:
		return "FLOAT";
	case GDM::Type::DOUBLE:
		return "DOUBLE";
	default:
		GRender::pout("Type unkown");
		assert(false);
		return "";
	}
}

static GDM::Type label2Type(const std::string &label)
{
	if (label.compare("GROUP") == 0)
		return GDM::Type::GROUP;

	else if (label.compare("INT32") == 0)
		return GDM::Type::INT32;

	else if (label.compare("INT64") == 0)
		return GDM::Type::INT64;

	else if (label.compare("UINT8") == 0)
		return GDM::Type::UINT8;

	else if (label.compare("UINT16") == 0)
		return GDM::Type::UINT16;

	else if (label.compare("UINT32") == 0)
		return GDM::Type::UINT32;

	else if (label.compare("UINT64") == 0)
		return GDM::Type::UINT64;

	else if (label.compare("FLOAT") == 0)
		return GDM::Type::FLOAT;

	else if (label.compare("DOUBLE") == 0)
		return GDM::Type::DOUBLE;

	else
	{
		GRender::pout("Type unkown");
		assert(false);
		return GDM::Type::NONE;
	}
}

template <typename TP>
static void rewrite(const char *buf, uint64_t pos, uint8_t *ptr)
{
	TP val = static_cast<TP>(std::stod(buf));
	uint8_t *vv = reinterpret_cast<uint8_t *>(&val);
	std::copy(vv, vv + sizeof(TP), ptr + pos * sizeof(TP));
}

template <typename TP>
static void lineFunction(GDM::Data* data, int selected, uint32_t id)
{
	GDM::Shape sp = data->getShape();
	const TP* ptr = data->getArray<TP>();
	uint32_t N = selected == 0 ? sp.width : sp.height;

	std::vector<TP> vecX(N), vecY(N);

	for (uint32_t k = 0; k < N; k++)
	{
		vecX[k] = TP(k);
		vecY[k] = selected == 0 ? ptr[id * sp.width + k] : ptr[k * sp.width + id];
	}

	ImPlot::PlotLine("function",vecX.data(), vecY.data(), N);


}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

GDEditor::GDEditor(void)
{
	fs::current_path(INSTALL_PATH);

	GRender::pout("Welcome to my GDEditor!!");
	initialize("GDEditor", 1200, 800, "assets/GDEditor/layout.ini");
}

GDEditor::~GDEditor(void) {}

void GDEditor::onUserUpdate(float deltaTime)
{
	if (close_file.size() > 0)
	{
		vFile.erase(close_file);
		close_file = "";

		if (vFile.size() > 0)
			currentFile = &(vFile.begin()->second);
		else
			currentFile = nullptr;

		currentObj = nullptr;
		addObj.view = false;
		addObj.group = nullptr;
	}

	bool
		ctrl = (keyboard[GKey::LEFT_CONTROL] == GEvent::PRESS) || (keyboard[GKey::RIGHT_CONTROL] == GEvent::PRESS),
		shift = (keyboard[GKey::LEFT_SHIFT] == GEvent::PRESS) || (keyboard[GKey::RIGHT_SHIFT] == GEvent::PRESS),
		I = keyboard['I'] == GEvent::PRESS,
		P = keyboard['P'] == GEvent::PRESS,
		N = keyboard['N'] == GEvent::PRESS,
		O = keyboard['O'] == GEvent::PRESS,
		S = keyboard['S'] == GEvent::PRESS;

	if ((ctrl & shift) & I)
		view_imguidemo = true;

	if ((ctrl & shift) & P)
		view_implotdemo = true;

	if (ctrl & N)
		dialog.createDialog(GDialog::SAVE, "New file...", {"gdm", "gd"}, this, [](const std::string &path, void *ptr) -> void
							{ reinterpret_cast<GDEditor *>(ptr)->openFile(path); });

	if (ctrl & O)
		dialog.createDialog(GDialog::OPEN, "Open file...", {"gdm", "gd"}, this, [](const std::string &path, void *ptr) -> void
							{ reinterpret_cast<GDEditor *>(ptr)->openFile(path); });

}

void GDEditor::ImGuiLayer(void)
{
	if (view_imguidemo)
		ImGui::ShowDemoWindow(&view_imguidemo);

	if (view_implotdemo)
		ImPlot::ShowDemoWindow(&view_implotdemo);

	if (plotPointer)
		(this->*plotWindow)();

	if (addObj.view)
		addObject(addObj.group);

	if (currentFile)
	{
		treeViewWindow();
		detailWindow();
	}
}

void GDEditor::ImGuiMenuLayer(void)
{

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New file...", "Ctrl+N"))
			dialog.createDialog(GDialog::SAVE, "New file...", {"gdm", "gd"}, this, [](const std::string &path, void *ptr) -> void
								{ reinterpret_cast<GDEditor *>(ptr)->openFile(path); });

		if (ImGui::MenuItem("Open...", "Ctrl+O"))
			dialog.createDialog(GDialog::OPEN, "Open file...", {"gdm", "gd"}, this, [](const std::string &path, void *ptr) -> void
								{ reinterpret_cast<GDEditor *>(ptr)->openFile(path); });

		if (ImGui::MenuItem("Save"))
			saveFile();

		if (ImGui::MenuItem("Exit"))
			closeApp();

		ImGui::EndMenu();
	} 

	if (ImGui::BeginMenu("Tools"))
	{
		if (ImGui::MenuItem("Show mailbox"))
			mailbox.setActive();

		if (ImGui::MenuItem("Release memory"))
		{
			for (auto &[name, arq] : vFile)
				releaseMemory(&arq);
		}

		ImGui::EndMenu();
	} // file-menu
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void GDEditor::recursiveTreeLoop(GDM::Group *group, ImGuiTreeNodeFlags nodeFlags)
{

	std::string remove = "";

	for (auto &[label, obj] : group->children())
	{
		ImGui::PushID(label.c_str());

		if (obj->getType() == GDM::Type::GROUP)
		{

			bool openTree = ImGui::TreeNodeEx(label.c_str(), nodeFlags);

			float fSize = ImGui::GetWindowContentRegionWidth() - 8.5f * ImGui::GetFontSize();

			ImGui::SameLine(fSize);
			if (ImGui::Button("Details", {3.5f * ImGui::GetFontSize(), 0}))
				currentObj = obj;

			ImGui::SameLine();
			if (ImGui::Button("+", {2.0f * ImGui::GetFontSize(), 0}))
			{
				addObj.view = true;
				addObj.group = reinterpret_cast<GDM::Group *>(obj);
			}

			ImGui::SameLine();

			if (ImGui::Button("-", {2.0f * ImGui::GetFontSize(), 0}))
				remove = label;

			if (openTree)
			{
				recursiveTreeLoop(reinterpret_cast<GDM::Group *>(obj), nodeFlags);
				ImGui::TreePop();
			} // if-treenode
		}

		else
		{
			bool selected = false;
			if (ImGui::Selectable(label.c_str(), &selected))
				currentObj = obj;
		}

		ImGui::PopID();
	} // loop-children

	// Removing group if necessary
	if (remove.size() > 0)
		group->remove(remove);
}

void GDEditor::treeViewWindow(void)
{
	const ImVec2 workpos = ImGui::GetMainViewport()->WorkPos;
	ImGui::SetNextWindowPos({workpos.x + 20, workpos.y + 40}, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize({400, 700}, ImGuiCond_FirstUseEver);

	ImGui::Begin("Tree view");

	if (ImGui::BeginTabBar("MyTabBar"))
	{
		for (auto &[label, arq] : vFile)
		{
			const std::string &name = label.filename().string();
			if (ImGui::BeginTabItem(name.c_str()))
			{
				currentFile = &arq;
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}

	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_None;
	nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
	nodeFlags |= ImGuiTreeNodeFlags_Framed;
	nodeFlags |= ImGuiTreeNodeFlags_FramePadding;
	nodeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth;
	nodeFlags |= ImGuiTreeNodeFlags_AllowItemOverlap;

	ImGui::PushID(currentFile->getLabel().c_str());

	std::string name = currentFile->getFilePath().filename().string();
	bool openTree = ImGui::TreeNodeEx(name.c_str(), nodeFlags);

	float fSize = ImGui::GetWindowContentRegionWidth() - 10.0f * ImGui::GetFontSize();

	ImGui::SameLine(fSize);
	if (ImGui::Button("+", {2.0f * ImGui::GetFontSize(), 0}))
	{
		addObj.view = true;
		addObj.group = currentFile;
	}

	ImGui::SameLine();
	if (ImGui::Button("Details", {3.5f * ImGui::GetFontSize(), 0}))
		currentObj = currentFile;

	ImGui::SameLine();
	if (ImGui::Button("Close", { 3.5f * ImGui::GetFontSize(), 0 }))
	{
		plotPointer = nullptr;
		plotWindow = nullptr;
		close_file = currentFile->getFilePath().string();
		mailbox.createInfo("Closing file " + close_file);
	}

	if (openTree)
	{
		recursiveTreeLoop(reinterpret_cast<GDM::Group *>(currentFile), nodeFlags);
		ImGui::TreePop();
	}

	ImGui::PopID();

	ImGui::End();
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void GDEditor::detailWindow(void)
{
	const ImVec2 workpos = ImGui::GetMainViewport()->WorkPos;
	ImGui::SetNextWindowPos({workpos.x + 450, workpos.y + 40}, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize({700, 700}, ImGuiCond_FirstUseEver);

	ImGui::Begin("Details");

	if (currentObj == nullptr)
	{
		ImGui::Text("No object selected");
		ImGui::End();

		return;
	}

	auto text = [&](const std::string &title, const std::string &txt) -> void
	{
		fonts.text(title.c_str(), "bold");
		ImGui::SameLine();
		ImGui::Text(txt.c_str());
	};

	fonts.text("Label:", "bold");
	ImGui::SameLine();

	static char locLabel[GDM::MAX_LABEL_SIZE] = {0x00};
	sprintf(locLabel, "%s", currentObj->getLabel().c_str());
	if (ImGui::InputText("##label", locLabel, GDM::MAX_LABEL_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
		currentObj->rename(locLabel);

	if (currentObj->parent)
	{
		std::string par = "";
		GDM::Object *obj = currentObj;
		while (obj->parent)
		{
			obj = obj->parent;
			par = "/" + obj->getLabel() + par;
		}

		text("Path:", par.c_str());
	}

	text("Type:", type2Label(currentObj->getType()).c_str());

	if (currentObj->getType() == GDM::Type::GROUP)
		text("Number of children:", std::to_string(reinterpret_cast<GDM::Group *>(currentObj)->getNumChildren()));
	else
	{
		GDM::Data *dt = reinterpret_cast<GDM::Data *>(currentObj);
		GDM::Shape shape = dt->getShape();
		text("Shape:", "{ " + std::to_string(shape.height) + ", " + std::to_string(shape.width) + " }");
	}

	ImGui::Spacing();
	if (ImGui::Button("Delete"))
	{
		GDM::Group *ptr = currentObj->parent;
		ptr->remove(currentObj->getLabel());
		currentObj = ptr;

		ImGui::End();
		return;
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	//////////////////////////////////////////////////////////
	// All types have descriptions
	GDM::Description &description = currentObj->descriptions();

	fonts.text("Description:", "bold");
	ImGui::SameLine();

	if (ImGui::Button("Add"))
	{
		description["---"] = "---";
	}

	if (description.size() > 0)
	{
		// Creatign table to display description
		ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | ImGuiTableFlags_Borders;
		if (ImGui::BeginTable("descriptionTable", 3, flags, {0, std::min<float>(256, 1.5f * (description.size() + 1) * ImGui::GetFontSize())}))
		{
			// Header
			ImGui::TableSetupColumn("Label");
			ImGui::TableSetupColumn("Description");
			ImGui::TableHeadersRow();

			// Main body
			std::string remove = "", add = "";
			for (auto &[label, desc] : description)
			{
				ImGui::TableNextRow();
				ImGui::PushID(label.c_str());

				ImGui::TableSetColumnIndex(0);
				static char loc1[GDM::MAX_LABEL_SIZE];
				sprintf(loc1, "%s", label.c_str());

				ImGui::SetNextItemWidth(10.0f * ImGui::GetFontSize());
				if (ImGui::InputText("##label", loc1, GDM::MAX_LABEL_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					add = std::string(loc1);
					remove = label;
				}

				ImGui::TableSetColumnIndex(1);
				static char loc2[512] = {0x00};
				sprintf(loc2, "%s", desc.c_str());

				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 18.0f * ImGui::GetFontSize());
				if (ImGui::InputText("##desc", loc2, 512, ImGuiInputTextFlags_EnterReturnsTrue))
					description[label] = std::string(loc2);

				ImGui::TableSetColumnIndex(2);
				if (ImGui::Button("Remove"))
					remove = label;

				ImGui::PopID();
			}

			// Removing description if necessary
			if (add.size() > 0)
				description[add] = description[remove];

			if (remove.size() > 0)
				description.erase(remove);

			ImGui::EndTable();
		}
	}

	if (currentObj->getType() == GDM::Type::GROUP)
	{
		ImGui::End();
		return;
	}

	//////////////////////////////////////////////////////////
	// We have a data type, so we should display its values if demanded

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	GDM::Data *dt = reinterpret_cast<GDM::Data *>(currentObj);

	fonts.text("Value:", "bold");

	if (dt->getSizeBytes() > sizeof(uint64_t))
	{
		ImGui::SameLine();
		if (ImGui::Button(dt->isLoaded() ? "Hide" : "View"))
		{
			if (dt->isLoaded())
				dt->release();

			else
				dt->load(); // this will load data
		}
	}

	if (dt->isLoaded())
	{
		if (dt->getSizeBytes() > sizeof(uint64_t))
		{
			ImGui::SameLine();
			if (ImGui::Button("Heatmap"))
			{
				plotPointer = dt;
				plotWindow = &GDEditor::plotHeatmap;
			}

			ImGui::SameLine();
			if (ImGui::Button("Line plot"))
			{
				plotPointer = dt;
				plotWindow = &GDEditor::plotLines;
			}
			
		}
	}

	else
	{
		ImGui::End();
		return;
	}

	GDM::Shape shape = dt->getShape();
	GDM::Type type = dt->getType();
	uint8_t *ptr = dt->getRawBuffer(); // This is the raw buffer pointer

	uint32_t maxRows = std::min<uint32_t>(32, shape.height);
	uint32_t maxCols = std::min<uint32_t>(32, shape.width);

	static uint32_t rowZero = 0, rowTop = maxRows;
	static uint32_t colZero = 0, colTop = maxCols;

	if (maxRows < shape.height)
	{
		int val = static_cast<int32_t>(rowZero);
		ImGui::SliderInt("Rows", &val, 0, shape.height - 32);

		rowZero = std::min(static_cast<uint32_t>(val), shape.height);
		rowTop = std::min(rowZero + 32, shape.height);
	}
	else
	{
		rowZero = 0;
		rowTop = maxRows;
	}

	if (maxCols < shape.width)
	{
		int val = static_cast<int32_t>(colZero);
		ImGui::SliderInt("Cols", &val, 0, shape.width - 32);

		colZero = std::min(static_cast<uint32_t>(val), shape.width);
		colTop = std::min(colZero + 32, shape.width);
	}
	else
	{
		colZero = 0;
		colTop = maxCols;
	}

	ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | ImGuiTableFlags_Borders | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_NoHostExtendY;
	if (ImGui::BeginTable("dataTable", maxCols + 1, flags))
	{
		ImGui::TableNextRow();

		for (uint32_t column = 1; column <= maxCols; column++)
		{
			ImGui::TableSetColumnIndex(column);
			fonts.text(std::to_string(colZero + column - 1).c_str(), "bold");
		}

		// Main body
		for (uint32_t row = rowZero; row < rowTop; row++)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			fonts.text(std::to_string(row).c_str(), "bold");

			for (uint32_t column = colZero; column < colTop; column++)
			{
				uint32_t ct = row * shape.width + column;

				ImGui::TableSetColumnIndex(column + 1 - colZero);

				char buf[64] = {0x00};

				switch (type)
				{
				case GDM::Type::INT32:
					sprintf(buf, "%d", reinterpret_cast<int32_t *>(ptr)[ct]);
					break;
				case GDM::Type::INT64:
					sprintf(buf, "%jd", reinterpret_cast<int64_t *>(ptr)[ct]);
					break;
				case GDM::Type::UINT8:
					sprintf(buf, "%u", reinterpret_cast<uint8_t *>(ptr)[ct]);
					break;
				case GDM::Type::UINT16:
					sprintf(buf, "%u", reinterpret_cast<uint16_t *>(ptr)[ct]);
					break;
				case GDM::Type::UINT32:
					sprintf(buf, "%u", reinterpret_cast<uint32_t *>(ptr)[ct]);
					break;
				case GDM::Type::UINT64:
					sprintf(buf, "%ju", reinterpret_cast<uint64_t *>(ptr)[ct]);
					break;
				case GDM::Type::FLOAT:
					sprintf(buf, "%.6f", reinterpret_cast<float *>(ptr)[ct]);
					break;
				case GDM::Type::DOUBLE:
					sprintf(buf, "%.6lf", reinterpret_cast<double *>(ptr)[ct]);
					break;
				}

				ImGui::PushID(std::to_string(row * shape.width + column).c_str());
				ImGui::SetNextItemWidth(5.0f * ImGui::GetFontSize());

				if (ImGui::InputText("##decimal", buf, 64, ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
				{
					switch (type)
					{
					case GDM::Type::INT32:
						rewrite<int32_t>(buf, ct, ptr);
						break;
					case GDM::Type::INT64:
						rewrite<int64_t>(buf, ct, ptr);
						break;
					case GDM::Type::UINT8:
						rewrite<uint8_t>(buf, ct, ptr);
						break;
					case GDM::Type::UINT16:
						rewrite<uint16_t>(buf, ct, ptr);
						break;
					case GDM::Type::UINT32:
						rewrite<uint32_t>(buf, ct, ptr);
						break;
					case GDM::Type::UINT64:
						rewrite<uint64_t>(buf, ct, ptr);
						break;
					case GDM::Type::FLOAT:
						rewrite<float>(buf, ct, ptr);
						break;
					case GDM::Type::DOUBLE:
						rewrite<double>(buf, ct, ptr);
						break;
					}
				}

				ImGui::PopID();

				// Seeking next element in array
				ct++;
			}
		}

		ImGui::EndTable();
	}

	ImGui::End();
}

void GDEditor::plotHeatmap(void)
{

	const ImVec2 workpos = ImGui::GetMainViewport()->WorkPos;
	ImGui::SetNextWindowPos({ workpos.x + 40, workpos.y + 40}, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize({ 700, 700 }, ImGuiCond_FirstUseEver);

	bool is_open = true; // So we can close this window from this function
	ImGui::Begin("Plots", &is_open);

	static float scale_min = 0;
	static float scale_max = 1.0f;

	static ImPlotColormap map = ImPlotColormap_Viridis;
	if (ImPlot::ColormapButton(ImPlot::GetColormapName(map), ImVec2(225, 0), map)) {
		map = (map + 1) % ImPlot::GetColormapCount();
		ImPlot::BustColorCache("##Heatmap1");
	}

	ImGui::SameLine();
	ImGui::LabelText("##Colormap Index", "%s", "Change Colormap");
	ImGui::SetNextItemWidth(225);
	ImGui::DragFloatRange2("Min / Max", &scale_min, &scale_max, 0.01f, -20, 20);
	
	GDM::Shape sp = plotPointer->getShape();
	const uint8_t* ptr = plotPointer->getRawBuffer();

	float ratio = float(sp.height) / float(sp.width);
	float width = 0.8f * ImGui::GetContentRegionAvailWidth(),
		height = width * ratio;

	ImPlot::PushColormap(map);
	if (ImPlot::BeginPlot("##Heatmap1", NULL, NULL, {width, height }, ImPlotFlags_NoLegend))
	{
		switch (plotPointer->getType())
		{
		case GDM::Type::INT32:
			ImPlot::PlotHeatmap("heat", reinterpret_cast<const ImS32*>(ptr), sp.height, sp.width, scale_min, scale_max, NULL);
			break;

		case GDM::Type::INT64:
			ImPlot::PlotHeatmap("heat", reinterpret_cast<const ImS64*>(ptr), sp.height, sp.width, scale_min, scale_max, NULL);
			break;

		case GDM::Type::UINT8:
			ImPlot::PlotHeatmap("heat", reinterpret_cast<const ImU8*>(ptr), sp.height, sp.width, scale_min, scale_max, NULL);
			break;

		case GDM::Type::UINT16:
			ImPlot::PlotHeatmap("heat", reinterpret_cast<const ImU16*>(ptr), sp.height, sp.width, scale_min, scale_max, NULL);
			break;

		case GDM::Type::UINT32:
			ImPlot::PlotHeatmap("heat", reinterpret_cast<const ImU32*>(ptr), sp.height, sp.width, scale_min, scale_max, NULL);
			break;

		case GDM::Type::UINT64:
			ImPlot::PlotHeatmap("heat", reinterpret_cast<const ImU64*>(ptr), sp.height, sp.width, scale_min, scale_max, NULL);
			break;

		case GDM::Type::FLOAT:
			ImPlot::PlotHeatmap("heat", reinterpret_cast<const float*>(ptr), sp.height, sp.width, scale_min, scale_max, NULL);
			break;

		case GDM::Type::DOUBLE:
			ImPlot::PlotHeatmap("heat", reinterpret_cast<const double*>(ptr), sp.height, sp.width, scale_min, scale_max, NULL);
			break;

		default:
			throw "Type not recognized!!";
			break;
		}

		ImPlot::EndPlot();
	}
	ImGui::SameLine();
	ImPlot::ColormapScale("##HeatScale", scale_min, scale_max, { 0.15f * width, height });

	ImGui::End();

	if (!is_open)
		plotPointer = nullptr;
}

void GDEditor::plotLines(void)
{

	const ImVec2 workpos = ImGui::GetMainViewport()->WorkPos;
	ImGui::SetNextWindowPos({ workpos.x + 40, workpos.y + 40 }, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize({ 700, 700 }, ImGuiCond_FirstUseEver);

	GDM::Shape sp = plotPointer->getShape();

	bool is_open = true; // So we can close this window from this function
	ImGui::Begin("Plots", &is_open);


	static int selected = 0;
	ImGui::RadioButton("Rows", &selected, 0); ImGui::SameLine();
	ImGui::RadioButton("Columns", &selected, 1); 

	static int id = 0;
	ImGui::DragInt("ID", &id, 1.0f, 0, selected == 0 ? sp.height-1 : sp.width-1);
	
	const char* labx = "Index";
		char laby[64] = { 0 };
	sprintf(laby, "%s %d", (selected == 0 ? "Row" : "Column"), id);
	

	float width = 0.95f * ImGui::GetContentRegionAvailWidth(),
		height = 0.7f * width;

	if (ImPlot::BeginPlot(plotPointer->getLabel().c_str(), labx, laby, { width, height }, ImPlotFlags_NoLegend)) {

		ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
		switch (plotPointer->getType())
		{
		case GDM::Type::INT32:
			lineFunction<ImS32>(plotPointer, selected, id);
			break;

		case GDM::Type::INT64:
			lineFunction<ImS64>(plotPointer, selected, id);
			break;
			
		case GDM::Type::UINT8:
			lineFunction<ImU8>(plotPointer, selected, id);
			break;
			
		case GDM::Type::UINT16:
			lineFunction<ImU16>(plotPointer, selected, id);
			break;
			
		case GDM::Type::UINT32:
			lineFunction<ImU32>(plotPointer, selected, id);
			break;
			
		case GDM::Type::UINT64:
			lineFunction<ImU64>(plotPointer, selected, id);
			break;
			
		case GDM::Type::FLOAT:
			lineFunction<float>(plotPointer, selected, id);
			break;
			
		case GDM::Type::DOUBLE:
			lineFunction<double>(plotPointer, selected, id);
			break;
			
		default:
			throw "Type not recognized!!";
			break;
		}
		
		ImPlot::EndPlot();
	}

	ImGui::PopID();
	ImGui::End();

	if (!is_open)
		plotPointer = nullptr;
}

void GDEditor::addObject(GDM::Group *group)
{
	ImGui::Begin("Add object", &addObj.view);

	fonts.text("Label:", "bold");
	ImGui::SameLine();

	bool check = false;

	static char buf[GDM::MAX_LABEL_SIZE] = {0x00};
	if (ImGui::InputText("##addLabel", buf, GDM::MAX_LABEL_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
		check = true;

	const char *items[] = {"GROUP", "INT32", "INT64", "UINT8", "UINT16", "UINT32", "UINT64", "FLOAT", "DOUBLE"};
	static int item_current_idx = 0;				   // Here we store our selection data as an index.
	const char *combo_label = items[item_current_idx]; // Label to preview before opening the combo (technically it could be anything)

	fonts.text("Type:", "bold");
	ImGui::SameLine();
	if (ImGui::BeginCombo("##type", combo_label))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(items[n], is_selected))
				item_current_idx = n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	GDM::Type type = label2Type(items[item_current_idx]);
	static int32_t dim[2] = {1, 1};

	if (type != GDM::Type::GROUP)
	{
		// Determinining
		fonts.text("Shape:", "bold");
		ImGui::SameLine();
		ImGui::DragInt2("##shape", dim, 0.1f, 1);
	}

	if (ImGui::Button("Add") || check)
	{
		if (type == GDM::Type::GROUP)
			addObj.group->addGroup(buf); // TODO: Check if label already exists

		else
		{
			GDM::Shape shape = {static_cast<uint32_t>(dim[0]), static_cast<uint32_t>(dim[1])};

			switch (type)
			{
			case GDM::Type::INT32:
				addObj.group->add<int32_t>(buf, nullptr, shape);
				break;
			case GDM::Type::INT64:
				addObj.group->add<int64_t>(buf, nullptr, shape);
				break;
			case GDM::Type::UINT8:
				addObj.group->add<uint8_t>(buf, nullptr, shape);
				break;
			case GDM::Type::UINT16:
				addObj.group->add<uint16_t>(buf, nullptr, shape);
				break;
			case GDM::Type::UINT32:
				addObj.group->add<uint32_t>(buf, nullptr, shape);
				break;
			case GDM::Type::UINT64:
				addObj.group->add<uint64_t>(buf, nullptr, shape);
				break;
			case GDM::Type::FLOAT:
				addObj.group->add<float>(buf, nullptr, shape);
				break;
			case GDM::Type::DOUBLE:
				addObj.group->add<double>(buf, nullptr, shape);
				break;
			}
		}

		addObj.view = false;
	}

	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
		addObj.view = false;

	ImGui::End();
}

void GDEditor::releaseMemory(GDM::Group* group)
{
	for (auto& [label, obj] : group->children())
	{
		if (obj->getType() == GDM::Type::GROUP)
		{
			releaseMemory(reinterpret_cast<GDM::Group*>(obj));
		}
		else
		{
			GDM::Data* ptr = reinterpret_cast<GDM::Data*>(obj);
			if (ptr->isLoaded())
				ptr->release();
		}

	} 
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void GDEditor::openFile(const fs::path &inPath)
{

	if (vFile.find(inPath) != vFile.end())
		mailbox.createWarn("File already openned!!");
	else
	{
		mailbox.createInfo("Openning file: " + inPath.string());
		vFile.emplace(inPath, inPath);
		currentFile = &vFile[inPath];
	}
}

void GDEditor::saveFile(void)
{ 
	if (currentFile)
	{
		mailbox.createInfo("Saving file to " + currentFile->getFilePath().string());
		currentFile->save();
	}
}
