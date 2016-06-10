#include "visualizer.hpp"
#include "../Simulation/particle.hpp"
#include "camera.hpp"
#include "resource_manager.hpp"
#include "vec3.hpp"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <algorithm>
#include <ctype.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

Visualizer::Visualizer() {
    m_pause = false;
    m_running = true;
    m_iteration_number = 0;
    m_screen_width = 900;
    m_screen_height = 900;
    m_input = false;
    m_scale = 5000000000;
    load_object_data_from_file("test.txt");
    init_SDL();
    load_textures();
    main_loop();
    ;
}

Visualizer::Visualizer(std::string filename) {
    m_time_simulated = 0;
    m_pause = false;
    m_running = true;
    m_iteration_number = 0;
    m_screen_width = 900;
    m_screen_height = 900;
    m_input = false;
    m_perspective = 'x';
    m_console_is_open = false;
    m_draw_it_number = false;
    m_rotation_active = false;
    m_draw_number_of_particles = false;
    m_time_simulated = 0;
    m_draw_dt = false;
    m_draw_ids = false;
    m_camera = {-m_screen_width / 2, -m_screen_height / 2, m_screen_width, m_screen_height};
    m_camera_object.init(&m_screen_width, &m_screen_height, 45.0);
    m_scale = 5000000000;
    std::cout << "HERE" << std::endl;
    load_object_data_from_file(filename);
    calculate_collision_points();
    std::cout << "HERE" << std::endl;
    init_SDL();
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    load_textures();
    load_fonts();
    main_loop();
}

Visualizer::~Visualizer() {
    SDL_Quit();
}

