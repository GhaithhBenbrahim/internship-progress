#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

Model::Model() : modelListener(0)
{

}

void Model::tick()
{

}

void Model::saveToggleButton (bool state)
{
	toggleButtonState = state;
}

bool Model::getToggleButton()
{
	return toggleButtonState;
}
