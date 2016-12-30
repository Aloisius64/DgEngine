#pragma once

#include <DgUtility\DgCamera.h>
#include <DgUtility\DgUtility.h>
#include <DgAppScene.h>
#include <DgAppManager.h>

#include <vmath.h>
#include <stdio.h>
#include <Windows.h>

enum {
	PARTICLE_GROUP_SIZE = 128,
	PARTICLE_GROUP_COUNT = 8000,
	PARTICLE_COUNT = (PARTICLE_GROUP_SIZE * PARTICLE_GROUP_COUNT),
	MAX_ATTRACTORS = 64
};

class ParticlesSimulator : public DgAppScene {
public:
	ParticlesSimulator() {}
	ParticlesSimulator(string nameScene, string pathToXmlFile)
		: DgAppScene(nameScene, pathToXmlFile) {
		int i;

		// Initialize our compute program
		createComputeProgram();

		dt_location = glGetUniformLocation(compute_prog, "dt");

		glGenVertexArrays(1, &render_vao);
		glBindVertexArray(render_vao);
		glGenBuffers(2, buffers);
		glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
		glBufferData(GL_ARRAY_BUFFER, PARTICLE_COUNT * sizeof(vmath::vec4), NULL, GL_DYNAMIC_COPY);

		vmath::vec4 * positions = (vmath::vec4 *)glMapBufferRange(GL_ARRAY_BUFFER,
			0,
			PARTICLE_COUNT * sizeof(vmath::vec4),
			GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT);

		for(i = 0; i<PARTICLE_COUNT; i++) {
			positions[i] = vmath::vec4(random_vector(-10.0f, 10.0f), random_float());
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, velocity_buffer);
		glBufferData(GL_ARRAY_BUFFER, PARTICLE_COUNT * sizeof(vmath::vec4), NULL, GL_DYNAMIC_COPY);

		vmath::vec4 * velocities = (vmath::vec4 *)glMapBufferRange(GL_ARRAY_BUFFER,
			0,
			PARTICLE_COUNT * sizeof(vmath::vec4),
			GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT);
		for(i = 0; i<PARTICLE_COUNT; i++) {
			velocities[i] = vmath::vec4(random_vector(-0.1f, 0.1f), 0.0f);
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		glGenTextures(2, tbos);
		for(i = 0; i<2; i++) {
			glBindTexture(GL_TEXTURE_BUFFER, tbos[i]);
			glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, buffers[i]);
		}

		glGenBuffers(1, &attractor_buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, attractor_buffer);
		glBufferData(GL_UNIFORM_BUFFER, 32*sizeof(vmath::vec4), NULL, GL_STATIC_DRAW);
		for(i = 0; i<MAX_ATTRACTORS; i++) {
			attractor_masses[i] = 0.5f+random_float() * 0.5f;
		}
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, attractor_buffer);