void Visualizer::init_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() == -1) {
        std::cout << "TFF_Init failure: " << TTF_GetError() << std::endl;
    }
    m_window = SDL_CreateWindow("Visualizer", 100, 100, m_screen_width, m_screen_height,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (m_window == nullptr) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    m_renderer =
        SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (m_renderer == nullptr) {
        SDL_DestroyWindow(m_window);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

const unsigned c_scroll_velo = 100000;
std::string command = "";
void Visualizer::main_loop() {
    m_frame_time = 0;
    m_current_frames_per_second = 0;
    while (m_running == true) {
        m_start_time = SDL_GetTicks();
        update();
        draw_main_loop();
        m_iteration_number++;
        if (m_iteration_number == m_object_positions.size()) {
            m_iteration_number = 0;
        }
        /*
        SDL_Delay(10);
        // std::cout << lol++ << std::endl;
        for (unsigned long i = 0; m_object_positions.size() < i; i++) {
            if (m_collision_data[i].size() > 0) {
                for (unsigned long j = 0; j < m_collision_data[i].size(); j++) {
                    // std::cout << m_collision_data[i][j].to_string() << std::endl;
                }
            }
            std::cout << std::endl;
        }*/

        m_frame_time += SDL_GetTicks() - m_start_time;
        m_frames_per_second++;
        if (m_frame_time > 1000) {
            m_current_frames_per_second = m_frames_per_second;
            m_frames_per_second = 0;
            m_frame_time = 0;
        }
    }
    m_resource_manager.shutdown();
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}
/** pause loop*/
void Visualizer::pause_loop() {
    while (m_pause == true) {
        update();
    }
}
/** function for drawing Text */
void Visualizer::draw_text(std::string text, int pos_x, int pos_y, SDL_Color color) {
    SDL_Rect rect{pos_x, pos_y, 12 * (int)text.length(), 12};
    SDL_Surface *font_surf =
        TTF_RenderText_Blended(m_resource_manager.get_font("data-latin"), text.c_str(), color);
    SDL_Texture *font_tex = SDL_CreateTextureFromSurface(m_renderer, font_surf);
    SDL_RenderCopy(m_renderer, font_tex, nullptr, &rect);
    SDL_FreeSurface(font_surf);
    SDL_DestroyTexture(font_tex);
}
int fuck_this = 0;
/** Function for getting and processing user keyboard input*/
void Visualizer::update() {
    unsigned long zoom = 50000000000;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            m_running = false;
            m_pause = false;
            break;

        case SDL_MOUSEWHEEL:
            if (m_scale <= 100000001) {
                zoom = 10000000;
            }
            m_camera_object.m_position.z += (event.wheel.y * 500000000.f);
            if (m_scale == 0) {
                m_scale = zoom;
            }
            draw_main_loop();
            break;

        case SDL_TEXTINPUT:
            if (m_input == true) {
                command += event.text.text;
                std::cout << command << std::endl;
                return;
            }
            break;
        case SDL_WINDOWEVENT_RESIZED:
            std::cout << "HERE WE ARE AND IT SHOULD WORK" << std::endl;
            SDL_GetWindowSize(m_window, &m_screen_width, &m_screen_height);
            // m_screen_width = event.window.data1;
            // m_screen_height = event.window.data2;
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_RIGHT) {
                std::cout << "rotation now active" << std::endl;
                m_rotation_active = true;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_RIGHT) {
                std::cout << "rotation now inactive" << std::endl;
                m_rotation_active = false;
            }
            break;
        case SDL_MOUSEMOTION:
            if (m_rotation_active) {
                int x_rel = event.motion.xrel;
                int y_rel = event.motion.yrel;
                float x_angle = 0;
                float y_angle = 0;
                x_angle = glm::radians(y_rel / 2.f);
                y_angle = glm::radians(x_rel / 2.f);

                glm::quat x_rot =
                    glm::quat(cos(y_angle / 2),
                              sin(y_angle / 2) * (glm::normalize(m_camera_object.m_plane_vec)));
                glm::quat y_rot = glm::quat(
                    cos(x_angle / 2),
                    sin(x_angle / 2) * (glm::normalize(glm::cross(m_camera_object.m_plane_vec,
                                                                  m_camera_object.m_orientation))));
                m_camera_object.rotate(x_rot);
                m_camera_object.rotate(y_rot);
              
                if (x_angle != 0 || y_angle != 0) {
                    fuck_this++;
                }
            }
            break;

        case SDL_KEYDOWN:
            if (m_input == false) {
                switch (event.key.keysym.sym) {
                case SDLK_w:
                    m_camera_object.m_position +=
                        (glm::normalize(m_camera_object.m_orientation)) * 1000000000.f;
                    break;
                case SDLK_s:
                    m_camera_object.m_position -=
                        (glm::normalize(m_camera_object.m_orientation) * 1000000000.f);
                    break;

                case SDLK_a:
                    m_camera_object.m_position +=
                        glm::normalize(glm::cross(m_camera_object.m_plane_vec,
                                                  m_camera_object.m_orientation)) *
                        1000000000.f;
                    break;
                case SDLK_d:
                    m_camera_object.m_position -=
                        glm::normalize(glm::cross(m_camera_object.m_plane_vec,
                                                  m_camera_object.m_orientation)) *
                        1000000000.f;
                    break;
                case SDLK_SPACE:
                    m_camera_object.m_position +=
                        glm::normalize(m_camera_object.m_plane_vec) * 1000000000.f;
                    break;
                case SDLK_LSHIFT:
                    m_camera_object.m_position -=
                        glm::normalize(m_camera_object.m_plane_vec) * 1000000000.f;
                    break;
                }
            }
            break;
        case SDL_KEYUP:

            if (m_input == true) {
                switch (event.key.keysym.sym) {

                case SDLK_RETURN:
                    if (command == "") {
                        break;
                    }
                    handle_console_input(command);
                    std::cout << "test1" << std::endl;
                    m_input = false;
                    command = "";
                    ;
                    draw_main_loop();
                    m_input = false;
                    break;
                case SDLK_BACKSPACE:
                    if (m_input && command != "") {
                        command.erase(command.end() - 1);
                    }
                    break;
                }
            } else {
                switch (event.key.keysym.sym) {

                case SDLK_RETURN:
                    std::cout << "test1" << std::endl;
                    m_input = true;
                    break;

                case SDLK_BACKQUOTE:
                    m_console_is_open = !m_console_is_open;
                    break;

                case SDLK_DOWN:
                    m_camera_object.m_position.y -= c_scroll_velo;
                    break;

                case SDLK_UP:
                    m_camera_object.m_position.y += c_scroll_velo;
                    break;

                case SDLK_LEFT:
                    m_camera_object.m_position.x += c_scroll_velo;
                    break;

                case SDLK_RIGHT:
                    m_camera_object.m_position.x -= c_scroll_velo;
                    break;

                case SDLK_ESCAPE:
                    m_running = false;
                    break;

                case SDLK_n:
                    if (m_input)
                        break;
                    if (m_pause == true) {
                        m_iteration_number++;
                        draw_main_loop();
                    }
                    break;

                case SDLK_p:
                    if (m_input)
                        break;
                    m_pause = !m_pause;
                    pause_loop();
                    break;

                case SDLK_r:
                    if (!m_input) {
                        m_iteration_number = 0;
                        m_time_simulated = 0;
                        m_camera_object.to_start_position();
                        draw_main_loop();
                    }
                    break;

                case SDLK_b:
                    if (m_input)
                        break;
                    if (m_iteration_number == 0) {
                        m_iteration_number = m_object_positions.size() - 1;
                    } else {
                        m_iteration_number--;
                    }
                    draw_main_loop();
                    break;

                case SDLK_x:
                    if (m_input)
                        break;
                    m_perspective = 'x';
                    break;

                case SDLK_y:
                    if (m_input)
                        break;
                    m_perspective = 'y';
                    break;
                }
            }
        }
    }
}

