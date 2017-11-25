#ifndef SHADER_H
#define SHADER_H

#include <glad\glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

class Shader
{
public:
	GLuint id;

	Shader(const char* vertPath, const char* fragPath, const char* geoPath = nullptr);

	void use();
	void setUniformb(const std::string &name, bool value);
	void setUniform1i(const std::string &name, int value);
	void setUniform1f(const std::string &name, float value);
	void setUniform2f(const std::string &name, float v1, float v2);
	void setUniform3f(const std::string &name, float v1, float v2, float v3);

private:
	std::map<std::string, GLuint> locationsMap;
	GLuint getLoc(const std::string &name);
	void checkCompileErrors(GLuint shader, std::string type);
};

#endif
