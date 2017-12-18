#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Gui.h"
#include "j1Scene.h"

#include "UIImage.h"
#include "UIButton.h"


j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	if(App->map->Load("iso_walk.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if(App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	debug_tex = App->tex->Load("maps/path2.png");

	window = (UIImage*)App->gui->AddUIImage(300, 100, { 0, 512, 483, 512 }, this);
	window->draggable = true;
	buttons.PushBack((UIButton*)App->gui->AddUIButton(350, 150, { 0,113,229,69 }, { 411,169,229,69 }, { 642,169,229,69 }, this, window));
	UIButton* draggable_btn = (UIButton*)App->gui->AddUIButton(350, 190, { 0,113,229,69 }, { 411,169,229,69 }, { 642,169,229,69 }, this, window);
	draggable_btn->draggable = true;
	buttons.PushBack(draggable_btn);

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{

	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if(App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if(origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	// Gui ---
	
	// -------
	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= floor(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) {
		if (!App->gui->DeleteAllUIElements())
			LOG("Error deleting all the UIElements!");
	}

	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN) {
		bool isAnyButtonFocused = false;
		for (int i = 0; i < buttons.Count(); i++)
		{
			if (buttons[i]->current_state == STATE_FOCUSED) {
				isAnyButtonFocused = true;
				buttons[i]->current_state = STATE_NORMAL;
				buttons[i]->UpdateButtonWithSelfRect(buttons[i]->btn_normal);
				if (i + 1  != buttons.Count()) { //is not the final button
					buttons[i + 1]->current_state = STATE_FOCUSED;
					buttons[i + 1]->UpdateButtonWithSelfRect(buttons[i]->btn_focused);
				}
				else {
					buttons[0]->current_state = STATE_FOCUSED;
					buttons[0]->UpdateButtonWithSelfRect(buttons[i]->btn_focused);
				}
				break;
			}

		}

		if (!isAnyButtonFocused && buttons.Count() > 0) {
			buttons[0]->current_state = STATE_FOCUSED;
			buttons[0]->UpdateButtonWithSelfRect(buttons[0]->btn_focused);
		}
	}

	App->map->Draw();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(),
					map_coordinates.x, map_coordinates.y);

	//App->win->SetTitle(title.GetString());

	// Debug pathfinding ------------------------------
	//int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	App->render->Blit(debug_tex, p.x, p.y);

	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for(uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(debug_tex, pos.x, pos.y);
	}

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");
	if (!App->gui->DeleteAllUIElements())
		LOG("Error deleting all the UIElements!");

	return true;
}

void j1Scene::OnUITrigger(UIElement* elementTriggered, UI_State ui_state) {
	if (elementTriggered->type == IMAGE) {
		UIImage* tmpImg = (UIImage*)elementTriggered;
		switch (ui_state)
		{
		case STATE_NORMAL:
			break;
		case STATE_MOUSE_ENTER:
			break;
		case STATE_MOUSE_LEAVE:
			break;
		case STATE_LEFT_MOUSE_PRESSED:
			break;
		case STATE_NO_DEF:
			break;
		default:
			break;
		}
	}
	else if (elementTriggered->type == BUTTON) {
		UIButton* tmpBtn = (UIButton*)elementTriggered;
		switch (ui_state)
		{
		case STATE_NORMAL:
			break;
		case STATE_MOUSE_ENTER: {
			for (int i = 0; i < buttons.Count(); i++)
			{
				if (buttons[i]->current_state == STATE_FOCUSED) {
					buttons[i]->current_state = STATE_NORMAL;
					tmpBtn->UpdateButtonWithSelfRect(tmpBtn->btn_normal);
					break;
				}
			}
			break;
		}
		case STATE_MOUSE_LEAVE:
			LOG("MOUSE LEAVE");
			break;
		case STATE_LEFT_MOUSE_PRESSED:
			LOG("LEFT PRESSED");
			break;
		case STATE_LEFT_MOUSE_RELEASED:
			LOG("LEFT RELEASED");
			break;
		case STATE_NO_DEF:
			break;
		default:
			break;
		}
	}
}