/** function for processing console input */
void Visualizer::handle_console_input(std::string input) {
    char *c_input = (char *)input.c_str();
    std::vector<char *> token_vector;
    char *token;
    token = strtok(c_input, " ");
    token_vector.push_back(token);

    while ((token = strtok(NULL, " ")) != NULL) {
        token_vector.push_back(token);
    }

    if (strcmp(token_vector[0], "draw_ids") == 0 || strcmp(token_vector[0], "dis") == 0) {
        m_draw_ids = !m_draw_ids;
        return;
    }

    if (strcmp(token_vector[0], "help") == 0) {
        return;
    }

    if (strcmp(token_vector[0], "draw_it_number") == 0 || strcmp(token_vector[0], "din") == 0) {
        m_draw_it_number = !m_draw_it_number;
        return;
    }

    // For the occasional rage quit!!
    if (strcmp(token_vector[0], "fuc*_this_shit") == 0) {
        m_running = false;
    }

    if (strcmp(token_vector[0], "show_particle_number") == 0 ||
        strcmp(token_vector[0], "spn") == 0) {
        m_draw_number_of_particles = !m_draw_number_of_particles;
    }
    if (strcmp(token_vector[0], "show_simulated_time") == 0 ||
        strcmp(token_vector[0], "sst") == 0) {
        m_draw_time_simulated = !m_draw_time_simulated;
    }

    if (strcmp(token_vector[0], "show_delta_time") == 0 || strcmp(token_vector[0], "sdt") == 0) {
        m_draw_dt = !m_draw_dt;
    }

    if (strcmp(token_vector[0], "display_data") == 0 || strcmp(token_vector[0], "dd") == 0) {
        unsigned long particle_id;

        if (token_vector.size() > 2) {
            std::cout << "Error: To many arguments. Needed arguments: 1 Given arguments: "
                      << token_vector.size() - 1 << std::endl;
            return;
        }

        if (token_vector.size() < 2) {
            std::cout << "Error: Missing arguments. Needed arguments: 1 Given arguments: "
                      << token_vector.size() - 1 << std::endl;
            return;
        }

        if (!is_all_digits(token_vector[1])) {
            std::cout << "Error: argument 1 has non digit parts." << std::endl;
            return;
        }

        particle_id = atoi(token_vector[1]);

        if (particle_id >= m_object_positions.size()) {
            std::cout << "Error: Argument 1 is too large, should be smaller than "
                      << m_object_positions.size() - 1 << std::endl;
            return;
        }

        auto it = m_display_data_active.find(particle_id);
        if (it != m_display_data_active.end()) {
            m_display_data_active.erase(it);
            return;
        }

        m_display_data_active.insert(particle_id);

        return;
    }

    if (strcmp(token_vector[0], "show_grav_range") == 0 || strcmp(token_vector[0], "sgr") == 0) {
        if (token_vector.size() < 3) {
            std::cout << "Error: missing arguments ( " << 3 - token_vector.size() << "/3 )"
                      << std::endl;
        }

        if (token_vector.size() > 3) {
            std::cout << "Error: too many arguments ( " << token_vector.size() - 3 << "/3 )"
                      << std::endl;
        }

        if (!is_all_digits(token_vector[1])) {
            std::cout << "Error: argument 1 has non digit parts." << std::endl;
            return;
        }
        if (!is_all_digits(token_vector[2])) {
            std::cout << "Error: argument 2 has non digit parts." << std::endl;
            return;
        }

        unsigned long id = strtoul(token_vector[1], NULL, 0);
        double min_force; // = (double)strtod(token_vector[2],NULL);
        std::istringstream conv(token_vector[2]);
        conv >> min_force;
        auto it = m_grav_range_draw_active.find(id);
        if (it != m_grav_range_draw_active.end()) {
            m_grav_range_draw_active.erase(it);
        } else {
            m_grav_range_draw_active.insert(std::pair<unsigned long, double>(id, min_force));
        }
        return;
    }

    if (strcmp(token_vector[0], "focus_on_object") == 0 || strcmp(token_vector[0], "foo") == 0) {
        unsigned long index =
            m_object_id_maps[m_iteration_number].find(strtoul(token_vector[1], NULL, 0))->second;
        unsigned long x =
            (m_object_positions[m_iteration_number][index].x / m_scale) - (m_camera.w / 2);
        unsigned long y =
            (m_object_positions[m_iteration_number][index].y / m_scale) - (m_camera.h / 2);
        unsigned long z = m_object_positions[m_iteration_number][index].z / m_scale;
        m_camera.x = x; //- (m_camera.h / 2);
        m_camera.y = z;
        m_camera.y = y; // + (m_camera.w / 2);
        return;
    }

    if (strcmp(token_vector[0], "clear_displayed_data") == 0 ||
        strcmp(token_vector[0], "cdd") == 0) {
        m_display_data_active.clear();
    }

    if (strcmp(token_vector[0], "jmp") == 0) {
        if (token_vector.size() > 2) {
            std::cout << "Error: To many arguments. Needed arguments: 1 Given arguments: "
                      << token_vector.size() - 1 << std::endl;
            return;
        }

        if (!is_all_digits(token_vector[1])) {
            std::cout << "Error: argument 1 has non digit parts." << std::endl;
            return;
        }

        unsigned long new_it_number = atoi(token_vector[1]);

        if (new_it_number > m_object_positions.size()) {
            std::cout << "Error: iteration number to large. Max iteration is: "
                      << m_object_positions.size() - 1 << std::endl;
            return;
        }

        m_iteration_number = new_it_number;

        return;
    }

    if (strcmp(token_vector[0], "clear_all_trajectorys") == 0 ||
        strcmp(token_vector[0], "cat") == 0) {
        clear_trajectory_lines();
        std::cout << m_line_draw_active.size() << std::endl;
        return;
    }

    if (strcmp(token_vector[0], "show_trajectory") == 0 || strcmp(token_vector[0], "st") == 0) {
        unsigned long particle_id;
        unsigned long draw_length = m_object_positions.size();

        if (token_vector.size() > 3) {
            std::cout << "Error: To many arguments. Needed arguments: at least 1, at most 2. Given "
                         "arguments: "
                      << token_vector.size() - 1 << std::endl;
            return;
        }

        if (token_vector.size() < 2) {
            std::cout
                << "Error: Missing arguments. Needed arguments: at 1 needed. Given arguments: "
                << token_vector.size() - 1 << std::endl;
            return;
        }

        if (token_vector.size() == 3) {

            if (!is_all_digits(token_vector[2])) {
                std::cout << "Error: argument 2 has non digit parts." << std::endl;
                return;
            }

            draw_length = atoi(token_vector[2]);
        }

        if (!is_all_digits(token_vector[1])) {
            std::cout << "Error: argument 1 has non digit parts." << std::endl;
            return;
        }
        particle_id = atoi(token_vector[1]);

        if (particle_id >= m_object_positions.size()) {
            std::cout << "Error: Argument 1 is too large, should be smaller than "
                      << m_object_positions.size() - 1 << std::endl;
            return;
        }
        m_line_draw_active.insert(std::make_pair(particle_id, draw_length));
        generate_trajectory_line(particle_id);
        return;
    }

    if (strcmp(token_vector[0], "clear_trajectory") == 0 || strcmp(token_vector[0], "ct") == 0) {
        unsigned long particle_id;

        if (token_vector.size() > 3) {
            std::cout << "Error: To many arguments. Needed arguments: at least 1, at most 2. Given "
                         "arguments: "
                      << token_vector.size() - 1 << std::endl;
            return;
        }

        if (token_vector.size() < 2) {
            std::cout
                << "Error: Missing arguments. Needed arguments: at 1 needed. Given arguments: "
                << token_vector.size() - 1 << std::endl;
            return;
        }

        if (!is_all_digits(token_vector[1])) {
            std::cout << "Error: argument 1 has non digit parts." << std::endl;
            return;
        }
        particle_id = atoi(token_vector[1]);

        if (particle_id >= m_object_positions.size()) {
            std::cout << "Error: Argument 1 is too large, should be smaller than "
                      << m_object_positions.size() - 1 << std::endl;
            return;
        }

        m_line_draw_active.erase(particle_id);
        return;
    }
    std::cout << "no such command known: " << command << std::endl;
    return;
}

