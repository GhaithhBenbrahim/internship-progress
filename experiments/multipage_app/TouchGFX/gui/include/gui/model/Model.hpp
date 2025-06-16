#ifndef MODEL_HPP
#define MODEL_HPP

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();

    void saveToggleButton (bool state);
    bool getToggleButton ();
protected:
    ModelListener* modelListener;
    bool toggleButtonState ;
};

#endif // MODEL_HPP
