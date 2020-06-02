#ifndef PARTICLE_GENERATOR_HPP
#define PARTICLE_GENERATOR_HPP

#include <vector>
#include <glm/glm.hpp>

#include "myShader.h"

using namespace std;

// represents a single particle and its state
struct Particle {
	glm::vec2 position, velocity;
	glm::vec4 color;
	float life;

	Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) {}
};

// container for all particles
class ParticleGenerator {
private:
	vector<Particle> particles;
	unsigned int amount;
	myShader shader;
	//Texture2D texture;
	unsigned int VAO;

	void init(); // initializes buffer and vertex attributes
	unsigned int firstUnusedParticle(); // first particle index that's unused (life <= 0.0f)
	void respawnParticle(Particle& particle, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
	
public:
	ParticleGenerator(myShader shader, unsigned int amount);
	void Update(float dt, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f)); // update all particles
	void Draw(); // render all particles
};

#endif // !PARTICLE_GENERATOR_HPP