bool Visualizer::is_all_digits(char *text) {

    int string_length = strlen(text);

    for (int i = 0; i < string_length - 2; i++) {
        if (text[i] == '.') {
            i++;
        }

        if (!isdigit(text[i])) {
            return false;
        }
    }
    return true;
}

void Visualizer::draw_main_loop() {
    // int pos_z;
    SDL_RenderClear(m_renderer);
    // std::cout << m_iteration_number << std::endl;
    render_texture(m_resource_manager.get_texture("Background"), 0, 0, 0, 1920, 1080);
    draw_all_trajectory_lines();
    display_all_grav_ranges();
    draw_all_object_circles();

    for (auto it : m_object_id_maps[m_iteration_number]) {
        unsigned long index = it.second;

        glm::vec4 plane_pos = m_camera_object.calculate_position_on_draw_plane(
            m_object_positions[m_iteration_number][index]);

        if (plane_pos.w < glm::radians(32.f)) {
            render_texture(m_resource_manager.get_texture("Triangle_Green"), plane_pos.x - 8,
                           plane_pos.y - 8, 0, 16, 16);

            if (m_draw_ids) {
                draw_text(std::to_string(m_object_ids[m_iteration_number][index]), plane_pos.x + 18,
                          plane_pos.y - 18, {255, 255, 25, 255});
            }
        }
    }

    if (m_draw_number_of_particles == true) {
        draw_text("Number of particles:" +
                      std::to_string(m_object_positions[m_iteration_number].size()),
                  50, 130, {255, 255, 255, 255});
    }

    if (m_draw_dt) {
        draw_text("Current dt:" + std::to_string(1 / m_iteration_dts[m_iteration_number]), 50, 160,
                  {255, 255, 255, 255});
    }

    if (m_draw_time_simulated) {
        draw_text("Time:" + format_seconds_to_time(m_time_simulated), 50, 190,
                  {255, 255, 255, 255});
    }

    if (m_draw_it_number == true) {
        draw_text("Iteration: " + std::to_string(m_iteration_number) + " / " +
                      std::to_string(m_object_ids.size()),
                  50, 100, {255, 255, 255, 255});
    }

    draw_data();

    if (m_console_is_open) {
        render_texture(m_resource_manager.get_texture("ConBack"), 100, 100, 0, 450, 600);
    }

    if (m_input && command != "") {
        draw_text(command, 50, 50, {255, 255, 255, 255});
    }
    draw_collison_marks();
    draw_text(std::to_string(m_camera_object.m_position.x) + " " +
                  std::to_string(m_camera_object.m_position.y) + " " +
                  std::to_string(m_camera_object.m_position.z),
              50, 700, {255, 255, 255, 255});

    draw_text(std::to_string(m_camera_object.m_orientation.x) + " " +
                  std::to_string(m_camera_object.m_orientation.y) + " " +
                  std::to_string(m_camera_object.m_orientation.z),
              50, 750, {255, 255, 255, 255});

    draw_text(std::to_string(m_camera_object.m_plane_vec.x) + " " +
                  std::to_string(m_camera_object.m_plane_vec.y) + " " +
                  std::to_string(m_camera_object.m_plane_vec.z),
              50, 800, {255, 255, 255, 255});

    draw_text(std::to_string(m_current_frames_per_second), 10, 850, {255, 255, 25, 255});
    SDL_RenderPresent(m_renderer);

    m_time_simulated += 1 / m_iteration_dts[m_iteration_number];
}

