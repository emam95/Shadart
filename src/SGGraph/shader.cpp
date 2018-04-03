/*
 * Copyright (c) 2018 Mohamed Emam
 * The code is licensed under the MIT License.
 * You can check the file LICENSE for the full license.
 */

#include "shader.h"

using namespace SGGraph;

Shader::Shader(const std::string& vertPath, const std::string& fragPath, const std::string& geoPath)
{
	std::string vertCode;
	std::string fragCode;
	std::string geoCode;
	std::ifstream vertFile(vertPath);
	std::ifstream fragFile(fragPath);

	if(vertFile.is_open() && fragFile.is_open())
	{
		std::stringstream vertStream, fragStream;

		vertStream << vertFile.rdbuf();
		fragStream << fragFile.rdbuf();

		vertFile.close();
		fragFile.close();

		vertCode = vertStream.str();
		fragCode = fragStream.str();

		if (geoPath == "")
		{
			std::ifstream geoFile(geoPath);
			std::stringstream geoStream;
			geoStream << geoFile.rdbuf();
			geoFile.close();
			geoCode = geoStream.str();
		}
	}
	else
	{
		std::cout << "ERROR::SHADER::READING_FILE" << std::endl;
	}

	const char* vCode = vertCode.c_str();
	const char* fCode = fragCode.c_str();

	GLuint vert, frag;
	int success;
	char log[512];

	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vCode, NULL);
	glCompileShader(vert);
	checkCompileErrors(vert, "VERTEX");

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fCode, NULL);
	glCompileShader(frag);
	checkCompileErrors(frag, "FRAGMENT");

	GLuint geo;
	if (!geoPath.empty())
	{
		const char* gCode = geoCode.c_str();
		geo = glCreateShader(geo);
		glShaderSource(geo, 1, &gCode, NULL);
		glCompileShader(geo);
		checkCompileErrors(geo, "GEOMETRY");
	}

	id = glCreateProgram();
	glAttachShader(id, vert);
	glAttachShader(id, frag);
	if (!geoPath.empty())
		glAttachShader(id, geo);
	glLinkProgram(id);
	checkCompileErrors(id, "PROGRAM");

	glDeleteShader(vert);
	glDeleteShader(frag);
	if (!geoPath.empty())
		glDeleteShader(geo);

}

void Shader::use()
{
	glUseProgram(id);
}

void Shader::setUniformb(const std::string &name, bool value)
{
	GLuint loc = getLoc(name);
	glUniform1i(loc, (int)value);
}

void Shader::setUniform1i(const std::string &name, int value)
{
	GLuint loc = getLoc(name);
	glUniform1i(loc, value);
}

void Shader::setUniform1f(const std::string &name, float value)
{
	GLuint loc = getLoc(name);
	glUniform1f(loc, value);
}

void Shader::setUniform2f(const std::string &name, float v1, float v2)
{
	GLuint loc = getLoc(name);
	glUniform2f(loc, v1, v2);
}

void Shader::setUniform3f(const std::string &name, float v1, float v2, float v3)
{
	GLuint loc = getLoc(name);
	glUniform3f(loc, v1, v2, v3);
}

GLuint Shader::getLoc(const std::string &name) 
{
	std::map<std::string, GLuint>::iterator iter = locationsMap.find(name);
	if (iter != locationsMap.end())
		return iter->second;

	GLuint loc = glGetUniformLocation(id, name.c_str());
	locationsMap.insert(std::pair<std::string, GLuint>(name, loc));
	return loc;
}

void Shader::checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar log[1024];

	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, log);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR::" << type << "\n" << log << "\n -- --------------------- --" << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, log);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR::" << type << "\n" << log << "\n -- --------------------- --" << std::endl;
		}
	}
}

void Shader::deleteShader()
{
	glUseProgram(0);
	glDeleteProgram(id);
	locationsMap.clear();
}