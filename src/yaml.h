#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include <godot_cpp/classes/sprite2d.hpp>

namespace godot {

class YAML : public Sprite2D {
	GDCLASS(YAML, Sprite2D)

private:
	double time_passed;

protected:
	static void _bind_methods();

public:
	YAML();
	~YAML();

	void _process(double delta) override;
};

}

#endif