void Visualizer::set_color(SDL_Color color) {
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
}

void Visualizer::generate_trajectory_line(unsigned long particle_id) {
    unsigned long particle_index_a;
    SDL_Color line_segment_color;

    std::vector<std::pair<SDL_Color, glm::vec3>> line_points;
    for (unsigned long iteration = 0; iteration < m_object_positions.size(); iteration++) {
        particle_index_a = m_object_id_maps[iteration].find(particle_id)->second;
        glm::vec3 line_point(m_object_positions[iteration][particle_index_a].x,
                             m_object_positions[iteration][particle_index_a].y,
                             m_object_positions[iteration][particle_index_a].z);

        line_segment_color = set_color_gradient_for_speed(
            length(m_object_velocities[iteration][particle_index_a]), 200000, {255, 0, 0, 255},
            {0, 0, 255, 255}, {0, 0, 255, 255});

        line_points.push_back(std::make_pair(line_segment_color, line_point));
    }
    m_trajectory_lines.insert(std::make_pair(particle_id, line_points));
}

void Visualizer::draw_all_trajectory_lines() {
    if (!m_line_draw_active.empty()) {
        for (auto it = m_line_draw_active.begin(); it != m_line_draw_active.end(); ++it) {
            for (unsigned long index = 0; index < m_object_masses.size() - 1; index++) {
                glm::vec4 pos = m_camera_object.calculate_position_on_draw_plane(
                    m_trajectory_lines[it->first][index].second);
                if (pos.w < glm::radians(35.f)) {
                    glm::vec4 pos2 = m_camera_object.calculate_position_on_draw_plane(
                        m_trajectory_lines[it->first][index + 1].second);
                    if (pos2.w < glm::radians(35.f)) {
                        set_color(m_trajectory_lines[it->first][index].first);
                        SDL_RenderDrawLine(m_renderer, pos.x, pos.y, pos2.x, pos2.y);
                        // set_color({255,255,0,255});
                        // SDL_RenderDrawLine(m_renderer,x1,y1,x2,y2);
                    }
                }
            }
        }
    }
    set_color({225, 255, 255, 255});
}

void Visualizer::display_all_grav_ranges() {
    for (auto it = m_grav_range_draw_active.begin(); it != m_grav_range_draw_active.end(); it++) {
        // std::cout << it->first << " " << it->second<< std::endl;
        display_grav_range(it->first, it->second);
    }
}

void Visualizer::draw_all_object_circles() {
    for (unsigned long i = 0; i < m_object_positions[m_iteration_number].size(); i++) {
        draw_object_circle(m_object_id_maps[m_iteration_number][i]);
    }
}

void Visualizer::display_grav_range(unsigned long id, double min_force) {
    // std::cout << "START" << std::endl;
    double sqrtM = sqrt(m_object_masses[m_iteration_number][id]);
    // std::cout << sqrtM << std::endl;
    double sqrtG = sqrt(0.000000000066742);
    // std::cout << sqrtG << std::endl;
    double sqrtMin = sqrt(min_force);
    // std::cout << min_force << std::endl;
    double erg = sqrtG / sqrtMin;
    // std::cout << erg << std::endl;
    erg = erg * sqrtM;
    // std::cout << erg << std::endl;
    unsigned long x, y, z;
    z = 0;
    double width_height = ((erg * 2) / m_scale);
    // std::cout << width_height << std::endl;
    // std::cout << m_scale << std::endl << m_object_radiuses[m_iteration_number][id] << std::endl
    // << width_height<< std::endl;
    x = (((m_object_positions[m_iteration_number][id].x / m_scale) - (width_height / 2)) -
         m_camera.x);
    y = (((m_object_positions[m_iteration_number][id].y / m_scale) - (width_height / 2)) -
         m_camera.y);
    render_texture(m_resource_manager.get_texture("grav_range"), x, y, z, width_height,
                   width_height);
}

