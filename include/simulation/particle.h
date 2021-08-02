#ifndef ENGINE_PARTICLE_H
#define ENGINE_PARTICLE_H

namespace glpp {
	template<size_t Dimension = 2>
	struct Particle {
		ml::vector<float, Dimension> position;
		ml::vector<float, Dimension> velocity;
		ml::vector<float, Dimension> acceleration;
		Particle(const ml::vector<float, Dimension>& _position, const ml::vector<float, Dimension>& _velocity, const ml::vector<float, Dimension>& _acceleration):
			position(_position), velocity(_velocity), acceleration(_acceleration) {
		}
	};

	//struct Particle3D {
	//	ml::vec3
	//
	//};
	//
	//struct Particle2D {
	//    ml::vec3
	//
	//};
}

#endif //ENGINE_PARTICLE_H
