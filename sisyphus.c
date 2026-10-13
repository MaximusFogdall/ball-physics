#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.14159265358979323846
#define RAD 57.295779513

struct Point { float x; float y; };
struct Ball { float x_p; float y_p; float x_v; float y_v; 
float rad; float mass; float rest; float fric; float imp_dir; float imp_mag; float con_dir; float con_mag; bool collisions;};
struct Joint { struct Ball *body1; struct Ball *body2; float dist; float stren; };

void Force(struct Ball *obj, float dir, float mag, float dt) {
    float radians = ((float) dir * PI) / 180;
    //printf("%p \n", obj);
    fflush(stdout);
    obj->x_v += ((cos(radians) * mag) / obj->mass) * dt;
    obj->y_v += ((sin(radians) * mag) / obj->mass) * dt;
}

void Impulse(struct Ball *obj, float dir, float mag) {
    float radians = ((float) dir * PI) / 180;
    obj->x_v += ((cos(radians) * mag) / obj->mass);
    obj->y_v += ((sin(radians) * mag) / obj->mass);
}

float Distance(float x1, float y1, float x2, float y2) {
    return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}


int main(void)
{
    int screenWidth = 1400;
    int screenHeight = 1050;
    
    int tool = 0;
    int temp_tool = 0;
    
    struct Ball *selected_item;
    struct Ball *menu_item = NULL;
    int offset_x;
    int offset_y;
    struct Ball *body1 = NULL;
    struct Ball *body2 = NULL;
    
    int line_count = 0;
    
    InitWindow(screenWidth, screenHeight, "Sisyphus");

    SetTargetFPS(120);
    
    struct Point (*lines)[2048] = malloc(256 * sizeof(*lines));
    int line_lengths[256] = {0};
    
    struct Ball *balls = malloc(48 * sizeof(*balls));
    struct Ball *balls_copy = malloc(48 * sizeof(*balls_copy));
    //struct Ball ball = {500, 300, 0, 0, 40, 5, 0.1, 0.05, 0, 0, 270, 980 * 5, 0};
    //balls[0] = ball;
    
    //struct Ball ball2 = {900, 300, 0, 0, 40, 1, 0.1, 0.05, 0, 0, 270, 980, 0};
    //balls[1] = ball2;
    int ball_count = 0;
    
    struct Joint *joints = malloc(256 * sizeof(*joints));
    int joint_count = 0;
    
    int simulation = 0;
    int first_frame = 1;
    
    //printf("%p \n", &balls[0]);
    //printf("%p \n", &balls[1]);
    fflush(stdout);
    
    char string_radius[32];
    bool edit_radius = false;
    
    char string_mass[32];
    bool edit_mass = false;
    
    char string_friction[32];
    bool edit_friction = false;
    
    char string_restitution[32];
    bool edit_restitution = false;
    
    char string_impulse[32];
    bool edit_impulse = false;
    
    char string_impulse_vect[32];
    bool edit_impulse_vect = false;
    
    char string_gravity[32];
    bool edit_gravity = false;
    
    char string_gravity_vect[32];
    bool edit_gravity_vect = false;
    
    bool collisions = false;
    
    char string_joint_forces[32][32];
    bool edit_joint[32] = {false};
    struct Joint **editable_joints = malloc(32 * sizeof(*editable_joints));
    int editable_joint_count = 0;
    
    
    //GuiSetStyle(VALUEBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(VALUEBOX, TEXT_PADDING, 4);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 32);
    GuiSetStyle(CHECKBOX, TEXT_PADDING, 4);

        
    while (!WindowShouldClose())
    {
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        float dt = GetFrameTime();
        
        for (int i = 0; i < line_count; i++) {
            DrawLine((int) lines[i][0].x, (int) lines[i][0].y, (int) lines[i][line_lengths[i] - 1].x, (int) lines[i][line_lengths[i] - 1].y, WHITE);
        }
        for (int i = 0; i < joint_count; i++) {
            DrawLine((int) joints[i].body1->x_p, (int) joints[i].body1->y_p, (int) joints[i].body2->x_p, (int) joints[i].body2->y_p, RED);
            if (!simulation) {
                joints[i].dist = Distance(joints[i].body1->x_p, joints[i].body1->y_p, joints[i].body2->x_p, joints[i].body2->y_p);
            }
        }
        
        if (simulation) {
            menu_item = NULL;
            for (int i = 0; i < ball_count; i++) {
                //printf("%p \n", &balls[i]);
                fflush(stdout);
                if (first_frame) {
                    Impulse(balls + i, balls[i].imp_dir, balls[i].imp_mag);
                }
                
                Force(balls + i, balls[i].con_dir, balls[i].con_mag, dt);

                float closest = balls[i].rad;
                struct Point closest_pt;
                
                
                for (int b = 0; b < ball_count; b++) {
                    if (b != i && balls[b].collisions) {
                        int point_found = 0;    
                        float d = Distance(balls[i].x_p, balls[i].y_p, balls[b].x_p, balls[b].y_p);
                        if (d < balls[b].rad + balls[i].rad) {
                            closest = d;
                            point_found = 1;
                        }
                        
                        if (point_found) {
                            float new_angle = ((atan2(-balls[i].y_p - -balls[b].y_p, balls[i].x_p - balls[b].x_p)) * RAD);
                            float new_angle_other = ((atan2(-balls[b].y_p - -balls[i].y_p, balls[b].x_p - balls[i].x_p)) * RAD);
                            
   
                            float current_vel = sqrt(pow(balls[i].x_v, 2) + pow(balls[i].y_v, 2));
                            float current_vel_other = sqrt(pow(balls[b].x_v, 2) + pow(balls[b].y_v, 2));
                            
                            printf("%f %f\n\n", new_angle, new_angle_other);
                            fflush(stdout);

                            
                            float radians = (new_angle * PI) / 180;
                            float radians_other = (new_angle_other * PI) / 180;
                            
                            float pt_dist = balls[i].rad + balls[b].rad  - closest;
                            balls[i].x_p += cos(radians) * pt_dist * 0.5;
                            balls[i].y_p -= sin(radians) * pt_dist * 0.5;
                            balls[b].x_p += cos(radians_other) * pt_dist * 0.5;
                            balls[b].y_p -= sin(radians_other) * pt_dist * 0.5;
                           

                            float normal_vect_x = cos(radians) * 1;
                            float normal_vect_y = sin(radians) * 1;
                            float normal_vect_x_other = cos(radians_other) * 1;
                            float normal_vect_y_other = sin(radians_other) * 1;

                            float speed_along_normal = balls[i].x_v * normal_vect_x + balls[i].y_v * normal_vect_y;
                            float speed_along_normal_other = balls[b].x_v * normal_vect_x_other + balls[b].y_v * normal_vect_y_other;
                            float speed_along_tang = current_vel - speed_along_normal;

                            Force(balls + i, new_angle, -speed_along_normal, dt);
                            Force(balls + b, new_angle_other, -speed_along_normal_other, dt);
                            
                            Force(balls + i, new_angle, balls[i].con_mag, dt);
                            Force(balls + b, new_angle_other, balls[b].con_mag, dt);
                            
                            

                            float new_force = truncf((-2 * speed_along_normal) * (balls[i].rest) * (balls[i].mass));
                            
                            balls[i].x_v = balls[i].x_v * (1 - balls[i].fric / 10);
                            balls[i].y_v = balls[i].y_v * (1 - balls[i].fric / 10);
                            
                            balls[b].x_v = balls[b].x_v * (1 - balls[b].fric / 10);
                            balls[b].y_v = balls[b].y_v * (1 - balls[b].fric / 10);
                            //float new_force_other = truncf((-2 * speed_along_normal_other) * (balls[b].rest) * (balls[b].mass));

                            //if (new_force > 100) {
                                Impulse(balls + i, new_angle, new_force);
                                //Impulse(balls + b, new_angle_other, new_force_other);
                            //}
                        }
                    }
                }
                
                
                for (int l = 0; l < line_count; l++) {
                    int point_found = 0;
                    for (int p = 0; p < line_lengths[l]; p++) {
                        float d = Distance(balls[i].x_p, balls[i].y_p, lines[l][p].x, lines[l][p].y);
                        if (d < closest) {
                            closest = d;
                            closest_pt.x = lines[l][p].x;
                            closest_pt.y = lines[l][p].y;
                            point_found = 1;
                        }
                    }
                    
                    if (point_found) {
                        struct Point line_start = lines[l][0];
                        struct Point line_end = lines[l][line_lengths[l] - 1];
                        
                        float new_angle;
                        
                        if (line_end.x - line_start.x != 0) {
                            new_angle = (int) ((atan2(-line_end.y - -line_start.y, line_end.x - line_start.x)) * RAD) % 360;
                        } else { new_angle = 90; }
                        
                        int side = (line_start.x - line_end.x) * (-balls[i].y_p - -line_end.y) - (-line_start.y - -line_end.y)
                        * (balls[i].x_p - line_end.x);
                                                
                        if (side < 0) {
                            new_angle += 90;
                        } else if (side > 0) {
                            new_angle -= 90;
                        }
                        
                        float current_vel = sqrt(pow(balls[i].x_v, 2) + pow(balls[i].y_v, 2));

                        float pt_dist = balls[i].rad - closest;
                        
                        float radians = (new_angle * PI) / 180;
                        balls[i].x_p += cos(radians) * pt_dist;
                        balls[i].y_p -= sin(radians) * pt_dist;

                       
                        balls[i].x_v = balls[i].x_v * (1 - balls[i].fric / 10);
                        balls[i].y_v = balls[i].y_v * (1 - balls[i].fric / 10);

                        float normal_vect_x = cos(radians) * 1;
                        float normal_vect_y = sin(radians) * 1;

                        float speed_along_normal = balls[i].x_v * normal_vect_x + balls[i].y_v * normal_vect_y;
                        float speed_along_tang = current_vel - speed_along_normal;

                        Force(balls + i, new_angle, -balls[i].con_mag, dt);

                        float new_force = truncf((-2 * speed_along_normal) * (balls[i].rest) * (balls[i].mass));

                        //if (new_force > 100) {
                            Impulse(balls + i, new_angle, new_force);
                        //}
                        
                    }
                }
                
                DrawCircle((int) balls[i].x_p, (int) balls[i].y_p, balls[i].rad, WHITE);
                
                balls[i].x_p += balls[i].x_v * dt;
                balls[i].y_p -= balls[i].y_v * dt;
            }
            
            for (int i = 0; i < joint_count; i++) {
                float d = Distance(joints[i].body1->x_p, joints[i].body1->y_p, joints[i].body2->x_p, joints[i].body2->y_p);
                float difference = d - joints[i].dist;
                float spring_force = -joints[i].stren * difference;
                int dir1 = (int) ((atan2(-joints[i].body1->y_p - -joints[i].body2->y_p, joints[i].body1->x_p - joints[i].body2->x_p)) * RAD) % 360;
                int dir2 = (int) ((atan2(-joints[i].body2->y_p - -joints[i].body1->y_p, joints[i].body2->x_p - joints[i].body1->x_p)) * RAD) % 360;
                
                
                Force(joints[i].body1, dir1, spring_force, dt);
                Force(joints[i].body2, dir2, spring_force, dt);
                
                //printf("%f %d %f\n", difference, dir1, spring_force);
                fflush(stdout);
            }
            
            first_frame = 0;
        } else {
            for (int i = 0; i < ball_count; i++) {
                DrawCircle((int) balls[i].x_p, (int) balls[i].y_p, balls[i].rad, WHITE);
            }
        }
        
        if (IsKeyPressed(KEY_S)) {
            if (!simulation) { 
                memcpy(balls_copy, balls, 48 * sizeof(*balls));
                first_frame = 1;
                simulation = !simulation;
            } else {
                first_frame = 0;
                simulation = !simulation;
                memcpy(balls, balls_copy, 48 * sizeof(*balls_copy));
            }
        }
        
        if (IsKeyPressed(KEY_ONE)) {
            tool = 0;
        }
        if (IsKeyPressed(KEY_TWO)) {
            tool = 1;
        }
        if (IsKeyPressed(KEY_THREE)) {
            tool = 2;
        }
        
        
        if (menu_item) {
            char ptr_string[32];
            snprintf(ptr_string, sizeof(ptr_string), "%p", (void *)menu_item);
            DrawText(ptr_string, 1190, 10, 20, LIGHTGRAY);
            if (GuiValueBoxFloat((Rectangle){ 1190, 35, 200, 40 }, "Radius", string_radius, &menu_item->rad, edit_radius)) {
                if (tool > -1) { temp_tool = tool; tool = -1; } 
                edit_radius = !edit_radius; menu_item->rad = Clamp(menu_item->rad, 5.0f, 1000.0f);
                //if (!edit_radius) { tool = temp_tool; } 
                snprintf(string_radius, sizeof(string_radius), "%.2f", menu_item->rad); }
                
            if (GuiValueBoxFloat((Rectangle){ 1190, 85, 200, 40 }, "Mass", string_mass, &menu_item->mass, edit_mass)) {
                if (tool > -1) { temp_tool = tool; tool = -1; } 
                edit_mass = !edit_mass; menu_item->mass = Clamp(menu_item->mass, 1.0f, 1000.0f);
                //if (!edit_mass) { tool = temp_tool; } 
                snprintf(string_mass, sizeof(string_mass), "%.2f", menu_item->mass); }
                
            if (GuiValueBoxFloat((Rectangle){ 1190, 135, 200, 40 }, "Friction", string_friction, &menu_item->fric, edit_friction)) {
                if (tool > -1) { temp_tool = tool; tool = -1; } 
                edit_friction = !edit_friction; menu_item->fric = Clamp(menu_item->fric, 0.01f, 1.0f);
                //if (!edit_friction) { tool = temp_tool; } 
                snprintf(string_friction, sizeof(string_friction), "%.2f", menu_item->fric); }
                
            if (GuiValueBoxFloat((Rectangle){ 1190, 185, 200, 40 }, "Restitution", string_restitution, &menu_item->rest, edit_restitution)) {
                if (tool > -1) { temp_tool = tool; tool = -1; } 
                edit_restitution = !edit_restitution; menu_item->rest = Clamp(menu_item->rest, 0.1f, 1.0f);
                //if (!edit_restitution) { tool = temp_tool; } 
                snprintf(string_restitution, sizeof(string_restitution), "%.2f", menu_item->rest); }
                
            if (GuiValueBoxFloat((Rectangle){ 1190, 235, 200, 40 }, "Gravity", string_gravity, &menu_item->con_mag, edit_gravity)) {
                if (tool > -1) { temp_tool = tool; tool = -1; } 
                edit_gravity = !edit_gravity; 
                //if (!edit_gravity) { tool = temp_tool; } 
                snprintf(string_gravity, sizeof(string_gravity), "%.2f", menu_item->con_mag); }
                
            if (GuiValueBoxFloat((Rectangle){ 1190, 285, 200, 40 }, "Gravity Vect.", string_gravity_vect, &menu_item->con_dir, edit_gravity_vect)) {
                if (tool > -1) { temp_tool = tool; tool = -1; } 
                edit_gravity_vect = !edit_gravity_vect; menu_item->con_dir = Clamp(menu_item->con_dir, 0.0f, 360.0f);
                //if (!edit_gravity_vect) { tool = temp_tool; } 
                snprintf(string_gravity_vect, sizeof(string_gravity_vect), "%.2f", menu_item->con_dir); }
                
            if (GuiValueBoxFloat((Rectangle){ 1190, 335, 200, 40 }, "Impulse", string_impulse, &menu_item->imp_mag, edit_impulse)) {
                if (tool > -1) { temp_tool = tool; tool = -1; } 
                edit_impulse = !edit_impulse;
                //if (!edit_impulse) { tool = temp_tool; } 
                snprintf(string_impulse, sizeof(string_impulse), "%.2f", menu_item->imp_mag); }
                
            if (GuiValueBoxFloat((Rectangle){ 1190, 385, 200, 40 }, "Impulse Vect.", string_impulse_vect, &menu_item->imp_dir, edit_impulse_vect)) {
                if (tool > -1) { temp_tool = tool; tool = -1; } 
                edit_impulse_vect = !edit_impulse_vect; menu_item->imp_dir = Clamp(menu_item->imp_dir, 0.0f, 360.0f);
                //if (!edit_impulse_vect) { tool = temp_tool; } 
                snprintf(string_impulse_vect, sizeof(string_impulse_vect), "%.2f", menu_item->imp_dir); }
                
            if (GuiCheckBox((Rectangle){ 1190, 435, 40, 40 }, "Collisions", &menu_item->collisions)) {
                tool = -1;
                //snprintf(string_impulse_vect, sizeof(string_impulse_vect), "%.2f", impulse_vect);
            }
            
            for (int j = 0; j < editable_joint_count; j++) {
                char ptr_string[32];
                
                if (menu_item == editable_joints[j]->body1) {
                    snprintf(ptr_string, sizeof(ptr_string), "w/%p", (void *)editable_joints[j]->body2);
                    DrawText(ptr_string, 1170, 490 + (j * 75), 20, LIGHTGRAY);
                }
                
                if (menu_item == editable_joints[j]->body2) {
                    snprintf(ptr_string, sizeof(ptr_string), "w/%p", (void *)editable_joints[j]->body1);
                    DrawText(ptr_string, 1170, 490 + (j * 75), 20, LIGHTGRAY);
                }
                
                if (GuiValueBoxFloat((Rectangle){ 1190, 515 + (j * 75), 200, 40 }, "Joint Strength", 
                    string_joint_forces[j], &editable_joints[j]->stren, edit_joint[j])) {
            
                    tool = -1;
                    editable_joints[j]->stren = Clamp(editable_joints[j]->stren, 10.0f, 100000.0f);
                    edit_joint[j] = !edit_joint[j]; 
                    snprintf(string_joint_forces[j], sizeof(string_joint_forces[j]), "%.2f", editable_joints[j]->stren);
                }       
                
            }
        }
                    
        if (!simulation) {
            if (tool == 0) {
                struct Point point1;
                struct Point point2;
                if (IsMouseButtonPressed(0)) {
                    point1.x = GetMouseX();
                    point1.y = GetMouseY();
                }
                if (IsMouseButtonReleased(0)) {
                    point2.x = GetMouseX();
                    point2.y = GetMouseY();
                    
                    if (point2.x < point1.x) {
                        struct Point temp_point = point1;
                        point1 = point2;
                        point2 = temp_point;
                    }
                    
                    int d = (int) Distance(point1.x, point1.y, point2.x, point2.y);
                    if (d > 1) {
                        float angle = (atan2(-point2.y - -point1.y, point2.x - point1.x));
                        for (int i = 0; i <= d; i++) {
                            lines[line_count][i].x = point1.x + (i * cos(angle));
                            lines[line_count][i].y = point1.y - (i * sin(angle));
                        }
                        line_lengths[line_count] = (int) d;
                        line_count++;
                    }

                }
                
                if (IsMouseButtonDown(0)) {
                    DrawLine((int) point1.x, (int) point1.y, GetMouseX(), GetMouseY(), WHITE);
                }
            }
            
            if (tool == 2) {
                if (IsMouseButtonPressed(0)) {
                    body1 = NULL;
                    body2 = NULL;
                    for (int i = 0; i < ball_count; i++) {
                        float d = Distance((float) GetMouseX(), (float) GetMouseY(), balls[i].x_p, balls[i].y_p);
                        if (d < balls[i].rad) {
                            //printf("%p \n", balls + i);
                            fflush(stdout);
                            body1 = balls + i;
                            break;
                        }
                    }
                }
                
                if (IsMouseButtonReleased(0)) {
                    for (int i = 0; i < ball_count; i++) {
                        float d = Distance((float) GetMouseX(), (float) GetMouseY(), balls[i].x_p, balls[i].y_p);
                        if (d < balls[i].rad) {
                            //printf("%p \n", balls + i);
                            fflush(stdout);
                            body2 = balls + i;
                            break;
                        }
                    }
                    
                    if (body2 && body1) {
                        float d = Distance(body1->x_p, body1->y_p, body2->x_p, body2->y_p);
                        struct Joint joint = {body1, body2, 0, 900};
                        joints[joint_count] = joint;
                        joint_count++;
                    }
                }
                
                if (body1) {
                    if (IsMouseButtonDown(0)) {
                        DrawLine((int) body1->x_p, (int) body1->y_p, GetMouseX(), GetMouseY(), RED);
                    }
                }
            }
            
            if (tool == 1) {
                if (IsMouseButtonPressed(0)) {
                    struct Ball new_ball = {GetMouseX(), GetMouseY(), 0, 0, 40, 5, 0.8, 0.05, 0, 0, 270, 5 * 980, 0};
                    balls[ball_count] = new_ball;
                    ball_count++;
                }
            }
            
            if (IsMouseButtonPressed(1)) {
                for (int i = 0; i < ball_count; i++) {
                    float d = Distance((float) GetMouseX(), (float) GetMouseY(), balls[i].x_p, balls[i].y_p);
                    if (d < balls[i].rad) {
                        selected_item = balls + i;
                        
                        menu_item = balls + i;
                        snprintf(string_radius, sizeof(string_radius), "%.2f", menu_item->rad);
                        snprintf(string_mass, sizeof(string_mass), "%.2f", menu_item->mass);
                        snprintf(string_friction, sizeof(string_friction), "%.2f", menu_item->fric);
                        snprintf(string_restitution, sizeof(string_restitution), "%.2f", menu_item->rest);
                        snprintf(string_gravity, sizeof(string_gravity), "%.2f", menu_item->con_mag);
                        snprintf(string_gravity_vect, sizeof(string_gravity_vect), "%.2f", menu_item->con_dir);
                        snprintf(string_impulse, sizeof(string_impulse), "%.2f", menu_item->imp_mag);
                        snprintf(string_impulse_vect, sizeof(string_impulse_vect), "%.2f", menu_item->imp_dir);
                        
                        editable_joint_count = 0;
                        for (int j = 0; j < joint_count; j++) {
                            if (joints[j].body1 == balls + i || joints[j].body2 == balls + i) {
                                editable_joints[editable_joint_count] = &joints[j];
                                snprintf(string_joint_forces[editable_joint_count], sizeof(string_joint_forces[editable_joint_count]), "%.2f", joints[j].stren);
                                editable_joint_count++;
                            }
                        }
                        
                        offset_x = balls[i].x_p - (int) GetMouseX();
                        offset_y = balls[i].y_p - (int) GetMouseY();
                        break;
                    }
                }
            } 
            
            if (IsMouseButtonDown(1)) {
                if (selected_item) {
                    selected_item->x_p = (int) GetMouseX() + offset_x;
                    selected_item->y_p = (int) GetMouseY() + offset_y;
                    selected_item->x_v = 0;
                    selected_item->y_v = 0;
                }
            } else {
                selected_item = NULL;
            }
        }
        
        
        EndDrawing();
    }
    
    free(balls);
    free(lines);
    free(joints);

    CloseWindow();
    return 0;
}