		// Now create a simple program to visualize the result
		createRenderProgram();
	}
	virtual ~ParticlesSimulator() {
		glUseProgram(0);
		glDeleteProgram(compute_prog);
		glDeleteProgram(render_prog);
		glDeleteVertexArrays(1, &render_vao);
	}

	virtual void RenderSceneCB() {
#pragma region ComputeShader
		static const GLuint start_ticks = GetTickCount()-100000;
		GLuint current_ticks = GetTickCount();
		static GLuint last_ticks = current_ticks;
		float time = ((start_ticks-current_ticks)&0xFFFFF)/float(0xFFFFF);
		float delta_time = (float) (current_ticks-last_ticks) * 0.075f;

		vmath::vec4 * attractors = (vmath::vec4 *)glMapBufferRange(GL_UNIFORM_BUFFER,
			0,
			32*sizeof(vmath::vec4),
			GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT);
		for(int i = 0; i<32; i++) {
			attractors[i] = vmath::vec4(sinf(time * (float) (i+4) * 7.5f * 20.0f) * 50.0f,
				cosf(time * (float) (i+7) * 3.9f * 20.0f) * 50.0f,
				sinf(time * (float) (i+3) * 5.3f * 20.0f) * cosf(time * (float) (i+5) * 9.1f) * 100.0f,
				attractor_masses[i]);
		}
		glUnmapBuffer(GL_UNIFORM_BUFFER);

		// If dt is too large, the system could explode, so cap it to
		// some maximum allowed value
		delta_time = (delta_time>=2.0f) ? 2.0f : delta_time;

		// Activate the compute program and bind the position and velocity buffers
		glUseProgram(compute_prog);
		glBindImageTexture(0, velocity_tbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(1, position_tbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		// Set delta time
		glUniform1f(dt_location, delta_time);
		// Dispatch
		glDispatchCompute(PARTICLE_GROUP_COUNT, 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
#pragma endregion

#pragma region Render
		//DgCamera::getInstance()->OnRender();

		float fovy = DgCamera::getInstance()->getPerspectiveProjectionInfo().fovy;
		aspect_ratio = DgCamera::getInstance()->getPerspectiveProjectionInfo().width/DgCamera::getInstance()->getPerspectiveProjectionInfo().height;
		float zNear = DgCamera::getInstance()->getPerspectiveProjectionInfo().zNear;
		float zFar = DgCamera::getInstance()->getPerspectiveProjectionInfo().zFar;

		float xCamPos = DgCamera::getInstance()->getPosition().x;
		float yCamPos = DgCamera::getInstance()->getPosition().y;
		float zCamPos = DgCamera::getInstance()->getPosition().z;

		//printf("x %g, y %g, z %g\n", -xCamPos, -yCamPos, zCamPos);

		vmath::mat4 mvp = vmath::perspective(fovy, aspect_ratio, zNear, zFar) *
			vmath::translate(-xCamPos, -yCamPos, zCamPos) *
			vmath::rotate(time * 1000.0f, vmath::vec3(0.0f, 1.0f, 0.0f));
	
		// Clear, select the rendering program and draw a full screen quad
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(render_prog);
		glUniformMatrix4fv(0, 1, GL_FALSE, mvp);
		glBindVertexArray(render_vao);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		//glPointSize(2.0f);
		glDrawArrays(GL_POINTS, 0, PARTICLE_COUNT);
#pragma endregion

		last_ticks = current_ticks;
	}

	static inline float random_float() {
		float res;
		unsigned int tmp;
		static unsigned int seed = 0xFFFF0C59;

		seed *= 16807;

		tmp = seed^(seed>>4)^(seed<<15);

		*((unsigned int *) &res) = (tmp>>9)|0x3F800000;

		return (res-1.0f);
	}

	static vmath::vec3 random_vector(float minmag = 0.0f, float maxmag = 1.0f) {
		vmath::vec3 randomvec(random_float() * 2.0f-1.0f, random_float() * 2.0f-1.0f, random_float() * 2.0f-1.0f);
		randomvec = normalize(randomvec);
		randomvec *= (random_float() * (maxmag-minmag)+minmag);

		return randomvec;
	}

private:
	void createComputeProgram() {
		compute_prog = glCreateProgram();
		string shader_content;
		dgReadFile("./Shaders/Particles/particles.compute.glsl", shader_content);
		GLuint shaderObj = glCreateShader(GL_COMPUTE_SHADER);
		const GLchar* p[1];
		p[0] = shader_content.c_str();
		GLint Lengths[1] = {(GLint) shader_content.size()};
		glShaderSource(shaderObj, 1, p, Lengths);
		glCompileShader(shaderObj);

		GLint Success = 0;

		glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &Success);
		if(!Success) {
			GLchar InfoLog[1024];
			glGetShaderInfoLog(shaderObj, 1024, NULL, InfoLog);
			fprintf(stderr, "Error compiling compute shader: '%s'\n", InfoLog);
		}

		glAttachShader(compute_prog, shaderObj);
		glLinkProgram(compute_prog);

		GLchar ErrorLog[1024] = {0};

		glGetProgramiv(compute_prog, GL_LINK_STATUS, &Success);
		if(Success==0) {
			glGetProgramInfoLog(compute_prog, sizeof(ErrorLog), NULL, ErrorLog);
			fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		}

		glValidateProgram(compute_prog);
		glGetProgramiv(compute_prog, GL_VALIDATE_STATUS, &Success);
		if(!Success) {
			glGetProgramInfoLog(compute_prog, sizeof(ErrorLog), NULL, ErrorLog);
			fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		}
		glDeleteShader(shaderObj);

		GLCheckError();
	}

	void createRenderProgram() {
		GLint Success;

		//Create and compile vertex shader
		GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		string vertex_content;
		dgReadFile("./Shaders/Particles/particles.vs.glsl", vertex_content);
		const GLchar* p1[1];
		p1[0] = vertex_content.c_str();
		GLint Lengths1[1] = {(GLint) vertex_content.size()};
		glShaderSource(vertex_shader, 1, p1, Lengths1);
		glCompileShader(vertex_shader);
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &Success);
		if(!Success) {
			GLchar InfoLog[1024];
			glGetShaderInfoLog(vertex_shader, 1024, NULL, InfoLog);
			fprintf(stderr, "Error compiling vertex shader: '%s'\n", InfoLog);
		}

		//Create and compile fragment shader
		GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		string fragment_content;
		dgReadFile("./Shaders/Particles/particles.fs.glsl", fragment_content);
		const GLchar* p2[1];
		p2[0] = fragment_content.c_str();
		GLint Lengths2[1] = {(GLint) fragment_content.size()};
		glShaderSource(fragment_shader, 1, p2, Lengths2);
		glCompileShader(fragment_shader);
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &Success);
		if(!Success) {
			GLchar InfoLog[1024];
			glGetShaderInfoLog(fragment_shader, 1024, NULL, InfoLog);
			fprintf(stderr, "Error compiling fragment shader: '%s'\n", InfoLog);
		}

		//Create program
		render_prog = glCreateProgram();
		glAttachShader(render_prog, vertex_shader);
		glAttachShader(render_prog, fragment_shader);
		glLinkProgram(render_prog);

		GLchar ErrorLog[1024] = {0};

		glGetProgramiv(render_prog, GL_LINK_STATUS, &Success);
		if(Success==0) {
			glGetProgramInfoLog(render_prog, sizeof(ErrorLog), NULL, ErrorLog);
			fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		}

		glValidateProgram(render_prog);
		glGetProgramiv(render_prog, GL_VALIDATE_STATUS, &Success);
		if(!Success) {
			glGetProgramInfoLog(render_prog, sizeof(ErrorLog), NULL, ErrorLog);
			fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		}

		//Delete the shaders as the program has them now
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		GLCheckError();
	}

	// Compute program
	GLuint  compute_prog;
	GLint   dt_location;
	// Attractor UBO
	GLuint  attractor_buffer;
	// Program, vao and vbo to render a full screen quad
	GLuint  render_prog;
	GLuint  render_vao;
	GLuint  render_vbo;
	// Mass of the attractors
	float attractor_masses[MAX_ATTRACTORS];
	float aspect_ratio;

	// Posisition and velocity buffers
	union {
		struct {
			GLuint position_buffer;
			GLuint velocity_buffer;
		};
		GLuint buffers[2];
	};

	// TBOs
	union {
		struct {
			GLuint position_tbo;
			GLuint velocity_tbo;
		};
		GLuint tbos[2];
	};
};