void Visualizer::display_data(unsigned long particle_id) {
    unsigned long particle_index = m_object_id_maps[m_iteration_number].find(particle_id)->second;

    glm::vec3 pos = m_object_positions[m_iteration_number][particle_index];
    glm::vec4 plane_pos = m_camera_object.calculate_position_on_draw_plane(pos);
    pos = {(pos.x / 149597870700.0), (pos.y / 149597870700.0), (pos.z / 149597870700.0)};

    double dist = glm::length(pos);

    int pos_x = plane_pos.x;
    int pos_y = plane_pos.y;

    std::vector<SDL_Point> line;

    // std::cout << m_object_positions[index][obj_id].x / m_scale - m_camera.x << std::endl;
    // std::cout << m_object_positions[index][obj_id].y / m_scale + m_camera.y << std::endl;

    // glm::vec3 current_velo = m_object_positions[m_iteration_number + 1][particle_id] -
    // m_object_positions[m_iteration_number][particle_id];
    SDL_Color color = {176, 33, 38, 255};
    draw_text("Coordinates:", pos_x + 20, pos_y, color);
    draw_text(glm::to_string(pos), pos_x + 60, pos_y + 12, color);
    draw_text("Distance to Sun:", pos_x + 20, pos_y + 24, color);
    draw_text(std::to_string(dist) + " AU", pos_x + 60, pos_y + 36, color);
    draw_text("Velocity:", pos_x + 20, pos_y + 48, color);
    draw_text(std::to_string(glm::length(m_object_velocities[m_iteration_number][particle_index])) +
                  " m/s",
              pos_x + 60, pos_y + 60, color);
    draw_text("Mass:", pos_x + 20, pos_y + 72, color);
    draw_text(
        std::to_string(m_object_masses[m_iteration_number][particle_index] / 100000000000000) +
            " x 10^15",
        pos_x + 60, pos_y + 84, color);
    draw_text("Radius:", pos_x + 20, pos_y + 96, color);
    draw_text(
        std::to_string(m_object_radiuses[m_iteration_number][particle_index] / 149597870700.0),
        pos_x + 60, pos_y + 108, color);
}

void Visualizer::draw_data() {
    for (auto it = m_display_data_active.begin(); it != m_display_data_active.end(); ++it) {
        display_data(*it);
    }
}
SDL_Color Visualizer::set_color_gradient_for_speed(double speed, double max_speed,
                                                   SDL_Color highest_speed_color,
                                                   SDL_Color middle_speed_color,
                                                   SDL_Color lowest_speed_color) {
    if (speed < max_speed / 4) {
        middle_speed_color = highest_speed_color;
    } else {
        middle_speed_color = lowest_speed_color;
    }
    double val = sqrt(speed / max_speed);
    if (val > 1) {
        val = 1;
    }
    Uint8 r = lowest_speed_color.r + val * (highest_speed_color.r - lowest_speed_color.r);
    Uint8 g = lowest_speed_color.g + val * (highest_speed_color.g - lowest_speed_color.g);
    Uint8 b = lowest_speed_color.b + val * (highest_speed_color.b - lowest_speed_color.b);
    return {r, g, b, 255};
}

void Visualizer::clear_trajectory_lines() {
    m_line_draw_active.clear();
}

void Visualizer::load_textures() {
    m_resource_manager.load_texture("Triangle_Green", "Textures/Green_triangle.bmp", m_renderer);
    m_resource_manager.load_texture("Background", "Textures/Background.bmp", m_renderer);
    m_resource_manager.load_texture("ConBack", "Textures/ConsoleBackground.bmp", m_renderer);

    m_resource_manager.load_texture("Circle", "Textures/circle.bmp", m_renderer);
    m_resource_manager.load_texture("grav_range", "Textures/grav_range.bmp", m_renderer);
    m_resource_manager.load_texture("collision_detected", "Textures/coll.bmp", m_renderer);
}

void Visualizer::draw_object_circle(unsigned long id) {
    unsigned long z = 0;
    // std::cout << m_scale << std::endl << m_object_radiuses[m_iteration_number][id] << std::endl
    // << width_height<< std::endl;
    // x = (((m_object_positions[m_iteration_number][id].x/ m_scale) - (width_height/2)) -
    // m_camera.x) ;
    // y = (((m_object_positions[m_iteration_number][id].y/ m_scale) - (width_height/2)) -
    // m_camera.y);

    glm::vec4 plane_pos = m_camera_object.calculate_position_on_draw_plane(
        m_object_positions[m_iteration_number][id]);

    if (glm::degrees(plane_pos.w) < 45 / 2) {
        double new_d = glm::length(m_camera_object.m_position + m_camera_object.m_orientation +
                                   glm::vec3(plane_pos));
        double radius = m_object_radiuses[m_iteration_number][id];
        double d =
            glm::length(m_camera_object.m_position - m_object_positions[m_iteration_number][id]);
        double width_height = ((new_d * radius) / d);

        float gegen_kath = sin(plane_pos.w) * d;
        double an_kath = sqrt((d * d) - (gegen_kath * gegen_kath));
        double orient_length = glm::length(m_camera_object.m_orientation);

        double x = glm::length((glm::normalize(m_camera_object.m_orientation) * gegen_kath) -
                               m_object_positions[m_iteration_number][id]);
        double test = orient_length * radius * sqrt(x * x + an_kath * an_kath + radius * radius) /
                      (an_kath * an_kath - radius * radius);
        width_height = test;
        render_texture(m_resource_manager.get_texture("Circle"), plane_pos.x - width_height / 2,
                       plane_pos.y - width_height / 2, z, width_height, width_height);
    }
}

