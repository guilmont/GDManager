#include "gdeditor.h"


static std::string type2Label(GDM::Type type)
{
	switch (type)
	{
		case GDM::Type::GROUP:
			return "GROUP";
		case GDM::Type::INT32:
			return "INT32";
		case GDM::Type::UINT8:
			return "UINT8";
		case GDM::Type::UINT16:
			return "UINT16";
		case GDM::Type::UINT32:
			return "UINT32";
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
	
	else if (label.compare("UINT8") == 0)
		return GDM::Type::UINT8;
	
	else if (label.compare("UINT16") == 0)
		return GDM::Type::UINT16;
	
	else if (label.compare("UINT32") == 0)
		return GDM::Type::UINT32;
	
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

template<typename TP>
static void rewrite(const char *buf, uint64_t pos, uint8_t* ptr)
{
	TP val = static_cast<TP>(std::stod(buf));
	uint8_t* vv = reinterpret_cast<uint8_t*>(&val);
	std::copy(vv, vv + sizeof(TP), ptr + pos * sizeof(TP));
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


GDEditor::GDEditor(void)
{
	GRender::pout("Welcome to my GDEditor!!");
	GRender::pout("Current path:", std::filesystem::current_path());

	initialize("GDEditor", 1200, 800);

}

GDEditor::~GDEditor(void)
{
	if (arq)
		delete arq;
}

void GDEditor::onUserUpdate(float deltaTime)
{
	if (close_file)
	{
		delete arq;
		arq = nullptr;
		current = nullptr;
		addObj.view = false;
		addObj.group = nullptr;

		close_file = false;
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
	{
		if (arq == nullptr)
			arq = new GDM::File();
	}

	if (ctrl & O)
		dialog.createDialog(GDialog::OPEN, "Open file...", { "gdm", "gd" }, this, [](const std::string& path, void* ptr) -> void { reinterpret_cast<GDEditor*>(ptr)->open(path); });

	if (ctrl & S)
		dialog.createDialog(GDialog::SAVE, "Save file...", { "gdm", "gd" }, this, [](const std::string& path, void* ptr) -> void { reinterpret_cast<GDEditor*>(ptr)->save(path); });


}

void GDEditor::ImGuiLayer(void)
{
	if (view_imguidemo)
		ImGui::ShowDemoWindow(&view_imguidemo);

	if (view_implotdemo)
		ImPlot::ShowDemoWindow(&view_implotdemo);

	if (addObj.view)
		addObject(addObj.group);

	if (arq != nullptr)
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
		{
			if (arq == nullptr)
				arq = new GDM::File();
		}

		if (ImGui::MenuItem("Open...", "Ctrl+O"))
			dialog.createDialog(GDialog::OPEN, "Open file...", { "gdm", "gd" }, this, [](const std::string& path, void* ptr) -> void { reinterpret_cast<GDEditor*>(ptr)->open(path); });

		if (ImGui::MenuItem("Save...", "Ctrl+S"))
			dialog.createDialog(GDialog::SAVE, "Save file...", { "gdm", "gd" }, this, [](const std::string& path, void* ptr) -> void { reinterpret_cast<GDEditor*>(ptr)->save(path); });

		if (ImGui::MenuItem("Exit"))
			closeApp();

		ImGui::EndMenu();
	} // file-menu
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void GDEditor::recursiveTreeLoop(GDM::Group* group, ImGuiTreeNodeFlags nodeFlags)
{

	std::string remove = "";

	for (auto& [label, obj] : group->children())
	{
		ImGui::PushID(label.c_str());

		if (obj->getType() == GDM::Type::GROUP)
		{

			bool openTree = ImGui::TreeNodeEx(label.c_str(), nodeFlags);

			float fSize = ImGui::GetWindowContentRegionWidth() - 8.5f * ImGui::GetFontSize();
			
			ImGui::SameLine(fSize);
			if (ImGui::Button("Details", { 3.5f * ImGui::GetFontSize(), 0 }))
			{
				current = obj;
				view_data = false;
			}

			ImGui::SameLine();
			if (ImGui::Button("+", { 2.0f * ImGui::GetFontSize(), 0 }))
			{
				addObj.view = true;
				addObj.group = reinterpret_cast<GDM::Group *>(obj);
			}

			ImGui::SameLine();

			if (ImGui::Button("-", { 2.0f * ImGui::GetFontSize(), 0 }))
				remove = label;

			if (openTree)
			{
				recursiveTreeLoop(reinterpret_cast<GDM::Group*>(obj), nodeFlags);
				ImGui::TreePop();
			} // if-treenode
		}

		else
		{
			bool selected = false;
			if (ImGui::Selectable(label.c_str(), &selected))
			{
				current = obj;
				view_data = false;
			}

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
	ImGui::SetNextWindowPos({ workpos.x + 20 * DPI_FACTOR, workpos.y + 40 * DPI_FACTOR }, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize({ 400 * DPI_FACTOR, 700 * DPI_FACTOR }, ImGuiCond_FirstUseEver);

	ImGui::Begin("Tree view");

	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_None;
	nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
	nodeFlags |= ImGuiTreeNodeFlags_Framed;
	nodeFlags |= ImGuiTreeNodeFlags_FramePadding;
	nodeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth;
	nodeFlags |= ImGuiTreeNodeFlags_AllowItemOverlap;

	ImGui::PushID(arq->getLabel().c_str());

	std::string name = arq->getFilePath().filename().string();
	bool openTree = ImGui::TreeNodeEx(name.c_str(), nodeFlags);

	float fSize = ImGui::GetWindowContentRegionWidth() - 10.0f*ImGui::GetFontSize();
	
	ImGui::SameLine(fSize);
	if (ImGui::Button("+", { 2.0f * ImGui::GetFontSize(), 0 }))
	{
		addObj.view = true;
		addObj.group = arq;
	}

	ImGui::SameLine();
	if (ImGui::Button("Details", { 3.5f * ImGui::GetFontSize(), 0 }))
	{
		current = arq;
		view_data = false;
	}

	ImGui::SameLine();
	if (ImGui::Button("Close", { 3.5f * ImGui::GetFontSize(), 0 }))
		close_file = true;

	if (openTree)
	{
		recursiveTreeLoop(reinterpret_cast<GDM::Group*>(arq), nodeFlags);
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
	ImGui::SetNextWindowPos({ workpos.x + 450 * DPI_FACTOR, workpos.y + 40 * DPI_FACTOR }, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize({ 700 * DPI_FACTOR, 700 * DPI_FACTOR }, ImGuiCond_FirstUseEver);

	ImGui::Begin("Details");

	if (current == nullptr)
	{
		ImGui::Text("No object selected");
		ImGui::End();

		return;
	}


	auto text = [&](const std::string& title, const std::string& txt) -> void {
		fonts.text(title.c_str(), "bold");
		ImGui::SameLine();
		ImGui::Text(txt.c_str()); 
	};

	fonts.text("Label:", "bold");
	ImGui::SameLine();
	
	static char locLabel[GDM::MAX_LABEL_SIZE] = {0x00};
	sprintf(locLabel, "%s", current->getLabel().c_str());
	if (ImGui::InputText("##label", locLabel, GDM::MAX_LABEL_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
		current->rename(locLabel);

	if (current->parent)
		text("Parent:", current->parent->getLabel().c_str());
	
	text("Type:", type2Label(current->getType()).c_str());

	if (current->getType() == GDM::Type::GROUP)
		text("Number of children:", std::to_string(reinterpret_cast<GDM::Group*>(current)->getNumChildren()));
	else
	{
		GDM::Data* dt = reinterpret_cast<GDM::Data*>(current);
		GDM::Shape shape = dt->getShape();
		text("Shape:", "{ " + std::to_string(shape.height) + ", " + std::to_string(shape.width) + " }");

	}

	ImGui::Spacing();
	if (ImGui::Button("Delete"))
	{
		GDM::Group* ptr = current->parent;
		ptr->remove(current->getLabel());
		current = ptr;

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
	GDM::Description &description = current->descriptions();

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
		if (ImGui::BeginTable("descriptionTable", 3, flags, { 0, std::min<float>(256 * DPI_FACTOR, 1.5f * (description.size() + 1) * ImGui::GetFontSize()) }))
		{
			// Header
			ImGui::TableSetupColumn("Label");
			ImGui::TableSetupColumn("Description");
			ImGui::TableHeadersRow();

			// Main body
			std::string remove = "", add = "";
			for (auto& [label, desc] : description)
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
				static char loc2[512] = { 0x00 };
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


	if (current->getType() == GDM::Type::GROUP)
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

	fonts.text("Value:", "bold");
	ImGui::SameLine();
	if (ImGui::Button("View"))
		view_data = !view_data;

	if (!view_data)
	{
		ImGui::End();
		return;
	}

	GDM::Data* dt = reinterpret_cast<GDM::Data*>(current);
	GDM::Shape shape = dt->getShape();
	GDM::Type type = dt->getType();
	uint8_t* ptr = dt->getRawBuffer(); // This is the raw buffer pointer

	ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | ImGuiTableFlags_Borders;

	if (ImGui::BeginTable("dataTable", shape.width + 1, flags, { 0, std::min<float>(256 * DPI_FACTOR, 1.5f * (shape.height + 1) * ImGui::GetFontSize()) }))
	{
		ImGui::TableNextRow();

		for (uint32_t column = 1; column <= shape.width; column++)
		{
			ImGui::TableSetColumnIndex(column);
			fonts.text(std::to_string(column - 1).c_str(), "bold");
		}

		// Main body
		uint64_t ct = 0;
		for (uint32_t row = 0; row < shape.height; row++)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			fonts.text(std::to_string(row).c_str(), "bold");

			for (uint32_t column = 0; column < shape.width; column++)
			{
				ImGui::TableSetColumnIndex(column+1);

				char buf[64] = { 0x00 };

				switch (type)
				{
				case GDM::Type::INT32:
					sprintf(buf, "%d", reinterpret_cast<int32_t*>(ptr)[ct]);
					break;
				case GDM::Type::UINT8:
					sprintf(buf, "%d", reinterpret_cast<uint8_t*>(ptr)[ct]);
					break;
				case GDM::Type::UINT16:
					sprintf(buf, "%d", reinterpret_cast<uint16_t*>(ptr)[ct]);
					break;
				case GDM::Type::UINT32:
					sprintf(buf, "%d", reinterpret_cast<uint32_t*>(ptr)[ct]);
					break;
				case GDM::Type::FLOAT:
					sprintf(buf, "%.6f", reinterpret_cast<float*>(ptr)[ct]);
					break;
				case GDM::Type::DOUBLE:
					sprintf(buf, "%lf", reinterpret_cast<double*>(ptr)[ct]);
					break;
				}
			

				ImGui::PushID(std::to_string(row*shape.width + column).c_str());
				ImGui::SetNextItemWidth(5.0f * ImGui::GetFontSize());

				if (ImGui::InputText("##decimal", buf, 64, ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
				{
					switch (type)
					{
					case GDM::Type::INT32:
						rewrite<int32_t>(buf, ct, ptr);
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

void GDEditor::addObject(GDM::Group* group)
{
	ImGui::Begin("Add object", &addObj.view);

	fonts.text("Label:", "bold");
	ImGui::SameLine();

	bool check = false;

	static char buf[GDM::MAX_LABEL_SIZE] = { 0x00 };
	if (ImGui::InputText("##addLabel", buf, GDM::MAX_LABEL_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
		check = true;


	const char* items[] = { "GROUP", "INT32", "UINT32", "UINT8", "UINT16", "FLOAT", "DOUBLE"};
	static int item_current_idx = 0; // Here we store our selection data as an index.
	const char* combo_label = items[item_current_idx];  // Label to preview before opening the combo (technically it could be anything)

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
	static int32_t dim[2] = { 1,1 };

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
			GDM::Shape shape = { static_cast<uint32_t>(dim[0]), static_cast<uint32_t>(dim[1]) };

			switch (type)
			{
			case GDM::Type::INT32:
				addObj.group->add<int32_t>(buf, nullptr, shape);
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

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void GDEditor::open(const fs::path& inPath)
{
	if (arq == nullptr)
		arq = new GDM::File(inPath);

	else
		GRender::pout("WARNING: One needs to close file before openning another!");
}

void GDEditor::save(const fs::path& outPath)
{
	if (outPath.compare(arq->getFilePath()) == 0)
		GRender::pout("ERROR: Cannot save files under same name!!!");

	else
		arq->save(outPath);
}





