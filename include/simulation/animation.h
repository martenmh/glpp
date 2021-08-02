#ifndef ENGINE_ANIMATION_H
#define ENGINE_ANIMATION_H

#include "particle.h"

namespace glpp {
	class KeyFrame {
	};

	class Animation: public std::vector<KeyFrame> {
		size_t                   m_frameRate = 0;
		std::vector<Particle<2>> m_particles;

	  public:
		std::vector<Particle<2>>& particles() {
			return m_particles;
		}

		void addParticle(const Particle<2>& particle) {
			m_particles.push_back(particle);
		}

		void update() {
			for(auto& particle: m_particles) {
				particle.position += (particle.velocity);
			}
		}
		void play() {
		}

		void limitFrameRate(size_t frameRate) {
			m_frameRate = frameRate;
		}
	};
}

#endif //ENGINE_ANIMATION_H