void Visualizer::calculate_collision_points() {

    std::cout << "size: " << m_collision_data.size() << std::endl;
    for (unsigned long i = 0; i < m_collision_data.size(); i++) {
        std::vector<glm::vec3> it_collision_points;
        for (unsigned long j = 0; j < m_collision_data[i].size(); j++) {
            std::cout << i << " " << j << " " << m_collision_data[i].size() << std::endl;
            unsigned long obj_1_index = m_object_id_maps[i].find(m_collision_data[i][j].y)->second;
            unsigned long obj_2_index = m_object_id_maps[i].find(m_collision_data[i][j].z)->second;
            glm::vec3 pos1 = m_object_positions[i][obj_1_index];
            glm::vec3 vec1 = m_object_velocities[i][obj_1_index];
            glm::vec3 pos2 = m_object_positions[i][obj_2_index];
            glm::vec3 vec2 = m_object_velocities[i][obj_2_index];
            float t = -glm::dot((pos1 - pos2), vec1 - vec2) / glm::dot(vec1 - vec2, vec1 - vec2);
            it_collision_points.push_back((pos1 + (vec1 * t)));
        }
        m_collision_points.push_back(it_collision_points);
    }
}

void Visualizer::draw_collison_marks() {
    int width_height = 32;
    for (unsigned long j = 0; j < m_iteration_number; j++) {
        for (unsigned long i = 0; i < m_collision_points[j].size(); i++) {
            glm::vec4 plane_pos =
                m_camera_object.calculate_position_on_draw_plane(m_collision_points[j][i]);
            if (glm::degrees(plane_pos.w) < 45 / 2) {
                render_texture(m_resource_manager.get_texture("collision_detected"), plane_pos.x,
                               plane_pos.y, plane_pos.z, width_height, width_height);
                draw_text(glm::to_string(m_collision_data[j][i]), plane_pos.x + 36, plane_pos.y,
                          {255, 255, 255, 255});
            }
        }
    }
}

void Visualizer::load_fonts() {
    m_resource_manager.load_font("data-unifon", "Fonts/data-unifon.ttf", 32);
    m_resource_manager.load_font("data-latin", "Fonts/data-latin.ttf", 32);
    m_resource_manager.load_font("aero", "Fonts/Aero.ttf", 64);
}

void Visualizer::render_texture(SDL_Texture *texture, int x, int y, int z, int p_width,
                                int p_height) {
    // Setup the destination rectangle to be at the position we w64t
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = p_width;
    dst.h = p_height;
    SDL_RenderCopy(m_renderer, texture, NULL, &dst);
}

