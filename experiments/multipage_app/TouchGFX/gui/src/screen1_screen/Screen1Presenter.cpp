#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

Screen1Presenter::Screen1Presenter(Screen1View& v)
    : view(v)
{

}

void Screen1Presenter::activate()
{
	view.setToggleState (model->getToggleButton());

}

void Screen1Presenter::deactivate()
{

}

void Screen1Presenter::saveToggleButton (bool state)
{
	model->saveToggleButton(state);
}
