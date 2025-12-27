#include "shader.hpp"
#include <fstream>


std::string get_file_contents(const char* filename)
{
    if(std::ifstream in(filename, std::ios::binary); in)
    {
        std::string contents;
        in.seekg(0,std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0,std::ios::beg);
        in.read(&contents[0],static_cast<long long>(contents.size()));
        in.close();
        return(contents);
    }
    throw std::system_error(errno, std::generic_category(), "Failed to open file");
}

Shader::Shader(const char* vertexFile, const char* fragmentFile,const char* geometryFile, const char* TCSFile, const char* TESFile)
{
    const std::string vertexCode = get_file_contents(vertexFile);
    const std::string fragmentCode = get_file_contents(fragmentFile);
    //const std::string geometryCode = get_file_contents(geometryFile);
    const std::string TCSCode = get_file_contents(TCSFile);
    const std::string TESCode = get_file_contents(TESFile);

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();
    //const char* geometrySource = geometryCode.c_str();
    const char* TCSSource = TCSCode.c_str();
    const char* TESSource = TESCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexSource,nullptr);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentSource,nullptr);
    glCompileShader(fragmentShader);

     glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // GLuint geometryShader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(geometryShader,1,&geometrySource,nullptr);
    // glCompileShader(geometryShader);
    //
    // glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
    // if (!success) {
    //     glGetShaderInfoLog(geometryShader, 512, nullptr, infoLog);
    //     std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    // }

    GLuint TCSShader = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(TCSShader,1,&TCSSource,nullptr);
    glCompileShader(TCSShader);

    glGetShaderiv(TCSShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(TCSShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    GLuint TESShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(TESShader,1,&TESSource,nullptr);
    glCompileShader(TESShader);

    glGetShaderiv(TESShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(TESShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    ID  = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    //glAttachShader(ID, geometryShader);
    glAttachShader(ID, TCSShader);
    glAttachShader(ID, TESShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    //glDeleteShader(geometryShader);
    glDeleteShader(TCSShader);
    glDeleteShader(TESShader);
}

void Shader::setBool(const std::string &name, const bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string &name, const int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, const float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, const float x, const float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, const float x, const float y, const float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::Activate() const {
    glUseProgram(ID);
}

void Shader::Delete() const
{
    glDeleteProgram(ID);
}