void Visualizer::load_object_data_from_file(std::string filepath) {

    std::cout << "I should be here" << std::endl;
    std::vector<char *> file_name_token;
    std::ifstream file(filepath, std::ifstream::binary);
    std::string s;
    std::vector<glm::vec3> it_pos_vector;
    std::vector<glm::vec3> it_velo_vector;
    std::vector<double> it_masses;
    std::vector<double> it_radiuses;
    std::vector<unsigned long> it_ids;
    std::map<unsigned long, unsigned long> it_ids_map;
    std::vector<glm::u64vec3> it_collision_data;
    // int next_bar = 1;
    std::string bars = "";

    for (int i = 0; i < 51; ++i) {
        bars = bars + " ";
    }
    std::cout << "== Loading data from file:" << std::endl;
    if (file.is_open()) {
        getline(file, s);
        getline(file, s);

        while (getline(file, s)) {
            if (s == ">") {
                m_object_id_maps.push_back(it_ids_map);
                m_object_ids.push_back(it_ids);
                m_object_positions.push_back(it_pos_vector);
                m_object_velocities.push_back(it_velo_vector);
                m_object_masses.push_back(it_masses);
                m_object_radiuses.push_back(it_radiuses);
                // std::cout << m_object_ids.size() << std::endl;
                // stT::cout << m_object_positions.size() << std::endl;
                it_pos_vector.clear();
                it_masses.clear();
                it_velo_vector.clear();
                it_radiuses.clear();
                it_ids.clear();
                it_ids_map.clear();
                // if(m_object_positions.size() % (number_of_iterations / 50) == 0)
                //{
                //    bars.at(next_bar) = '|';
                //    next_bar ++;
                //}
                printf("%lu\r", m_object_ids.size());
                getline(file, s);
                std::stringstream ss(s);
                std::string it_num, it_num_of_particles, it_dt;
                getline(ss, it_num, ' ');
                getline(ss, it_num_of_particles, ' ');

                getline(ss, it_dt, ' ');
                m_iteration_dts.push_back(std::strtod(it_dt.c_str(), NULL));
                // Collisons
                getline(file, s);
                std::stringstream collision_data_stream(s);

                std::string iteration_number, id1, id2;
                it_collision_data.clear();

                while (getline(collision_data_stream, iteration_number, ' ') &&
                       iteration_number.compare("NO_COLLISIONS") != 0) {
                    getline(collision_data_stream, id1, ' ');
                    getline(collision_data_stream, id2, ' ');
                    it_collision_data.push_back({std::strtoul(iteration_number.c_str(), NULL, 0),
                                                 std::strtoul(id1.c_str(), NULL, 0),
                                                 std::strtoul(id2.c_str(), NULL, 0)});
                    std::cout << "tezt" << glm::to_string(it_collision_data.back()) << std::endl;
                    ;
                }
                m_collision_data.push_back(it_collision_data);

            } else {
                // std::cout << s << std::endl;
                std::string id, x, y, z, velo_x, velo_y, velo_z, mass, radius;
                std::stringstream ss(s);
                getline(ss, id, ' ');
                getline(ss, x, ' ');
                getline(ss, y, ' ');
                getline(ss, z, ' ');
                getline(ss, velo_x, ' ');
                getline(ss, velo_y, ' ');
                getline(ss, velo_z, ' ');
                getline(ss, mass, ' ');
                getline(ss, radius, ' ');

                it_ids_map.insert(std::pair<unsigned long, unsigned long>(
                    std::strtoul(id.c_str(), NULL, 0), it_ids_map.size()));
                it_ids.push_back(std::strtoul(id.c_str(), NULL, 0));
                it_pos_vector.push_back(glm::vec3(std::stod(x), std::stod(y), std::stod(z)));
                it_velo_vector.push_back(
                    glm::vec3(std::stod(velo_x), std::stod(velo_y), std::stod(velo_z)));
                it_masses.push_back(std::stod(mass));
                it_radiuses.push_back(std::stod(radius));
                // std::cout << it_vector.back().toString() << std::endl;
            }
            /*
               for(auto it : it_ids_map)
               {
               std::cout << it.first << " " << it.second << " , ";
               }
               */
        }
        std::cout << "\n == Loading done ==" << std::endl;
        file.close();

    } else {
        std::cout << "file could not be opened" << std::endl;
    }
}

std::string Visualizer::format_seconds_to_time(double time) {
    time_t seconds((int)time);
    tm *p = gmtime(&seconds);

    return (std::to_string(p->tm_year) + " " + std::to_string(p->tm_yday) + " " +
            std::to_string(p->tm_hour) + " " + std::to_string(p->tm_min));
}
/*
   void Visualizer::load_particle_archive_from_file(std::string filename)
   {
   std::ifstream ifs("../Simulation/archive.test",std::ios::binary);
   boost::archive::binary_iarchive ia(ifs, boost::archive::no_header);
   double cur_iteration = 0;
   double max_iteration = 1;
   double it_dt;

   std::vector<glm::vec3 > it_pos_vector;
   std::vector<glm::vec3 > it_velo_vector;
   std::vector<double> it_masses;
   std::vector<double> it_radiuses;
   std::vector<unsigned long>it_ids;
   std::map<unsigned long, unsigned long>it_ids_map;
   while(cur_iteration < max_iteration)
   {


   std::cout << "HERE1" << std::endl;
   ia >> cur_iteration;
   std::cout << "HERE2" << std::endl;
   ia >> max_iteration;
   std::cout << cur_iteration << " " <<  max_iteration << std::endl;
   std::cout << "HERE3" << std::endl;
   m_object_positions.push_back(std::vector<glm::vec3>());
   ia >> m_object_positions.back();

   std::cout << "HERE4" << std::endl;
   ia >> it_velo_vector;
   std::cout << "HERE5" << std::endl;
   ia >> it_masses;
   std::cout << "HERE6" << std::endl;
   ia >> it_radiuses;
   std::cout << "HERE7" << std::endl;
   ia >> it_ids;
   std::cout << "HERE8" << std::endl;
   ia >> it_dt;

   for(unsigned long i = 0; i < it_pos_vector.size(); i ++)
   {
   std::cout << it_pos_vector[i].toString() << std::endl;
   }

   for(unsigned long i = 0; i < it_ids.size(); i++)
   {
   it_ids_map.insert(std::pair<unsigned long, unsigned long>(it_ids[i],i));
   }
   m_object_id_maps.push_back(it_ids_map);
   m_object_ids.push_back(it_ids);
   m_object_positions.push_back(it_pos_vector);
   m_object_velocities.push_back(it_velo_vector);
   m_object_masses.push_back(it_masses);
   m_object_radiuses.push_back(it_radiuses);
        //std::cout << m_object_ids.size() << std::endl;
        //stT::cout << m_object_positions.size() << std::endl;

        std::cout << it_pos_vector.size() << std::endl;
        }
        }

*/
