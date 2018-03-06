#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"

#include "UILabel.h"
#include "UIImage.h"
#include "UIButton.h"

#include "UIElement.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.GetString());


	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	for (int i = 0; i < ui_elements.capacity(); i++)
		if (ui_elements.at(i) != nullptr) ui_elements[i]->Update();

	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{

	for (int i = 0; i < ui_elements.capacity(); i++)
		if (ui_elements.at(i) != nullptr) ui_elements[i]->Draw(atlas);

	for (int i = 0; i < ui_elements.capacity(); i++) {
		if (ui_elements[i]->to_destroy) {
			delete(ui_elements[i]);
			ui_elements[i] = nullptr;
			ui_elements.erase(ui_elements.cbegin() + i);
			ui_elements.shrink_to_fit();
		}
	}

	LOG("NUM ELEM: %i", ui_elements.size());

	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");
	//TODO unload tex

	for (uint i = 0; i < ui_elements.capacity(); ++i)
	{
		if (ui_elements[i] != nullptr)
		{
			delete ui_elements[i];
			ui_elements[i] = nullptr;
			ui_elements.erase(ui_elements.cbegin() + i);
			ui_elements.shrink_to_fit();
		}
	}

	ui_elements.clear();

	return true;
}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

bool j1Gui::DeleteUIElement(UIElement &element) {
	
	for (int i = 0; i < ui_elements.capacity(); i++) {
		if (ui_elements.at(i) == &element) {
			ui_elements[i]->to_destroy = true;
			return true;
		}
	}

	return false;
}

bool j1Gui::DeleteAllUIElements() {
	bool ret = false;

	for (int i = 0; i < ui_elements.capacity(); i++) {
		if (ui_elements.at(i) != nullptr) ui_elements[i]->to_destroy = true;
		ret = true;
	}

	return ret;
}

UIElement* j1Gui::AddUIImage(int position_x, int position_y, SDL_Rect rect, j1Module* callback, UIElement* parent) {

	UIElement* tmp_img = new UIImage(position_x, position_y, IMAGE, rect, callback, parent);
	ui_elements.push_back(tmp_img);
	return tmp_img;

	LOG("Error: Cant add the UIImage");
	return nullptr;
}

UIElement* j1Gui::AddUIButton(int position_x, int position_y, SDL_Rect normal_rect, SDL_Rect focused_rect, SDL_Rect pressed_rect, j1Module* callback, UIElement* parent) {

	UIElement* tmpBtn = new UIButton(position_x, position_y, BUTTON, normal_rect,focused_rect,pressed_rect, callback, parent);
	ui_elements.push_back(tmpBtn);
	return tmpBtn;

	LOG("Error: Cant add the UIButton");
	return nullptr;
}

UIElement* j1Gui::AddUILabel(int position_x, int position_y, p2SString text, Color color, UIElement* parent) {

	UIElement* tmp_lbl = new UILabel(position_x, position_y, LABEL, text, color, parent);
	ui_elements.push_back(tmp_lbl);
	return tmp_lbl;
	
	LOG("Error: Cant add the UILabel");
	return nullptr;
}

UIElement* j1Gui::GetElementUnderMouse(int x, int y)
{
	for (int i = ui_elements.capacity() - 1; i >= 0 ; i--) {
		if (ui_elements[i] != nullptr)
		{
			if ((x > ui_elements[i]->GetScreenPosition().x && x < ui_elements[i]->GetScreenPosition().x + ui_elements[i]->GetRect().w) && (y > ui_elements[i]->GetScreenPosition().y && y < ui_elements[i]->GetScreenPosition().y + ui_elements[i]->GetRect().h))
			{
				UIElement* tmp_element_to_return = ui_elements[i];
				return tmp_element_to_return;
			}
		}
	}
	return nullptr;
}

// class Gui ---------------------------------------------------

