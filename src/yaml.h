#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include <godot_cpp/core/object.hpp>

namespace godot {

class YAML : public Object {
	GDCLASS(YAML, Object)

private:
	double time_passed;

protected:
	static void _bind_methods();

public:
	YAML();
	~YAML();

  String version();
};

}

#endif
