#ifndef FGV_SHADER_H
#define FGV_SHADER_H

#include <string>
#include <stdexcept>

#include "GL/glew.h"

/**
 * GL shader loader helper
 * by J. Coffin
 * from: http://stackoverflow.com/questions/2795044/easy-framework-for-opengl-shaders-in-c-c
 *
 * Usage:
 * void draw() {
 *  static shader_prog wall_shader(wall_vertex, wall_frag);
 *  static shader_prog skin_shader(skin_vertex, skin_frag);
 *
 *  wall_shader();
 *  draw_walls();
 *
 *  skin_shader();
 *  draw_skin();
 * }
 *
 */
class shader_prog {
    GLuint vertex_shader, fragment_shader, prog;

    template <int N>
    GLuint compile(GLuint type, char const *(&source)[N]) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, N, source, NULL);
        glCompileShader(shader);
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, ' ');
            glGetShaderInfoLog(shader, length, &length, &log[0]);
            throw std::logic_error(log);
            return false;
        }
        return shader;
    }

public:
    template <int N, int M>
    shader_prog(GLchar const *(&v_source)[N], GLchar const *(&f_source)[M]) {
        vertex_shader = compile(GL_VERTEX_SHADER, v_source);
        fragment_shader = compile(GL_FRAGMENT_SHADER, f_source);
        prog = glCreateProgram();
        glAttachShader(prog, vertex_shader);
        glAttachShader(prog, fragment_shader);
        glLinkProgram(prog);
    }

    operator GLuint() { return prog; }
    void operator()() { glUseProgram(prog); }

    ~shader_prog() {
        glDeleteProgram(prog);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }
};

#endif
